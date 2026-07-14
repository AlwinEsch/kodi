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

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

using KODI::RENDERING::VULKAN::UTILS::ErrorString;

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

  Destroy();
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

  /*










  */

  VkResult result = VK_SUCCESS;

  size_t image_count = m_surface->SwapChain()->AmmountSwapChainImages();

  // Initialize per-frame resources.
  // Every swapchain image has its own command pool and fence manager.
  // This makes it very easy to keep track of when we can reset command buffers and such.
  context.per_frame.clear();
  context.per_frame.resize(image_count);

  for (size_t i = 0; i < image_count; i++)
  {
    PerFrame& per_frame = context.per_frame[i];

    VkFenceCreateInfo info{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    result = vkCreateFence(m_vkDevice, &info, nullptr, &per_frame.queue_submit_fence);
    if (result != VK_SUCCESS)
    {
      CLog::Log(LOGERROR, "Vulkan: Failed to create fence, ERROR: {0}", ErrorString(result));
      return false;
    }

    auto pool = m_deviceQueue->CreateCommandPool();
    if (!pool)
    {
      CLog::Log(LOGERROR, "Vulkan: Failed to create command pool");
      return false;
    }

    auto framebuffer =
        CVulkanFramebuffer::Create(m_deviceQueue.get(), pool.get(), m_vkRenderPass, m_surface.get(),
                                   m_surface->SwapChain()->m_images[i].image);
    if (!framebuffer)
    {
      CLog::Log(LOGERROR, "Vulkan: Failed to create framebuffer");
      return false;
    }

    per_frame.primary_command_pool = std::move(pool);
    per_frame.primary_command_buffer = framebuffer->CommandBuffer();
    per_frame.swapchain_image_view = framebuffer->vkImageView();
    per_frame.swapchain_framebuffer = framebuffer->vkFramebuffer();
    m_framebuffers.push_back(std::move(framebuffer));
  }
  /*












  */
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

  //init_framebuffers();

  ////////auto instance = m_deviceQueue->VulkanInstance();
  ////////auto device = m_deviceQueue->VulkanDevice();
  ////////auto physicalDevice = m_deviceQueue->VulkanPhysicalDevice();

  ////////VkDeviceSize buffer_size = sizeof(TEST___vertices[0]) * TEST___vertices.size();
  ////////UTILS::vulkanCreateBuffer(
  ////////    instance, device, physicalDevice, buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
  ////////    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  ////////    TEST___vertex_buffer, TEST___vertex_buffer_memory);
  ////////// Map the memory and copy the vertex data
  ////////void* data;
  ////////if (vkMapMemory(device, TEST___vertex_buffer_memory, 0, buffer_size, 0, &data) != VK_SUCCESS)
  ////////{
  ////////  throw std::runtime_error("Failed to map vertex buffer memory");
  ////////}
  ////////memcpy(data, TEST___vertices.data(), static_cast<size_t>(buffer_size));
  ////////vkUnmapMemory(device, TEST___vertex_buffer_memory);
  /////////*

  ////////*/
  ////////m_surface =
  ////////    std::make_unique<CVulkanSurface>(m_deviceQueue->VulkanInstance(), GetVulkanSurface());
  ////////if (!m_surface->Initialize(m_deviceQueue.get(), SurfaceFormat::FORMAT_RGBA_32))
  ////////{
  ////////  CLog::Log(LOGERROR, "Vulkan: Failed to initialize surface");
  ////////  return false;
  ////////}

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

  update(0);
  ////uint32_t index = m_surface->SwapChain()->CurrentImageIndex();

  ////TEST___render_triangle(index);
  ////m_surface->SwapBuffers();
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
      .setLayoutCount = layout != VK_NULL_HANDLE ? 1 : 0,
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
  vmaCreateBuffer(m_deviceQueue->VMAAllocator(), &buffer_info, &buffer_alloc_ci, &vertex_buffer,
                  &vertex_buffer_allocation, &buffer_alloc_info);
  if (buffer_alloc_info.pMappedData)
  {
    memcpy(buffer_alloc_info.pMappedData, vertices.data(), buffer_size);
  }
  else
  {
    CLog::Log(LOGERROR, "Vulkan: Could not map vertex buffer.");
  }
}

VkResult CVulkanRenderSystem::acquire_next_image(uint32_t* image)
{
  VkResult result = VK_SUCCESS;

  VkSemaphore acquire_semaphore;
  if (context.recycled_semaphores.empty())
  {
    VkSemaphoreCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };
    result = vkCreateSemaphore(m_vkDevice, &info, nullptr, &acquire_semaphore);
    if (result != VK_SUCCESS)
    {
      CLog::Log(LOGERROR, "Vulkan: Failed to create semaphore, ERROR: {0}", ErrorString(result));
      return result;
    }
  }
  else
  {
    acquire_semaphore = context.recycled_semaphores.back();
    context.recycled_semaphores.pop_back();
  }

  VkSwapchainKHR swapchain = m_surface->SwapChain()->vkSwapchain();
  result = vkAcquireNextImageKHR(m_vkDevice, swapchain, UINT64_MAX, acquire_semaphore,
                                 VK_NULL_HANDLE, image);

  if (result != VK_SUCCESS)
  {
    context.recycled_semaphores.push_back(acquire_semaphore);
    return result;
  }

  // If we have outstanding fences for this swapchain image, wait for them to complete first.
  // After begin frame returns, it is safe to reuse or delete resources which
  // were used previously.
  //
  // We wait for fences which completes N frames earlier, so we do not stall,
  // waiting for all GPU work to complete before this returns.
  // Normally, this doesn't really block at all,
  // since we're waiting for old frames to have been completed, but just in case.
  if (context.per_frame[*image].queue_submit_fence != VK_NULL_HANDLE)
  {
    vkWaitForFences(m_vkDevice, 1, &context.per_frame[*image].queue_submit_fence, true, UINT64_MAX);
    vkResetFences(m_vkDevice, 1, &context.per_frame[*image].queue_submit_fence);
  }

  if (context.per_frame[*image].primary_command_pool != nullptr)
  {
    vkResetCommandPool(m_vkDevice, context.per_frame[*image].primary_command_pool->vkCommandPool(),
                       0);
  }

  // Recycle the old semaphore back into the semaphore manager.
  VkSemaphore old_semaphore = context.per_frame[*image].swapchain_acquire_semaphore;

  if (old_semaphore != VK_NULL_HANDLE)
  {
    context.recycled_semaphores.push_back(old_semaphore);
  }

  context.per_frame[*image].swapchain_acquire_semaphore = acquire_semaphore;

  return VK_SUCCESS;
}

void CVulkanRenderSystem::render_triangle(uint32_t swapchain_index)
{
  // Render to this framebuffer.
  VkFramebuffer framebuffer = context.per_frame[swapchain_index].swapchain_framebuffer;

  // Allocate or re-use a primary command buffer.
  VkCommandBuffer cmd =
      context.per_frame[swapchain_index].primary_command_buffer->GetVulkanCommandBuffer();

  // We will only submit this once before it's recycled.
  VkCommandBufferBeginInfo begin_info{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      .pInheritanceInfo = nullptr,
  };
  // Begin command recording
  vkBeginCommandBuffer(cmd, &begin_info);

  // Set clear color values.
  VkClearValue clear_value{.color = {{0.01f, 0.01f, 0.033f, 1.0f}}};

  // Begin the render pass.
  VkRenderPassBeginInfo rp_begin{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .pNext = nullptr,
      .renderPass = m_vkRenderPass,
      .framebuffer = framebuffer,
      .renderArea = {.offset = {.x = 0, .y = 0}, .extent = {.width = m_width, .height = m_height}},
      .clearValueCount = 1,
      .pClearValues = &clear_value,
  };

  // We will add draw commands in the same command buffer.
  vkCmdBeginRenderPass(cmd, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

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
  vkCmdBindVertexBuffers(cmd, 0, 1, &vertex_buffer, &offset);

  // Draw three vertices with one instance from the currently bound vertex bound.
  vkCmdDraw(cmd, 3, 1, 0, 0);

  // Complete render pass.
  vkCmdEndRenderPass(cmd);

  // Complete the command buffer.
  VkResult result = vkEndCommandBuffer(cmd);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to end command buffer, ERROR: {0}", ErrorString(result));
    return;
  }

  // Submit it to the queue with a release semaphore.
  if (context.per_frame[swapchain_index].swapchain_release_semaphore == VK_NULL_HANDLE)
  {
    VkSemaphoreCreateInfo semaphore_info{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };
    VkResult result =
        vkCreateSemaphore(m_vkDevice, &semaphore_info, nullptr,
                          &context.per_frame[swapchain_index].swapchain_release_semaphore);
    if (result != VK_SUCCESS)
    {
      CLog::Log(LOGERROR, "Vulkan: Failed to create semaphore, ERROR: {0}", ErrorString(result));
      return;
    }
  }

  VkPipelineStageFlags wait_stage{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  VkSubmitInfo info{
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &context.per_frame[swapchain_index].swapchain_acquire_semaphore,
      .pWaitDstStageMask = &wait_stage,
      .commandBufferCount = 1,
      .pCommandBuffers = &cmd,
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &context.per_frame[swapchain_index].swapchain_release_semaphore};
  // Submit command buffer to graphics queue
  result = vkQueueSubmit(m_deviceQueue->VulkanQueue(), 1, &info,
                         context.per_frame[swapchain_index].queue_submit_fence);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to submit command buffer, ERROR: {0}", ErrorString(result));
    return;
  }
}

VkResult CVulkanRenderSystem::present_image(uint32_t index)
{
  VkSwapchainKHR swapchain = m_surface->SwapChain()->vkSwapchain();

  VkPresentInfoKHR present{
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext = nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &context.per_frame[index].swapchain_release_semaphore,
      .swapchainCount = 1,
      .pSwapchains = &swapchain,
      .pImageIndices = &index,
      .pResults = nullptr,
  };
  // Present swapchain image
  return vkQueuePresentKHR(m_deviceQueue->VulkanQueue(), &present);
}

void CVulkanRenderSystem::Destroy()
{
  // When destroying the application, we need to make sure the GPU is no longer accessing any resources
  // This is done by doing a device wait idle, which blocks until the GPU signals
  if (m_vkDevice != VK_NULL_HANDLE)
  {
    vkDeviceWaitIdle(m_vkDevice);
  }

  for (auto& per_frame : context.per_frame)
  {
    if (per_frame.queue_submit_fence != VK_NULL_HANDLE)
    {
      vkDestroyFence(m_vkDevice, per_frame.queue_submit_fence, nullptr);

      per_frame.queue_submit_fence = VK_NULL_HANDLE;
    }

    //if (per_frame.primary_command_buffer != VK_NULL_HANDLE)
    //{
    //  per_frame.primary_command_buffer->Destroy();
    //  per_frame.primary_command_buffer.reset();
    //}

    if (per_frame.primary_command_pool != nullptr)
    {
      per_frame.primary_command_pool->Destroy();
      per_frame.primary_command_pool.reset();
    }

    if (per_frame.swapchain_acquire_semaphore != VK_NULL_HANDLE)
    {
      vkDestroySemaphore(m_vkDevice, per_frame.swapchain_acquire_semaphore, nullptr);

      per_frame.swapchain_acquire_semaphore = VK_NULL_HANDLE;
    }

    if (per_frame.swapchain_release_semaphore != VK_NULL_HANDLE)
    {
      vkDestroySemaphore(m_vkDevice, per_frame.swapchain_release_semaphore, nullptr);

      per_frame.swapchain_release_semaphore = VK_NULL_HANDLE;
    }
  }

  context.per_frame.clear();

  for (auto semaphore : context.recycled_semaphores)
  {
    vkDestroySemaphore(m_vkDevice, semaphore, nullptr);
  }

  /*TEMP NOTE: DONE*/
  if (m_vkPipelineLayout != VK_NULL_HANDLE)
  {
    vkDestroyPipelineLayout(m_vkDevice, m_vkPipelineLayout, nullptr);
    m_vkPipelineLayout = VK_NULL_HANDLE;
  }

  m_renderPass.reset();

  DestroyFramebuffers();

  if (vertex_buffer_allocation != VK_NULL_HANDLE)
  {
    vmaDestroyBuffer(m_deviceQueue->VMAAllocator(), vertex_buffer, vertex_buffer_allocation);
  }

  m_deviceQueue->Destroy();
}

void CVulkanRenderSystem::update(float delta_time)
{
  uint32_t index;

  VkResult result = acquire_next_image(&index);

  // Handle outdated error in acquire.
  if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR)
  {
    resize(m_width, m_height);
    result = acquire_next_image(&index);
  }

  if (result != VK_SUCCESS)
  {
    vkQueueWaitIdle(m_deviceQueue->VulkanQueue());
    return;
  }


  render_triangle(index);
  result = present_image(index);

  // Handle Outdated error in present.
  if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR)
  {
    resize(m_width, m_height);
  }
  else if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to present swapchain image, ERROR: {0}",
              ErrorString(result));
  }
}

bool CVulkanRenderSystem::resize(const uint32_t width, const uint32_t height)
{
  //if (m_vkDevice == VK_NULL_HANDLE)
  //{
  //  return false;
  //}

  //VkSurfaceCapabilitiesKHR surface_properties;
  //VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
  //    m_deviceQueue->VulkanPhysicalDevice(), GetVulkanSurface(), &surface_properties);
  //if (result != VK_SUCCESS)
  //{
  //  CLog::Log(LOGERROR, "Vulkan: Failed to get surface capabilities, ERROR: {0}",
  //            ErrorString(result));
  //  return false;
  //}

  //// Only rebuild the swapchain if the dimensions have changed
  //if (surface_properties.currentExtent.width == m_width &&
  //    surface_properties.currentExtent.height == m_height)
  //{
  //  return false;
  //}

  //vkDeviceWaitIdle(m_vkDevice);

  //for (auto& framebuffer : context.swapchain_framebuffers)
  //{
  //  vkDestroyFramebuffer(m_vkDevice, framebuffer, nullptr);
  //}

  //m_height = width;
  //m_width = height;

  //m_surface->Reshape({{0, 0}, {m_width, m_height}}, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);

  //init_swapchain();
  //init_framebuffers();
  return true;
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
