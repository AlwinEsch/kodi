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
#include "VulkanSurface.h"
#include "VulkanSwapChain.h"
#include "VulkanUtils.h"
#include "filesystem/File.h"
#include "filesystem/SpecialProtocol.h"
#include "guilib/DirtyRegion.h"
#include "guilib/graphics/vulkan/VulkanGUITexture.h"
#include "platform/MessagePrinter.h"
#include "rendering/vulkan/VulkanDeviceQueue.h"
#include "rendering/vulkan/VulkanInstance.h"
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

CVulkanRenderSystem::CVulkanRenderSystem() : CRenderSystemBase()
{
}

CVulkanRenderSystem::~CVulkanRenderSystem() = default;

bool CVulkanRenderSystem::InitRenderSystem()
{
  fprintf(stderr, "->>>>>>>>>>>>>>>>>>>>>>>--> %s (%u, %u)\n", __PRETTY_FUNCTION__, m_size.width, m_size.height);
  // VkSurfaceKHR becomes created in the platform-specific implementation of CVulkanRenderSystem,
  // so we need to get it from there.
  // Further confirm with an assert that it is not null and child classes have initialized it.
  VkSurfaceKHR surface = GetVulkanSurface();
  assert(surface);

  // Get the required and optional device extensions from the platform-specific implementation of
  // CVulkanRenderSystem.
  std::vector<const char*> requiredExtensions = GetRequiredDeviceExtensions();
  std::vector<const char*> optionalExtensions = GetOptionalDeviceExtensions();

  // Set the GPU vendor and device IDs to 0, which means any GPU will be accepted.
  // TODO: In the future, we may want to allow select a specific GPU, automatically or manually.
  uint32_t gpuVendorId{0};
  uint32_t gpuDeviceId{0};

  // Create now the device queue, which will handle the Vulkan device and queue creation.
  m_deviceQueue = std::make_unique<CVulkanDeviceQueue>(this);
  if (!m_deviceQueue->Initialize(DeviceQueueOption::GRAPHICS_QUEUE_FLAG |
                                     DeviceQueueOption::PRESENTATION_SUPPORT_QUEUE_FLAG,
                                 gpuVendorId, gpuDeviceId, requiredExtensions, optionalExtensions,
                                 0, false, false))
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to initialize device queue");
    return false;
  }

  // Create now the other Vulkan objects, such as the surface, swap chain, and pipeline where are
  // independent from OS type. The Vulkan surface is created in the platform-specific implementation
  // of CVulkanRenderSystem, so we need to get it from there and initialize it with the surface and
  // swap chain.

  m_surface =
      std::make_unique<CVulkanSurface>(GetVulkanInstance()->GetVkInstance(), GetVulkanSurface());
  if (!m_surface->InitializeSurface(m_deviceQueue.get(), SurfaceFormat::DEFAULT_SURFACE_FORMAT))
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to initialize Vulkan surface");
    return false;
  }

  m_surface->Reshape(m_size);

  /*









  */
  //auto instance = m_deviceQueue->GetVulkanInstance();
  //auto device = m_deviceQueue->GetVulkanDevice();
  //auto physicalDevice = m_deviceQueue->GetVulkanPhysicalDevice();

  //VkDeviceSize buffer_size = sizeof(TEST___vertices[0]) * TEST___vertices.size();
  //UTILS::vulkanCreateBuffer(
  //    instance, device, physicalDevice, buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
  //    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  //    TEST___vertex_buffer, TEST___vertex_buffer_memory);
  //// Map the memory and copy the vertex data
  //void* data;
  //if (vkMapMemory(device, TEST___vertex_buffer_memory, 0, buffer_size, 0, &data) != VK_SUCCESS)
  //{
  //  throw std::runtime_error("Failed to map vertex buffer memory");
  //}
  //memcpy(data, TEST___vertices.data(), static_cast<size_t>(buffer_size));
  //vkUnmapMemory(device, TEST___vertex_buffer_memory);

  //TEST___init_pipeline();
  /*









  */
  m_bRenderCreated = true;

  CVulkanGUITexture::Register();

  return true;
}

bool CVulkanRenderSystem::ResetRenderSystem(int width, int height)
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return false;

  if (static_cast<uint32_t>(width) == m_size.width &&
      static_cast<uint32_t>(height) == m_size.height)
    return true;

  m_size.width = static_cast<uint32_t>(width);
  m_size.height = static_cast<uint32_t>(height);

  m_surface->Reshape(m_size);

  return true;
}

bool CVulkanRenderSystem::DestroyRenderSystem()
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);

  m_deviceQueue.reset();

  //TEST___Deinit();

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

  m_surface->SwapBuffers();

  //TEST___update(0.0f);
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

//----------------------------------------------------------------------------------------------------------------------------------------------------------
//##########################################################################################################################################################
//----------------------------------------------------------------------------------------------------------------------------------------------------------
//void CVulkanRenderSystem::TEST___Deinit()
//{
//  // Don't release anything until the GPU is completely idle.
//  if (m_deviceQueue->GetVulkanDevice() != VK_NULL_HANDLE)
//  {
//    vkDeviceWaitIdle(m_deviceQueue->GetVulkanDevice());
//  }
//
//  for (auto& per_frame : TEST___per_frame)
//  {
//    TEST___teardown_per_frame(per_frame);
//  }
//
//  TEST___per_frame.clear();
//
//  for (auto semaphore : TEST___recycled_semaphores)
//  {
//    vkDestroySemaphore(m_deviceQueue->GetVulkanDevice(), semaphore, nullptr);
//  }
//
//  if (TEST___pipeline != VK_NULL_HANDLE)
//  {
//    vkDestroyPipeline(m_deviceQueue->GetVulkanDevice(), TEST___pipeline, nullptr);
//  }
//
//  if (TEST___pipeline_layout != VK_NULL_HANDLE)
//  {
//    vkDestroyPipelineLayout(m_deviceQueue->GetVulkanDevice(), TEST___pipeline_layout, nullptr);
//  }
//
//  for (VkImageView image_view : TEST___swapchain_image_views)
//  {
//    vkDestroyImageView(m_deviceQueue->GetVulkanDevice(), image_view, nullptr);
//  }
//
//  //m_surface->DeinitializeSwapChain();
//
//  if (TEST___vertex_buffer != VK_NULL_HANDLE)
//  {
//    vkDestroyBuffer(m_deviceQueue->GetVulkanDevice(), TEST___vertex_buffer, nullptr);
//    TEST___vertex_buffer = VK_NULL_HANDLE;
//  }
//
//  if (TEST___vertex_buffer_memory != VK_NULL_HANDLE)
//  {
//    vkFreeMemory(m_deviceQueue->GetVulkanDevice(), TEST___vertex_buffer_memory, nullptr);
//    TEST___vertex_buffer_memory = VK_NULL_HANDLE;
//  }
//}
//
//void CVulkanRenderSystem::TEST___init_pipeline()
//{
//  auto device = m_deviceQueue->GetVulkanDevice();
//
//  // Create a blank pipeline layout.
//  // We are not binding any resources to the pipeline in this first sample.
//  VkPipelineLayoutCreateInfo layout_info{.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
//                                         .pNext = nullptr,
//                                         .flags = 0,
//                                         .setLayoutCount = 0,
//                                         .pSetLayouts = nullptr,
//                                         .pushConstantRangeCount = 0,
//                                         .pPushConstantRanges = nullptr};
//  if (vkCreatePipelineLayout(device, &layout_info, nullptr, &TEST___pipeline_layout) != VK_SUCCESS)
//  {
//    throw std::runtime_error("Failed to create pipeline layout");
//  }
//
//  // Define the vertex input binding description
//  VkVertexInputBindingDescription binding_description{
//      .binding = 0, .stride = sizeof(Vertex), .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};
//
//  // Define the vertex input attribute descriptions
//  std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions{
//      {{.location = 0,
//        .binding = 0,
//        .format = VK_FORMAT_R32G32_SFLOAT,
//        .offset = offsetof(Vertex, position)},
//       {.location = 1,
//        .binding = 0,
//        .format = VK_FORMAT_R32G32B32_SFLOAT,
//        .offset = offsetof(Vertex, color)}}};
//
//  // Create the vertex input state
//  VkPipelineVertexInputStateCreateInfo vertex_input{
//      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
//      .pNext = nullptr,
//      .flags = 0,
//      .vertexBindingDescriptionCount = 1,
//      .pVertexBindingDescriptions = &binding_description,
//      .vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size()),
//      .pVertexAttributeDescriptions = attribute_descriptions.data()};
//
//  // Specify we will use triangle lists to draw geometry.
//  VkPipelineInputAssemblyStateCreateInfo input_assembly{
//      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
//      .pNext = nullptr,
//      .flags = 0,
//      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
//      .primitiveRestartEnable = VK_FALSE};
//
//  // Specify rasterization state.
//  VkPipelineRasterizationStateCreateInfo raster{
//      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
//      .pNext = nullptr,
//      .flags = 0,
//      .depthClampEnable = VK_FALSE,
//      .rasterizerDiscardEnable = VK_FALSE,
//      .polygonMode = VK_POLYGON_MODE_FILL,
//      .cullMode = VK_CULL_MODE_BACK_BIT,
//      .frontFace = VK_FRONT_FACE_CLOCKWISE,
//      .depthBiasEnable = VK_FALSE,
//      .depthBiasConstantFactor = 0.0f,
//      .depthBiasClamp = 0.0f,
//      .depthBiasSlopeFactor = 0.0f,
//      .lineWidth = 1.0f};
//
//  // Specify that these states will be dynamic, i.e. not part of pipeline state object.
//  std::vector<VkDynamicState> dynamic_states = {
//      VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_CULL_MODE,
//      VK_DYNAMIC_STATE_FRONT_FACE, VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY};
//
//  // Our attachment will write to all color channels, but no blending is enabled.
//  VkPipelineColorBlendAttachmentState blend_attachment{
//      .blendEnable = VK_FALSE,
//      .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
//      .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
//      .colorBlendOp = VK_BLEND_OP_ADD,
//      .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
//      .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
//      .alphaBlendOp = VK_BLEND_OP_ADD,
//      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
//                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};
//
//  VkPipelineColorBlendStateCreateInfo blend{
//      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
//      .pNext = nullptr,
//      .flags = 0,
//      .logicOpEnable = VK_FALSE,
//      .logicOp = VK_LOGIC_OP_COPY,
//      .attachmentCount = 1,
//      .pAttachments = &blend_attachment,
//      .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}};
//
//  // We will have one viewport and scissor box.
//  VkPipelineViewportStateCreateInfo viewport{
//      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
//      .pNext = nullptr,
//      .flags = 0,
//      .viewportCount = 1,
//      .pViewports = nullptr, // We will set the viewport dynamically.
//      .scissorCount = 1,
//      .pScissors = nullptr // We will set the scissor dynamically.
//  };
//
//  // Disable all depth testing.
//  VkPipelineDepthStencilStateCreateInfo depth_stencil{
//      .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
//      .pNext = nullptr,
//      .flags = 0,
//      .depthTestEnable = VK_FALSE,
//      .depthWriteEnable = VK_FALSE,
//      .depthCompareOp = VK_COMPARE_OP_NEVER,
//      .depthBoundsTestEnable = VK_FALSE,
//      .stencilTestEnable = VK_FALSE,
//      .front = {},
//      .back = {},
//      .minDepthBounds = 0.0f,
//      .maxDepthBounds = 1.0f};
//
//  // No multisampling.
//  VkPipelineMultisampleStateCreateInfo multisample{
//      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
//      .pNext = nullptr,
//      .flags = 0,
//      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
//      .sampleShadingEnable = VK_FALSE,
//      .minSampleShading = 1.0f,
//      .pSampleMask = nullptr,
//      .alphaToCoverageEnable = VK_FALSE,
//      .alphaToOneEnable = VK_FALSE};
//
//  VkPipelineDynamicStateCreateInfo dynamic_state_info{
//      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
//      .pNext = nullptr,
//      .flags = 0,
//      .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
//      .pDynamicStates = dynamic_states.data()};
//
//  // Load our SPIR-V shaders.
//
//  // Vertex stage of the pipeline
//  std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages{};
//
//  shader_stages[0] = {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
//                      .pNext = nullptr,
//                      .flags = 0,
//                      .stage = VK_SHADER_STAGE_VERTEX_BIT,
//                      .module = UTILS::vulkanCreateShaderModule(device, "triangle.vert.spv"),
//                      .pName = "main",
//                      .pSpecializationInfo = nullptr};
//
//  // Fragment stage of the pipeline
//  shader_stages[1] = {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
//                      .pNext = nullptr,
//                      .flags = 0,
//                      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
//                      .module = UTILS::vulkanCreateShaderModule(device, "triangle.frag.spv"),
//                      .pName = "main",
//                      .pSpecializationInfo = nullptr};
//
//  // Pipeline rendering info (for dynamic rendering).
//  VkPipelineRenderingCreateInfo pipeline_rendering_info{
//      .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
//      .pNext = nullptr,
//      .viewMask = 0,
//      .colorAttachmentCount = 1,
//      .pColorAttachmentFormats = &m_surface->GetSwapChain()->GetSwapchainFormat(),
//      .depthAttachmentFormat = VK_FORMAT_UNDEFINED,
//      .stencilAttachmentFormat = VK_FORMAT_UNDEFINED};
//
//  // Create the graphics pipeline.
//  VkGraphicsPipelineCreateInfo pipe{
//      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
//      .pNext = &pipeline_rendering_info,
//      .flags = 0,
//      .stageCount = static_cast<uint32_t>(shader_stages.size()),
//      .pStages = shader_stages.data(),
//      .pVertexInputState = &vertex_input,
//      .pInputAssemblyState = &input_assembly,
//      .pTessellationState = nullptr,
//      .pViewportState = &viewport,
//      .pRasterizationState = &raster,
//      .pMultisampleState = &multisample,
//      .pDepthStencilState = &depth_stencil,
//      .pColorBlendState = &blend,
//      .pDynamicState = &dynamic_state_info,
//      .layout = TEST___pipeline_layout, // We need to specify the pipeline layout up front
//      .renderPass = VK_NULL_HANDLE, // Since we are using dynamic rendering this will set as null
//      .subpass = 0, // We will be using the first subpass in the render pass
//      .basePipelineHandle = VK_NULL_HANDLE,
//      .basePipelineIndex = -1};
//
//  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipe, nullptr, &TEST___pipeline) !=
//      VK_SUCCESS)
//  {
//    throw std::runtime_error("Failed to create graphics pipeline");
//  }
//
//  // Pipeline is baked, we can delete the shader modules now.
//  vkDestroyShaderModule(device, shader_stages[0].module, nullptr);
//  vkDestroyShaderModule(device, shader_stages[1].module, nullptr);
//}
//
//void CVulkanRenderSystem::TEST___update(float delta_time)
//{
//  uint32_t index;
//
//  auto res = TEST___acquire_next_swapchain_image(&index);
//
//  // Handle outdated error in acquire.
//  if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR)
//  {
//    if (!TEST___resize(m_size.width, m_size.height))
//    {
//      CLog::Log(LOGERROR, "Vulkan: Resize failed");
//    }
//    res = TEST___acquire_next_swapchain_image(&index);
//  }
//
//  if (res != VK_SUCCESS)
//  {
//    vkQueueWaitIdle(m_deviceQueue->GetVulkanQueue());
//    return;
//  }
//
//  TEST___render_triangle(index);
//  res = TEST___present_image(index);
//
//  // Handle Outdated error in present.
//  if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR)
//  {
//    if (!TEST___resize(m_size.width, m_size.height))
//    {
//      CLog::Log(LOGINFO, "Vulkan: Resize failed");
//    }
//  }
//  else if (res != VK_SUCCESS)
//  {
//    CLog::Log(LOGERROR, "Vulkan: Failed to present swapchain image.");
//  }
//}
//
//void CVulkanRenderSystem::TEST___render_triangle(uint32_t swapchain_index)
//{
//  // Allocate or re-use a primary command buffer.
//  VkCommandBuffer cmd = TEST___per_frame[swapchain_index].primary_command_buffer;
//
//  // We will only submit this once before it's recycled.
//  VkCommandBufferBeginInfo begin_info{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
//                                      .pNext = nullptr,
//                                      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
//                                      .pInheritanceInfo = nullptr};
//
//  // Begin command recording
//  if (vkBeginCommandBuffer(cmd, &begin_info) != VK_SUCCESS)
//  {
//    throw std::runtime_error("Failed to begin command buffer");
//  }
//
//  // Before starting rendering, transition the swapchain image to COLOR_ATTACHMENT_OPTIMAL
//  TEST___transition_image_layout(cmd, TEST___swapchain_images[swapchain_index],
//                                 VK_IMAGE_LAYOUT_UNDEFINED,
//                                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
//                                 0, // srcAccessMask (no need to wait for previous operations)
//                                 VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, // dstAccessMask
//                                 VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, // srcStage
//                                 VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT // dstStage
//  );
//  // Set clear color values.
//  VkClearValue clear_value{.color = {{0.01f, 0.01f, 0.033f, 1.0f}}};
//
//  // Set up the rendering attachment info
//  VkRenderingAttachmentInfo color_attachment{
//      .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
//      .pNext = nullptr,
//      .imageView = TEST___swapchain_image_views[swapchain_index],
//      .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
//      .resolveMode = VK_RESOLVE_MODE_NONE,
//      .resolveImageView = VK_NULL_HANDLE,
//      .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
//      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
//      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
//      .clearValue = clear_value};
//
//  // Begin rendering
//  VkRenderingInfo rendering_info{
//      .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
//      .pNext = nullptr,
//      .flags = 0,
//      .renderArea =
//          {// Initialize the nested `VkRect2D` structure
//           .offset = {0, 0}, // Initialize the `VkOffset2D` inside `renderArea`
//           .extent =
//               {// Initialize the `VkExtent2D` inside `renderArea`
//                .width = m_size.width,
//                .height = m_size.height}},
//      .layerCount = 1,
//      .viewMask = 0,
//      .colorAttachmentCount = 1,
//      .pColorAttachments = &color_attachment,
//      .pDepthAttachment = nullptr,
//      .pStencilAttachment = nullptr};
//
//  vkCmdBeginRendering(cmd, &rendering_info);
//
//  // Bind the graphics pipeline.
//  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, TEST___pipeline);
//
//  // Set dynamic states
//
//  // Set viewport dynamically
//  VkViewport vp{.x = 0.0f,
//                .y = 0.0f,
//                .width = static_cast<float>(m_size.width),
//                .height = static_cast<float>(m_size.height),
//                .minDepth = 0.0f,
//                .maxDepth = 1.0f};
//
//  vkCmdSetViewport(cmd, 0, 1, &vp);
//
//  // Set scissor dynamically
//  VkRect2D scissor{.offset = {.x = 0, .y = 0}, .extent = {m_size.width, m_size.height}};
//
//  vkCmdSetScissor(cmd, 0, 1, &scissor);
//
//  // Since we declared VK_DYNAMIC_STATE_CULL_MODE as dynamic in the pipeline,
//  // we need to set the cull mode here. VK_CULL_MODE_NONE disables face culling,
//  // meaning both front and back faces will be rendered.
//  vkCmdSetCullMode(cmd, VK_CULL_MODE_NONE);
//
//  // Since we declared VK_DYNAMIC_STATE_FRONT_FACE as dynamic,
//  // we need to specify the winding order considered as the front face.
//  // VK_FRONT_FACE_CLOCKWISE indicates that vertices defined in clockwise order
//  // are considered front-facing.
//  vkCmdSetFrontFace(cmd, VK_FRONT_FACE_CLOCKWISE);
//
//  // Since we declared VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY as dynamic,
//  // we need to set the primitive topology here. VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
//  // tells Vulkan that the input vertex data should be interpreted as a list of triangles.
//  vkCmdSetPrimitiveTopology(cmd, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
//
//  // Bind the vertex buffer
//  VkDeviceSize offset = {0};
//  vkCmdBindVertexBuffers(cmd, 0, 1, &TEST___vertex_buffer, &offset);
//
//  // Draw three vertices with one instance.
//  vkCmdDraw(cmd, static_cast<uint32_t>(TEST___vertices.size()), 1, 0, 0);
//
//  // Complete rendering.
//  vkCmdEndRendering(cmd);
//
//  // After rendering , transition the swapchain image to PRESENT_SRC
//  TEST___transition_image_layout(cmd, TEST___swapchain_images[swapchain_index],
//                                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
//                                 VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
//                                 VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, // srcAccessMask
//                                 0, // dstAccessMask
//                                 VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, // srcStage
//                                 VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT // dstStage
//  );
//
//  // Complete the command buffer.
//  if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
//  {
//    CLog::Log(LOGERROR, "Vulkan: Failed to end command buffer");
//    return;
//  }
//
//  // Submit it to the queue with a release semaphore.
//  if (TEST___per_frame[swapchain_index].swapchain_release_semaphore == VK_NULL_HANDLE)
//  {
//    VkSemaphoreCreateInfo semaphore_info = {
//        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, .pNext = nullptr, .flags = 0};
//    if (vkCreateSemaphore(m_deviceQueue->GetVulkanDevice(), &semaphore_info, nullptr,
//                          &TEST___per_frame[swapchain_index].swapchain_release_semaphore) !=
//        VK_SUCCESS)
//    {
//      CLog::Log(LOGERROR, "Vulkan: Failed to create release semaphore");
//      return;
//    }
//  }
//
//  // Using TOP_OF_PIPE here to ensure that the command buffer does not begin executing any pipeline stages
//  // (including the layout transition) until the swapchain image is actually acquired (signaled by the semaphore).
//  // This prevents the GPU from starting operations too early and guarantees that the image is ready
//  // before any rendering commands run.
//  VkPipelineStageFlags wait_stage{VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT};
//
//  VkSubmitInfo info{
//      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
//      .pNext = nullptr,
//      .waitSemaphoreCount = 1,
//      .pWaitSemaphores = &TEST___per_frame[swapchain_index].swapchain_acquire_semaphore,
//      .pWaitDstStageMask = &wait_stage,
//      .commandBufferCount = 1,
//      .pCommandBuffers = &cmd,
//      .signalSemaphoreCount = 1,
//      .pSignalSemaphores = &TEST___per_frame[swapchain_index].swapchain_release_semaphore};
//
//  // Submit command buffer to graphics queue
//  if (vkQueueSubmit(m_deviceQueue->GetVulkanQueue(), 1, &info,
//                    TEST___per_frame[swapchain_index].queue_submit_fence) != VK_SUCCESS)
//  {
//    CLog::Log(LOGERROR, "Vulkan: Failed to submit command buffer");
//    return;
//  }
//}
//
//void CVulkanRenderSystem::TEST___transition_image_layout(VkCommandBuffer cmd,
//                                                         VkImage image,
//                                                         VkImageLayout oldLayout,
//                                                         VkImageLayout newLayout,
//                                                         VkAccessFlags2 srcAccessMask,
//                                                         VkAccessFlags2 dstAccessMask,
//                                                         VkPipelineStageFlags2 srcStage,
//                                                         VkPipelineStageFlags2 dstStage)
//{
//  // Initialize the VkImageMemoryBarrier2 structure
//  VkImageMemoryBarrier2 image_barrier{
//      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
//      .pNext = nullptr,
//
//      // Specify the pipeline stages and access masks for the barrier
//      .srcStageMask = srcStage, // Source pipeline stage mask
//      .srcAccessMask = srcAccessMask, // Source access mask
//      .dstStageMask = dstStage, // Destination pipeline stage mask
//      .dstAccessMask = dstAccessMask, // Destination access mask
//
//      // Specify the old and new layouts of the image
//      .oldLayout = oldLayout, // Current layout of the image
//      .newLayout = newLayout, // Target layout of the image
//
//      // We are not changing the ownership between queues
//      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//
//      // Specify the image to be affected by this barrier
//      .image = image,
//
//      // Define the subresource range (which parts of the image are affected)
//      .subresourceRange = {
//          .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, // Affects the color aspect of the image
//          .baseMipLevel = 0, // Start at mip level 0
//          .levelCount = 1, // Number of mip levels affected
//          .baseArrayLayer = 0, // Start at array layer 0
//          .layerCount = 1 // Number of array layers affected
//      }};
//
//  // Initialize the VkDependencyInfo structure
//  VkDependencyInfo dependency_info{
//      .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
//      .pNext = nullptr,
//      .dependencyFlags = 0, // No special dependency flags
//      .memoryBarrierCount = 0, // No memory barriers
//      .pMemoryBarriers = nullptr, // No memory barriers
//      .bufferMemoryBarrierCount = 0, // No buffer memory barriers
//      .pBufferMemoryBarriers = nullptr, // No buffer memory barriers
//      .imageMemoryBarrierCount = 1, // Number of image memory barriers
//      .pImageMemoryBarriers = &image_barrier // Pointer to the image memory barrier(s)
//  };
//
//  // Record the pipeline barrier into the command buffer
//  vkCmdPipelineBarrier2(cmd, &dependency_info);
//}
//
//bool CVulkanRenderSystem::TEST___resize(const uint32_t, const uint32_t)
//{
//  if (m_deviceQueue->GetVulkanDevice() == VK_NULL_HANDLE)
//  {
//    return false;
//  }
//
//  VkSurfaceCapabilitiesKHR surface_properties;
//  if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_deviceQueue->GetVulkanPhysicalDevice(),
//                                                GetVulkanSurface(),
//                                                &surface_properties) != VK_SUCCESS)
//  {
//    CLog::Log(LOGERROR, "Vulkan: Failed to get surface capabilities");
//    return false;
//  }
//
//  // Only rebuild the swapchain if the dimensions have changed
//  if (surface_properties.currentExtent.width == m_size.width &&
//      surface_properties.currentExtent.height == m_size.height)
//  {
//    return false;
//  }
//
//  vkDeviceWaitIdle(m_deviceQueue->GetVulkanDevice());
//
//  //TEST___init_swapchain();
//  return true;
//}
//
//void CVulkanRenderSystem::TEST___teardown_per_frame(PerFrame& per_frame)
//{
//  if (per_frame.queue_submit_fence != VK_NULL_HANDLE)
//  {
//    vkDestroyFence(m_deviceQueue->GetVulkanDevice(), per_frame.queue_submit_fence, nullptr);
//
//    per_frame.queue_submit_fence = VK_NULL_HANDLE;
//  }
//
//  if (per_frame.primary_command_buffer != VK_NULL_HANDLE)
//  {
//    vkFreeCommandBuffers(m_deviceQueue->GetVulkanDevice(), per_frame.primary_command_pool, 1,
//                         &per_frame.primary_command_buffer);
//
//    per_frame.primary_command_buffer = VK_NULL_HANDLE;
//  }
//
//  if (per_frame.primary_command_pool != VK_NULL_HANDLE)
//  {
//    vkDestroyCommandPool(m_deviceQueue->GetVulkanDevice(), per_frame.primary_command_pool, nullptr);
//
//    per_frame.primary_command_pool = VK_NULL_HANDLE;
//  }
//
//  if (per_frame.swapchain_acquire_semaphore != VK_NULL_HANDLE)
//  {
//    vkDestroySemaphore(m_deviceQueue->GetVulkanDevice(), per_frame.swapchain_acquire_semaphore,
//                       nullptr);
//
//    per_frame.swapchain_acquire_semaphore = VK_NULL_HANDLE;
//  }
//
//  if (per_frame.swapchain_release_semaphore != VK_NULL_HANDLE)
//  {
//    vkDestroySemaphore(m_deviceQueue->GetVulkanDevice(), per_frame.swapchain_release_semaphore,
//                       nullptr);
//
//    per_frame.swapchain_release_semaphore = VK_NULL_HANDLE;
//  }
//}
//
//void CVulkanRenderSystem::TEST___init_per_frame(PerFrame& per_frame)
//{
//  VkFenceCreateInfo info{.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
//                         .pNext = nullptr,
//                         .flags = VK_FENCE_CREATE_SIGNALED_BIT};
//  if (vkCreateFence(m_deviceQueue->GetVulkanDevice(), &info, nullptr,
//                    &per_frame.queue_submit_fence) != VK_SUCCESS)
//  {
//    throw std::runtime_error("Failed to create fence for per frame data.");
//  }
//
//  VkCommandPoolCreateInfo cmd_pool_info{.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
//                                        .pNext = nullptr,
//                                        .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
//                                        .queueFamilyIndex =
//                                            static_cast<uint32_t>(TEST___graphics_queue_index)};
//  if (vkCreateCommandPool(m_deviceQueue->GetVulkanDevice(), &cmd_pool_info, nullptr,
//                          &per_frame.primary_command_pool) != VK_SUCCESS)
//  {
//    throw std::runtime_error("Failed to create command pool for per frame data.");
//  }
//
//  VkCommandBufferAllocateInfo cmd_buf_info{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
//                                           .pNext = nullptr,
//                                           .commandPool = per_frame.primary_command_pool,
//                                           .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
//                                           .commandBufferCount = 1};
//  if (vkAllocateCommandBuffers(m_deviceQueue->GetVulkanDevice(), &cmd_buf_info,
//                               &per_frame.primary_command_buffer) != VK_SUCCESS)
//  {
//    throw std::runtime_error("Failed to create command buffer for per frame data.");
//  }
//}
//
//VkResult CVulkanRenderSystem::TEST___present_image(uint32_t index)
//{
//  VkPresentInfoKHR present{
//      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
//      .pNext = nullptr,
//      .waitSemaphoreCount = 1,
//      .pWaitSemaphores = &TEST___per_frame[index].swapchain_release_semaphore,
//      .swapchainCount = 1,
//      .pSwapchains = &m_surface->GetSwapChain()->GetSwapchain(),
//      .pImageIndices = &index,
//      .pResults = nullptr,
//  };
//
//  // Present swapchain image
//  return vkQueuePresentKHR(m_deviceQueue->GetVulkanQueue(), &present);
//}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
