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
#include "VulkanExtensions.h"
#include "VulkanMatrix.h"
#include "filesystem/File.h"
#include "filesystem/SpecialProtocol.h"
#include "guilib/DirtyRegion.h"
#include "guilib/graphics/vulkan/VulkanGUITexture.h"
#include "platform/MessagePrinter.h"
#include "settings/AdvancedSettings.h"
#include "settings/SettingsComponent.h"
#include "utils/FileUtils.h"
#include "utils/MathUtils.h"
#include "utils/SystemInfo.h"
#include "utils/TimeUtils.h"
#include "utils/StringUtils.h"
//#include "utils/VulkanUtils.h"
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

bool CVulkanRenderSystem::InitRenderSystem()
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);

  m_deviceQueue = CreateVulkanDeviceQueue(this,
                                          DeviceQueueOption::GRAPHICS_QUEUE_FLAG |
                                              DeviceQueueOption::PRESENTATION_SUPPORT_QUEUE_FLAG,
                                          0, false, false);
  if (!m_deviceQueue)
  {
    return false;
  }

  TEST___swapchain_dimensions.width = 640;
  TEST___swapchain_dimensions.height = 480;
  TEST___init_vertex_buffer();
  TEST___init_swapchain();
  TEST___init_render_pass();
  TEST___init_pipeline();
  //TEST___init_framebuffers();

  m_bRenderCreated = true;

  CVulkanGUITexture::Register();

  return true;
}

bool CVulkanRenderSystem::ResetRenderSystem(int width, int height)
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  m_width = width;
  m_height = height;

  return true;
}

bool CVulkanRenderSystem::DestroyRenderSystem()
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);

  TEST___deinit_vertex_buffer();

  m_deviceQueue.reset();

  m_bRenderCreated = false;

  return true;
}

bool CVulkanRenderSystem::BeginRender()
{
  //fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return false;

  return true;
}

bool CVulkanRenderSystem::EndRender()
{
  //fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return false;

  return true;
}

void CVulkanRenderSystem::InvalidateColorBuffer()
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return;
}

bool CVulkanRenderSystem::ClearBuffers(KODI::UTILS::COLOR::Color color)
{
  //fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return false;

  return true;
}

bool CVulkanRenderSystem::IsExtSupported(const char* extension) const
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!extension)
    return false;

  for (const auto& [extName, specVersion] : m_vulkanExtensions)
  {
    if (extName == extension)
      return true;
  }

  return false;
}

void CVulkanRenderSystem::PresentRender(bool rendered, bool videoLayer)
{
  //fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);

  if (!m_bRenderCreated)
    return;
}

void CVulkanRenderSystem::CaptureStateBlock()
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return;
}

void CVulkanRenderSystem::ApplyStateBlock()
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return;
}

void CVulkanRenderSystem::SetCameraPosition(const CPoint& camera,
                                            int screenWidth,
                                            int screenHeight,
                                            float stereoFactor)
{
  //fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return;
}

void CVulkanRenderSystem::Project(float& x, float& y, float& z)
{
  //fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
}

void CVulkanRenderSystem::GetViewPort(CRect& viewPort)
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return;
}

void CVulkanRenderSystem::SetViewPort(const CRect& viewPort)
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return;
}

bool CVulkanRenderSystem::ScissorsCanEffectClipping()
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);

  return false;
}

CRect CVulkanRenderSystem::ClipRectToScissorRect(const CRect& rect)
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  //if (!m_pShader[m_method])
  return CRect();
}

void CVulkanRenderSystem::SetScissors(const CRect& rect)
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return;
}

void CVulkanRenderSystem::ResetScissors()
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
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

void CVulkanRenderSystem::TEST___init_vertex_buffer()
{
  // Vertex data for a single colored triangle
  const std::vector<Vertex> vertices = {{{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
                                        {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

  const VkDeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();

  // Copy Vertex data to a buffer accessible by the device

  VkBufferCreateInfo buffer_info{};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = buffer_size;
  buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

  // We use the Vulkan Memory Allocator to find a memory type that can be written and mapped from the host
  // On most setups this will return a memory type that resides in VRAM and is accessible from the host
  VmaAllocationCreateInfo buffer_alloc_ci{};
  buffer_alloc_ci.flags =
      VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
  buffer_alloc_ci.usage = VMA_MEMORY_USAGE_AUTO;
  buffer_alloc_ci.requiredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

  VmaAllocationInfo buffer_alloc_info{};
  VkResult ret =
      vmaCreateBuffer(m_deviceQueue->GetVMAAllocator(), &buffer_info, &buffer_alloc_ci,
                      &TEST___vertex_buffer, &TEST___vertex_buffer_allocation, &buffer_alloc_info);
  if (ret == VK_SUCCESS && buffer_alloc_info.pMappedData)
  {
    memcpy(buffer_alloc_info.pMappedData, vertices.data(), buffer_size);
  }
  else
  {
    throw std::runtime_error("Could not map vertex buffer.");
  }
}

void CVulkanRenderSystem::TEST___deinit_vertex_buffer()
{
  if (TEST___vertex_buffer != VK_NULL_HANDLE)
  {
    vmaDestroyBuffer(m_deviceQueue->GetVMAAllocator(), TEST___vertex_buffer,
                     TEST___vertex_buffer_allocation);
    TEST___vertex_buffer = VK_NULL_HANDLE;
    TEST___vertex_buffer_allocation = VK_NULL_HANDLE;
  }
}

void CVulkanRenderSystem::TEST___init_swapchain()
{
  VkSurfaceCapabilitiesKHR surface_properties{};
  if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_deviceQueue->GetVulkanPhysicalDevice(),
                                                GetVulkanSurface(),
                                                &surface_properties) != VK_SUCCESS)
    throw std::runtime_error("Could not get surface capabilities.");

  VkSurfaceFormatKHR format =
      TEST___select_surface_format(m_deviceQueue->GetVulkanPhysicalDevice(), GetVulkanSurface());

  VkExtent2D swapchain_size{};
  if (surface_properties.currentExtent.width == 0xFFFFFFFF)
  {
    swapchain_size.width = TEST___swapchain_dimensions.width;
    swapchain_size.height = TEST___swapchain_dimensions.height;
  }
  else
  {
    swapchain_size = surface_properties.currentExtent;
  }

  // Determine the number of VkImage's to use in the swapchain.
  // Ideally, we desire to own 1 image at a time, the rest of the images can
  // either be rendered to and/or being queued up for display.
  uint32_t desired_swapchain_images = surface_properties.minImageCount + 1;
  if ((surface_properties.maxImageCount > 0) &&
      (desired_swapchain_images > surface_properties.maxImageCount))
  {
    // Application must settle for fewer images than desired.
    desired_swapchain_images = surface_properties.maxImageCount;
  }

  // Figure out a suitable surface transform.
  VkSurfaceTransformFlagBitsKHR pre_transform;
  if (surface_properties.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
  {
    pre_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  }
  else
  {
    pre_transform = surface_properties.currentTransform;
  }

  VkSwapchainKHR old_swapchain = TEST___m_swapchain;

  // Find a supported composite type.
  VkCompositeAlphaFlagBitsKHR composite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  if (surface_properties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
  {
    composite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  }
  else if (surface_properties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
  {
    composite = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
  }
  else if (surface_properties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
  {
    composite = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
  }
  else if (surface_properties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
  {
    composite = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
  }

  fprintf(stderr, "Vulkan: Creating swapchain with %d images of size %dx%d\n",
          desired_swapchain_images, swapchain_size.width, swapchain_size.height);

  VkSwapchainCreateInfoKHR info{.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                                .pNext = nullptr,
                                .flags = 0,
                                .surface = GetVulkanSurface(),
                                .minImageCount = desired_swapchain_images,
                                .imageFormat = format.format,
                                .imageColorSpace = format.colorSpace,
                                .imageExtent = swapchain_size,
                                .imageArrayLayers = 1,
                                .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                .queueFamilyIndexCount = 0,
                                .pQueueFamilyIndices = nullptr,
                                .preTransform = pre_transform,
                                .compositeAlpha = composite,
                                .presentMode = VK_PRESENT_MODE_FIFO_KHR,
                                .clipped = true,
                                .oldSwapchain = old_swapchain};

  if (vkCreateSwapchainKHR(m_deviceQueue->GetVulkanDevice(), &info, nullptr, &TEST___m_swapchain) !=
      VK_SUCCESS)
  {
    throw std::runtime_error("Failed to create swapchain.");
  }

  if (old_swapchain != VK_NULL_HANDLE)
  {
    for (VkImageView image_view : TEST___swapchain_image_views)
    {
      vkDestroyImageView(m_deviceQueue->GetVulkanDevice(), image_view, nullptr);
    }

    for (auto& per_frame : TEST___per_frame)
    {
      TEST___teardown_per_frame(per_frame);
    }

    TEST___swapchain_image_views.clear();

    vkDestroySwapchainKHR(m_deviceQueue->GetVulkanDevice(), old_swapchain, nullptr);
  }

  TEST___swapchain_dimensions = {swapchain_size.width, swapchain_size.height, format.format};

  uint32_t image_count;
  if (vkGetSwapchainImagesKHR(m_deviceQueue->GetVulkanDevice(), TEST___m_swapchain, &image_count,
                              nullptr) != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to get swapchain images.");
  }

  /// The swapchain images.
  std::vector<VkImage> swapchain_images(image_count);
  if (vkGetSwapchainImagesKHR(m_deviceQueue->GetVulkanDevice(), TEST___m_swapchain, &image_count,
                              swapchain_images.data()) != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to get swapchain images.");
  }

  // Initialize per-frame resources.
  // Every swapchain image has its own command pool and fence manager.
  // This makes it very easy to keep track of when we can reset command buffers and such.
  TEST___per_frame.clear();
  TEST___per_frame.resize(image_count);

  for (size_t i = 0; i < image_count; i++)
  {
    TEST___init_per_frame(TEST___per_frame[i]);
  }

  for (size_t i = 0; i < image_count; i++)
  {
    // Create an image view which we can render into.
    VkImageViewCreateInfo view_info{.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                    .pNext = nullptr,
                                    .flags = 0,
                                    .image = swapchain_images[i],
                                    .viewType = VK_IMAGE_VIEW_TYPE_2D,
                                    .format = TEST___swapchain_dimensions.format,
                                    .components = {.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                   .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                   .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                   .a = VK_COMPONENT_SWIZZLE_IDENTITY},
                                    .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                                         .baseMipLevel = 0,
                                                         .levelCount = 1,
                                                         .baseArrayLayer = 0,
                                                         .layerCount = 1}};

    VkImageView image_view;
    if (vkCreateImageView(m_deviceQueue->GetVulkanDevice(), &view_info, nullptr, &image_view) !=
        VK_SUCCESS)
      throw std::runtime_error("Failed to create image view.");

    TEST___swapchain_image_views.push_back(image_view);
  }
}

void CVulkanRenderSystem::TEST___init_render_pass()
{
  VkAttachmentDescription attachment{
      .flags = 0,
      .format = TEST___swapchain_dimensions.format, // Backbuffer format.
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
          VK_IMAGE_LAYOUT_PRESENT_SRC_KHR // After the render pass is complete, we will transition to PRESENT_SRC_KHR
  };

  // We have one subpass. This subpass has one color attachment.
  // While executing this subpass, the attachment will be in attachment optimal layout.
  VkAttachmentReference color_ref = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

  // We will end up with two transitions.
  // The first one happens right before we start subpass #0, where
  // UNDEFINED is transitioned into COLOR_ATTACHMENT_OPTIMAL.
  // The final layout in the render pass attachment states PRESENT_SRC_KHR, so we
  // will get a final transition from COLOR_ATTACHMENT_OPTIMAL to PRESENT_SRC_KHR.
  VkSubpassDescription subpass{.flags = 0,
                               .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                               .inputAttachmentCount = 0,
                               .pInputAttachments = nullptr,
                               .colorAttachmentCount = 1,
                               .pColorAttachments = &color_ref,
                               .pResolveAttachments = nullptr,
                               .pDepthStencilAttachment = nullptr,
                               .preserveAttachmentCount = 0,
                               .pPreserveAttachments = nullptr};

  // Create a dependency to external events.
  // We need to wait for the WSI semaphore to signal.
  // Only pipeline stages which depend on COLOR_ATTACHMENT_OUTPUT_BIT will
  // actually wait for the semaphore, so we must also wait for that pipeline stage.
  VkSubpassDependency dependency{.srcSubpass = VK_SUBPASS_EXTERNAL,
                                 .dstSubpass = 0,
                                 .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                 .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                 .srcAccessMask = 0,
                                 .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                                  VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                 .dependencyFlags = 0};

  // Finally, create the renderpass.
  VkRenderPassCreateInfo rp_info{.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                                 .pNext = nullptr,
                                 .flags = 0,
                                 .attachmentCount = 1,
                                 .pAttachments = &attachment,
                                 .subpassCount = 1,
                                 .pSubpasses = &subpass,
                                 .dependencyCount = 1,
                                 .pDependencies = &dependency};

  if (vkCreateRenderPass(m_deviceQueue->GetVulkanDevice(), &rp_info, nullptr,
                         &TEST___render_pass) != VK_SUCCESS)
    throw std::runtime_error("Failed to create render pass.");
}

void CVulkanRenderSystem::TEST___init_pipeline()
{
  // Create a blank pipeline layout.
  // We are not binding any resources to the pipeline in this first sample.
  VkPipelineLayoutCreateInfo layout_info{.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                                         .pNext = nullptr,
                                         .flags = 0,
                                         .setLayoutCount = 0,
                                         .pSetLayouts = nullptr,
                                         .pushConstantRangeCount = 0,
                                         .pPushConstantRanges = nullptr};
  if (vkCreatePipelineLayout(m_deviceQueue->GetVulkanDevice(), &layout_info, nullptr,
                             &TEST___pipeline_layout) != VK_SUCCESS)
    throw std::runtime_error("Failed to create pipeline layout");

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
      .minSampleShading = 1.0f,
      .pSampleMask = nullptr,
      .alphaToCoverageEnable = VK_FALSE,
      .alphaToOneEnable = VK_FALSE};

  // Specify that these states will be dynamic, i.e. not part of pipeline state object.
  std::array<VkDynamicState, 2>
      dynamics{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamic{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .dynamicStateCount = static_cast<uint32_t>(dynamics.size()),
      .pDynamicStates = dynamics.data()};

  // Load our SPIR-V shaders.

  std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages{};

  // Vertex stage of the pipeline
  shader_stages[0] = {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                      .pNext = nullptr,
                      .flags = 0,
                      .stage = VK_SHADER_STAGE_VERTEX_BIT,
                      .module = TEST___load_shader_module("triangle.vert.spv"),
                      .pName = "main",
                      .pSpecializationInfo = nullptr};

  // Fragment stage of the pipeline
  shader_stages[1] = {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                      .pNext = nullptr,
                      .flags = 0,
                      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                      .module = TEST___load_shader_module("triangle.frag.spv"),
                      .pName = "main",
                      .pSpecializationInfo = nullptr};

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
      .layout = TEST___pipeline_layout, // We need to specify the pipeline layout up front
      .renderPass = TEST___render_pass, // We need to specify the render pass up front
      .subpass = 0, // We will be using the first subpass in the render pass
      .basePipelineHandle = VK_NULL_HANDLE,
      .basePipelineIndex = -1
  };

  if (vkCreateGraphicsPipelines(m_deviceQueue->GetVulkanDevice(), VK_NULL_HANDLE, 1, &pipe, nullptr,
                                &TEST___pipeline) != VK_SUCCESS)
    throw std::runtime_error("Failed to create graphics pipeline.");

  // Pipeline is baked, we can delete the shader modules now.
  vkDestroyShaderModule(m_deviceQueue->GetVulkanDevice(), shader_stages[0].module, nullptr);
  vkDestroyShaderModule(m_deviceQueue->GetVulkanDevice(), shader_stages[1].module, nullptr);
}

VkSurfaceFormatKHR CVulkanRenderSystem::TEST___select_surface_format(
    VkPhysicalDevice gpu, VkSurfaceKHR surface, std::vector<VkFormat> const& preferred_formats)
{
  uint32_t surface_format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &surface_format_count, nullptr);
  assert(0 < surface_format_count);
  std::vector<VkSurfaceFormatKHR> supported_surface_formats(surface_format_count);
  vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &surface_format_count,
                                       supported_surface_formats.data());

  auto it = std::ranges::find_if(supported_surface_formats,
                                 [&preferred_formats](VkSurfaceFormatKHR surface_format)
                                 {
                                   return std::ranges::any_of(
                                       preferred_formats, [&surface_format](VkFormat format)
                                       { return format == surface_format.format; });
                                 });

  // We use the first supported format as a fallback in case none of the preferred formats is available
  return it != supported_surface_formats.end() ? *it : supported_surface_formats[0];
}

void CVulkanRenderSystem::TEST___init_per_frame(TEST___PerFrame& per_frame)
{
  VkFenceCreateInfo info{.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                         .pNext = nullptr,
                         .flags = VK_FENCE_CREATE_SIGNALED_BIT};
  if (vkCreateFence(m_deviceQueue->GetVulkanDevice(), &info, nullptr,
                    &per_frame.queue_submit_fence) != VK_SUCCESS)
    throw std::runtime_error("Failed to create fence.");

  VkCommandPoolCreateInfo cmd_pool_info{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
      .queueFamilyIndex = static_cast<uint32_t>(m_deviceQueue->GetVulkanQueueIndex())};
  if (vkCreateCommandPool(m_deviceQueue->GetVulkanDevice(), &cmd_pool_info, nullptr,
                          &per_frame.primary_command_pool) != VK_SUCCESS)
    throw std::runtime_error("Failed to create command pool.");

  VkCommandBufferAllocateInfo cmd_buf_info{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                           .pNext = nullptr,
                                           .commandPool = per_frame.primary_command_pool,
                                           .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                           .commandBufferCount = 1};
  if (vkAllocateCommandBuffers(m_deviceQueue->GetVulkanDevice(), &cmd_buf_info,
                               &per_frame.primary_command_buffer) != VK_SUCCESS)
    throw std::runtime_error("Failed to allocate command buffer.");
}

void CVulkanRenderSystem::TEST___teardown_per_frame(TEST___PerFrame& per_frame)
{
  if (per_frame.queue_submit_fence != VK_NULL_HANDLE)
  {
    vkDestroyFence(m_deviceQueue->GetVulkanDevice(), per_frame.queue_submit_fence, nullptr);

    per_frame.queue_submit_fence = VK_NULL_HANDLE;
  }

  if (per_frame.primary_command_buffer != VK_NULL_HANDLE)
  {
    vkFreeCommandBuffers(m_deviceQueue->GetVulkanDevice(), per_frame.primary_command_pool, 1,
                         &per_frame.primary_command_buffer);

    per_frame.primary_command_buffer = VK_NULL_HANDLE;
  }

  if (per_frame.primary_command_pool != VK_NULL_HANDLE)
  {
    vkDestroyCommandPool(m_deviceQueue->GetVulkanDevice(), per_frame.primary_command_pool, nullptr);

    per_frame.primary_command_pool = VK_NULL_HANDLE;
  }

  if (per_frame.swapchain_acquire_semaphore != VK_NULL_HANDLE)
  {
    vkDestroySemaphore(m_deviceQueue->GetVulkanDevice(), per_frame.swapchain_acquire_semaphore,
                       nullptr);

    per_frame.swapchain_acquire_semaphore = VK_NULL_HANDLE;
  }

  if (per_frame.swapchain_release_semaphore != VK_NULL_HANDLE)
  {
    vkDestroySemaphore(m_deviceQueue->GetVulkanDevice(), per_frame.swapchain_release_semaphore,
                       nullptr);

    per_frame.swapchain_release_semaphore = VK_NULL_HANDLE;
  }
}

VkShaderModule CVulkanRenderSystem::TEST___load_shader_module(const std::string& filename) const
{
  XFILE::CFileStream file;

  std::string path = CSpecialProtocol::TranslatePath(KODI::UTILS::StringUtils::Format(
      "special://xbmc/system/shaders/Vulkan/{}", filename));
  if (!file.Open(path))
  {
    CLog::Log(LOGERROR, "CYUVShaderGLSL::CYUVShaderGLSL - failed to open file {}", path);
    return nullptr;
  }

  std::vector<uint8_t> spirv;
  if (XFILE::CFile().LoadFile(path, spirv) <= 0)
  {
    CLog::LogF(LOGERROR, "Failed to load file {}", path);
    return nullptr;
  }

  VkShaderModuleCreateInfo module_info{.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                                       .pNext = nullptr,
                                       .flags = 0,
                                       .codeSize = spirv.size(),
                                       .pCode = reinterpret_cast<uint32_t*>(spirv.data())};

  VkShaderModule shader_module;
  if (vkCreateShaderModule(m_deviceQueue->GetVulkanDevice(), &module_info, nullptr,
                           &shader_module) != VK_SUCCESS)
    throw std::runtime_error("Failed to create shader module.");

  return shader_module;
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
