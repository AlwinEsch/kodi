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
#include "rendering/vulkan/VulkanSwapChain.h"
#include "rendering/vulkan/VulkanUtils.h"
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

  VkSurfaceKHR surface = GetVulkanSurface();
  if (!surface)
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

  m_surface = std::make_unique<CVulkanSurface>(m_deviceQueue->VulkanInstance(), surface);
  if (!m_surface->Initialize(m_deviceQueue.get(), SurfaceFormat::FORMAT_RGBA_32))
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to initialize surface");
    return false;
  }

  m_surface->Reshape({{0, 0}, {m_width, m_height}}, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);

  init_swapchain();

  init_vertex_buffer();

  // Create the necessary objects for rendering.
  init_render_pass();
  init_pipeline();
  init_framebuffers();

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

  ////////VkRect2D imageSize{{0, 0}, {m_width, m_height}};

  ////////m_surface->Reshape(imageSize, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);

  ////////m_SwapchainFormat = m_surface->vkSurfaceFormat().format;

  ////////TEST___init_swapchain();
  ////////init_render_pass();
  ////////TEST___init_pipeline();

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
    vkDestroyFramebuffer(m_deviceQueue->VulkanDevice(), framebuffer->vkFramebuffer(), nullptr);
    vkDestroyImageView(m_deviceQueue->VulkanDevice(), framebuffer->vkImageView(), nullptr);
    framebuffer.reset();
  }
}

/**
 * @brief Initializes the vertex buffer by creating it, allocating memory, binding the memory, and uploading vertex data.
 * @note This function must be called after the Vulkan device has been initialized.
 * @throws std::runtime_error if any Vulkan operation fails.
 */
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

/**
 * @brief Initializes the Vulkan swapchain.
 */
void CVulkanRenderSystem::init_swapchain()
{
  VkResult result = VK_SUCCESS;

  m_SwapchainFormat = m_surface->vkSurfaceFormat().format;

  VkSwapchainKHR swapchain = m_surface->SwapChain()->vkSwapchain();

  uint32_t image_count;
  result = vkGetSwapchainImagesKHR(m_deviceQueue->VulkanDevice(), swapchain, &image_count, nullptr);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to get swapchain image count, ERROR: {0}",
              ErrorString(result));
    return;
  }

  /// The swapchain images.
  std::vector<VkImage> swapchain_images(image_count);
  result = vkGetSwapchainImagesKHR(m_deviceQueue->VulkanDevice(), swapchain, &image_count,
                                   swapchain_images.data());
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to get swapchain images, ERROR: {0}", ErrorString(result));
    return;
  }

  // Initialize per-frame resources.
  // Every swapchain image has its own command pool and fence manager.
  // This makes it very easy to keep track of when we can reset command buffers and such.
  context.per_frame.clear();
  context.per_frame.resize(image_count);

  for (size_t i = 0; i < image_count; i++)
  {
    PerFrame& per_frame = context.per_frame[i];

    VkFenceCreateInfo info{.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                           .pNext = nullptr,
                           .flags = VK_FENCE_CREATE_SIGNALED_BIT};
    result =
        vkCreateFence(m_deviceQueue->VulkanDevice(), &info, nullptr, &per_frame.queue_submit_fence);
    if (result != VK_SUCCESS)
    {
      CLog::Log(LOGERROR, "Vulkan: Failed to create fence, ERROR: {0}", ErrorString(result));
      return;
    }

    std::unique_ptr<CVulkanCommandPool> pool = m_deviceQueue->CreateCommandPool();
    if (!pool)
    {
      CLog::Log(LOGERROR, "Vulkan: Failed to create command pool");
      return;
    }

    std::unique_ptr<CVulkanCommandBuffer> command_buffer = pool->CreatePrimaryCommandBuffer();
    if (!command_buffer)
    {
      pool->Destroy();
      CLog::Log(LOGERROR, "Vulkan: Failed to create command buffer");
      return;
    }

    per_frame.primary_command_pool = std::move(pool);
    per_frame.primary_command_buffer = std::move(command_buffer);
  }

  for (size_t i = 0; i < image_count; i++)
  {
    // Create an image view which we can render into.
    VkImageViewCreateInfo view_info{.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                    .pNext = nullptr,
                                    .flags = 0,
                                    .image = swapchain_images[i],
                                    .viewType = VK_IMAGE_VIEW_TYPE_2D,
                                    .format = m_SwapchainFormat,
                                    .components = {.r = VK_COMPONENT_SWIZZLE_R,
                                                   .g = VK_COMPONENT_SWIZZLE_G,
                                                   .b = VK_COMPONENT_SWIZZLE_B,
                                                   .a = VK_COMPONENT_SWIZZLE_A},
                                    .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                                         .baseMipLevel = 0,
                                                         .levelCount = 1,
                                                         .baseArrayLayer = 0,
                                                         .layerCount = 1}};

    VkImageView image_view;
    result = vkCreateImageView(m_deviceQueue->VulkanDevice(), &view_info, nullptr, &image_view);
    if (result != VK_SUCCESS)
    {
      CLog::Log(LOGERROR, "Vulkan: Failed to create image view, ERROR: {0}", ErrorString(result));
      return;
    }

    context.swapchain_image_views.push_back(image_view);
  }
}

/**
 * @brief Initializes the Vulkan render pass.
 */
void CVulkanRenderSystem::init_render_pass()
{
  VkAttachmentDescription attachment{
      .flags = 0, // No flags.
      .format = m_SwapchainFormat, // Backbuffer format.
      .samples = VK_SAMPLE_COUNT_1_BIT, // Not multisampled.
      .loadOp =
          VK_ATTACHMENT_LOAD_OP_CLEAR, // When starting the frame, we want tiles to be cleared.
      .storeOp =
          VK_ATTACHMENT_STORE_OP_STORE, // When ending the frame, we want tiles to be written out.
      .stencilLoadOp =
          VK_ATTACHMENT_LOAD_OP_DONT_CARE, // Don't care about stencil since we're not using it.
      .stencilStoreOp =
          VK_ATTACHMENT_STORE_OP_DONT_CARE, // Don't care about stencil since we're not using it.
      .initialLayout =
          VK_IMAGE_LAYOUT_UNDEFINED, // The image layout will be undefined when the render pass begins.
      .finalLayout =
          VK_IMAGE_LAYOUT_PRESENT_SRC_KHR // After the render pass is complete, we will transition to PRESENT_SRC_KHR layout.
  };

  // We have one subpass. This subpass has one color attachment.
  // While executing this subpass, the attachment will be in attachment optimal layout.
  VkAttachmentReference color_ref = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

  // We will end up with two transitions.
  // The first one happens right before we start subpass #0, where
  // UNDEFINED is transitioned into COLOR_ATTACHMENT_OPTIMAL.
  // The final layout in the render pass attachment states PRESENT_SRC_KHR, so we
  // will get a final transition from COLOR_ATTACHMENT_OPTIMAL to PRESENT_SRC_KHR.
  VkSubpassDescription subpass{
      .flags = 0,
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .inputAttachmentCount = 0,
      .pInputAttachments = nullptr,
      .colorAttachmentCount = 1,
      .pColorAttachments = &color_ref,
      .pResolveAttachments = nullptr,
      .pDepthStencilAttachment = nullptr,
      .preserveAttachmentCount = 0,
      .pPreserveAttachments = nullptr,
  };

  // Create a dependency to external events.
  // We need to wait for the WSI semaphore to signal.
  // Only pipeline stages which depend on COLOR_ATTACHMENT_OUTPUT_BIT will
  // actually wait for the semaphore, so we must also wait for that pipeline stage.
  VkSubpassDependency dependency{
      .srcSubpass = VK_SUBPASS_EXTERNAL,
      .dstSubpass = 0,
      .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .srcAccessMask = 0,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .dependencyFlags = 0,
  };

  // Since we changed the image layout, we need to make the memory visible to
  // color attachment to modify.
  dependency.srcAccessMask = 0;
  dependency.dstAccessMask =
      VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  // Finally, create the renderpass.
  VkRenderPassCreateInfo rp_info{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .attachmentCount = 1,
      .pAttachments = &attachment,
      .subpassCount = 1,
      .pSubpasses = &subpass,
      .dependencyCount = 1,
      .pDependencies = &dependency,
  };

  VkResult result =
      vkCreateRenderPass(m_deviceQueue->VulkanDevice(), &rp_info, nullptr, &context.render_pass);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create render pass, ERROR: {0}", ErrorString(result));
    return;
  }
}

/**
 * @brief Initializes the Vulkan pipeline.
 */
void CVulkanRenderSystem::init_pipeline()
{
  // Create a blank pipeline layout.
  // We are not binding any resources to the pipeline in this first sample.
  VkPipelineLayoutCreateInfo layout_info{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .setLayoutCount = 0,
      .pSetLayouts = nullptr,
      .pushConstantRangeCount = 0,
      .pPushConstantRanges = nullptr,
  };
  VkResult result = vkCreatePipelineLayout(m_deviceQueue->VulkanDevice(), &layout_info, nullptr,
                                           &context.pipeline_layout);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create pipeline layout, ERROR: {0}",
              ErrorString(result));
    return;
  }

  // The Vertex input properties define the interface between the vertex buffer and the vertex shader.

  // Specify we will use triangle lists to draw geometry.
  VkPipelineInputAssemblyStateCreateInfo input_assembly{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      .primitiveRestartEnable = VK_FALSE};

  // Define the vertex input binding.
  VkVertexInputBindingDescription binding_description{
      .binding = 0, .stride = sizeof(Vertex), .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};

  // Define the vertex input attribute.
  std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions{
      {{.location = 0,
        .binding = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(Vertex, position)},
       {.location = 1,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex, color)}}};

  // Define the pipeline vertex input.
  // Create the vertex input state
  VkPipelineVertexInputStateCreateInfo vertex_input{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .vertexBindingDescriptionCount = 1,
      .pVertexBindingDescriptions = &binding_description,
      .vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size()),
      .pVertexAttributeDescriptions = attribute_descriptions.data()};

  // Specify rasterization state.
  VkPipelineRasterizationStateCreateInfo raster{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .depthClampEnable = VK_FALSE,
      .rasterizerDiscardEnable = VK_FALSE,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .cullMode = VK_CULL_MODE_BACK_BIT,
      .frontFace = VK_FRONT_FACE_CLOCKWISE,
      .depthBiasEnable = VK_FALSE,
      .depthBiasConstantFactor = 0.0f,
      .depthBiasClamp = 0.0f,
      .depthBiasSlopeFactor = 0.0f,
      .lineWidth = 1.0f};

  // Our attachment will write to all color channels, but no blending is enabled.
  VkPipelineColorBlendAttachmentState blend_attachment{
      .blendEnable = VK_FALSE,
      .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
      .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
      .colorBlendOp = VK_BLEND_OP_ADD,
      .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
      .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
      .alphaBlendOp = VK_BLEND_OP_ADD,
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};

  VkPipelineColorBlendStateCreateInfo blend{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .logicOpEnable = VK_FALSE,
      .logicOp = VK_LOGIC_OP_COPY,
      .attachmentCount = 1,
      .pAttachments = &blend_attachment,
      .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}};

  // We will have one viewport and scissor box.
  VkPipelineViewportStateCreateInfo viewport{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .viewportCount = 1,
      .pViewports = nullptr, // We will set the viewport dynamically.
      .scissorCount = 1,
      .pScissors = nullptr // We will set the scissor dynamically.
  };

  // Disable all depth testing.
  VkPipelineDepthStencilStateCreateInfo depth_stencil{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .depthTestEnable = VK_FALSE,
      .depthWriteEnable = VK_FALSE,
      .depthCompareOp = VK_COMPARE_OP_NEVER,
      .depthBoundsTestEnable = VK_FALSE,
      .stencilTestEnable = VK_FALSE,
      .front = {},
      .back = {},
      .minDepthBounds = 0.0f,
      .maxDepthBounds = 1.0f};

  // No multisampling.
  VkPipelineMultisampleStateCreateInfo multisample{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
      .sampleShadingEnable = VK_FALSE,
      .minSampleShading = 0.0f,
      .pSampleMask = nullptr,
      .alphaToCoverageEnable = VK_FALSE,
      .alphaToOneEnable = VK_FALSE,
  };

  // Specify that these states will be dynamic, i.e. not part of pipeline state object.
  std::array<VkDynamicState, 2> dynamics{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamic{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .dynamicStateCount = static_cast<uint32_t>(dynamics.size()),
      .pDynamicStates = dynamics.data(),
  };

  // Load our SPIR-V shaders.
  std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages{};

  // Vertex stage of the pipeline
  shader_stages[0] = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = UTILS::vulkanCreateShaderModule(m_deviceQueue->VulkanDevice(),
                                                "vulkan_shader_gr0_vert_test_triangle.spv"),
      .pName = "main",
      .pSpecializationInfo = nullptr,
  };

  // Fragment stage of the pipeline
  shader_stages[1] = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = UTILS::vulkanCreateShaderModule(m_deviceQueue->VulkanDevice(),
                                                "vulkan_shader_gr0_frag_test_triangle.spv"),
      .pName = "main",
      .pSpecializationInfo = nullptr,
  };

  VkGraphicsPipelineCreateInfo pipe{
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stageCount = static_cast<uint32_t>(shader_stages.size()),
      .pStages = shader_stages.data(),
      .pVertexInputState = &vertex_input,
      .pInputAssemblyState = &input_assembly,
      .pTessellationState = nullptr,
      .pViewportState = &viewport,
      .pRasterizationState = &raster,
      .pMultisampleState = &multisample,
      .pDepthStencilState = &depth_stencil,
      .pColorBlendState = &blend,
      .pDynamicState = &dynamic,
      .layout = context.pipeline_layout, // We need to specify the pipeline layout up front
      .renderPass = context.render_pass, // We need to specify the render pass up front
      .subpass = 0, // We will be rendering in the first subpass
      .basePipelineHandle = VK_NULL_HANDLE,
      .basePipelineIndex = -1};

  result = vkCreateGraphicsPipelines(m_deviceQueue->VulkanDevice(), VK_NULL_HANDLE, 1, &pipe,
                                     nullptr, &context.pipeline);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create graphics pipeline, ERROR: {0}",
              ErrorString(result));
    return;
  }

  // Pipeline is baked, we can delete the shader modules now.
  vkDestroyShaderModule(m_deviceQueue->VulkanDevice(), shader_stages[0].module, nullptr);
  vkDestroyShaderModule(m_deviceQueue->VulkanDevice(), shader_stages[1].module, nullptr);
}

/**
 * @brief Acquires an image from the swapchain.
 * @param[out] image The swapchain index for the acquired image.
 * @returns Vulkan result code
 */
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
    result = vkCreateSemaphore(m_deviceQueue->VulkanDevice(), &info, nullptr, &acquire_semaphore);
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
  result = vkAcquireNextImageKHR(m_deviceQueue->VulkanDevice(), swapchain, UINT64_MAX,
                                 acquire_semaphore, VK_NULL_HANDLE, image);

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
    vkWaitForFences(m_deviceQueue->VulkanDevice(), 1, &context.per_frame[*image].queue_submit_fence,
                    true, UINT64_MAX);
    vkResetFences(m_deviceQueue->VulkanDevice(), 1, &context.per_frame[*image].queue_submit_fence);
  }

  if (context.per_frame[*image].primary_command_pool != nullptr)
  {
    vkResetCommandPool(m_deviceQueue->VulkanDevice(),
                       context.per_frame[*image].primary_command_pool->vkCommandPool(), 0);
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

/**
 * @brief Renders a triangle to the specified swapchain image.
 * @param swapchain_index The swapchain index for the image being rendered.
 */
void CVulkanRenderSystem::render_triangle(uint32_t swapchain_index)
{
  // Render to this framebuffer.
  VkFramebuffer framebuffer = context.swapchain_framebuffers[swapchain_index];

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
      .renderPass = context.render_pass,
      .framebuffer = framebuffer,
      .renderArea = {.offset = {.x = 0, .y = 0}, .extent = {.width = m_width, .height = m_height}},
      .clearValueCount = 1,
      .pClearValues = &clear_value,
  };

  // We will add draw commands in the same command buffer.
  vkCmdBeginRenderPass(cmd, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

  // Bind the graphics pipeline.
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, context.pipeline);

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
        vkCreateSemaphore(m_deviceQueue->VulkanDevice(), &semaphore_info, nullptr,
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

/**
 * @brief Presents an image to the swapchain.
 * @param index The swapchain index previously obtained from @ref acquire_next_image.
 * @returns Vulkan result
 *  code
 */
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

/**
 * @brief Initializes the Vulkan framebuffers.
 */
void CVulkanRenderSystem::init_framebuffers()
{
  context.swapchain_framebuffers.clear();

  // Create framebuffer for each swapchain image view
  for (auto& image_view : context.swapchain_image_views)
  {
    // Build the framebuffer.
    VkFramebufferCreateInfo fb_info{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderPass = context.render_pass,
        .attachmentCount = 1,
        .pAttachments = &image_view,
        .width = m_width,
        .height = m_height,
        .layers = 1,
    };

    VkFramebuffer framebuffer;
    VkResult result =
        vkCreateFramebuffer(m_deviceQueue->VulkanDevice(), &fb_info, nullptr, &framebuffer);
    if (result != VK_SUCCESS)
    {
      CLog::Log(LOGERROR, "Vulkan: Failed to create framebuffer, ERROR: {0}", ErrorString(result));
      return;
    }

    context.swapchain_framebuffers.push_back(framebuffer);
  }
}

void CVulkanRenderSystem::Destroy()
{
  // When destroying the application, we need to make sure the GPU is no longer accessing any resources
  // This is done by doing a device wait idle, which blocks until the GPU signals
  if (m_deviceQueue->VulkanDevice() != VK_NULL_HANDLE)
  {
    vkDeviceWaitIdle(m_deviceQueue->VulkanDevice());
  }

  for (auto& framebuffer : context.swapchain_framebuffers)
  {
    vkDestroyFramebuffer(m_deviceQueue->VulkanDevice(), framebuffer, nullptr);
  }

  for (auto& per_frame : context.per_frame)
  {
    if (per_frame.queue_submit_fence != VK_NULL_HANDLE)
    {
      vkDestroyFence(m_deviceQueue->VulkanDevice(), per_frame.queue_submit_fence, nullptr);

      per_frame.queue_submit_fence = VK_NULL_HANDLE;
    }

    if (per_frame.primary_command_buffer != VK_NULL_HANDLE)
    {
      per_frame.primary_command_buffer->Destroy();
      per_frame.primary_command_buffer.reset();
    }

    if (per_frame.primary_command_pool != nullptr)
    {
      per_frame.primary_command_pool->Destroy();
      per_frame.primary_command_pool.reset();
    }

    if (per_frame.swapchain_acquire_semaphore != VK_NULL_HANDLE)
    {
      vkDestroySemaphore(m_deviceQueue->VulkanDevice(), per_frame.swapchain_acquire_semaphore,
                         nullptr);

      per_frame.swapchain_acquire_semaphore = VK_NULL_HANDLE;
    }

    if (per_frame.swapchain_release_semaphore != VK_NULL_HANDLE)
    {
      vkDestroySemaphore(m_deviceQueue->VulkanDevice(), per_frame.swapchain_release_semaphore,
                         nullptr);

      per_frame.swapchain_release_semaphore = VK_NULL_HANDLE;
    }
  }

  context.per_frame.clear();

  for (auto semaphore : context.recycled_semaphores)
  {
    vkDestroySemaphore(m_deviceQueue->VulkanDevice(), semaphore, nullptr);
  }

  if (context.pipeline != VK_NULL_HANDLE)
  {
    vkDestroyPipeline(m_deviceQueue->VulkanDevice(), context.pipeline, nullptr);
  }

  if (context.pipeline_layout != VK_NULL_HANDLE)
  {
    vkDestroyPipelineLayout(m_deviceQueue->VulkanDevice(), context.pipeline_layout, nullptr);
  }

  if (context.render_pass != VK_NULL_HANDLE)
  {
    vkDestroyRenderPass(m_deviceQueue->VulkanDevice(), context.render_pass, nullptr);
  }

  for (VkImageView image_view : context.swapchain_image_views)
  {
    vkDestroyImageView(m_deviceQueue->VulkanDevice(), image_view, nullptr);
  }

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
  if (m_deviceQueue->VulkanDevice() == VK_NULL_HANDLE)
  {
    return false;
  }

  VkSurfaceCapabilitiesKHR surface_properties;
  VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      m_deviceQueue->VulkanPhysicalDevice(), GetVulkanSurface(), &surface_properties);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to get surface capabilities, ERROR: {0}",
              ErrorString(result));
    return false;
  }

  // Only rebuild the swapchain if the dimensions have changed
  if (surface_properties.currentExtent.width == m_width &&
      surface_properties.currentExtent.height == m_height)
  {
    return false;
  }

  vkDeviceWaitIdle(m_deviceQueue->VulkanDevice());

  for (auto& framebuffer : context.swapchain_framebuffers)
  {
    vkDestroyFramebuffer(m_deviceQueue->VulkanDevice(), framebuffer, nullptr);
  }

  m_height = width;
  m_width = height;

  m_surface->Reshape({{0, 0}, {m_width, m_height}}, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);

  init_swapchain();
  init_framebuffers();
  return true;
}

//std::unique_ptr<vkb::Application> create_hello_triangle()
//{
//  return std::make_unique<CVulkanRenderSystem>();
//}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
