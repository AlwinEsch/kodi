/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <vector>

#include <vulkan/vulkan_core.h>

namespace KODI::RENDERING::VULKAN::UTILS
{

inline VkMemoryAllocateInfo vkMemoryAllocateInfo()
{
  VkMemoryAllocateInfo memAllocInfo{};
  memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  return memAllocInfo;
}

inline VkMemoryAllocateInfo vkMemoryAllocateInfo(VkDeviceSize allocationSize,
                                                 uint32_t memoryTypeIndex)
{
  VkMemoryAllocateInfo memAllocInfo{};
  memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memAllocInfo.allocationSize = allocationSize;
  memAllocInfo.memoryTypeIndex = memoryTypeIndex;
  return memAllocInfo;
}

inline VkMappedMemoryRange vkMappedMemoryRange(VkDeviceMemory memory,
                                               VkDeviceSize offset,
                                               VkDeviceSize size)
{
  VkMappedMemoryRange mappedMemoryRange{};
  mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  mappedMemoryRange.memory = memory;
  mappedMemoryRange.offset = offset;
  mappedMemoryRange.size = size;
  return mappedMemoryRange;
}

inline VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo(VkCommandPool commandPool,
                                                               VkCommandBufferLevel level,
                                                               uint32_t bufferCount)
{
  VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
  commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocateInfo.commandPool = commandPool;
  commandBufferAllocateInfo.level = level;
  commandBufferAllocateInfo.commandBufferCount = bufferCount;
  return commandBufferAllocateInfo;
}

inline VkCommandPoolCreateInfo vkCommandPoolCreateInfo()
{
  VkCommandPoolCreateInfo cmdPoolCreateInfo{};
  cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  return cmdPoolCreateInfo;
}

inline VkCommandBufferBeginInfo vkCommandBufferBeginInfo()
{
  VkCommandBufferBeginInfo cmdBufferBeginInfo{};
  cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  return cmdBufferBeginInfo;
}

inline VkCommandBufferInheritanceInfo vkCommandBufferInheritanceInfo()
{
  VkCommandBufferInheritanceInfo cmdBufferInheritanceInfo{};
  cmdBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
  return cmdBufferInheritanceInfo;
}

inline VkRenderPassBeginInfo vkRenderPassBeginInfo()
{
  VkRenderPassBeginInfo renderPassBeginInfo{};
  renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  return renderPassBeginInfo;
}

inline VkRenderPassCreateInfo vkRenderPassCreateInfo()
{
  VkRenderPassCreateInfo renderPassCreateInfo{};
  renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  return renderPassCreateInfo;
}

/** @brief Initialize an image memory barrier with no image transfer ownership */
inline VkImageMemoryBarrier vkImageMemoryBarrier()
{
  VkImageMemoryBarrier imageMemoryBarrier{};
  imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  return imageMemoryBarrier;
}

/** @brief Initialize a buffer memory barrier with no image transfer ownership */
inline VkBufferMemoryBarrier vkBufferMemoryBarrier()
{
  VkBufferMemoryBarrier bufferMemoryBarrier{};
  bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
  bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  return bufferMemoryBarrier;
}

inline VkMemoryBarrier vkMemoryBarrier()
{
  VkMemoryBarrier memoryBarrier{};
  memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
  return memoryBarrier;
}

inline VkImageCreateInfo vkImageCreateInfo()
{
  VkImageCreateInfo imageCreateInfo{};
  imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  return imageCreateInfo;
}

inline VkSamplerCreateInfo vkSamplerCreateInfo()
{
  VkSamplerCreateInfo samplerCreateInfo{};
  samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerCreateInfo.maxAnisotropy = 1.0f;
  return samplerCreateInfo;
}

inline VkImageViewCreateInfo vkImageViewCreateInfo()
{
  VkImageViewCreateInfo imageViewCreateInfo{};
  imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  return imageViewCreateInfo;
}

inline VkFramebufferCreateInfo vkFramebufferCreateInfo()
{
  VkFramebufferCreateInfo framebufferCreateInfo{};
  framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  return framebufferCreateInfo;
}

inline VkSemaphoreCreateInfo vkSemaphoreCreateInfo()
{
  VkSemaphoreCreateInfo semaphoreCreateInfo{};
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  return semaphoreCreateInfo;
}

inline VkFenceCreateInfo vkFenceCreateInfo(VkFenceCreateFlags flags = 0)
{
  VkFenceCreateInfo fenceCreateInfo{};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCreateInfo.flags = flags;
  return fenceCreateInfo;
}

inline VkSubmitInfo vkSubmitInfo()
{
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  return submitInfo;
}

inline VkViewport vkViewport(float width, float height, float minDepth, float maxDepth)
{
  VkViewport viewport{};
  viewport.width = width;
  viewport.height = height;
  viewport.minDepth = minDepth;
  viewport.maxDepth = maxDepth;
  return viewport;
}

inline VkRect2D vkRect2D(int32_t width, int32_t height, int32_t offsetX, int32_t offsetY)
{
  VkRect2D rect2D{};
  rect2D.extent.width = width;
  rect2D.extent.height = height;
  rect2D.offset.x = offsetX;
  rect2D.offset.y = offsetY;
  return rect2D;
}

inline VkBufferCreateInfo vkBufferCreateInfo()
{
  VkBufferCreateInfo bufCreateInfo{};
  bufCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  return bufCreateInfo;
}

inline VkBufferCreateInfo vkBufferCreateInfo(VkBufferUsageFlags usage,
                                             VkDeviceSize size,
                                             VkSharingMode sharingMode)
{
  VkBufferCreateInfo bufCreateInfo{};
  bufCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufCreateInfo.usage = usage;
  bufCreateInfo.size = size;
  bufCreateInfo.sharingMode = sharingMode;
  return bufCreateInfo;
}

inline VkDescriptorPoolCreateInfo vkDescriptorPoolCreateInfo(uint32_t poolSizeCount,
                                                             VkDescriptorPoolSize* pPoolSizes,
                                                             uint32_t maxSets)
{
  VkDescriptorPoolCreateInfo descriptorPoolInfo{};
  descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptorPoolInfo.poolSizeCount = poolSizeCount;
  descriptorPoolInfo.pPoolSizes = pPoolSizes;
  descriptorPoolInfo.maxSets = maxSets;
  return descriptorPoolInfo;
}

inline VkDescriptorPoolCreateInfo vkDescriptorPoolCreateInfo(
    const std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets)
{
  VkDescriptorPoolCreateInfo descriptorPoolInfo{};
  descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  descriptorPoolInfo.pPoolSizes = poolSizes.data();
  descriptorPoolInfo.maxSets = maxSets;
  return descriptorPoolInfo;
}

inline VkDescriptorPoolSize vkDescriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount)
{
  VkDescriptorPoolSize descriptorPoolSize{};
  descriptorPoolSize.type = type;
  descriptorPoolSize.descriptorCount = descriptorCount;
  return descriptorPoolSize;
}

inline VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding(VkDescriptorType type,
                                                                 VkShaderStageFlags stageFlags,
                                                                 uint32_t binding,
                                                                 uint32_t descriptorCount = 1)
{
  VkDescriptorSetLayoutBinding setLayoutBinding{};
  setLayoutBinding.descriptorType = type;
  setLayoutBinding.stageFlags = stageFlags;
  setLayoutBinding.binding = binding;
  setLayoutBinding.descriptorCount = descriptorCount;
  return setLayoutBinding;
}

inline VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo(
    const VkDescriptorSetLayoutBinding* pBindings, uint32_t bindingCount)
{
  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
  descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptorSetLayoutCreateInfo.pBindings = pBindings;
  descriptorSetLayoutCreateInfo.bindingCount = bindingCount;
  return descriptorSetLayoutCreateInfo;
}

inline VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo(
    const std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
  descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptorSetLayoutCreateInfo.pBindings = bindings.data();
  descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  return descriptorSetLayoutCreateInfo;
}

inline VkPipelineLayoutCreateInfo vkPipelineLayoutCreateInfo(
    const VkDescriptorSetLayout* pSetLayouts, uint32_t setLayoutCount = 1)
{
  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.setLayoutCount = setLayoutCount;
  pipelineLayoutCreateInfo.pSetLayouts = pSetLayouts;
  return pipelineLayoutCreateInfo;
}

inline VkPipelineLayoutCreateInfo vkPipelineLayoutCreateInfo(uint32_t setLayoutCount = 1)
{
  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.setLayoutCount = setLayoutCount;
  return pipelineLayoutCreateInfo;
}

inline VkDescriptorSetAllocateInfo vkDescriptorSetAllocateInfo(
    VkDescriptorPool descriptorPool,
    const VkDescriptorSetLayout* pSetLayouts,
    uint32_t descriptorSetCount)
{
  VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
  descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  descriptorSetAllocateInfo.descriptorPool = descriptorPool;
  descriptorSetAllocateInfo.pSetLayouts = pSetLayouts;
  descriptorSetAllocateInfo.descriptorSetCount = descriptorSetCount;
  return descriptorSetAllocateInfo;
}

inline VkDescriptorImageInfo vkDescriptorImageInfo(VkSampler sampler,
                                                   VkImageView imageView,
                                                   VkImageLayout imageLayout)
{
  VkDescriptorImageInfo descriptorImageInfo{};
  descriptorImageInfo.sampler = sampler;
  descriptorImageInfo.imageView = imageView;
  descriptorImageInfo.imageLayout = imageLayout;
  return descriptorImageInfo;
}

inline VkWriteDescriptorSet vkWriteDescriptorSet(VkDescriptorSet dstSet,
                                                 VkDescriptorType type,
                                                 uint32_t binding,
                                                 VkDescriptorBufferInfo* bufferInfo,
                                                 uint32_t descriptorCount = 1)
{
  VkWriteDescriptorSet writeDescriptorSet{};
  writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writeDescriptorSet.dstSet = dstSet;
  writeDescriptorSet.descriptorType = type;
  writeDescriptorSet.dstBinding = binding;
  writeDescriptorSet.pBufferInfo = bufferInfo;
  writeDescriptorSet.descriptorCount = descriptorCount;
  return writeDescriptorSet;
}

inline VkWriteDescriptorSet vkWriteDescriptorSet(VkDescriptorSet dstSet,
                                                 VkDescriptorType type,
                                                 uint32_t binding,
                                                 VkDescriptorImageInfo* imageInfo,
                                                 uint32_t descriptorCount = 1)
{
  VkWriteDescriptorSet writeDescriptorSet{};
  writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writeDescriptorSet.dstSet = dstSet;
  writeDescriptorSet.descriptorType = type;
  writeDescriptorSet.dstBinding = binding;
  writeDescriptorSet.pImageInfo = imageInfo;
  writeDescriptorSet.descriptorCount = descriptorCount;
  return writeDescriptorSet;
}

inline VkVertexInputBindingDescription vkVertexInputBindingDescription(uint32_t binding,
                                                                       uint32_t stride,
                                                                       VkVertexInputRate inputRate)
{
  VkVertexInputBindingDescription vInputBindDescription{};
  vInputBindDescription.binding = binding;
  vInputBindDescription.stride = stride;
  vInputBindDescription.inputRate = inputRate;
  return vInputBindDescription;
}

inline VkVertexInputAttributeDescription vkVertexInputAttributeDescription(uint32_t binding,
                                                                           uint32_t location,
                                                                           VkFormat format,
                                                                           uint32_t offset)
{
  VkVertexInputAttributeDescription vInputAttribDescription{};
  vInputAttribDescription.location = location;
  vInputAttribDescription.binding = binding;
  vInputAttribDescription.format = format;
  vInputAttribDescription.offset = offset;
  return vInputAttribDescription;
}

inline VkPipelineVertexInputStateCreateInfo vkPipelineVertexInputStateCreateInfo()
{
  VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
  pipelineVertexInputStateCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  return pipelineVertexInputStateCreateInfo;
}

inline VkPipelineVertexInputStateCreateInfo vkPipelineVertexInputStateCreateInfo(
    const std::vector<VkVertexInputBindingDescription>& vertexBindingDescriptions,
    const std::vector<VkVertexInputAttributeDescription>& vertexAttributeDescriptions)
{
  VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
  pipelineVertexInputStateCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount =
      static_cast<uint32_t>(vertexBindingDescriptions.size());
  pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vertexBindingDescriptions.data();
  pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(vertexAttributeDescriptions.size());
  pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions =
      vertexAttributeDescriptions.data();
  return pipelineVertexInputStateCreateInfo;
}

inline VkPipelineInputAssemblyStateCreateInfo vkPipelineInputAssemblyStateCreateInfo(
    VkPrimitiveTopology topology,
    VkPipelineInputAssemblyStateCreateFlags flags,
    VkBool32 primitiveRestartEnable)
{
  VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
  pipelineInputAssemblyStateCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  pipelineInputAssemblyStateCreateInfo.topology = topology;
  pipelineInputAssemblyStateCreateInfo.flags = flags;
  pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = primitiveRestartEnable;
  return pipelineInputAssemblyStateCreateInfo;
}

inline VkPipelineRasterizationStateCreateInfo vkPipelineRasterizationStateCreateInfo(
    VkPolygonMode polygonMode,
    VkCullModeFlags cullMode,
    VkFrontFace frontFace,
    VkPipelineRasterizationStateCreateFlags flags = 0)
{
  VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
  pipelineRasterizationStateCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  pipelineRasterizationStateCreateInfo.polygonMode = polygonMode;
  pipelineRasterizationStateCreateInfo.cullMode = cullMode;
  pipelineRasterizationStateCreateInfo.frontFace = frontFace;
  pipelineRasterizationStateCreateInfo.flags = flags;
  pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
  pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
  return pipelineRasterizationStateCreateInfo;
}

inline VkPipelineColorBlendAttachmentState vkPipelineColorBlendAttachmentState(
    VkColorComponentFlags colorWriteMask, VkBool32 blendEnable)
{
  VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};
  pipelineColorBlendAttachmentState.colorWriteMask = colorWriteMask;
  pipelineColorBlendAttachmentState.blendEnable = blendEnable;
  return pipelineColorBlendAttachmentState;
}

inline VkPipelineColorBlendStateCreateInfo vkPipelineColorBlendStateCreateInfo(
    uint32_t attachmentCount,
    const VkPipelineColorBlendAttachmentState* pAttachments,
    VkLogicOp logicOp = VK_LOGIC_OP_CLEAR)
{
  VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
  pipelineColorBlendStateCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  pipelineColorBlendStateCreateInfo.attachmentCount = attachmentCount;
  pipelineColorBlendStateCreateInfo.pAttachments = pAttachments;
  pipelineColorBlendStateCreateInfo.logicOp = logicOp;
  return pipelineColorBlendStateCreateInfo;
}

inline VkPipelineDepthStencilStateCreateInfo vkPipelineDepthStencilStateCreateInfo(
    VkBool32 depthTestEnable, VkBool32 depthWriteEnable, VkCompareOp depthCompareOp)
{
  VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{};
  pipelineDepthStencilStateCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  pipelineDepthStencilStateCreateInfo.depthTestEnable = depthTestEnable;
  pipelineDepthStencilStateCreateInfo.depthWriteEnable = depthWriteEnable;
  pipelineDepthStencilStateCreateInfo.depthCompareOp = depthCompareOp;
  pipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
  return pipelineDepthStencilStateCreateInfo;
}

inline VkPipelineViewportStateCreateInfo vkPipelineViewportStateCreateInfo(
    uint32_t viewportCount, uint32_t scissorCount, VkPipelineViewportStateCreateFlags flags = 0)
{
  VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
  pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  pipelineViewportStateCreateInfo.viewportCount = viewportCount;
  pipelineViewportStateCreateInfo.scissorCount = scissorCount;
  pipelineViewportStateCreateInfo.flags = flags;
  return pipelineViewportStateCreateInfo;
}

inline VkPipelineMultisampleStateCreateInfo vkPipelineMultisampleStateCreateInfo(
    VkSampleCountFlagBits rasterizationSamples, VkPipelineMultisampleStateCreateFlags flags = 0)
{
  VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
  pipelineMultisampleStateCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  pipelineMultisampleStateCreateInfo.rasterizationSamples = rasterizationSamples;
  pipelineMultisampleStateCreateInfo.flags = flags;
  return pipelineMultisampleStateCreateInfo;
}

inline VkPipelineDynamicStateCreateInfo vkPipelineDynamicStateCreateInfo(
    const VkDynamicState* pDynamicStates,
    uint32_t dynamicStateCount,
    VkPipelineDynamicStateCreateFlags flags = 0)
{
  VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
  pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  pipelineDynamicStateCreateInfo.pDynamicStates = pDynamicStates;
  pipelineDynamicStateCreateInfo.dynamicStateCount = dynamicStateCount;
  pipelineDynamicStateCreateInfo.flags = flags;
  return pipelineDynamicStateCreateInfo;
}

inline VkPipelineDynamicStateCreateInfo vkPipelineDynamicStateCreateInfo(
    const std::vector<VkDynamicState>& pDynamicStates, VkPipelineDynamicStateCreateFlags flags = 0)
{
  VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
  pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  pipelineDynamicStateCreateInfo.pDynamicStates = pDynamicStates.data();
  pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(pDynamicStates.size());
  pipelineDynamicStateCreateInfo.flags = flags;
  return pipelineDynamicStateCreateInfo;
}

inline VkPipelineTessellationStateCreateInfo vkPipelineTessellationStateCreateInfo(
    uint32_t patchControlPoints)
{
  VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo{};
  pipelineTessellationStateCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
  pipelineTessellationStateCreateInfo.patchControlPoints = patchControlPoints;
  return pipelineTessellationStateCreateInfo;
}

inline VkPipelineShaderStageCreateInfo vkPipelineShaderStageCreateInfo(VkShaderStageFlagBits stage,
                                                                       VkShaderModule shaderModule,
                                                                       const char* pName = "main")
{
  VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo{};
  pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  pipelineShaderStageCreateInfo.stage = stage;
  pipelineShaderStageCreateInfo.module = shaderModule;
  pipelineShaderStageCreateInfo.pName = pName;
  return pipelineShaderStageCreateInfo;
}

inline VkGraphicsPipelineCreateInfo vkPipelineCreateInfo(VkPipelineLayout layout,
                                                         VkRenderPass renderPass,
                                                         VkPipelineCreateFlags flags = 0)
{
  VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
  pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineCreateInfo.layout = layout;
  pipelineCreateInfo.renderPass = renderPass;
  pipelineCreateInfo.flags = flags;
  pipelineCreateInfo.basePipelineIndex = -1;
  pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
  return pipelineCreateInfo;
}

inline VkGraphicsPipelineCreateInfo vkPipelineCreateInfo()
{
  VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
  pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineCreateInfo.basePipelineIndex = -1;
  pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
  return pipelineCreateInfo;
}

inline VkComputePipelineCreateInfo vkComputePipelineCreateInfo(VkPipelineLayout layout,
                                                               VkPipelineCreateFlags flags = 0)
{
  VkComputePipelineCreateInfo computePipelineCreateInfo{};
  computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  computePipelineCreateInfo.layout = layout;
  computePipelineCreateInfo.flags = flags;
  return computePipelineCreateInfo;
}

inline VkPushConstantRange vkPushConstantRange(VkShaderStageFlags stageFlags,
                                               uint32_t size,
                                               uint32_t offset)
{
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = stageFlags;
  pushConstantRange.offset = offset;
  pushConstantRange.size = size;
  return pushConstantRange;
}

inline VkBindSparseInfo vkBindSparseInfo()
{
  VkBindSparseInfo bindSparseInfo{};
  bindSparseInfo.sType = VK_STRUCTURE_TYPE_BIND_SPARSE_INFO;
  return bindSparseInfo;
}

/** @brief Initialize a map entry for a shader specialization constant */
inline VkSpecializationMapEntry vkSpecializationMapEntry(uint32_t constantID,
                                                         uint32_t offset,
                                                         size_t size)
{
  VkSpecializationMapEntry specializationMapEntry{};
  specializationMapEntry.constantID = constantID;
  specializationMapEntry.offset = offset;
  specializationMapEntry.size = size;
  return specializationMapEntry;
}

/** @brief Initialize a specialization constant info structure to pass to a shader stage */
inline VkSpecializationInfo vkSpecializationInfo(uint32_t mapEntryCount,
                                                 const VkSpecializationMapEntry* mapEntries,
                                                 size_t dataSize,
                                                 const void* data)
{
  VkSpecializationInfo specializationInfo{};
  specializationInfo.mapEntryCount = mapEntryCount;
  specializationInfo.pMapEntries = mapEntries;
  specializationInfo.dataSize = dataSize;
  specializationInfo.pData = data;
  return specializationInfo;
}

/** @brief Initialize a specialization constant info structure to pass to a shader stage */
inline VkSpecializationInfo vkSpecializationInfo(
    const std::vector<VkSpecializationMapEntry>& mapEntries, size_t dataSize, const void* data)
{
  VkSpecializationInfo specializationInfo{};
  specializationInfo.mapEntryCount = static_cast<uint32_t>(mapEntries.size());
  specializationInfo.pMapEntries = mapEntries.data();
  specializationInfo.dataSize = dataSize;
  specializationInfo.pData = data;
  return specializationInfo;
}

// Ray tracing related
inline VkAccelerationStructureGeometryKHR vkAccelerationStructureGeometryKHR()
{
  VkAccelerationStructureGeometryKHR accelerationStructureGeometryKHR{};
  accelerationStructureGeometryKHR.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
  return accelerationStructureGeometryKHR;
}

inline VkAccelerationStructureBuildGeometryInfoKHR vkAccelerationStructureBuildGeometryInfoKHR()
{
  VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfoKHR{};
  accelerationStructureBuildGeometryInfoKHR.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
  return accelerationStructureBuildGeometryInfoKHR;
}

inline VkAccelerationStructureBuildSizesInfoKHR vkAccelerationStructureBuildSizesInfoKHR()
{
  VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfoKHR{};
  accelerationStructureBuildSizesInfoKHR.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
  return accelerationStructureBuildSizesInfoKHR;
}

inline VkRayTracingShaderGroupCreateInfoKHR vkRayTracingShaderGroupCreateInfoKHR()
{
  VkRayTracingShaderGroupCreateInfoKHR rayTracingShaderGroupCreateInfoKHR{};
  rayTracingShaderGroupCreateInfoKHR.sType =
      VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
  return rayTracingShaderGroupCreateInfoKHR;
}

inline VkRayTracingPipelineCreateInfoKHR vkRayTracingPipelineCreateInfoKHR()
{
  VkRayTracingPipelineCreateInfoKHR rayTracingPipelineCreateInfoKHR{};
  rayTracingPipelineCreateInfoKHR.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
  return rayTracingPipelineCreateInfoKHR;
}

inline VkWriteDescriptorSetAccelerationStructureKHR vkWriteDescriptorSetAccelerationStructureKHR()
{
  VkWriteDescriptorSetAccelerationStructureKHR writeDescriptorSetAccelerationStructureKHR{};
  writeDescriptorSetAccelerationStructureKHR.sType =
      VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
  return writeDescriptorSetAccelerationStructureKHR;
}

inline VkAccelerationStructureCreateInfoKHR vkAccelerationStructureCreateInfoKHR()
{
  VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfoKHR{};
  accelerationStructureCreateInfoKHR.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
  return accelerationStructureCreateInfoKHR;
}

inline VkPipelineCacheCreateInfo vkPipelineCacheCreateInfo()
{
  VkPipelineCacheCreateInfo pipelineCacheCreateInfo{};
  pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
  return pipelineCacheCreateInfo;
}

inline VkPipelineCacheCreateInfo vkPipelineCacheCreateInfo(VkPipelineCacheCreateFlags flags,
                                                           size_t initialDataSize = 0,
                                                           const void* pInitialData = nullptr)
{
  VkPipelineCacheCreateInfo pipelineCacheCreateInfo{};
  pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
  pipelineCacheCreateInfo.flags = flags;
  pipelineCacheCreateInfo.initialDataSize = initialDataSize;
  pipelineCacheCreateInfo.pInitialData = pInitialData;
  return pipelineCacheCreateInfo;
}

inline VkMemoryAllocateFlagsInfo vkMemoryAllocateFlagsInfo(VkMemoryAllocateFlags flags = 0)
{
  VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo{};
  memoryAllocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
  memoryAllocateFlagsInfo.flags = flags;
  return memoryAllocateFlagsInfo;
}

} // namespace KODI::RENDERING::VULKAN::UTILS
