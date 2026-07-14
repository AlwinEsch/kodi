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
#include "rendering/vulkan/VulkanUtils.h"
#include "rendering/vulkan/shaders/VulkanShaderControl.h"
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

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

namespace
{

const int kAnimationSteps = 240;
static int iteration_ = 0;

float CurrentFraction()
{
  float fraction = (sinf(iteration_ * 2 * std::numbers::pi_v<float> / kAnimationSteps) + 1) / 2;
  return fraction;
}

float NextFraction()
{
  float fraction = CurrentFraction();
  iteration_++;
  iteration_ %= kAnimationSteps;
  return fraction;
}

} // namespace

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
    CLog::Log(LOGERROR, "Vulkan: Failed to initialize device queue");
    return nullptr;
  }

  return deviceQueue;
}

CVulkanRenderSystem::CVulkanRenderSystem() : CRenderSystemBase()
{
}

CVulkanRenderSystem::~CVulkanRenderSystem()
{

  //Destroy();
}

bool CVulkanRenderSystem::InitRenderSystem()
{

  m_vkSurface = GetVulkanSurface();
  if (!m_vkSurface)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create window surface.");
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

  m_vkInstance = m_deviceQueue->VulkanInstance();
  m_vkDevice = m_deviceQueue->VulkanDevice();

  m_surface = std::make_unique<CVulkanSurface>(m_vkInstance, m_vkSurface);
  if (!m_surface->Initialize(m_deviceQueue.get(), SurfaceFormat::FORMAT_RGBA_32))
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to initialize surface");
    return false;
  }

  m_vkSwapchainFormat = m_surface->vkSurfaceFormat().format;

  m_renderPass = CVulkanRenderPass::Create(m_vkSwapchainFormat, m_vkDevice);
  if (!m_renderPass)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create render pass");
    return false;
  }

  m_surface->Reshape({{0, 0}, {m_width, m_height}}, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);

  m_vkRenderPass = m_renderPass->vkRenderPass();
  m_vkSwapchain = m_surface->vkSwapchain();

  CVulkanSwapChain* vulkan_swap_chain = m_surface->SwapChain();
  const uint32_t num_images = vulkan_swap_chain->AmmountSwapChainImages();
  m_framebuffers.resize(num_images);
  m_commandPool.resize(num_images);

  for (uint32_t i = 0; i < num_images; ++i)
  {
    m_commandPool[i] = m_deviceQueue->CreateCommandPool();
  }

  init_vertex_buffer();

  // Create the necessary objects for rendering.
  //init_render_pass();

  m_vkPipelineLayout = CreatePipelineLayout();
  if (m_vkPipelineLayout == VK_NULL_HANDLE)
  {
    // Log error already logged in CreatePipelineLayout()
    return false;
  }

  m_shaderControl = std::make_unique<CVulkanShaderControl>();
  if (!m_shaderControl->CreateAllShaders(m_vkDevice, m_vkPipelineLayout, m_vkRenderPass))
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to initialize shader control");
    return false;
  }

  m_vkPipeline = m_shaderControl->GetPipeline(VULKAN_TEST_SHADER);

  m_bRenderCreated = true;

  CVulkanGUITexture::Register();

  return true;
}

bool CVulkanRenderSystem::DestroyRenderSystem()
{
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
  //////TEST___Deinit();

  //////m_deviceQueue.reset();

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

  return true;
}

bool CVulkanRenderSystem::EndRender()
{

  if (!m_bRenderCreated)
    return false;

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

  VkClearValue clear_value = {
      .color =
          {
              .float32 = {.5f, 1.f - NextFraction(), .5f, 1.f},
          },
  };

  CVulkanSwapChain* vulkan_swap_chain = m_surface->SwapChain();
  {
    CVulkanScopedWrite scoped_write(vulkan_swap_chain);
    const uint32_t image = scoped_write.ImageIndex();

    auto& framebuffer = m_framebuffers[image];
    if (!framebuffer)
    {
      framebuffer =
          CVulkanFramebuffer::Create(m_deviceQueue.get(), m_commandPool[image].get(),
                                     m_vkRenderPass, m_surface.get(), scoped_write.Image());
      if (!framebuffer)
      {
        CLog::Log(LOGERROR, "Vulkan: Failed to create framebuffer");
        return;
      }
    }

    CVulkanCommandBuffer& command_buffer = *framebuffer->CommandBuffer();
    {
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
        vkCmdPipelineBarrier(cmd,
                             GetPipelineStageFlags(m_deviceQueue.get(), old_layout),
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
          .renderArea = vulkan_swap_chain->Size(),
          .clearValueCount = 1,
          .pClearValues = &clear_value,
      };

      vkCmdBeginRenderPass(cmd, &begin_info,
                           VK_SUBPASS_CONTENTS_INLINE);

      //@{
      {
        // Bind the graphics pipeline.
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vkPipeline);

        VkViewport vp{.x = 500.0f,
                      .y = 500.0f,
                      .width = static_cast<float>(m_width-500),
                      .height = static_cast<float>(m_height-500),
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

      vkCmdEndRenderPass(cmd);

      // Transfer image layout back to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR for
      // presenting.
      {
        VkImageLayout old_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        VkImageLayout layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
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
        vkCmdPipelineBarrier(cmd,
                             GetPipelineStageFlags(m_deviceQueue.get(), old_layout),
                             GetPipelineStageFlags(m_deviceQueue.get(), layout),
                             0 /* dependencyFlags */, 0 /* memoryBarrierCount */,
                             nullptr /* pMemoryBarriers */, 0 /* bufferMemoryBarrierCount */,
                             nullptr /* pBufferMemoryBarriers */, 1, &image_memory_barrier);
      }
    }

    VkSemaphore begin_semaphore = scoped_write.BeginSemaphore();
    VkSemaphore end_semaphore = scoped_write.EndSemaphore();
    if (!command_buffer.Submit(1, &begin_semaphore, 1, &end_semaphore))
    {
      CLog::Log(LOGERROR, "Vulkan: Failed to swap buffer");
      return;
    }
  }
  m_surface->SwapBuffers();
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

  //////const VkRect2D& imageSize = m_surface->GetImageSize();

  //////viewPort.x1 = imageSize.offset.x;
  //////viewPort.y1 = m_height - imageSize.offset.y - imageSize.extent.height;
  //////viewPort.x2 = imageSize.offset.x + imageSize.extent.width;
  //////viewPort.y2 = viewPort.y1 + imageSize.extent.height;
}

void CVulkanRenderSystem::SetViewPort(const CRect& viewPort)
{

  if (!m_bRenderCreated)
    return;

  VkViewport viewport{};
  viewport.x = viewPort.x1;
  viewport.y = viewPort.y1;
  viewport.width = viewPort.x2 - viewPort.x1;
  viewport.height = viewPort.y2 - viewPort.y1;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  ////// TODO: Is this correct? Should we set the viewport for all command buffers or just the current one?
  ////for (const auto& per_frame : TEST___per_frame)
  ////  vkCmdSetViewport(per_frame.primary_command_buffer->GetVulkanCommandBuffer(), 0, 1, &viewport);
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

  if (!m_bRenderCreated)
    return;

  VkRect2D scissor{};
  scissor.offset.x = MathUtils::round_int(static_cast<double>(rect.x1));
  scissor.offset.y = MathUtils::round_int(static_cast<double>(rect.y1));
  scissor.extent.width = MathUtils::round_int(static_cast<double>(rect.x2 - rect.x1));
  scissor.extent.height = MathUtils::round_int(static_cast<double>(rect.y2 - rect.y1));

  //////// TODO: Is this correct? Should we set the viewport for all command buffers or just the current one?
  //////for (const auto& per_frame : TEST___per_frame)
  //////  vkCmdSetScissor(per_frame.primary_command_buffer->GetVulkanCommandBuffer(), 0, 1, &scissor);
}

void CVulkanRenderSystem::ResetScissors()
{
  SetScissors(CRect(0, 0, static_cast<float>(m_width), static_cast<float>(m_height)));
}

void CVulkanRenderSystem::SetDepthCulling(DepthCulling culling)
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
}

bool CVulkanRenderSystem::SupportsStereo(RenderStereoMode mode) const
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  return CRenderSystemBase::SupportsStereo(mode);
}

std::string CVulkanRenderSystem::GetShaderPath(const std::string& filename)
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  return "Vulkan/";
}

bool CVulkanRenderSystem::CreateFramebuffers()
{
  return true;
}

void CVulkanRenderSystem::DestroyFramebuffers()
{
  VkResult result = vkQueueWaitIdle(m_deviceQueue->VulkanQueue());
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR,
              "Vulkan: Failed to wait for device idle before destroying framebuffers. ERROR {0}",
              ErrorString(result));
  }

  for (std::unique_ptr<CVulkanFramebuffer>& framebuffer : m_framebuffers)
  {
    if (!framebuffer)
      continue;

    framebuffer->CommandBuffer()->Destroy();
    vkDestroyFramebuffer(m_vkDevice, framebuffer->vkFramebuffer(), nullptr);
    vkDestroyImageView(m_vkDevice, framebuffer->vkImageView(), nullptr);
    framebuffer.reset();
  }
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
  VkResult result = vkCreatePipelineLayout(m_vkDevice, &layout_info, nullptr, &pipeline_layout);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create pipeline layout, ERROR: {0}",
              ErrorString(result));
    return VK_NULL_HANDLE;
  }

  return pipeline_layout;
}

void CVulkanRenderSystem::InitialiseShaders()
{
}

void CVulkanRenderSystem::ReleaseShaders()
{
}

void CVulkanRenderSystem::EnableShader(ShaderMethodVulkan method)
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
  VkBufferCreateInfo buffer_info{
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .size = buffer_size,
      .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 0,
      .pQueueFamilyIndices = nullptr,
  };

  // We use the Vulkan Memory Allocator to find a memory type that can be written and mapped from the host
  // On most setups this will return a memory type that resides in VRAM and is accessible from the host
  VmaAllocationCreateInfo buffer_alloc_ci{
      .flags =
          VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
      .usage = VMA_MEMORY_USAGE_AUTO,
      .requiredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      .preferredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
      .memoryTypeBits = 0,
      .pool = nullptr,
      .pUserData = nullptr,
      .priority = 0.0f,
  };

  VmaAllocationInfo buffer_alloc_info{};
  vmaCreateBuffer(m_deviceQueue->VMAAllocator(), &buffer_info, &buffer_alloc_ci, &m_vertex_buffer,
                  &m_vertex_buffer_allocation, &buffer_alloc_info);
  if (buffer_alloc_info.pMappedData)
  {
    memcpy(buffer_alloc_info.pMappedData, vertices.data(), buffer_size);
  }
  else
  {
    CLog::Log(LOGERROR, "Vulkan: Could not map vertex buffer.");
  }
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
