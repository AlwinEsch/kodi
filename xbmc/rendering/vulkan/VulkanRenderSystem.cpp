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
#include "VulkanUtils.h"
#include "filesystem/File.h"
#include "filesystem/SpecialProtocol.h"
#include "guilib/DirtyRegion.h"
#include "guilib/graphics/vulkan/VulkanGUITexture.h"
#include "platform/MessagePrinter.h"
#include "rendering/vulkan/VulkanCommandBuffer.h"
#include "rendering/vulkan/VulkanCommandPool.h"
#include "rendering/vulkan/VulkanSwapChain.h"
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
}

bool CVulkanRenderSystem::InitRenderSystem()
{
  m_deviceQueue = CreateVulkanDeviceQueue(this,
                                          DeviceQueueOption::GRAPHICS_QUEUE_FLAG |
                                              DeviceQueueOption::PRESENTATION_SUPPORT_QUEUE_FLAG,
                                          0, false, false);
  if (!m_deviceQueue)
  {
    return false;
  }

  auto instance = m_deviceQueue->GetVulkanInstance();
  auto device = m_deviceQueue->GetVulkanDevice();
  auto physicalDevice = m_deviceQueue->GetVulkanPhysicalDevice();

  VkDeviceSize buffer_size = sizeof(TEST___vertices[0]) * TEST___vertices.size();
  UTILS::vulkanCreateBuffer(
      instance, device, physicalDevice, buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      TEST___vertex_buffer, TEST___vertex_buffer_memory);
  // Map the memory and copy the vertex data
  void* data;
  if (vkMapMemory(device, TEST___vertex_buffer_memory, 0, buffer_size, 0, &data) != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to map vertex buffer memory");
  }
  memcpy(data, TEST___vertices.data(), static_cast<size_t>(buffer_size));
  vkUnmapMemory(device, TEST___vertex_buffer_memory);
  /*





  */
  m_surface =
      std::make_unique<CVulkanSurface>(m_deviceQueue->GetVulkanInstance(), GetVulkanSurface());
  if (!m_surface->Initialize(m_deviceQueue.get(), SurfaceFormat::FORMAT_RGBA_32))
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to initialize surface");
    return false;
  }

  VkRect2D imageSize{{0, 0}, {m_width, m_height}};

  m_surface->Reshape(imageSize, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);

  m_SwapchainFormat = m_surface->GetSurfaceFormat().format;

  TEST___init_swapchain();
  TEST___init_pipeline();

  m_bRenderCreated = true;

  CVulkanGUITexture::Register();

  return true;
}

bool CVulkanRenderSystem::ResetRenderSystem(int width, int height)
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return false;

  if (static_cast<uint32_t>(width) == m_width && static_cast<uint32_t>(height) == m_height)
    return true;

  m_width = static_cast<uint32_t>(width);
  m_height = static_cast<uint32_t>(height);

  return true;
}

bool CVulkanRenderSystem::DestroyRenderSystem()
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);

  TEST___Deinit();

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

  uint32_t index = m_surface->GetSwapChain()->CurrentImageIndex();

  TEST___render_triangle(index);
  m_surface->SwapBuffers();
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
  if (!m_bRenderCreated)
    return;

  const VkRect2D& imageSize = m_surface->GetImageSize();

  viewPort.x1 = imageSize.offset.x;
  viewPort.y1 = m_height - imageSize.offset.y - imageSize.extent.height;
  viewPort.x2 = imageSize.offset.x + imageSize.extent.width;
  viewPort.y2 = viewPort.y1 + imageSize.extent.height;
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

  // TODO: Is this correct? Should we set the viewport for all command buffers or just the current one?
  for (const auto& per_frame : TEST___per_frame)
    vkCmdSetViewport(per_frame.primary_command_buffer->GetVulkanCommandBuffer(), 0, 1, &viewport);
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
  if (!m_bRenderCreated)
    return;

  VkRect2D scissor{};
  scissor.offset.x = MathUtils::round_int(static_cast<double>(rect.x1));
  scissor.offset.y = MathUtils::round_int(static_cast<double>(rect.y1));
  scissor.extent.width = MathUtils::round_int(static_cast<double>(rect.x2 - rect.x1));
  scissor.extent.height = MathUtils::round_int(static_cast<double>(rect.y2 - rect.y1));

  // TODO: Is this correct? Should we set the viewport for all command buffers or just the current one?
  for (const auto& per_frame : TEST___per_frame)
    vkCmdSetScissor(per_frame.primary_command_buffer->GetVulkanCommandBuffer(), 0, 1, &scissor);
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

void CVulkanRenderSystem::TEST___Deinit()
{
  // Don't release anything until the GPU is completely idle.
  if (m_deviceQueue->GetVulkanDevice() != VK_NULL_HANDLE)
  {
    vkDeviceWaitIdle(m_deviceQueue->GetVulkanDevice());
  }

  m_surface->Destroy();

  for (auto& per_frame : TEST___per_frame)
  {
    if (per_frame.queue_submit_fence != VK_NULL_HANDLE)
    {
      vkDestroyFence(m_deviceQueue->GetVulkanDevice(), per_frame.queue_submit_fence, nullptr);

      per_frame.queue_submit_fence = VK_NULL_HANDLE;
    }

    per_frame.primary_command_buffer->Deinitialize();
    per_frame.primary_command_buffer.reset();
    per_frame.commandPool->Deinitialize();
    per_frame.commandPool.reset();
  }

  TEST___per_frame.clear();

  if (TEST___pipeline != VK_NULL_HANDLE)
  {
    vkDestroyPipeline(m_deviceQueue->GetVulkanDevice(), TEST___pipeline, nullptr);
  }

  if (TEST___pipeline_layout != VK_NULL_HANDLE)
  {
    vkDestroyPipelineLayout(m_deviceQueue->GetVulkanDevice(), TEST___pipeline_layout, nullptr);
  }

  for (VkImageView image_view : TEST___swapchain_image_views)
  {
    vkDestroyImageView(m_deviceQueue->GetVulkanDevice(), image_view, nullptr);
  }

  if (TEST___vertex_buffer != VK_NULL_HANDLE)
  {
    vkDestroyBuffer(m_deviceQueue->GetVulkanDevice(), TEST___vertex_buffer, nullptr);
    TEST___vertex_buffer = VK_NULL_HANDLE;
  }

  if (TEST___vertex_buffer_memory != VK_NULL_HANDLE)
  {
    vkFreeMemory(m_deviceQueue->GetVulkanDevice(), TEST___vertex_buffer_memory, nullptr);
    TEST___vertex_buffer_memory = VK_NULL_HANDLE;
  }
}

void CVulkanRenderSystem::TEST___init_swapchain()
{
  // Initialize per-frame resources.
  // Every swapchain image has its own command pool and fence manager.
  // This makes it very easy to keep track of when we can reset command buffers and such.
  uint32_t image_count = m_surface->GetSwapChain()->AmmountSwapChainImages();
  TEST___per_frame.clear();
  TEST___per_frame.resize(image_count);

  for (size_t i = 0; i < image_count; i++)
  {
    PerFrame& per_frame = TEST___per_frame[i];

    VkFenceCreateInfo info{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    if (vkCreateFence(m_deviceQueue->GetVulkanDevice(), &info, nullptr,
                      &per_frame.queue_submit_fence) != VK_SUCCESS)
    {
      throw std::runtime_error("Failed to create fence for per frame data.");
    }

    per_frame.commandPool = m_deviceQueue->CreateCommandPool();
    per_frame.primary_command_buffer = per_frame.commandPool->CreatePrimaryCommandBuffer();
  }

  for (size_t i = 0; i < image_count; i++)
  {
    VkImage image;
    VkImageLayout layout;
    VkSemaphore acquireSemaphore;
    VkSemaphore presentSemaphore;
    if (!m_surface->GetSwapChain()->GetImage(i, &image, &layout, &acquireSemaphore,
                                             &presentSemaphore))
    {
      throw std::runtime_error("Failed to get swapchain image");
    }

    // Create an image view which we can render into.
    VkImageViewCreateInfo view_info{.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                    .pNext = nullptr,
                                    .flags = 0,
                                    .image = image,
                                    .viewType = VK_IMAGE_VIEW_TYPE_2D,
                                    .format = m_SwapchainFormat,
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
    {
      throw std::runtime_error("Failed to create image view");
    }

    TEST___swapchain_image_views.push_back(image_view);
  }
}

void CVulkanRenderSystem::TEST___init_pipeline()
{
  auto device = m_deviceQueue->GetVulkanDevice();

  // Create a blank pipeline layout.
  // We are not binding any resources to the pipeline in this first sample.
  VkPipelineLayoutCreateInfo layout_info{.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                                         .pNext = nullptr,
                                         .flags = 0,
                                         .setLayoutCount = 0,
                                         .pSetLayouts = nullptr,
                                         .pushConstantRangeCount = 0,
                                         .pPushConstantRanges = nullptr};
  if (vkCreatePipelineLayout(device, &layout_info, nullptr, &TEST___pipeline_layout) != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to create pipeline layout");
  }

  // Define the vertex input binding description
  VkVertexInputBindingDescription binding_description{
      .binding = 0, .stride = sizeof(Vertex), .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};

  // Define the vertex input attribute descriptions
  std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions{
      {{.location = 0,
        .binding = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(Vertex, position)},
       {.location = 1,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex, color)}}};

  // Create the vertex input state
  VkPipelineVertexInputStateCreateInfo vertex_input{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .vertexBindingDescriptionCount = 1,
      .pVertexBindingDescriptions = &binding_description,
      .vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size()),
      .pVertexAttributeDescriptions = attribute_descriptions.data()};

  // Specify we will use triangle lists to draw geometry.
  VkPipelineInputAssemblyStateCreateInfo input_assembly{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      .primitiveRestartEnable = VK_FALSE};

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

  // Specify that these states will be dynamic, i.e. not part of pipeline state object.
  std::vector<VkDynamicState> dynamic_states = {
      VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_CULL_MODE,
      VK_DYNAMIC_STATE_FRONT_FACE, VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY};

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

  VkPipelineDynamicStateCreateInfo dynamic_state_info{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
      .pDynamicStates = dynamic_states.data()};

  // Load our SPIR-V shaders.

  // Vertex stage of the pipeline
  std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages{};

  shader_stages[0] = {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                      .pNext = nullptr,
                      .flags = 0,
                      .stage = VK_SHADER_STAGE_VERTEX_BIT,
                      .module = UTILS::vulkanCreateShaderModule(device, "triangle.vert.spv"),
                      .pName = "main",
                      .pSpecializationInfo = nullptr};

  // Fragment stage of the pipeline
  shader_stages[1] = {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                      .pNext = nullptr,
                      .flags = 0,
                      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                      .module = UTILS::vulkanCreateShaderModule(device, "triangle.frag.spv"),
                      .pName = "main",
                      .pSpecializationInfo = nullptr};

  // Pipeline rendering info (for dynamic rendering).
  VkPipelineRenderingCreateInfo pipeline_rendering_info{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
      .pNext = nullptr,
      .viewMask = 0,
      .colorAttachmentCount = 1,
      .pColorAttachmentFormats = &m_SwapchainFormat,
      .depthAttachmentFormat = VK_FORMAT_UNDEFINED,
      .stencilAttachmentFormat = VK_FORMAT_UNDEFINED};

  // Create the graphics pipeline.
  VkGraphicsPipelineCreateInfo pipe{
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .pNext = &pipeline_rendering_info,
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
      .pDynamicState = &dynamic_state_info,
      .layout = TEST___pipeline_layout, // We need to specify the pipeline layout up front
      .renderPass = VK_NULL_HANDLE, // Since we are using dynamic rendering this will set as null
      .subpass = 0, // We will be using the first subpass in the render pass
      .basePipelineHandle = VK_NULL_HANDLE,
      .basePipelineIndex = -1};

  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipe, nullptr, &TEST___pipeline) !=
      VK_SUCCESS)
  {
    throw std::runtime_error("Failed to create graphics pipeline");
  }

  // Pipeline is baked, we can delete the shader modules now.
  vkDestroyShaderModule(device, shader_stages[0].module, nullptr);
  vkDestroyShaderModule(device, shader_stages[1].module, nullptr);
}

void CVulkanRenderSystem::TEST___render_triangle(uint32_t swapchain_index)
{
  auto& primary_command_buffer = TEST___per_frame[swapchain_index].primary_command_buffer;

  // Allocate or re-use a primary command buffer.
  VkCommandBuffer cmd = primary_command_buffer->GetVulkanCommandBuffer();

  // We will only submit this once before it's recycled.
  VkCommandBufferBeginInfo begin_info{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                      .pNext = nullptr,
                                      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                                      .pInheritanceInfo = nullptr};

  // Begin command recording
  if (vkBeginCommandBuffer(cmd, &begin_info) != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to begin command buffer");
  }

  // Before starting rendering, transition the swapchain image to COLOR_ATTACHMENT_OPTIMAL
  VkImage image;
  VkImageLayout layout;
  VkSemaphore acquireSemaphore;
  VkSemaphore presentSemaphore;
  if (!m_surface->GetSwapChain()->GetImage(swapchain_index, &image, &layout, &acquireSemaphore,
                                           &presentSemaphore))
  {
    throw std::runtime_error("Failed to get swapchain image");
  }

  primary_command_buffer->TransitionImageLayout(image, VK_IMAGE_LAYOUT_UNDEFINED,
                                                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

  // Set clear color values.
  VkClearValue clear_value{.color = {{0.01f, 0.01f, 0.033f, 1.0f}}};

  // Set up the rendering attachment info
  VkRenderingAttachmentInfo color_attachment{
      .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
      .pNext = nullptr,
      .imageView = TEST___swapchain_image_views[swapchain_index],
      .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .resolveMode = VK_RESOLVE_MODE_NONE,
      .resolveImageView = VK_NULL_HANDLE,
      .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .clearValue = clear_value};

  // Begin rendering
  VkRenderingInfo rendering_info{
      .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
      .pNext = nullptr,
      .flags = 0,
      .renderArea =
          {// Initialize the nested `VkRect2D` structure
           .offset = {0, 0}, // Initialize the `VkOffset2D` inside `renderArea`
           .extent =
               {// Initialize the `VkExtent2D` inside `renderArea`
                .width = m_width,
                .height = m_height}},
      .layerCount = 1,
      .viewMask = 0,
      .colorAttachmentCount = 1,
      .pColorAttachments = &color_attachment,
      .pDepthAttachment = nullptr,
      .pStencilAttachment = nullptr};

  vkCmdBeginRendering(cmd, &rendering_info);

  // Bind the graphics pipeline.
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, TEST___pipeline);

  // Set dynamic states

  // Set viewport dynamically
  VkViewport vp{.x = 0.0f,
                .y = 0.0f,
                .width = static_cast<float>(m_width),
                .height = static_cast<float>(m_height),
                .minDepth = 0.0f,
                .maxDepth = 1.0f};

  vkCmdSetViewport(cmd, 0, 1, &vp);

  // Set scissor dynamically
  VkRect2D scissor{.offset = {.x = 0, .y = 0}, .extent = {.width = m_width, .height = m_height}};

  vkCmdSetScissor(cmd, 0, 1, &scissor);

  // Since we declared VK_DYNAMIC_STATE_CULL_MODE as dynamic in the pipeline,
  // we need to set the cull mode here. VK_CULL_MODE_NONE disables face culling,
  // meaning both front and back faces will be rendered.
  vkCmdSetCullMode(cmd, VK_CULL_MODE_NONE);

  // Since we declared VK_DYNAMIC_STATE_FRONT_FACE as dynamic,
  // we need to specify the winding order considered as the front face.
  // VK_FRONT_FACE_CLOCKWISE indicates that vertices defined in clockwise order
  // are considered front-facing.
  vkCmdSetFrontFace(cmd, VK_FRONT_FACE_CLOCKWISE);

  // Since we declared VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY as dynamic,
  // we need to set the primitive topology here. VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
  // tells Vulkan that the input vertex data should be interpreted as a list of triangles.
  vkCmdSetPrimitiveTopology(cmd, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

  // Bind the vertex buffer
  VkDeviceSize offset = {0};
  vkCmdBindVertexBuffers(cmd, 0, 1, &TEST___vertex_buffer, &offset);

  // Draw three vertices with one instance.
  vkCmdDraw(cmd, static_cast<uint32_t>(TEST___vertices.size()), 1, 0, 0);

  // Complete rendering.
  vkCmdEndRendering(cmd);

  // After rendering , transition the swapchain image to PRESENT_SRC
  primary_command_buffer->TransitionImageLayout(image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

  // Complete the command buffer.
  if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to end command buffer");
    return;
  }

  // Submit it to the queue with a release semaphore.
  if (presentSemaphore == VK_NULL_HANDLE)
  {
    VkSemaphoreCreateInfo semaphore_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, .pNext = nullptr, .flags = 0};
    VkResult result = vkCreateSemaphore(m_deviceQueue->GetVulkanDevice(), &semaphore_info, nullptr,
                                        &presentSemaphore);
    if (result != VK_SUCCESS)
    {
      CLog::Log(LOGERROR, "Vulkan: Failed to create release semaphore");
      return;
    }
  }

  // Using TOP_OF_PIPE here to ensure that the command buffer does not begin executing any pipeline stages
  // (including the layout transition) until the swapchain image is actually acquired (signaled by the semaphore).
  // This prevents the GPU from starting operations too early and guarantees that the image is ready
  // before any rendering commands run.
  VkPipelineStageFlags wait_stage{VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT};

  VkProtectedSubmitInfo protected_submit_info = {};
  protected_submit_info.sType = VK_STRUCTURE_TYPE_PROTECTED_SUBMIT_INFO;
  protected_submit_info.protectedSubmit = false;

  VkSubmitInfo info{
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = &protected_submit_info,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &acquireSemaphore,
      .pWaitDstStageMask = &wait_stage,
      .commandBufferCount = 1,
      .pCommandBuffers = &cmd,
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &presentSemaphore,
  };

  // Submit command buffer to graphics queue
  if (vkQueueSubmit(m_deviceQueue->GetVulkanQueue(), 1, &info,
                    TEST___per_frame[swapchain_index].queue_submit_fence) != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to submit command buffer");
    return;
  }
}

bool CVulkanRenderSystem::TEST___resize(const uint32_t, const uint32_t)
{
  if (m_deviceQueue->GetVulkanDevice() == VK_NULL_HANDLE)
  {
    return false;
  }

  VkSurfaceCapabilitiesKHR surface_properties;
  if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_deviceQueue->GetVulkanPhysicalDevice(),
                                                GetVulkanSurface(),
                                                &surface_properties) != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to get surface capabilities");
    return false;
  }

  // Only rebuild the swapchain if the dimensions have changed
  if (surface_properties.currentExtent.width == m_width &&
      surface_properties.currentExtent.height == m_height)
  {
    return false;
  }

  vkDeviceWaitIdle(m_deviceQueue->GetVulkanDevice());

  TEST___init_swapchain();
  return true;
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
