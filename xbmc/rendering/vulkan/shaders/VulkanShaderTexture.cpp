/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanShaderTexture.h"

#include "ServiceBroker.h"
#include "rendering/vulkan/VulkanDeviceQueue.h"
#include "rendering/vulkan/utils/VulkanInitStructs.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "utils/log.h"
#include "windowing/WinSystem.h"

#include <cassert>

#define DEFAULT_FENCE_TIMEOUT 100000000000

using namespace KODI::RENDERING::VULKAN::UTILS;

namespace KODI::RENDERING::VULKAN
{

namespace
{
//constexpr const char* kVertexShaderFile = "vulkan_shader_gr1_vert.spv";
//constexpr const char* kFragmentShaderFile = "vulkan_shader_gr1_fraq_texture.spv";

constexpr const char* kVertexShaderFile = "triangle.vert.spv";
constexpr const char* kFragmentShaderFile = "triangle.frag.spv";

} // namespace

CVulkanShaderTexture::CVulkanShaderTexture(const VulkanData* vkData,
                                           CVulkanDeviceQueue* deviceQueue)
  : IVulkanShader(vkData, deviceQueue)
{
}

bool CVulkanShaderTexture::CreatePipelineLayout()
{
  std::array<VkDescriptorSetLayout, 2> setLayouts = {
      m_vkData->vkDescriptorSetLayout_Uniform,
      m_vkData->vkDescriptorSetLayout_Texture,
  };

  // Push constant ranges
  std::vector<VkPushConstantRange> pushConstantRanges = {
      // Push constant for fragment shader about color
      vkPushConstantRange(VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(glm::vec4), 0),
  };

  VkPipelineLayoutCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  info.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
  info.pSetLayouts = setLayouts.data();
  info.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
  info.pPushConstantRanges = pushConstantRanges.data();

  VK_CHECK_RESULT(vkCreatePipelineLayout(m_vkData->vkDevice, &info, nullptr, &m_vkPipelineLayout),
                  false);
  return true;
}

bool CVulkanShaderTexture::CreatePipeline()
{
  // Pipeline
  std::vector<VkDynamicState> enables = {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR,
      //VK_DYNAMIC_STATE_COLOR_BLEND_ENABLE_EXT,
  };
  auto dynState = vkPipelineDynamicStateCreateInfo(enables.data(), enables.size());
  auto inputAssembly = vkPipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

  VkPipelineColorBlendAttachmentState blendAttachmentState = {};
  // Enabled blending (With the background)
  //
  //
  // See CGUIFontTTFGL::FirstBegin for rationale. SDR uses accumulator
  // coverage alpha; HDR FBO composite uses a compensated squared-alpha
  // blend because the FBO is color-transformed to PQ before composite,
  // and alpha blending in non-linear space is mathematically wrong.
  if (CServiceBroker::GetWinSystem()->IsHdrComposite())
  {
    blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
  }
  else
  {
    blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
  }
  blendAttachmentState.blendEnable = VK_TRUE;
  blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  auto colorBlendState = vkPipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
  auto viewportState = vkPipelineViewportStateCreateInfo(1, 1, 0);
  auto multisampleState = vkPipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);
  auto raster = vkPipelineRasterStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE,
                                                VK_FRONT_FACE_COUNTER_CLOCKWISE);
  auto depthStencilState =
      vkPipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);

  // Vertex input state
  std::vector<VkVertexInputBindingDescription> vertexInputBindings = {
      vkVertexInputBindingDescr(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX),
  };
  std::vector<VkVertexInputAttributeDescription> inputAttributs{
      vkVertexInputAttrDescr(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, in_attrpos)),
      //vkVertexInputAttrDescr(0, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, in_attrcol)),
      vkVertexInputAttrDescr(0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, in_attrcord0)),
      vkVertexInputAttrDescr(0, 3, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, in_attrcord1)),
  };

  // Vertex input state used for pipeline creation
  auto inputState = vkPipelineVertexInputStateCreateInfo();
  inputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindings.size());
  inputState.pVertexBindingDescriptions = vertexInputBindings.data();
  inputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(inputAttributs.size());
  inputState.pVertexAttributeDescriptions = inputAttributs.data();

  // Load Vertex and Fragment shaders
  std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};
  shaderStages[0] = LoadShader(kVertexShaderFile, VK_SHADER_STAGE_VERTEX_BIT);
  shaderStages[1] = LoadShader(kFragmentShaderFile, VK_SHADER_STAGE_FRAGMENT_BIT);
  if (shaderStages[0].module == VK_NULL_HANDLE || shaderStages[1].module == VK_NULL_HANDLE)
  {
    CLog::Log(LOGERROR, "Failed to load shaders: {} and {}", kVertexShaderFile,
              kFragmentShaderFile);
    return false;
  }

  auto pipeline = vkPipelineCreateInfo(m_vkPipelineLayout, m_vkData->vkRenderPass);
  pipeline.stageCount = static_cast<uint32_t>(shaderStages.size());
  pipeline.pStages = shaderStages.data();
  pipeline.pVertexInputState = &inputState;
  pipeline.pInputAssemblyState = &inputAssembly;
  pipeline.pRasterizationState = &raster;
  pipeline.pColorBlendState = &colorBlendState;
  pipeline.pMultisampleState = &multisampleState;
  pipeline.pViewportState = &viewportState;
  pipeline.pDepthStencilState = &depthStencilState;
  pipeline.pDynamicState = &dynState;
  VK_CHECK_RESULT(vkCreateGraphicsPipelines(m_vkData->vkDevice, m_vkData->vkPipelineCache, 1,
                                            &pipeline, nullptr, &m_vkPipeline),
                  false);

  // Shader modules are no longer needed once the graphics m_pipeline has been created
  UnloadShader(shaderStages[0]);
  UnloadShader(shaderStages[1]);

  return true;
}

} // namespace KODI::RENDERING::VULKAN
