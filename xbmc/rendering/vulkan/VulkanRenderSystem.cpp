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

#include <glm/gtc/type_ptr.hpp>

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

  m_vkData.vkInstance = m_deviceQueue->vkInstance();
  m_vkData.vkDevice = m_deviceQueue->vkDevice();
  m_vkData.vkPhysicalDevice = m_deviceQueue->vkPhysicalDevice();
  m_vkData.vkQueue = m_deviceQueue->vkQueue();
  m_vkData.vkCommandPool = m_deviceQueue->CommandPool()->vkCommandPool();

  m_surface = std::make_unique<CVulkanSurface>(m_vkData.vkInstance, m_vkData.vkSurface);
  if (!m_surface->Initialize(m_deviceQueue.get(), SurfaceFormat::FORMAT_RGBA_32))
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to initialize surface ({0}:{1})", __FILENAME__, __LINE__);
    return false;
  }

  m_vkData.vkSwapchain = m_surface->vkSwapchain();
  m_vkData.vkSwapchainFormat = m_surface->vkSurfaceFormat().format;

  m_renderPass = CVulkanRenderPass::Create(m_vkData.vkSwapchainFormat, m_vkData.vkDevice);
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

  /*!
   * Make reset here as the width/height is already set from child class and the compare on
   * @ref CWinSystemWaylandVulkan::SetContextSize fails as already set to the same value, so
   * the reset is not done there.
   *
   * The earlier set is needed as Vulkan needs to know the size of the surface before creating the swapchain,
   * so it can be set in the child class before calling this function.
   */
  ResetRenderSystem(m_width, m_height);
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
  m_vkData.vkInstance = VK_NULL_HANDLE;
  m_vkData.vkDevice = VK_NULL_HANDLE;
  m_vkData.vkSwapchain = VK_NULL_HANDLE;
  m_vkData.vkRenderPass = VK_NULL_HANDLE;
  m_vkData.vkSwapchainFormat = VK_FORMAT_UNDEFINED;

  m_bRenderCreated = false;

  return true;
}

bool CVulkanRenderSystem::ResetRenderSystem(int width, int height)
{
  fprintf(stderr,
          "--------------------------------Vulkan: ResetRenderSystem: width=%d, height=%d\n", width,
          height);
  if (!m_bRenderCreated)
    return false;

  //if (static_cast<uint32_t>(width) == m_width && static_cast<uint32_t>(height) == m_height)
  //  return true;

  CRect rect(0, 0, width, height);
  SetViewPort(rect);

  fprintf(stderr,
          "----------------------------------222Vulkan: ResetRenderSystem: width=%d, height=%d\n",
          width, height);
  //vkCmdBeginRenderPass(m_currentVkCommandBuffer, &begin_info, VK_SUBPASS_CONTENTS_INLINE);

  vulkanMatrixModview.Clear();
  vulkanMatrixModview->LoadIdentity();
  //vulkanMatrixModview->Ortho(0.0f, width - 1, height - 1, 0.0f, -1.0f, 1.0f);
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
    vkCmdPipelineBarrier(
        m_currentVkCommandBuffer, GetPipelineStageFlags(m_deviceQueue.get(), old_layout),
        GetPipelineStageFlags(m_deviceQueue.get(), layout), 0 /* dependencyFlags */,
        0 /* memoryBarrierCount */, nullptr /* pMemoryBarriers */, 0 /* bufferMemoryBarrierCount */,
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

  //vkCmdSetScissor(m_currentVkCommandBuffer, 0, 1, &m_vkScissor);
  //vkCmdSetViewport(m_currentVkCommandBuffer, 0, 1, &m_vkViewport);

  m_indexBuffer = image;
  m_scopedWrite = std::move(scoped_write);

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

  float w = (float)m_vkViewport.width * 0.5f;
  float h = (float)m_vkViewport.height * 0.5f;

  //fprintf(stderr,
  //        "------------ Vulkan: SetCameraPosition: screenWidth=%d, screenHeight=%d, w=%f, h=%f, offset.x=%d, offset.y=%d, "
  //        "stereoFactor=%f\n",
  //        screenWidth, screenHeight,
  //        double(w), double(h), uint32_t(offset.x), uint32_t(offset.y), double(stereoFactor));

  //vulkanMatrixModview->LoadIdentity();
  //vulkanMatrixModview->Translatef(-(w + offset.x - stereoFactor), +(h + offset.y), 0);
  //vulkanMatrixModview->LookAt(0.0f, 0.0f, -2.0f * h, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f);
  //vulkanMatrixModview.Load();

  //vulkanMatrixProject->LoadIdentity();
  //vulkanMatrixProject->Frustum((-w - offset.x) * 0.5f, (w - offset.x) * 0.5f,
  //                             (-h + offset.y) * 0.5f, (h + offset.y) * 0.5f, h, 100 * h);
  //vulkanMatrixProject.Load();
}

void CVulkanRenderSystem::Project(float& x, float& y, float& z)
{
  //fprintf(stderr, "Vulkan: 1 Project: x=%f, y=%f, z=%f %f %f %f %f\n", double(x), double(y),
  //        double(z), double(m_vkViewport.x), double(m_vkViewport.y), double(m_vkViewport.width), double(m_vkViewport.height));
  //float coordX, coordY, coordZ;
  //if (CVulkanMatrix::Project(x, y, z, vulkanMatrixModview.Get(), vulkanMatrixProject.Get(),
  //                           &m_vkViewport, &coordX, &coordY, &coordZ))
  //{
  //  x = coordX;
  //  y = (float)(m_vkViewport.y + m_vkViewport.height - coordY);
  //  z = 0;
  //}
  //fprintf(stderr, "Vulkan: 2 Project: x=%f, y=%f, z=%f\n", double(x), double(y), double(z));
  //char* a = nullptr;
  //a[0] = 0;
}

void CVulkanRenderSystem::GetViewPort(CRect& viewPort)
{
  if (!m_bRenderCreated)
    return;

  viewPort.x1 = m_vkViewport.x;
  viewPort.y1 = m_height - m_vkViewport.y - m_vkViewport.height;
  viewPort.x2 = m_vkViewport.x + m_vkViewport.width;
  viewPort.y2 = viewPort.y1 + m_vkViewport.height;

  //const VkRect2D& imageSize = m_surface->vkImageSize();

  //viewPort.x1 = imageSize.offset.x;
  //viewPort.y1 = m_height - imageSize.offset.y - imageSize.extent.height;
  //viewPort.x2 = imageSize.offset.x + imageSize.extent.width;
  //viewPort.y2 = viewPort.y1 + imageSize.extent.height;
}

void CVulkanRenderSystem::SetViewPort(const CRect& viewPort)
{
  if (!m_bRenderCreated)
    return;

  m_vkViewport.x = viewPort.x1;
  m_vkViewport.y = m_height - viewPort.y1 - viewPort.Height();
  m_vkViewport.width = viewPort.Width();
  m_vkViewport.height = viewPort.Height();
  m_vkViewport.minDepth = 0.0f;
  m_vkViewport.maxDepth = 1.0f;

  m_vkScissor.offset.x = MathUtils::round_int(static_cast<double>(m_vkViewport.x));
  m_vkScissor.offset.y = MathUtils::round_int(static_cast<double>(m_vkViewport.y));
  m_vkScissor.extent.width = MathUtils::round_int(static_cast<double>(m_vkViewport.width));
  m_vkScissor.extent.height = MathUtils::round_int(static_cast<double>(m_vkViewport.height));
}

bool CVulkanRenderSystem::ScissorsCanEffectClipping()
{
  return false;
}

CRect CVulkanRenderSystem::ClipRectToScissorRect(const CRect& rect)
{
  return rect;
  //const float* projMatrix = vulkanMatrixProject.Get();
  //const float* modelMatrix = vulkanMatrixModview.Get();

  //const TransformMatrix& guiMatrix = CServiceBroker::GetWinSystem()->GetGfxContext().GetGUIMatrix();
  //CRect viewPort; // absolute positions of corners
  //CServiceBroker::GetRenderSystem()->GetViewPort(viewPort);

  //bool clipPossible =
  //    guiMatrix.m[0][1] == 0 && guiMatrix.m[1][0] == 0 && guiMatrix.m[2][0] == 0 &&
  //    guiMatrix.m[2][1] == 0 && modelMatrix[0 + 1 * 4] == 0 && modelMatrix[0 + 2 * 4] == 0 &&
  //    modelMatrix[1 + 0 * 4] == 0 && modelMatrix[1 + 2 * 4] == 0 && modelMatrix[2 + 0 * 4] == 0 &&
  //    modelMatrix[2 + 1 * 4] == 0 && projMatrix[0 + 1 * 4] == 0 && projMatrix[0 + 2 * 4] == 0 &&
  //    projMatrix[0 + 3 * 4] == 0 && projMatrix[1 + 0 * 4] == 0 && projMatrix[1 + 2 * 4] == 0 &&
  //    projMatrix[1 + 3 * 4] == 0 && projMatrix[3 + 0 * 4] == 0 && projMatrix[3 + 1 * 4] == 0 &&
  //    projMatrix[3 + 3 * 4] == 0;

  //float xFactor = 0.0;
  //float xOffset = 0.0;
  //float yFactor = 0.0;
  //float yOffset = 0.0;

  //if (clipPossible)
  //{
  //  xFactor = guiMatrix.m[0][0] * modelMatrix[0 + 0 * 4] * projMatrix[0 + 0 * 4];
  //  xOffset = (guiMatrix.m[0][3] * modelMatrix[0 + 0 * 4] + modelMatrix[0 + 3 * 4]) *
  //                  projMatrix[0 + 0 * 4];
  //  yFactor = guiMatrix.m[1][1] * modelMatrix[1 + 1 * 4] * projMatrix[1 + 1 * 4];
  //  yOffset = (guiMatrix.m[1][3] * modelMatrix[1 + 1 * 4] + modelMatrix[1 + 3 * 4]) *
  //                  projMatrix[1 + 1 * 4];
  //  float clipW = (guiMatrix.m[2][3] * modelMatrix[2 + 2 * 4] + modelMatrix[2 + 3 * 4]) *
  //                projMatrix[3 + 2 * 4];
  //  float xMult = (viewPort.x2 - viewPort.x1) / (2 * clipW);
  //  float yMult =
  //      (viewPort.y1 - viewPort.y2) / (2 * clipW); // correct for inverted window coordinate scheme
  //  xFactor = xFactor * xMult;
  //  xOffset = xOffset * xMult + (viewPort.x2 + viewPort.x1) / 2;
  //  yFactor = yFactor * yMult;
  //  yOffset = yOffset * yMult + (viewPort.y2 + viewPort.y1) / 2;
  //}

  //return CRect(rect.x1 * xFactor + xOffset, rect.y1 * yFactor + yOffset,
  //             rect.x2 * xFactor + xOffset, rect.y2 * yFactor + yOffset);
}

void CVulkanRenderSystem::SetScissors(const CRect& rect)
{
  if (!m_bRenderCreated || m_currentVkCommandBuffer == VK_NULL_HANDLE)
    return;

  m_vkScissor.offset.x = MathUtils::round_int(static_cast<double>(rect.x1));
  m_vkScissor.offset.y = MathUtils::round_int(static_cast<double>(m_height - rect.y1));
  m_vkScissor.extent.width = MathUtils::round_int(static_cast<double>(rect.x2 - rect.x1));
  m_vkScissor.extent.height = MathUtils::round_int(static_cast<double>(rect.y2 - rect.y1));
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

} // namespace KODI::RENDERING::VULKAN
