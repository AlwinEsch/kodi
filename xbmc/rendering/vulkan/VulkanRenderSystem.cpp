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

VkClearColorValue defaultClearColor = {{0.025f, 0.025f, 0.025f, 1.0f}};

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

  m_vkSurface = GetVulkanSurface();
  if (!m_vkSurface)
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
  m_vkDevice = m_deviceQueue->vkDevice();
  m_vkPhysicalDevice = m_deviceQueue->vkPhysicalDevice();

  m_surface = std::make_unique<CVulkanSurface>(m_vkInstance, m_vkSurface);
  if (!m_surface->Initialize(m_deviceQueue.get(), SurfaceFormat::FORMAT_RGBA_32))
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to initialize surface ({0}:{1})", __FILENAME__, __LINE__);
    return false;
  }

  m_vkSwapchainFormat = m_surface->vkSurfaceFormat().format;

  m_renderPass = CVulkanRenderPass::Create(m_vkSwapchainFormat, m_vkDevice);
  if (!m_renderPass)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create render pass ({0}:{1})", __FILENAME__, __LINE__);
    return false;
  }

  m_surface->Reshape({{0, 0}, {m_width, m_height}}, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);

  m_vkRenderPass = m_renderPass->vkRenderPass();
  m_vkSwapchain = m_surface->vkSwapchain();
  m_commandPool = m_deviceQueue->CreateCommandPool();
  if (!m_commandPool)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create command pool ({0}:{1})", __FILENAME__, __LINE__);
    return false;
  }
  m_vkCommandPool = m_commandPool->vkCommandPool();

  m_vkPipelineLayout = CreatePipelineLayout();
  if (m_vkPipelineLayout == VK_NULL_HANDLE)
  {
    // Log error already logged in CreatePipelineLayout()
    return false;
  }

  m_shaderControl = std::make_unique<CVulkanShaderControl>();
  if (!m_shaderControl->CreateAllShaders(m_vkDevice, m_vkPipelineLayout, m_vkRenderPass))
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

  m_vkPipeline = m_shaderControl->GetPipeline(VULKAN_SM_TEST);
  init_vertex_buffer();

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

  if (m_vertex_buffer != VK_NULL_HANDLE)
  {
    vkDestroyBuffer(m_vkDevice, m_vertex_buffer, nullptr);
    m_vertex_buffer = VK_NULL_HANDLE;
  }
  if (m_vertex_buffer_memory != VK_NULL_HANDLE)
  {
    vkFreeMemory(m_vkDevice, m_vertex_buffer_memory, nullptr);
    m_vertex_buffer_memory = VK_NULL_HANDLE;
  }

  for (auto& framebuffer : m_framebuffers)
  {
    if (!framebuffer)
      continue;

    framebuffer->Destroy();
    framebuffer.reset();
  }
  m_framebuffers.clear();

  if (m_commandPool)
  {
    m_commandPool->Destroy();
    m_commandPool.reset();
  }

  if (m_vkPipelineLayout != VK_NULL_HANDLE)
  {
    vkDestroyPipelineLayout(m_vkDevice, m_vkPipelineLayout, nullptr);
  }

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

  m_vkSurface = VK_NULL_HANDLE;
  m_vkSurfaceFormat = {};
  m_vkInstance = VK_NULL_HANDLE;
  m_vkDevice = VK_NULL_HANDLE;
  m_vkPipeline = VK_NULL_HANDLE;
  m_vkPipelineLayout = VK_NULL_HANDLE;
  m_vkSwapchain = VK_NULL_HANDLE;
  m_vkRenderPass = VK_NULL_HANDLE;
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

  m_width = static_cast<uint32_t>(width);
  m_height = static_cast<uint32_t>(height);

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
    framebuffer = std::make_unique<CVulkanFramebuffer>(m_vkDevice);
    if (!framebuffer->Create(m_deviceQueue.get(), m_commandPool.get(), m_vkRenderPass,
                             m_surface.get(), scoped_write.Image())) [[unlikely]]
    {
      CLog::Log(LOGERROR, "Vulkan: Failed to create framebuffer ({0}:{1})", __FILENAME__, __LINE__);
      framebuffer.reset();
      return false;
    }
  }

  VkClearValue clearValues[2]{};
  clearValues[0].color = defaultClearColor;
  clearValues[1].depthStencil = {1.0f, 0};

  CVulkanCommandBuffer& command_buffer = *framebuffer->CommandBuffer();

  CVulkanCommandBufferScoped recorder(command_buffer);
  VkCommandBuffer cmd = recorder.GetVulkanCommandBuffer();
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
    vkCmdPipelineBarrier(cmd, GetPipelineStageFlags(m_deviceQueue.get(), old_layout),
                         GetPipelineStageFlags(m_deviceQueue.get(), layout),
                         0 /* dependencyFlags */, 0 /* memoryBarrierCount */,
                         nullptr /* pMemoryBarriers */, 0 /* bufferMemoryBarrierCount */,
                         nullptr /* pBufferMemoryBarriers */, 1, &image_memory_barrier);
  }

  VkRenderPassBeginInfo begin_info = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .pNext = nullptr,
      .renderPass = m_vkRenderPass,
      .framebuffer = framebuffer->vkFramebuffer(),
      .renderArea = m_surface->SwapChain()->Size(),
      .clearValueCount = 2,
      .pClearValues = clearValues,
  };

  vkCmdBeginRenderPass(cmd, &begin_info, VK_SUBPASS_CONTENTS_INLINE);

  //@{
  {
    // Bind the graphics pipeline.
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vkPipeline);

    VkViewport vp{.x = 0.0f,
                  .y = 0.0f,
                  .width = static_cast<float>(m_width),
                  .height = static_cast<float>(m_height),
                  .minDepth = 0.0f,
                  .maxDepth = 1.0f};
    // Set viewport dynamically
    vkCmdSetViewport(cmd, 0, 1, &vp);

    VkRect2D scissor{
        .offset = {.x = 0, .y = 0},
        .extent = {.width = m_width, .height = m_height},
    };
    // Set scissor dynamically
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    // Bind the vertex buffer to source the draw calls from.
    VkDeviceSize offset = {0};
    vkCmdBindVertexBuffers(cmd, 0, 1, &m_vertex_buffer, &offset);

    // Draw three vertices with one instance from the currently bound vertex bound.
    vkCmdDraw(cmd, 3, 1, 0, 0);
  }
  //@}

  m_currentVkCommandBuffer = cmd;
  m_scopedWrite = std::move(scoped_write);

  return true;
}

bool CVulkanRenderSystem::EndRender()
{
  if (!m_bRenderCreated || !m_scopedWrite.has_value())
    return false;

  auto& framebuffer = m_framebuffers[m_scopedWrite->ImageIndex()];
  CVulkanCommandBuffer& command_buffer = *framebuffer->CommandBuffer();

  vkCmdEndRenderPass(m_currentVkCommandBuffer);

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
}

void CVulkanRenderSystem::ApplyStateBlock()
{

  if (!m_bRenderCreated)
    return;
}

void CVulkanRenderSystem::SetCameraPosition(const CPoint& camera,
                                            int screenWidth,
                                            int screenHeight,
                                            float stereoFactor)
{

  if (!m_bRenderCreated)
    return;
}

void CVulkanRenderSystem::Project(float& x, float& y, float& z)
{
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

VkPipelineLayout CVulkanRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout layout)
{
  // Implementation of Create
  // Create a blank pipeline layout.
  // We are not binding any resources to the pipeline in this first sample.
  VkPipelineLayoutCreateInfo layout_info{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pNext = VK_NULL_HANDLE,
      .flags = 0,
      .setLayoutCount = layout != VK_NULL_HANDLE ? 1u : 0u,
      .pSetLayouts = &layout,
      .pushConstantRangeCount = 0,
      .pPushConstantRanges = VK_NULL_HANDLE,
  };

  VkPipelineLayout pipeline_layout;
  VK_CHECK_RESULT(vkCreatePipelineLayout(m_vkDevice, &layout_info, nullptr, &pipeline_layout),
                  VK_NULL_HANDLE);

  return pipeline_layout;
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

void CVulkanRenderSystem::init_vertex_buffer()
{
  // Vertex data for a single colored triangle
  struct Vertex
  {
    glm::vec3 position;
    glm::vec3 color;
  };
  const std::vector<Vertex> vertices = {{{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
                                        {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

  const VkDeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();

  // Copy Vertex data to a buffer accessible by the device
  VkBufferCreateInfo buffer_info =
      UTILS::vkBufferCreateInfo(buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

  VK_CHECK_RESULT(vkCreateBuffer(m_vkDevice, &buffer_info, nullptr, &m_vertex_buffer));

  // Get memory requirements
  VkMemoryRequirements memory_requirements;
  vkGetBufferMemoryRequirements(m_vkDevice, m_vertex_buffer, &memory_requirements);

  // Allocate memory for the buffer
  VkMemoryAllocateInfo alloc_info{
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .pNext = VK_NULL_HANDLE,
      .allocationSize = memory_requirements.size,
      .memoryTypeIndex = m_deviceQueue->GetMemoryType(memory_requirements.memoryTypeBits,
                                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
  };
  VK_CHECK_RESULT(vkAllocateMemory(m_vkDevice, &alloc_info, nullptr, &m_vertex_buffer_memory));
  // Bind the buffer with the allocated memory
  VK_CHECK_RESULT(vkBindBufferMemory(m_vkDevice, m_vertex_buffer, m_vertex_buffer_memory, 0));

  // Map the memory and copy the vertex data
  void* data;
  VK_CHECK_RESULT(vkMapMemory(m_vkDevice, m_vertex_buffer_memory, 0, buffer_size, 0, &data));

  memcpy(data, vertices.data(), static_cast<size_t>(buffer_size));
  vkUnmapMemory(m_vkDevice, m_vertex_buffer_memory);
}

} // namespace KODI::RENDERING::VULKAN
