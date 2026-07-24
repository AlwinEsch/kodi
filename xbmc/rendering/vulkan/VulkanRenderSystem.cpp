/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanRenderSystem.h"

#include "ServiceBroker.h"
#include "URL.h"
#include "filesystem/File.h"
#include "filesystem/SpecialProtocol.h"
#include "guilib/DirtyRegion.h"
#include "guilib/graphics/vulkan/VulkanGUITexture.h"
#include "platform/MessagePrinter.h"
#include "rendering/vulkan/VulkanCommandBuffer.h"
#include "rendering/vulkan/VulkanCommandPool.h"
#include "rendering/vulkan/VulkanFramebuffer.h"
#include "rendering/vulkan/VulkanInstance.h"
#include "rendering/vulkan/VulkanMatrix.h"
#include "rendering/vulkan/VulkanRenderPass.h"
#include "rendering/vulkan/VulkanScopedWrite.h"
#include "rendering/vulkan/VulkanSwapChain.h"
#include "rendering/vulkan/shaders/VulkanShaderControl.h"
#include "rendering/vulkan/shaders/VulkanShaderTest.h"
#include "rendering/vulkan/shaders/VulkanShaderTexture.h"
#include "rendering/vulkan/utils/VulkanInitStructs.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "settings/AdvancedSettings.h"
#include "settings/DisplaySettings.h"
#include "settings/SettingsComponent.h"
#include "utils/FileUtils.h"
#include "utils/MathUtils.h"
#include "utils/StringUtils.h"
#include "utils/SystemInfo.h"
#include "utils/TimeUtils.h"
#include "utils/XTimeUtils.h"
#include "utils/log.h"
#include "windowing/GraphicContext.h"
#include "windowing/WinSystem.h"

#include <array>
#include <cassert>

using namespace std::chrono_literals;
using namespace KODI::GUILIB::GRAPHICS::VULKAN;
using namespace KODI::RENDERING::VULKAN::UTILS;

namespace KODI::RENDERING::VULKAN
{

namespace
{

VkClearColorValue defaultClearColor = {{0.0f, 0.0f, 0.0f, 1.0f}};

std::unique_ptr<CVulkanDeviceQueue> CreateVulkanDeviceQueue(CVulkanRenderSystem* vulkanRenderSystem,
                                                            DeviceQueueOptions options,
                                                            uint32_t heapMemoryLimit,
                                                            bool allowProtectedMemory,
                                                            bool isThreadSafe)
{
  assert(vulkanRenderSystem != nullptr);

  std::vector<const char*> requiredExtensions = vulkanRenderSystem->GetRequiredDeviceExtensions();
  std::vector<const char*> optionalExtensions = vulkanRenderSystem->GetOptionalDeviceExtensions();

  uint32_t gpuVendorId{0};
  uint32_t gpuDeviceId{0};

  auto deviceQueue = std::make_unique<CVulkanDeviceQueue>(vulkanRenderSystem);
  if (!deviceQueue->Initialize(options, gpuVendorId, gpuDeviceId, requiredExtensions,
                               optionalExtensions, heapMemoryLimit, allowProtectedMemory,
                               isThreadSafe))
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to initialize device queue ({0}:{1})", __FILENAME__,
              __LINE__);
    return nullptr;
  }

  return deviceQueue;
}

} // namespace

CVulkanRenderSystem::CVulkanRenderSystem() : CRenderSystemBase()
{
}

CVulkanRenderSystem::~CVulkanRenderSystem()
{

  //Destroy();
}

bool CVulkanRenderSystem::InitRenderSystem()
{
  CLog::Log(LOGDEBUG, "Vulkan: Render system becoming initialized ({0}:{1})", __FILENAME__,
            __LINE__);

  m_vkData.vkSurface = GetVulkanSurface();
  if (!m_vkData.vkSurface)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create window surface. ({0}:{1})", __FILENAME__,
              __LINE__);
    return false;
  }

  m_deviceQueue = CreateVulkanDeviceQueue(this,
                                          DeviceQueueOption::GRAPHICS_QUEUE_FLAG |
                                              DeviceQueueOption::PRESENTATION_SUPPORT_QUEUE_FLAG,
                                          0, false, false);
  if (!m_deviceQueue)
  {
    return false;
  }

  m_vkInstance = m_deviceQueue->vkInstance();
  m_vkData.vkDevice = m_deviceQueue->vkDevice();
  m_vkPhysicalDevice = m_deviceQueue->vkPhysicalDevice();
  m_vkData.vkQueue = m_deviceQueue->vkQueue();
  m_vkData.vkCommandPool = m_deviceQueue->CommandPool()->vkCommandPool();

  m_surface = std::make_unique<CVulkanSurface>(m_vkInstance, m_vkData.vkSurface);
  if (!m_surface->Initialize(m_deviceQueue.get(), SurfaceFormat::FORMAT_RGBA_32))
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to initialize surface ({0}:{1})", __FILENAME__, __LINE__);
    return false;
  }

  m_vkSwapchain = m_surface->vkSwapchain();
  m_vkSwapchainFormat = m_surface->vkSurfaceFormat().format;

  m_renderPass = CVulkanRenderPass::Create(m_vkSwapchainFormat, m_vkData.vkDevice);
  if (!m_renderPass)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create render pass ({0}:{1})", __FILENAME__, __LINE__);
    return false;
  }

  m_surface->Reshape({{0, 0}, {m_width, m_height}}, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);

  m_vkData.vkRenderPass = m_renderPass->vkRenderPass();

  if (!CreatePipeline())
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create pipeline ({0}:{1})", __FILENAME__, __LINE__);
    return false;
  }

  m_shaderControl = std::make_unique<CVulkanShaderControl>(&m_vkData, m_deviceQueue.get());
  if (!m_shaderControl->CreateAllShaders(m_vkData.vkDevice, m_vkData.vkRenderPass))
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to initialize shader control ({0}:{1})", __FILENAME__,
              __LINE__);
    return false;
  }

  CVulkanSwapChain* vulkan_swap_chain = m_surface->SwapChain();
  const uint32_t num_images = vulkan_swap_chain->AmmountSwapChainImages();
  m_framebuffers.resize(num_images);
  m_bRenderCreated = true;

  CVulkanGUITexture::Register();

  m_camera.type = Camera::CameraType::lookat;
  m_camera.setPosition(glm::vec3(0.0f, 0.0f, -2.5f));
  m_camera.setRotation(glm::vec3(0.0f));
  m_camera.setPerspective(60.0f, (float)m_width / (float)m_height, 1.0f, 256.0f);

  m_testShaderTexture =
      dynamic_cast<CVulkanShaderTexture*>(m_shaderControl->GetShader(VULKAN_SM_TEXTURE));

  m_pipeline = m_testShaderTexture->VulkanPipeline();
  m_pipelineLayout = m_testShaderTexture->VulkanPipelineLayout();

  m_testShader = dynamic_cast<CVulkanShaderTest*>(m_shaderControl->GetShader(VULKAN_SM_TEST));
  return true;
}

bool CVulkanRenderSystem::DestroyRenderSystem()
{
  if (!m_bRenderCreated)
    return false;

  CLog::Log(LOGDEBUG, "Vulkan: Render system becoming destroyed ({0}:{1})", __FILENAME__, __LINE__);

  m_shaderControl->DestroyAllShaders();

  VkResult result = vkQueueWaitIdle(m_deviceQueue->vkQueue());
  if (result != VK_SUCCESS)
  {
    LogVulkanError(result, "vkQueueWaitIdle", __FILENAME__, __LINE__);
  }

  for (auto& framebuffer : m_framebuffers)
  {
    if (!framebuffer)
      continue;

    framebuffer->Destroy();
    framebuffer.reset();
  }
  m_framebuffers.clear();

  if (m_renderPass)
  {
    m_renderPass.reset();
  }

  if (m_surface)
  {
    m_surface->Destroy();
    m_surface.reset();
  }

  if (m_deviceQueue)
  {
    m_deviceQueue->Destroy();
    m_deviceQueue.reset();
  }

  m_vkData.vkSurface = VK_NULL_HANDLE;
  m_vkData.vkSurfaceFormat = {};
  m_vkInstance = VK_NULL_HANDLE;
  m_vkData.vkDevice = VK_NULL_HANDLE;
  m_vkSwapchain = VK_NULL_HANDLE;
  m_vkData.vkRenderPass = VK_NULL_HANDLE;
  m_vkSwapchainFormat = VK_FORMAT_UNDEFINED;

  m_bRenderCreated = false;

  return true;
}

bool CVulkanRenderSystem::ResetRenderSystem(int width, int height)
{
  if (!m_bRenderCreated)
    return false;

  if (static_cast<uint32_t>(width) == m_width && static_cast<uint32_t>(height) == m_height)
    return true;

  auto& framebuffer = m_framebuffers[m_scopedWrite->ImageIndex()];

  m_width = static_cast<uint32_t>(width);
  m_height = static_cast<uint32_t>(height);

  VkClearValue clearValues[2]{};
  clearValues[0].color = defaultClearColor;
  clearValues[1].depthStencil = {1.0f, 0};

  VkRenderPassBeginInfo begin_info = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .pNext = nullptr,
      .renderPass = m_vkData.vkRenderPass,
      .framebuffer = framebuffer->vkFramebuffer(),
      .renderArea = m_surface->SwapChain()->Size(),
      .clearValueCount = 2,
      .pClearValues = clearValues,
  };

  //fprintf(stderr, "Vulkan: ResetRenderSystem: width=%d, height=%d\n", width, height);
  vkCmdBeginRenderPass(m_currentVkCommandBuffer, &begin_info, VK_SUBPASS_CONTENTS_INLINE);

  vulkanMatrixModview.Clear();
  vulkanMatrixModview->LoadIdentity();
  vulkanMatrixModview->Ortho(0.0f, width - 1, height - 1, 0.0f, -1.0f, 1.0f);
  vulkanMatrixModview.Load();

  vulkanMatrixProject.Clear();
  vulkanMatrixProject->LoadIdentity();
  vulkanMatrixProject.Load();

  vulkanMatrixTexture.Clear();
  vulkanMatrixTexture->LoadIdentity();
  vulkanMatrixTexture.Load();

  return true;
}

bool CVulkanRenderSystem::BeginRender()
{
  if (!m_bRenderCreated)
    return false;

  CVulkanScopedWrite scoped_write(m_surface->SwapChain());
  if (!scoped_write.Success()) [[unlikely]]
  {
    // Return false, and then the caller will make context lost.
    return false;
  }

  const uint32_t image = scoped_write.ImageIndex();

  auto& framebuffer = m_framebuffers[image];
  if (!framebuffer)
  {
    framebuffer = std::make_unique<CVulkanFramebuffer>(m_vkData.vkDevice);
    if (!framebuffer->Create(m_deviceQueue.get(), m_deviceQueue->CommandPool(),
                             m_vkData.vkRenderPass, m_surface.get(), scoped_write.Image()))
        [[unlikely]]
    {
      CLog::Log(LOGERROR, "Vulkan: Failed to create framebuffer ({0}:{1})", __FILENAME__, __LINE__);
      framebuffer.reset();
      return false;
    }
  }

  VkClearValue clearValues[2]{};
  clearValues[0].color = defaultClearColor;
  clearValues[1].depthStencil = {1.0f, 0};

  {
    CVulkanCommandBuffer& command_buffer = *framebuffer->CommandBuffer();

    assert(&command_buffer != nullptr);
    m_currentVkCommandBuffer = command_buffer.GetVulkanCommandBuffer();
    assert(m_currentVkCommandBuffer != VK_NULL_HANDLE);

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VK_CHECK_RESULT(vkBeginCommandBuffer(m_currentVkCommandBuffer, &begin_info), false);
  }

  {
    VkImageLayout old_layout = scoped_write.ImageLayout();
    VkImageLayout layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkImageMemoryBarrier image_memory_barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = nullptr,
        .srcAccessMask = GetAccessMask(old_layout),
        .dstAccessMask = GetAccessMask(layout),
        .oldLayout = old_layout,
        .newLayout = layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = scoped_write.Image(),
        .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
    };
    vkCmdPipelineBarrier(m_currentVkCommandBuffer, GetPipelineStageFlags(m_deviceQueue.get(), old_layout),
                         GetPipelineStageFlags(m_deviceQueue.get(), layout),
                         0 /* dependencyFlags */, 0 /* memoryBarrierCount */,
                         nullptr /* pMemoryBarriers */, 0 /* bufferMemoryBarrierCount */,
                         nullptr /* pBufferMemoryBarriers */, 1, &image_memory_barrier);
  }

  VkRenderPassBeginInfo begin_info = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .pNext = nullptr,
      .renderPass = m_vkData.vkRenderPass,
      .framebuffer = framebuffer->vkFramebuffer(),
      .renderArea = m_surface->SwapChain()->Size(),
      .clearValueCount = 2,
      .pClearValues = clearValues,
  };

  vkCmdBeginRenderPass(m_currentVkCommandBuffer, &begin_info, VK_SUBPASS_CONTENTS_INLINE);

  ////@{
  //{
  //  VkBuffer vkBuffer = m_testShader->VertexBuffer()->buffer;
  //  // Bind the graphics pipeline.
  //  vkCmdBindPipeline(m_currentVkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_testShader->VulkanPipeline());

  //  VkViewport vp{.x = static_cast<float>(m_width / 2), //0.0f,
  //                .y = static_cast<float>(m_height / 2), //0.0f,
  //                .width = static_cast<float>(m_width / 2),
  //                .height = static_cast<float>(m_height / 2),
  //                .minDepth = 0.0f,
  //                .maxDepth = 1.0f};
  //  // Set viewport dynamically
  //  vkCmdSetViewport(m_currentVkCommandBuffer, 0, 1, &vp);

  //  VkRect2D scissor{
  //      .offset = {.x = 0, .y = 0},
  //      .extent = {.width = m_width, .height = m_height},
  //  };
  //  // Set scissor dynamically
  //  vkCmdSetScissor(m_currentVkCommandBuffer, 0, 1, &scissor);

  //  // Bind the vertex buffer to source the draw calls from.
  //  VkDeviceSize offset = {0};
  //  vkCmdBindVertexBuffers(m_currentVkCommandBuffer, 0, 1, &vkBuffer, &offset);

  //  // Draw three vertices with one instance from the currently bound vertex bound.
  //  vkCmdDraw(m_currentVkCommandBuffer, 3, 1, 0, 0);
  //}
  ////@}

  m_indexBuffer = image;
  m_scopedWrite = std::move(scoped_write);

  //RenderTriangle(0.0f, 0.0f);
  //  RenderTriangle(static_cast<float>(m_width / 2), 0.0f);

  //fprintf(stderr, "Vulkan: BeginRender: width=%d, height=%d\n", m_width, m_height);

  return true;
}

bool CVulkanRenderSystem::EndRender()
{
  //RenderTriangle(static_cast<float>(m_width / 2), 0.0f);
  //fprintf(stderr, "Vulkan: EndRender: width=%d, height=%d\n", m_width, m_height);
  if (!m_bRenderCreated || !m_scopedWrite.has_value())
    return false;



  auto& framebuffer = m_framebuffers[m_scopedWrite->ImageIndex()];
  CVulkanCommandBuffer& command_buffer = *framebuffer->CommandBuffer();

  vkCmdEndRenderPass(m_currentVkCommandBuffer);

  VK_CHECK_RESULT(vkEndCommandBuffer(m_currentVkCommandBuffer), false);

  // Transfer image layout back to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR for
  // presenting.
  VkImageLayout old_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  VkImageLayout layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  VkImageMemoryBarrier image_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .pNext = VK_NULL_HANDLE,
      .srcAccessMask = GetAccessMask(old_layout),
      .dstAccessMask = GetAccessMask(layout),
      .oldLayout = old_layout,
      .newLayout = layout,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = m_scopedWrite->Image(),
      .subresourceRange =
          {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
  };
  vkCmdPipelineBarrier(m_currentVkCommandBuffer,
                       GetPipelineStageFlags(m_deviceQueue.get(), old_layout),
                       GetPipelineStageFlags(m_deviceQueue.get(), layout), 0, 0, VK_NULL_HANDLE, 0,
                       VK_NULL_HANDLE, 1, &image_memory_barrier);

  VkSemaphore begin_semaphore = m_scopedWrite->BeginSemaphore();
  VkSemaphore end_semaphore = m_scopedWrite->EndSemaphore();
  if (!command_buffer.Submit(1, &begin_semaphore, 1, &end_semaphore))
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to swap buffer ({0}:{1})", __FILENAME__, __LINE__);
    m_scopedWrite.reset();
    return false;
  }

  m_scopedWrite.reset();

  m_currentVkCommandBuffer = VK_NULL_HANDLE;

  m_surface->SwapBuffers();

  return true;
}

void CVulkanRenderSystem::InvalidateColorBuffer()
{

  if (!m_bRenderCreated)
    return;
}

bool CVulkanRenderSystem::ClearBuffers(KODI::UTILS::COLOR::Color color)
{
  if (!m_bRenderCreated)
    return false;

  return true;
}

bool CVulkanRenderSystem::IsExtSupported(const char* extension) const
{
  return false;
}

void CVulkanRenderSystem::PresentRender(bool rendered, bool videoLayer)
{
  if (!m_bRenderCreated)
    return;
}

void CVulkanRenderSystem::CaptureStateBlock()
{
  if (!m_bRenderCreated)
    return;

  vulkanMatrixProject.Push();
  vulkanMatrixModview.Push();
  vulkanMatrixTexture.Push();
}

void CVulkanRenderSystem::ApplyStateBlock()
{
  if (!m_bRenderCreated)
    return;

  vulkanMatrixProject.PopLoad();
  vulkanMatrixModview.PopLoad();
  vulkanMatrixTexture.PopLoad();
}

void CVulkanRenderSystem::SetCameraPosition(const CPoint& camera,
                                            int screenWidth,
                                            int screenHeight,
                                            float stereoFactor)
{
  if (!m_bRenderCreated)
    return;

  CPoint offset = camera - CPoint(screenWidth * 0.5f, screenHeight * 0.5f);

  float w = (float)m_viewPort.z * 0.5f;
  float h = (float)m_viewPort.w * 0.5f;

  vulkanMatrixModview->LoadIdentity();
  vulkanMatrixModview->Translatef(-(w + offset.x - stereoFactor), +(h + offset.y), 0);
  vulkanMatrixModview->LookAt(0.0f, 0.0f, -2.0f * h, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f);
  vulkanMatrixModview.Load();

  vulkanMatrixProject->LoadIdentity();
  vulkanMatrixProject->Frustum((-w - offset.x) * 0.5f, (w - offset.x) * 0.5f,
                               (-h + offset.y) * 0.5f, (h + offset.y) * 0.5f, h, 100 * h);
  vulkanMatrixProject.Load();
}

void CVulkanRenderSystem::Project(float& x, float& y, float& z)
{
  //float coordX, coordY, coordZ;
  //if (CVulkanMatrix::Project(x, y, z, vulkanMatrixModview.Get(), vulkanMatrixProject.Get(), m_viewPort,
  //                           &coordX,
  //                       &coordY, &coordZ))
  //{
  //  x = coordX;
  //  y = (float)(m_viewPort.y + m_viewPort.w - coordY);
  //  z = 0;
  //}
}

void CVulkanRenderSystem::GetViewPort(CRect& viewPort)
{
  if (!m_bRenderCreated)
    return;

  const VkRect2D& imageSize = m_surface->vkImageSize();

  viewPort.x1 = imageSize.offset.x;
  viewPort.y1 = m_height - imageSize.offset.y - imageSize.extent.height;
  viewPort.x2 = imageSize.offset.x + imageSize.extent.width;
  viewPort.y2 = viewPort.y1 + imageSize.extent.height;
}

void CVulkanRenderSystem::SetViewPort(const CRect& viewPort)
{
  if (!m_bRenderCreated || m_currentVkCommandBuffer == VK_NULL_HANDLE)
    return;

  VkViewport viewport =
      UTILS::vkViewport(viewPort.x2 - viewPort.x1, viewPort.y2 - viewPort.y1, 0.0f, 1.0f);

  vkCmdSetViewport(m_currentVkCommandBuffer, 0, 1, &viewport);
}

bool CVulkanRenderSystem::ScissorsCanEffectClipping()
{
  return false;
}

CRect CVulkanRenderSystem::ClipRectToScissorRect(const CRect& rect)
{

  //if (!m_pShader[m_method])
  return CRect();
}

void CVulkanRenderSystem::SetScissors(const CRect& rect)
{
  if (!m_bRenderCreated || m_currentVkCommandBuffer == VK_NULL_HANDLE)
    return;

  VkRect2D scissor;
  scissor.offset.x = MathUtils::round_int(static_cast<double>(rect.x1));
  scissor.offset.y = MathUtils::round_int(static_cast<double>(rect.y1));
  scissor.extent.width = MathUtils::round_int(static_cast<double>(rect.x2 - rect.x1));
  scissor.extent.height = MathUtils::round_int(static_cast<double>(rect.y2 - rect.y1));

  vkCmdSetScissor(m_currentVkCommandBuffer, 0, 1, &scissor);
}

void CVulkanRenderSystem::ResetScissors()
{
  SetScissors(CRect(0, 0, static_cast<float>(m_width), static_cast<float>(m_height)));
}

void CVulkanRenderSystem::SetDepthCulling(DepthCulling culling)
{
  fprintf(stderr, "---> %s", __PRETTY_FUNCTION__);
}

bool CVulkanRenderSystem::SupportsStereo(RenderStereoMode mode) const
{
  fprintf(stderr, "---> %s", __PRETTY_FUNCTION__);
  return CRenderSystemBase::SupportsStereo(mode);
}

std::string CVulkanRenderSystem::GetShaderPath(const std::string& filename)
{
  fprintf(stderr, "---> %s", __PRETTY_FUNCTION__);
  return "Vulkan/";
}

void CVulkanRenderSystem::InitialiseShaders()
{
}

void CVulkanRenderSystem::ReleaseShaders()
{
}

void CVulkanRenderSystem::EnableShader(ShaderId method)
{
}

void CVulkanRenderSystem::DisableShader()
{
}

bool CVulkanRenderSystem::CreatePipeline()
{
  VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
  pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
  VK_CHECK_RESULT(vkCreatePipelineCache(m_vkData.vkDevice, &pipelineCacheCreateInfo, nullptr,
                                        &m_vkData.vkPipelineCache),
                  false);
  return true;
}

void CVulkanRenderSystem::DestroyPipeline()
{
  if (m_vkData.vkPipelineCache != VK_NULL_HANDLE)
  {
    vkDestroyPipelineCache(m_vkData.vkDevice, m_vkData.vkPipelineCache, nullptr);
    m_vkData.vkPipelineCache = VK_NULL_HANDLE;
  }
}

void CVulkanRenderSystem::RenderTriangle(float x, float y)
{
  // Update the uniform m_buffer for the next frame
  ShaderData shaderData{};
  //const float* projMatrix = vulkanMatrixProject.Get();
  //const float* modelMatrix = vulkanMatrixModview.Get();

  //shaderData.projectionMatrix = glm::mat4(glm::mat4(glm::vec4(projMatrix[0], projMatrix[1], projMatrix[2], projMatrix[3]),
  //                                                   glm::vec4(projMatrix[4], projMatrix[5], projMatrix[6], projMatrix[7]),
  //                                                   glm::vec4(projMatrix[8], projMatrix[9], projMatrix[10], projMatrix[11]),
  //                                                   glm::vec4(projMatrix[12], projMatrix[13], projMatrix[14], projMatrix[15]));
  //shaderData.modelMatrix = glm::mat4(glm::mat4(glm::vec4(modelMatrix[0], modelMatrix[1], modelMatrix[2], modelMatrix[3]),
  //                                             glm::vec4(modelMatrix[4], modelMatrix[5], modelMatrix[6], modelMatrix[7]),
  //                                             glm::vec4(modelMatrix[8], modelMatrix[9], modelMatrix[10], modelMatrix[11]),
  //                                             glm::vec4(modelMatrix[12], modelMatrix[13], modelMatrix[14], modelMatrix[15])));

  //shaderData.projectionMatrix = m_camera.matrices.perspective;
  //shaderData.viewMatrix = m_camera.matrices.view;
  //shaderData.modelMatrix = glm::mat4(1.0f);
  shaderData.projectionMatrix = m_camera.matrices.perspective;
  shaderData.viewMatrix = m_camera.matrices.view;
  shaderData.modelMatrix = glm::mat4(1.0f);

  m_testShaderTexture->UpdateUniformBuffer(m_indexBuffer, shaderData);

  VkViewport viewport{.x = x,
                      .y = y,
                      .width = static_cast<float>(m_width / 2),
                      .height = static_cast<float>(m_height / 2),
                      .minDepth = 0.0f,
                      .maxDepth = 1.0f};
  vkCmdSetViewport(m_currentVkCommandBuffer, 0, 1, &viewport);
  // Update dynamic scissor state
  VkRect2D scissor{
      .offset = {.x = 0, .y = 0},
      .extent = {.width = m_width, .height = m_height},
  };
  vkCmdSetScissor(m_currentVkCommandBuffer, 0, 1, &scissor);
  // Bind m_descriptor set for the current frame's uniform m_buffer, so the shader uses the data from that m_buffer for this draw
  vkCmdBindDescriptorSets(
      m_currentVkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1,
      &m_testShaderTexture->GetUniformBuffer(m_indexBuffer)->descriptorSet, 0, nullptr);
  // Bind the rendering m_pipeline
  // The m_pipeline (state object) contains all states of the rendering m_pipeline, binding it will set all the states specified at m_pipeline creation time
  vkCmdBindPipeline(m_currentVkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
  // Bind triangle vertex m_buffer (contains position and colors)
  VkDeviceSize offsets[1]{0};
  vkCmdBindVertexBuffers(m_currentVkCommandBuffer, 0, 1,
                         &m_testShaderTexture->GetVertexBuffer()->buffer, offsets);
  // Bind triangle index m_buffer
  vkCmdBindIndexBuffer(m_currentVkCommandBuffer, m_testShaderTexture->GetIndexBuffer()->buffer, 0,
                       VK_INDEX_TYPE_UINT32);
  // Draw indexed triangle
  vkCmdDrawIndexed(m_currentVkCommandBuffer, m_testShaderTexture->GetIndexBuffer()->count, 1, 0, 0,
                   0);
}

} // namespace KODI::RENDERING::VULKAN
