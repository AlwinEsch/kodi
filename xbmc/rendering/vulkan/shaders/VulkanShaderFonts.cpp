/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanShaderFonts.h"

#include "guilib/GUIFontTTF.h"
#include "rendering/vulkan/utils/VulkanInitStructs.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "utils/log.h"

#include <array>
#include <cassert>

namespace KODI::RENDERING::VULKAN
{

namespace
{
//constexpr const char* kVertexShaderFile = "vulkan_shader_gr3_vert_simple.spv";
//constexpr const char* kFragmentShaderFile = "vulkan_shader_gr3_frag_fonts.spv";
constexpr const char* kVertexShaderFile = "text.vert.spv";
constexpr const char* kFragmentShaderFile = "text.frag.spv";
//constexpr const char* kVertexShaderFile = "vulkan_shader_gr0_vert_test_triangle.spv";
//constexpr const char* kFragmentShaderFile = "vulkan_shader_gr0_frag_test_triangle.spv";
} /* namespace */

using namespace KODI::RENDERING::VULKAN::UTILS;

CVulkanShaderFonts::CVulkanShaderFonts(CVulkanDeviceQueue* deviceQueue,
                                       VkDevice device,
                                       VkPipelineLayout pipelineLayout,
                                       VkRenderPass renderPass)
  : IVulkanShader(deviceQueue),
    m_vkDevice(device),
    m_vkPipelineLayout(pipelineLayout),
    m_vkRenderPass(renderPass)
{
  assert(m_vkDevice != VK_NULL_HANDLE);
  assert(m_vkPipelineLayout != VK_NULL_HANDLE);
  assert(m_vkRenderPass != VK_NULL_HANDLE);
}

bool CVulkanShaderFonts::SetupFontTexture(uint32_t width, uint32_t height, uint32_t depth)
{
  m_fontTextureExtent = {width, height, depth};
  return true;
}

bool CVulkanShaderFonts::Create(const VkPipelineCache& pipelineCache)
{
  //VkResult res = VK_SUCCESS;

  //// Font texture
  //VkImageCreateInfo imageInfo = vkImageCreateInfo();
  //imageInfo.imageType = VK_IMAGE_TYPE_2D;
  //imageInfo.format = VK_FORMAT_R8_UNORM;
  //imageInfo.extent.width = m_fontTextureExtent.width;
  //imageInfo.extent.height = m_fontTextureExtent.height;
  //imageInfo.extent.depth = m_fontTextureExtent.depth;
  //imageInfo.mipLevels = 1;
  //imageInfo.arrayLayers = 1;
  //imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  //imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  //imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  //imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  //imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  //res = vkCreateImage(m_vkDevice, &imageInfo, nullptr, &m_image);
  //if (res != VK_SUCCESS)
  //{
  //  LogVulkanError(res, "vkCreateImage", __FILENAME__, __LINE__);
  //  return false;
  //}

  //VkImageViewCreateInfo imageViewInfo = vkImageViewCreateInfo();
  //imageViewInfo.image = m_image;
  //imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  //imageViewInfo.format = imageInfo.format;
  //imageViewInfo.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
  //                            VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
  //imageViewInfo.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
  //res = vkCreateImageView(m_vkDevice, &imageViewInfo, nullptr, &m_imageView);
  //if (res != VK_SUCCESS)
  //{
  //  LogVulkanError(res, "vkCreateImageView", __FILENAME__, __LINE__);
  //  return false;
  //}

  //VkSamplerCreateInfo samplerInfo = vkSamplerCreateInfo();
  //samplerInfo.magFilter = VK_FILTER_LINEAR;
  //samplerInfo.minFilter = VK_FILTER_LINEAR;
  //samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  //samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  //samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  //samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  //samplerInfo.mipLodBias = 0.0f;
  //samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
  //samplerInfo.minLod = 0.0f;
  //samplerInfo.maxLod = 1.0f;
  //samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
  //res = vkCreateSampler(m_vkDevice, &samplerInfo, nullptr, &m_sampler);
  //if (res != VK_SUCCESS)
  //{
  //  LogVulkanError(res, "vkCreateSampler", __FILENAME__, __LINE__);
  //  return false;
  //}

  //// Descriptor
  //// Font uses a separate descriptor pool
  //std::array<VkDescriptorPoolSize, 1> poolSizes = {
  //    vkDescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1)};
  //auto descriptorPoolInfo =
  //    vkDescriptorPoolCreateInfo(static_cast<uint32_t>(poolSizes.size()), poolSizes.data(), 1);
  //res = vkCreateDescriptorPool(m_vkDevice, &descriptorPoolInfo, nullptr, &m_descriptorPool);
  //if (res != VK_SUCCESS)
  //{
  //  LogVulkanError(res, "vkCreateDescriptorPool", __FILENAME__, __LINE__);
  //  return false;
  //}

  //// Descriptor set layout
  //std::array<VkDescriptorSetLayoutBinding, 1> setLayoutBindings = {vkDescriptorSetLayoutBinding(
  //    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0)};
  //auto descriptorSetLayoutInfo = vkDescriptorSetLayoutCreateInfo(
  //    setLayoutBindings.data(), static_cast<uint32_t>(setLayoutBindings.size()));
  //res = vkCreateDescriptorSetLayout(m_vkDevice, &descriptorSetLayoutInfo, nullptr,
  //                                  &m_descriptorSetLayout);
  //if (res != VK_SUCCESS)
  //{
  //  LogVulkanError(res, "vkCreateDescriptorSetLayout", __FILENAME__, __LINE__);
  //  return false;
  //}

  //// Descriptor for the font image
  //auto texDescriptor =
  //    vkDescriptorImageInfo(m_sampler, m_imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  //// Descriptor set
  //auto descriptorSetAllocInfo =
  //    vkDescriptorSetAllocateInfo(m_descriptorPool, &m_descriptorSetLayout, 1);
  //res = vkAllocateDescriptorSets(m_vkDevice, &descriptorSetAllocInfo, &m_descriptorSet);
  //if (res != VK_SUCCESS)
  //{
  //  LogVulkanError(res, "vkAllocateDescriptorSets", __FILENAME__, __LINE__);
  //  return false;
  //}

  //std::array<VkWriteDescriptorSet, 1> writeDescriptorSets = {vkWriteDescriptorSet(
  //    m_descriptorSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, &texDescriptor)};
  //vkUpdateDescriptorSets(m_vkDevice, static_cast<uint32_t>(writeDescriptorSets.size()),
  //                       writeDescriptorSets.data(), 0, NULL);

  //// The Vertex input properties define the interface between the vertex buffer and the vertex shader.

  //// Specify we will use triangle lists to draw geometry.
  //VkPipelineInputAssemblyStateCreateInfo input_assembly =
  //    vkPipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);

  //// Define the vertex input binding.
  ////std::array<VkVertexInputBindingDescription, 2> binding_description{{
  ////    vkVertexInputBindingDescription(0, sizeof(SVertex), VK_VERTEX_INPUT_RATE_VERTEX),
  ////    vkVertexInputBindingDescription(1, sizeof(SVertex), VK_VERTEX_INPUT_RATE_VERTEX),
  ////}};

  ////// Define the vertex input attribute.
  ////std::array<VkVertexInputAttributeDescription, 4> attribute_descriptions{{
  ////    vkVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(SVertex, pos)),
  ////    vkVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32B32A32_SFLOAT,
  ////                                      offsetof(SVertex, col)),
  ////    vkVertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(SVertex, cord0)),
  ////    vkVertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32_SFLOAT, offsetof(SVertex, cord1)),
  ////}};

  //std::array<VkVertexInputBindingDescription, 2> vertexInputBindings = {
  //    vkVertexInputBindingDescription(0, sizeof(glm::vec4), VK_VERTEX_INPUT_RATE_VERTEX),
  //    vkVertexInputBindingDescription(1, sizeof(glm::vec4), VK_VERTEX_INPUT_RATE_VERTEX),
  //};
  //std::array<VkVertexInputAttributeDescription, 2> vertexInputAttributes = {
  //    vkVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT, 0),
  //    vkVertexInputAttributeDescription(1, 1, VK_FORMAT_R32G32_SFLOAT, sizeof(glm::vec2)),
  //};

  //// Define the pipeline vertex input.
  //// Create the vertex input state
  //VkPipelineVertexInputStateCreateInfo input = vkPipelineVertexInputStateCreateInfo();
  //input.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindings.size());
  //input.pVertexBindingDescriptions = vertexInputBindings.data();
  //input.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributes.size());
  //input.pVertexAttributeDescriptions = vertexInputAttributes.data();

  //// Specify rasterization state.
  //auto raster = vkPipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT,
  //                                                     VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);

  //// Enable blending, using alpha from red channel of the font texture (see text.frag)
  //VkPipelineColorBlendAttachmentState blend_attachment{
  //    .blendEnable = VK_TRUE,
  //    .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
  //    .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
  //    .colorBlendOp = VK_BLEND_OP_ADD,
  //    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
  //    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
  //    .alphaBlendOp = VK_BLEND_OP_ADD,
  //    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
  //                      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};
  //auto color_blend_state = vkPipelineColorBlendStateCreateInfo(1, &blend_attachment);

  //// We will have one viewport and scissor box.
  //VkPipelineViewportStateCreateInfo viewport = vkPipelineViewportStateCreateInfo(1, 1, 0);

  //// Disable all depth testing.
  //auto depth_stencil =
  //    vkPipelineDepthStencilStateCreateInfo(VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL);

  //// No multisampling.
  //auto multisample = vkPipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);

  //// Specify that these states will be dynamic, i.e. not part of pipeline state object.
  //std::array<VkDynamicState, 2> dyn{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  //auto dynamic = vkPipelineDynamicStateCreateInfo(dyn.data(), static_cast<uint32_t>(dyn.size()));

  //// Load our SPIR-V shaders.
  //VkShaderModule vert = vulkanCreateShaderModule(m_vkDevice, kVertexShaderFile);
  //VkShaderModule frag = vulkanCreateShaderModule(m_vkDevice, kFragmentShaderFile);
  //if (vert == VK_NULL_HANDLE || frag == VK_NULL_HANDLE)
  //{
  //  CLog::Log(LOGERROR, "Vulkan: Failed to create shader modules for graphics pipeline. ({}:{})",
  //            __FILENAME__, __LINE__);
  //  return false;
  //}

  //// Vertex and fragment stages of the pipeline
  //std::array<VkPipelineShaderStageCreateInfo, 2> stages{};
  //stages[0] = vkPipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vert);
  //stages[1] = vkPipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, frag);

  //auto pipe = vkPipelineCreateInfo(m_vkPipelineLayout, m_vkRenderPass);
  //pipe.stageCount = static_cast<uint32_t>(stages.size());
  //pipe.pStages = stages.data();
  //pipe.pVertexInputState = &input;
  //pipe.pInputAssemblyState = &input_assembly;
  //pipe.pViewportState = &viewport;
  //pipe.pRasterizationState = &raster;
  //pipe.pMultisampleState = &multisample;
  //pipe.pDepthStencilState = &depth_stencil;
  //pipe.pColorBlendState = &color_blend_state;
  //pipe.pDynamicState = &dynamic;

  //res = vkCreateGraphicsPipelines(m_vkDevice, VK_NULL_HANDLE, 1, &pipe, nullptr, &m_vkPipeline);
  //if (res != VK_SUCCESS)
  //{
  //  LogVulkanError(res, "vkCreateGraphicsPipelines", __FILENAME__, __LINE__);
  //  return false;
  //}

  //// Pipeline is baked, we can delete the shader modules now.
  //vkDestroyShaderModule(m_vkDevice, stages[0].module, nullptr);
  //vkDestroyShaderModule(m_vkDevice, stages[1].module, nullptr);

  return true;
}

void CVulkanShaderFonts::Destroy()
{
  if (m_vkPipeline != VK_NULL_HANDLE)
  {
    // Destroy the Vulkan pipeline
    vkDestroyPipeline(m_vkDevice, m_vkPipeline, nullptr);
    m_vkPipeline = VK_NULL_HANDLE;
  }
  if (m_sampler != VK_NULL_HANDLE)
  {
    // Destroy the Vulkan sampler
    vkDestroySampler(m_vkDevice, m_sampler, nullptr);
    m_sampler = VK_NULL_HANDLE;
  }
  if (m_imageView != VK_NULL_HANDLE)
  {
    // Destroy the Vulkan image view
    vkDestroyImageView(m_vkDevice, m_imageView, nullptr);
    m_imageView = VK_NULL_HANDLE;
  }
  if (m_image != VK_NULL_HANDLE)
  {
    // Destroy the Vulkan image
    vkDestroyImage(m_vkDevice, m_image, nullptr);
    m_image = VK_NULL_HANDLE;
  }
  if (m_imageMemory != VK_NULL_HANDLE)
  {
    // Free the Vulkan image memory
    vkFreeMemory(m_vkDevice, m_imageMemory, nullptr);
    m_imageMemory = VK_NULL_HANDLE;
  }
}

VkPipeline CVulkanShaderFonts::VulkanPipeline() const
{
  return m_vkPipeline;
}

} // namespace KODI::RENDERING::VULKAN
