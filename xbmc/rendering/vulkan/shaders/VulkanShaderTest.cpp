/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanShaderTest.h"

#include "rendering/vulkan/VulkanDeviceQueue.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "utils/log.h"

#include <array>
#include <cassert>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

using KODI::RENDERING::VULKAN::UTILS::ErrorString;

CVulkanShaderTest::CVulkanShaderTest(const VulkanData* vkData,
                                     CVulkanDeviceQueue* deviceQueue)
  : IVulkanShader(vkData, deviceQueue)
{
}

bool CVulkanShaderTest::Create()
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

  m_vkPipelineLayout = CreatePipelineLayout();
  //const VkDeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();

  // The Vertex input properties define the interface between the vertex buffer and the vertex shader.
  m_deviceQueue->CreateBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                               &m_vertexBuffer, sizeof(Vertex) * 3, vertices.data());

  // Specify we will use triangle lists to draw geometry.
  VkPipelineInputAssemblyStateCreateInfo input_assembly{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      .primitiveRestartEnable = VK_FALSE};

  // Define the vertex input binding.
  VkVertexInputBindingDescription binding_description{
      .binding = 0,
      .stride = sizeof(Vertex),
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  };

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
      .module =
          UTILS::vulkanCreateShaderModule(m_vkData->vkDevice, "vulkan_shader_gr0_vert_test_triangle.spv"),
      .pName = "main",
      .pSpecializationInfo = nullptr,
  };

  // Fragment stage of the pipeline
  shader_stages[1] = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module =
          UTILS::vulkanCreateShaderModule(m_vkData->vkDevice, "vulkan_shader_gr0_frag_test_triangle.spv"),
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
      .layout = m_vkPipelineLayout,
      .renderPass = m_vkData->vkRenderPass,
      .subpass = 0,
      .basePipelineHandle = VK_NULL_HANDLE,
      .basePipelineIndex = -1,
  };

  VK_CHECK_RESULT(
      vkCreateGraphicsPipelines(m_vkData->vkDevice, m_vkData->vkPipelineCache, 1, &pipe, nullptr, &m_vkPipeline),
      false);

  // Pipeline is baked, we can delete the shader modules now.
  vkDestroyShaderModule(m_vkData->vkDevice, shader_stages[0].module, nullptr);
  vkDestroyShaderModule(m_vkData->vkDevice, shader_stages[1].module, nullptr);

  return true;
}

void CVulkanShaderTest::Destroy()
{
  if (m_vkPipelineLayout != VK_NULL_HANDLE)
  {
    vkDestroyPipelineLayout(m_vkData->vkDevice, m_vkPipelineLayout, nullptr);
    m_vkPipelineLayout = VK_NULL_HANDLE;
  }
  if (m_vkPipeline != VK_NULL_HANDLE)
  {
    // Destroy the Vulkan pipeline
    vkDestroyPipeline(m_vkData->vkDevice, m_vkPipeline, nullptr);
    m_vkPipeline = VK_NULL_HANDLE;
  }
}

VkPipelineLayout CVulkanShaderTest::CreatePipelineLayout(VkDescriptorSetLayout layout)
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
  VK_CHECK_RESULT(
      vkCreatePipelineLayout(m_vkData->vkDevice, &layout_info, nullptr, &pipeline_layout),
      VK_NULL_HANDLE);

  return pipeline_layout;
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
