/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanShaderFonts.h"

#include "ServiceBroker.h"
#include "rendering/vulkan/VulkanDeviceQueue.h"
#include "rendering/vulkan/utils/VulkanInitStructs.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "utils/log.h"
#include "windowing/WinSystem.h"

#include <array>
#include <cassert>

using namespace KODI::RENDERING::VULKAN::UTILS;

namespace KODI::RENDERING::VULKAN
{

namespace
{

constexpr const char* kVertexShaderFile = "fonts_vert.spv";
constexpr const char* kVertexShaderFileClip = "fonts_vert_clip.spv";
constexpr const char* kFragmentShaderFile = "fonts_frag.spv";

} // namespace

CVulkanShaderFonts::CVulkanShaderFonts(const VulkanData* vkData,
                                           CVulkanDeviceQueue* deviceQueue)
  : IVulkanShader(vkData, deviceQueue)
{
}

bool CVulkanShaderFonts::CreatePipelineLayout()
{
  std::vector<VkPushConstantRange> pushConstantRanges;
  std::array<VkDescriptorSetLayout, 2> setLayouts = {
      m_vkData->vkDescriptorSetLayout_Uniform,
      m_vkData->vkDescriptorSetLayout_Texture,
  };

  VkPipelineLayoutCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  info.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
  info.pSetLayouts = setLayouts.data();

  VK_CHECK_RESULT(vkCreatePipelineLayout(m_vkData->vkDevice, &info, nullptr,
                                         &m_vkPipelineLayout),
                  false);

  return true;
}

void CVulkanShaderFonts::DestroyPipelineLayout()
{
  if (m_vkPipelineLayout != VK_NULL_HANDLE)
  {
    vkDestroyPipelineLayout(m_vkData->vkDevice, m_vkPipelineLayout, nullptr);
    m_vkPipelineLayout = VK_NULL_HANDLE;
  }
}

bool CVulkanShaderFonts::CreatePipeline()
{
  using Vertex = KODI::RENDERING::VULKAN::ShaderFontsVertex;

  //================================================================================================
  /// Pipeline creation info
  ///
  /// Becomes used by call of @ref vkCreateGraphicsPipelines
  ///
  /// Following set in the pipeline creation calls below
  /// - pipelineCreateInfo.stageCount;
  /// - pipelineCreateInfo.pStages;
  /// - pipelineCreateInfo.pVertexInputState;
  /// - pipelineCreateInfo.pInputAssemblyState;
  /// - pipelineCreateInfo.pTessellationState;
  /// - pipelineCreateInfo.pViewportState;
  /// - pipelineCreateInfo.pRasterizationState;
  /// - pipelineCreateInfo.pMultisampleState;
  /// - pipelineCreateInfo.pDepthStencilState;
  /// - pipelineCreateInfo.pColorBlendState;
  /// - pipelineCreateInfo.pDynamicState;
  ///@{

  VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
  pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineCreateInfo.flags = 0;
  pipelineCreateInfo.renderPass = m_vkData->vkRenderPass;
  pipelineCreateInfo.subpass = 0;
  pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineCreateInfo.basePipelineIndex = -1;
  pipelineCreateInfo.layout = m_vkPipelineLayout;

  ///@}
  //------------------------------------------------------------------------------------------------

  //================================================================================================
  /// Pipeline dynamic state
  ///@{

  std::vector<VkDynamicState> dynamicStates = {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR,
      //VK_DYNAMIC_STATE_COLOR_BLEND_ENABLE_EXT,
  };

  VkPipelineDynamicStateCreateInfo dynState{};
  dynState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynState.pDynamicStates = dynamicStates.data();
  dynState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynState.flags = 0;

  // Set VkGraphicsPipelineCreateInfo::pDynamicState to the dynamic state info
  pipelineCreateInfo.pDynamicState = &dynState;

  ///@}
  //------------------------------------------------------------------------------------------------

  //================================================================================================
  /// Pipeline input assembly state
  ///@{

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.flags = 0;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  // Set VkGraphicsPipelineCreateInfo::pInputAssemblyState to the input assembly state info
  pipelineCreateInfo.pInputAssemblyState = &inputAssembly;

  ///@}
  //------------------------------------------------------------------------------------------------

  //================================================================================================
  /// Pipeline color blend state
  ///
  /// The @ref CWinSystemBase::IsHdrComposite is set during the initialization of the windowing system and can be set here as
  /// it no more changes during the lifetime of the application.
  ///
  /// See @ref CVulkanGUIFontTTF::FirstBegin for rationale. SDR uses accumulator
  /// coverage alpha; HDR FBO composite uses a compensated squared-alpha
  /// blend because the FBO is color-transformed to PQ before composite,
  /// and alpha blending in non-linear space is mathematically wrong.
  ///@{

  VkPipelineColorBlendAttachmentState blendAttachmentState = {};
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

  VkPipelineColorBlendStateCreateInfo colorBlendState{};
  colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendState.flags = 0;
  colorBlendState.logicOpEnable = VK_FALSE;
  colorBlendState.logicOp = VK_LOGIC_OP_COPY; //VK_LOGIC_OP_CLEAR
  colorBlendState.attachmentCount = 1;
  colorBlendState.pAttachments = &blendAttachmentState;
  glm::vec4 blendConstants(0.0f, 0.0f, 0.0f, 0.0f);
  colorBlendState.blendConstants[0] = blendConstants.r;
  colorBlendState.blendConstants[1] = blendConstants.g;
  colorBlendState.blendConstants[2] = blendConstants.b;
  colorBlendState.blendConstants[3] = blendConstants.a;

  // Set VkGraphicsPipelineCreateInfo::pColorBlendState to the color blend state info
  pipelineCreateInfo.pColorBlendState = &colorBlendState;

  ///@}
  //------------------------------------------------------------------------------------------------

  //================================================================================================
  // Pipeline viewport state
  ///@{

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.flags = 0;
  viewportState.viewportCount = 1;
  viewportState.pViewports = nullptr; // Set dynamically
  viewportState.scissorCount = 1;
  viewportState.pScissors = nullptr; // Set dynamically

  // Set VkGraphicsPipelineCreateInfo::pViewportState to the viewport state info
  pipelineCreateInfo.pViewportState = &viewportState;

  ///@}
  //------------------------------------------------------------------------------------------------

  //================================================================================================
  // Multisample state
  ///@{

  VkPipelineMultisampleStateCreateInfo multisampleState = {};
  multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampleState.flags = 0;
  multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampleState.sampleShadingEnable = VK_FALSE;
  multisampleState.minSampleShading = 1.0f;
  multisampleState.pSampleMask = nullptr;
  multisampleState.alphaToCoverageEnable = VK_FALSE;
  multisampleState.alphaToOneEnable = VK_FALSE;

  // Set VkGraphicsPipelineCreateInfo::pMultisampleState to the multisample state info
  pipelineCreateInfo.pMultisampleState = &multisampleState;

  ///@}
  //------------------------------------------------------------------------------------------------

  //================================================================================================
  // Rasterization state
  ///@{

  VkPipelineRasterizationStateCreateInfo raster = {};
  raster.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  raster.flags = 0;
  raster.depthClampEnable = VK_FALSE;
  raster.rasterizerDiscardEnable = VK_FALSE;
  raster.polygonMode = VK_POLYGON_MODE_FILL;
  raster.cullMode = VK_CULL_MODE_NONE;
  raster.frontFace = VK_FRONT_FACE_CLOCKWISE;
  raster.depthBiasEnable = VK_FALSE;
  raster.depthBiasConstantFactor = 0.0f;
  raster.depthBiasClamp = 0.0f;
  raster.depthBiasSlopeFactor = 0.0f;
  raster.lineWidth = 1.0f;

  // Set VkGraphicsPipelineCreateInfo::pRasterizationState to the rasterization state info
  pipelineCreateInfo.pRasterizationState = &raster;

  ///@}
  //------------------------------------------------------------------------------------------------

  //================================================================================================
  // Depth stencil state
  ///@{

  VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
  depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencilState.flags = 0;
  depthStencilState.depthTestEnable = VK_TRUE;
  depthStencilState.depthWriteEnable = VK_TRUE;
  depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
  depthStencilState.depthBoundsTestEnable = VK_FALSE;
  depthStencilState.stencilTestEnable = VK_FALSE;

  depthStencilState.front.failOp = VK_STENCIL_OP_KEEP;
  depthStencilState.front.passOp = VK_STENCIL_OP_KEEP;
  depthStencilState.front.depthFailOp = VK_STENCIL_OP_INCREMENT_AND_CLAMP;
  depthStencilState.front.compareOp = VK_COMPARE_OP_ALWAYS;
  depthStencilState.front.compareMask = 0xff;
  depthStencilState.front.writeMask = 0xff;
  depthStencilState.front.reference = 0;

  depthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
  depthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
  depthStencilState.back.depthFailOp = VK_STENCIL_OP_DECREMENT_AND_CLAMP;
  depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
  depthStencilState.back.compareMask = 0xff;
  depthStencilState.back.writeMask = 0xff;
  depthStencilState.back.reference = 0;

  depthStencilState.minDepthBounds = 0.0f;
  depthStencilState.maxDepthBounds = 1.0f;

  // Set VkGraphicsPipelineCreateInfo::pDepthStencilState to the depth stencil state info
  pipelineCreateInfo.pDepthStencilState = &depthStencilState;

  ///@}
  //------------------------------------------------------------------------------------------------

  //================================================================================================
  /// Vertex input state
  ///
  /// In the vertex shader the following input attributes are defined:
  /// ```glsl
  /// layout(location = 0) in vec3 in_attrpos;
  /// layout(location = 1) in vec2 in_attrcord0;
  /// layout(location = 2) in vec2 in_attrcord1;
  /// ```
  ///@{

  std::vector<VkVertexInputBindingDescription> vertexInputBindings = {
      vkVertexInputBindingDescr(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX),
  };
  std::vector<VkVertexInputAttributeDescription> inputAttributs{
      vkVertexInputAttrDescr(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, in_attrpos)),
      vkVertexInputAttrDescr(0, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, in_attrcolor)),
      vkVertexInputAttrDescr(0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, in_attrcord0)),
  };

  // Vertex input state used for pipeline creation
  VkPipelineVertexInputStateCreateInfo inputState = {};
  inputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  inputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindings.size());
  inputState.pVertexBindingDescriptions = vertexInputBindings.data();
  inputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(inputAttributs.size());
  inputState.pVertexAttributeDescriptions = inputAttributs.data();

  // Set VkGraphicsPipelineCreateInfo::pVertexInputState to the vertex input state info
  pipelineCreateInfo.pVertexInputState = &inputState;

  ///@}
  //------------------------------------------------------------------------------------------------

  //================================================================================================
  /// Load Vertex and Fragment shaders
  ///
  ///@{

  // Prepare specialization data for fragment shader to enable/disable about
  // SPIR-V where becomes set during final compile to set limited color output
  // based on system settings.
  struct SpecializationData
  {
    uint32_t useLimitedColor{CServiceBroker::GetWinSystem()->UseLimitedColor() &&
                             !CServiceBroker::GetWinSystem()->IsHdrComposite()};
  } specializationData;

  std::array<VkSpecializationMapEntry, 1> specializationMapEntries;

  // Map the specialization data to the SPIR-V constant ID 0
  //
  // In the fragment shader the following specialization constant is defined:
  //```
  // layout(constant_id = 0) const bool useLimitedColor = false;
  //```

  specializationMapEntries[0].constantID = 0;
  specializationMapEntries[0].size = sizeof(specializationData.useLimitedColor);
  specializationMapEntries[0].offset = 0;

  VkSpecializationInfo specializationInfo = {};
  specializationInfo.mapEntryCount = static_cast<uint32_t>(specializationMapEntries.size());
  specializationInfo.pMapEntries = specializationMapEntries.data();
  specializationInfo.dataSize = sizeof(specializationData);
  specializationInfo.pData = &specializationData;

  // Load the vertex and fragment shader modules
  std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};
  pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
  pipelineCreateInfo.pStages = shaderStages.data();

  ///@}
  //------------------------------------------------------------------------------------------------

  auto vertShader = LoadShader(kVertexShaderFile, VK_SHADER_STAGE_VERTEX_BIT);
  auto vertShaderClip = LoadShader(kVertexShaderFileClip, VK_SHADER_STAGE_VERTEX_BIT);
  auto fragShader = LoadShader(kFragmentShaderFile, VK_SHADER_STAGE_FRAGMENT_BIT);
  if (vertShader.module == VK_NULL_HANDLE || vertShaderClip.module == VK_NULL_HANDLE ||
      fragShader.module == VK_NULL_HANDLE)
  {
    std::string failedShaders;
    if (vertShader.module == VK_NULL_HANDLE)
    {
      failedShaders += kVertexShaderFile;
      if (vertShaderClip.module == VK_NULL_HANDLE || fragShader.module == VK_NULL_HANDLE)
        failedShaders += ", ";
    }
    if (vertShaderClip.module == VK_NULL_HANDLE)
    {
      failedShaders += kVertexShaderFileClip;
      if (fragShader.module == VK_NULL_HANDLE)
        failedShaders += ", ";
    }
    if (fragShader.module == VK_NULL_HANDLE)
      failedShaders += kFragmentShaderFile;
    CLog::Log(LOGERROR, "Vulkan: Failed to load shaders: {0} ({1}:{2})", failedShaders, __FILENAME__,
              __LINE__);
    return false;
  }

  fragShader.pSpecializationInfo = &specializationInfo;


  shaderStages[0] = vertShader;
  shaderStages[1] = fragShader;
  VK_CHECK_RESULT(vkCreateGraphicsPipelines(m_vkData->vkDevice, m_vkData->vkPipelineCache, 1,
                                            &pipelineCreateInfo, nullptr,
                                            &m_vkPipeline[FONTS_TYPE_SCISSOR_CLIP]),
                  false);

  shaderStages[0] = vertShaderClip;
  VK_CHECK_RESULT(vkCreateGraphicsPipelines(m_vkData->vkDevice, m_vkData->vkPipelineCache, 1,
                                            &pipelineCreateInfo, nullptr,
                                            &m_vkPipeline[FONTS_TYPE_SHADER_CLIP]),
                  false);


  // Shader modules are no longer needed once the graphics m_pipeline has been created
  UnloadShader(vertShader);
  UnloadShader(vertShaderClip);
  UnloadShader(fragShader);

  return true;
}

void CVulkanShaderFonts::DestroyPipeline()
{
  for (auto& pipeline : m_vkPipeline)
  {
    if (pipeline != VK_NULL_HANDLE)
    {
      vkDestroyPipeline(m_vkData->vkDevice, pipeline, nullptr);
      pipeline = VK_NULL_HANDLE;
    }
  }
}

bool CVulkanShaderFonts::CreateUniformBuffers()
{
  assert(m_vkData->vkDescriptorPool != VK_NULL_HANDLE);
  assert(m_vkData->vkDescriptorSetLayout_Uniform != VK_NULL_HANDLE);

  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = m_vkData->vkDescriptorPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = &m_vkData->vkDescriptorSetLayout_Uniform;

  for (auto& entry : m_uniformBuffers)
  {
    VK_CHECK_RESULT(m_deviceQueue->CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                &entry, sizeof(VulkanUniformShaderClip), nullptr),
                    false);

    VK_CHECK_RESULT(vkBindBufferMemory(m_vkData->vkDevice, entry.buffer, entry.memory, 0), false);
    VK_CHECK_RESULT(vkMapMemory(m_vkData->vkDevice, entry.memory, 0,
                                sizeof(VulkanUniformShaderClip), 0,
                                (void**)&entry.mapped),
                    false);

    VK_CHECK_RESULT(vkAllocateDescriptorSets(m_vkData->vkDevice, &allocInfo, &entry.descriptorSet),
                    false);

    // The m_buffer's information is passed using a m_descriptor info structure
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = entry.buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(VulkanUniformShaderClip);

    VkWriteDescriptorSet uboWrite{};
    uboWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    uboWrite.dstSet = entry.descriptorSet;
    uboWrite.dstBinding = 0;
    uboWrite.dstArrayElement = 0;
    uboWrite.descriptorCount = 1;
    uboWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    uboWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(m_vkData->vkDevice, 1, &uboWrite, 0, nullptr);
  }

  return true;
}

void CVulkanShaderFonts::DestroyUniformBuffers()
{
  for (auto& entry : m_uniformBuffers)
  {
    if (entry.mapped)
    {
      vkUnmapMemory(m_vkData->vkDevice, entry.memory);
      entry.mapped = nullptr;
    }
    m_deviceQueue->DestroyBuffer(&entry);
    entry = {};
  }
}

void CVulkanShaderFonts::UpdateUniformBuffer(uint32_t index, const VulkanUniformScissorClip& uniformData)
{
  //VK_CHECK_RESULT(vkMapMemory(m_vkData->vkDevice, m_uniformBuffers[index].memory, 0, sizeof(VulkanUniformShaderClip),
  //                            0, (void**)&m_uniformBuffers[index].mapped));

  memcpy(m_uniformBuffers[index].mapped, &uniformData, sizeof(VulkanUniformScissorClip));

  //    VkMappedMemoryRange mappedRange{
  //    .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
  //    .pNext = nullptr,
  //    .memory = m_uniformBuffers[index].memory,
  //    .offset = 0,
  //    .size = sizeof(VulkanUniformScissorClip),
  //};
  //vkFlushMappedMemoryRanges(m_vkData->vkDevice, 1, &mappedRange);

  //vkUnmapMemory(m_vkData->vkDevice, m_uniformBuffers[index].memory);
  //VK_CHECK_RESULT(vkBindBufferMemory(m_vkData->vkDevice, m_uniformBuffers[index].buffer,
  //                                   m_uniformBuffers[index].memory, 0));
}

void CVulkanShaderFonts::UpdateUniformBuffer(uint32_t index, const VulkanUniformShaderClip& uniformData)
{
  memcpy(m_uniformBuffers[index].mapped, &uniformData, sizeof(VulkanUniformShaderClip));
}

} // namespace KODI::RENDERING::VULKAN
