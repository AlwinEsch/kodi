/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanShaderTexture.h"

#include "rendering/vulkan/VulkanDeviceQueue.h"
#include "rendering/vulkan/utils/VulkanInitStructs.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "utils/log.h"

#include <cassert>

#define DEFAULT_FENCE_TIMEOUT 100000000000

using namespace KODI::RENDERING::VULKAN::UTILS;

namespace KODI::RENDERING::VULKAN
{

namespace
{
constexpr const char* kVertexShaderFile = "vulkan_shader_gr1_vert.spv";
constexpr const char* kFragmentShaderFile = "vulkan_shader_gr1_fraq_texture.spv";

} // namespace

CVulkanShaderTexture::CVulkanShaderTexture(const VulkanData* vkData,
                                           CVulkanDeviceQueue* deviceQueue)
  : IVulkanShader(vkData, deviceQueue)
{
}

bool CVulkanShaderTexture::Create()
{
  createVertexBuffer();
  createUniformBuffers();
  createDescriptorSetLayout();
  createDescriptorPool();
  createDescriptorSets();
  createPipelines();
  return true;
}

void CVulkanShaderTexture::Destroy()
{
  if (m_vkPipeline != VK_NULL_HANDLE)
  {
    // Destroy the Vulkan pipeline
    vkDestroyPipeline(m_vkData->vkDevice, m_vkPipeline, nullptr);
    m_vkPipeline = VK_NULL_HANDLE;
  }
}

void CVulkanShaderTexture::prepareParticles()
{
  //struct VulkanMemoryData
  //{
  //  VkBuffer buffer{VK_NULL_HANDLE};
  //  VkDeviceMemory memory{VK_NULL_HANDLE};
  //  VkDeviceSize size{0};
  //  VkDeviceSize offset{0};
  //  void* mapped{nullptr};
  //};

  // One m_buffer per concurrent frame, so we can update one frame while the other is still rendering
  m_particles.resize(PARTICLE_COUNT);
  //for (auto& buffer : m_particleBuffers)
  //{
  //  buffer.size = m_particles.size() * sizeof(Vertices);

  //  VK_CHECK_RESULT(m_deviceQueue->CreateBuffer(
  //      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
  //      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffer.size,
  //      &buffer.buffer, &buffer.memory, m_particles.data()));

  //  // Map the m_memory and store the pointer for reuse
  //  VK_CHECK_RESULT(
  //      vkMapMemory(m_device, buffer.memory, 0, buffer.size, 0, &buffer.mappedMemory));
  //}
}

void CVulkanShaderTexture::createVertexBuffer()
{
  // A note on m_memory management in Vulkan in general:
  //	This is a very complex topic and while it's fine for an example application to small individual m_memory allocations that is not
  //	what should be done a real-world application, where you should allocate large chunks of m_memory at once instead.

  // Setup vertices
  std::vector<Vertex> m_vertexBuffer{
      {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
      {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
      {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
      {{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}}};
  uint32_t vertexBufferSize = static_cast<uint32_t>(m_vertexBuffer.size()) * sizeof(Vertex);

  // Setup indices
  std::vector<uint32_t> m_indexBuffer{0, 1, 2, 2, 3, 0};
  indices.count = static_cast<uint32_t>(m_indexBuffer.size());
  uint32_t indexBufferSize = indices.count * sizeof(uint32_t);

  VkMemoryAllocateInfo memAlloc{};
  memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  VkMemoryRequirements memReqs;

  // Static data like vertex and index m_buffer should be stored on the m_vkData->vkDevice m_memory for optimal (and fastest) access by the GPU
  //
  // To achieve this we use so-called "staging buffers" :
  // - Create a m_buffer that's visible to the host (and can be mapped)
  // - Copy the data to this m_buffer
  // - Create another m_buffer that's local on the m_vkData->vkDevice (VRAM) with the same size
  // - Copy the data from the host to the m_vkData->vkDevice using a command m_buffer
  // - Delete the host visible (staging) m_buffer
  // - Use the m_vkData->vkDevice local buffers for rendering
  //
  // Note: On unified m_memory architectures where host (CPU) and GPU share the same m_memory, staging is not necessary
  // To keep this sample easy to follow, there is no check for that in place

  struct StagingBuffer
  {
    VkDeviceMemory memory;
    VkBuffer buffer;
  };

  struct
  {
    StagingBuffer vertices;
    StagingBuffer indices;
  } stagingBuffers{};

  void* data;

  // Vertex m_buffer
  VkBufferCreateInfo vertexBufferInfoCI{};
  vertexBufferInfoCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  vertexBufferInfoCI.size = vertexBufferSize;
  // Buffer is used as the copy source
  vertexBufferInfoCI.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  // Create a host-visible m_buffer to copy the vertex data to (staging m_buffer)
  VK_CHECK_RESULT(vkCreateBuffer(m_vkData->vkDevice, &vertexBufferInfoCI, nullptr,
                                 &stagingBuffers.vertices.buffer));
  vkGetBufferMemoryRequirements(m_vkData->vkDevice, stagingBuffers.vertices.buffer, &memReqs);
  memAlloc.allocationSize = memReqs.size;
  // Request a host visible m_memory type that can be used to copy our data to
  // Also request it to be coherent, so that writes are visible to the GPU right after unmapping the m_buffer
  memAlloc.memoryTypeIndex = m_deviceQueue->GetMemoryType(memReqs.memoryTypeBits,
                                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  VK_CHECK_RESULT(
      vkAllocateMemory(m_vkData->vkDevice, &memAlloc, nullptr, &stagingBuffers.vertices.memory));
  // Map and copy
  VK_CHECK_RESULT(vkMapMemory(m_vkData->vkDevice, stagingBuffers.vertices.memory, 0,
                              memAlloc.allocationSize, 0, &data));
  memcpy(data, m_vertexBuffer.data(), vertexBufferSize);
  vkUnmapMemory(m_vkData->vkDevice, stagingBuffers.vertices.memory);
  VK_CHECK_RESULT(vkBindBufferMemory(m_vkData->vkDevice, stagingBuffers.vertices.buffer,
                                     stagingBuffers.vertices.memory, 0));

  // Create a m_vkData->vkDevice local m_buffer to which the (host local) vertex data will be copied and which will be used for rendering
  vertexBufferInfoCI.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  VK_CHECK_RESULT(
      vkCreateBuffer(m_vkData->vkDevice, &vertexBufferInfoCI, nullptr, &vertices.buffer));
  vkGetBufferMemoryRequirements(m_vkData->vkDevice, vertices.buffer, &memReqs);
  memAlloc.allocationSize = memReqs.size;
  memAlloc.memoryTypeIndex =
      m_deviceQueue->GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  VK_CHECK_RESULT(vkAllocateMemory(m_vkData->vkDevice, &memAlloc, nullptr, &vertices.memory));
  VK_CHECK_RESULT(vkBindBufferMemory(m_vkData->vkDevice, vertices.buffer, vertices.memory, 0));

  // Index m_buffer
  VkBufferCreateInfo indexbufferCI{};
  indexbufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  indexbufferCI.size = indexBufferSize;
  indexbufferCI.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  // Copy index data to a m_buffer visible to the host (staging m_buffer)
  VK_CHECK_RESULT(
      vkCreateBuffer(m_vkData->vkDevice, &indexbufferCI, nullptr, &stagingBuffers.indices.buffer));
  vkGetBufferMemoryRequirements(m_vkData->vkDevice, stagingBuffers.indices.buffer, &memReqs);
  memAlloc.allocationSize = memReqs.size;
  memAlloc.memoryTypeIndex = m_deviceQueue->GetMemoryType(memReqs.memoryTypeBits,
                                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  VK_CHECK_RESULT(
      vkAllocateMemory(m_vkData->vkDevice, &memAlloc, nullptr, &stagingBuffers.indices.memory));
  VK_CHECK_RESULT(
      vkMapMemory(m_vkData->vkDevice, stagingBuffers.indices.memory, 0, indexBufferSize, 0, &data));
  memcpy(data, m_indexBuffer.data(), indexBufferSize);
  vkUnmapMemory(m_vkData->vkDevice, stagingBuffers.indices.memory);
  VK_CHECK_RESULT(vkBindBufferMemory(m_vkData->vkDevice, stagingBuffers.indices.buffer,
                                     stagingBuffers.indices.memory, 0));

  // Create destination m_buffer with m_vkData->vkDevice only visibility
  indexbufferCI.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  VK_CHECK_RESULT(vkCreateBuffer(m_vkData->vkDevice, &indexbufferCI, nullptr, &indices.buffer));
  vkGetBufferMemoryRequirements(m_vkData->vkDevice, indices.buffer, &memReqs);
  memAlloc.allocationSize = memReqs.size;
  memAlloc.memoryTypeIndex =
      m_deviceQueue->GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  VK_CHECK_RESULT(vkAllocateMemory(m_vkData->vkDevice, &memAlloc, nullptr, &indices.memory));
  VK_CHECK_RESULT(vkBindBufferMemory(m_vkData->vkDevice, indices.buffer, indices.memory, 0));

  // Buffer copies have to be submitted to a m_queue, so we need a command m_buffer for them
  // Note: Some devices offer a dedicated transfer m_queue (with only the transfer bit set) that may be faster when doing lots of copies
  VkCommandBuffer copyCmd;

  VkCommandBufferAllocateInfo cmdBufAllocateInfo{};
  cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  cmdBufAllocateInfo.commandPool = m_vkData->vkCommandPool;
  cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  cmdBufAllocateInfo.commandBufferCount = 1;
  VK_CHECK_RESULT(vkAllocateCommandBuffers(m_vkData->vkDevice, &cmdBufAllocateInfo, &copyCmd));

  VkCommandBufferBeginInfo cmdBufInfo = vkCommandBufferBeginInfo();
  VK_CHECK_RESULT(vkBeginCommandBuffer(copyCmd, &cmdBufInfo));
  // Put m_buffer region copies into command m_buffer
  VkBufferCopy copyRegion{};
  // Vertex m_buffer
  copyRegion.size = vertexBufferSize;
  vkCmdCopyBuffer(copyCmd, stagingBuffers.vertices.buffer, vertices.buffer, 1, &copyRegion);
  // Index m_buffer
  copyRegion.size = indexBufferSize;
  vkCmdCopyBuffer(copyCmd, stagingBuffers.indices.buffer, indices.buffer, 1, &copyRegion);
  VK_CHECK_RESULT(vkEndCommandBuffer(copyCmd));

  // Submit the command m_buffer to the m_queue to finish the copy
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &copyCmd;

  // Create fence to ensure that the command m_buffer has finished executing
  VkFenceCreateInfo fenceCI{};
  fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCI.flags = 0;
  VkFence fence;
  VK_CHECK_RESULT(vkCreateFence(m_vkData->vkDevice, &fenceCI, nullptr, &fence));

  // Submit to the m_queue
  VK_CHECK_RESULT(vkQueueSubmit(m_vkData->vkQueue, 1, &submitInfo, fence));
  // Wait for the fence to signal that command m_buffer has finished executing
  VK_CHECK_RESULT(vkWaitForFences(m_vkData->vkDevice, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT));

  vkDestroyFence(m_vkData->vkDevice, fence, nullptr);
  vkFreeCommandBuffers(m_vkData->vkDevice, m_vkData->vkCommandPool, 1, &copyCmd);

  // Destroy staging buffers
  // Note: Staging m_buffer must not be deleted before the copies have been submitted and executed
  vkDestroyBuffer(m_vkData->vkDevice, stagingBuffers.vertices.buffer, nullptr);
  vkFreeMemory(m_vkData->vkDevice, stagingBuffers.vertices.memory, nullptr);
  vkDestroyBuffer(m_vkData->vkDevice, stagingBuffers.indices.buffer, nullptr);
  vkFreeMemory(m_vkData->vkDevice, stagingBuffers.indices.memory, nullptr);
}

void CVulkanShaderTexture::createUniformBuffers()
{
  // Prepare and initialize the per-frame uniform m_buffer blocks containing shader uniforms
  // Single uniforms like in OpenGL are no longer present in Vulkan. All hader uniforms are passed via uniform m_buffer blocks
  VkMemoryRequirements memReqs;

  // Vertex shader uniform m_buffer block
  VkBufferCreateInfo bufferInfo{};
  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.pNext = nullptr;
  allocInfo.allocationSize = 0;
  allocInfo.memoryTypeIndex = 0;

  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = sizeof(ShaderData);
  // This m_buffer will be used as a uniform m_buffer
  bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

  // Create the buffers
  m_uniformBuffers.resize(MAX_CONCURRENT_FRAMES);
  for (uint32_t i = 0; i < MAX_CONCURRENT_FRAMES; i++)
  {
    VK_CHECK_RESULT(
        vkCreateBuffer(m_vkData->vkDevice, &bufferInfo, nullptr, &m_uniformBuffers[i].buffer));
    // Get m_memory requirements including size, alignment and m_memory type
    vkGetBufferMemoryRequirements(m_vkData->vkDevice, m_uniformBuffers[i].buffer, &memReqs);
    allocInfo.allocationSize = memReqs.size;
    // Get the m_memory type index that supports host visible m_memory access
    // Most implementations offer multiple m_memory types and selecting the correct one to allocate m_memory from is crucial
    // We also want the m_buffer to be host coherent so we don't have to flush (or sync after every update.
    // Note: This may affect performance so you might not want to do this in a real world application that updates buffers on a regular base
    allocInfo.memoryTypeIndex = m_deviceQueue->GetMemoryType(
        memReqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // Allocate m_memory for the uniform m_buffer
    VK_CHECK_RESULT(
        vkAllocateMemory(m_vkData->vkDevice, &allocInfo, nullptr, &(m_uniformBuffers[i].memory)));
    // Bind m_memory to m_buffer
    VK_CHECK_RESULT(vkBindBufferMemory(m_vkData->vkDevice, m_uniformBuffers[i].buffer,
                                       m_uniformBuffers[i].memory, 0));
    // We map the m_buffer once, so we can update it without having to map it again
    VK_CHECK_RESULT(vkMapMemory(m_vkData->vkDevice, m_uniformBuffers[i].memory, 0,
                                sizeof(ShaderData), 0, (void**)&m_uniformBuffers[i].mapped));
  }
}

void CVulkanShaderTexture::createDescriptorSetLayout()
{
  // Binding 0: Uniform m_buffer (Vertex shader)
  VkDescriptorSetLayoutBinding layoutBinding{};
  layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  layoutBinding.descriptorCount = 1;
  layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  layoutBinding.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutCreateInfo descriptorLayoutCI{};
  descriptorLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptorLayoutCI.pNext = nullptr;
  descriptorLayoutCI.bindingCount = 1;
  descriptorLayoutCI.pBindings = &layoutBinding;
  VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_vkData->vkDevice, &descriptorLayoutCI, nullptr,
                                              &m_descriptorSetLayout));
}

void CVulkanShaderTexture::createDescriptorPool()
{
  // We need to tell the API the number of max. requested descriptors per type
  VkDescriptorPoolSize descriptorTypeCounts[1]{};
  // This example only one m_descriptor type (uniform m_buffer)
  descriptorTypeCounts[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  // We have one m_buffer (and as such m_descriptor) per frame
  descriptorTypeCounts[0].descriptorCount = MAX_CONCURRENT_FRAMES;
  // For additional types you need to add new entries in the type count list
  // E.g. for two combined image samplers :
  // typeCounts[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  // typeCounts[1].descriptorCount = 2;

  // Create the global m_descriptor pool
  // All descriptors used in this example are allocated from this pool
  VkDescriptorPoolCreateInfo descriptorPoolCI{};
  descriptorPoolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptorPoolCI.pNext = nullptr;
  descriptorPoolCI.poolSizeCount = 1;
  descriptorPoolCI.pPoolSizes = descriptorTypeCounts;
  // Set the max. number of m_descriptor sets that can be requested from this pool (requesting beyond this limit will result in an error)
  // Our sample will create one set per uniform m_buffer per frame
  descriptorPoolCI.maxSets = MAX_CONCURRENT_FRAMES;
  VK_CHECK_RESULT(
      vkCreateDescriptorPool(m_vkData->vkDevice, &descriptorPoolCI, nullptr, &m_descriptorPool));
}

void CVulkanShaderTexture::createDescriptorSets()
{
  // Allocate one m_descriptor set per frame from the global m_descriptor pool
  for (uint32_t i = 0; i < MAX_CONCURRENT_FRAMES; i++)
  {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_descriptorSetLayout;
    VK_CHECK_RESULT(vkAllocateDescriptorSets(m_vkData->vkDevice, &allocInfo,
                                             &m_uniformBuffers[i].descriptorSet));

    // Update the m_descriptor set determining the shader binding points
    // For every binding point used in a shader there needs to be one
    // m_descriptor set matching that binding point
    VkWriteDescriptorSet writeDescriptorSet{};

    // The m_buffer's information is passed using a m_descriptor info structure
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = m_uniformBuffers[i].buffer;
    bufferInfo.range = sizeof(ShaderData);

    // Binding 0 : Uniform m_buffer
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = m_uniformBuffers[i].descriptorSet;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSet.pBufferInfo = &bufferInfo;
    writeDescriptorSet.dstBinding = 0;
    vkUpdateDescriptorSets(m_vkData->vkDevice, 1, &writeDescriptorSet, 0, nullptr);
  }
}

void CVulkanShaderTexture::createPipelines()
{
  // Create the m_pipeline layout that is used to generate the rendering pipelines that are based on this m_descriptor set layout
  // In a more complex scenario you would have different m_pipeline layouts for different m_descriptor set layouts that could be reused
  VkPipelineLayoutCreateInfo pipelineLayoutCI{};
  pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCI.pNext = nullptr;
  pipelineLayoutCI.setLayoutCount = 1;
  pipelineLayoutCI.pSetLayouts = &m_descriptorSetLayout;
  VK_CHECK_RESULT(
      vkCreatePipelineLayout(m_vkData->vkDevice, &pipelineLayoutCI, nullptr, &m_pipelineLayout));

  // Create the graphics m_pipeline used in this example
  // Vulkan uses the concept of rendering pipelines to encapsulate fixed states, replacing OpenGL's complex state machine
  // A m_pipeline is then stored and hashed on the GPU making m_pipeline changes very fast
  // Note: There are still a few dynamic states that are not directly part of the m_pipeline (but the info that they are used is)

  VkGraphicsPipelineCreateInfo pipelineCI{};
  pipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  // The layout used for this m_pipeline (can be shared among multiple pipelines using the same layout)
  pipelineCI.layout = m_pipelineLayout;
  // Renderpass this m_pipeline is attached to
  pipelineCI.renderPass = m_renderPass;

  // Construct the different states making up the m_pipeline

  // Input assembly state describes how primitives are assembled
  // This m_pipeline will assemble vertex data as a triangle lists (though we only use one triangle)
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCI{};
  inputAssemblyStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyStateCI.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

  // Rasterization state
  VkPipelineRasterizationStateCreateInfo rasterizationStateCI{};
  rasterizationStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizationStateCI.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizationStateCI.cullMode = VK_CULL_MODE_NONE;
  rasterizationStateCI.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizationStateCI.depthClampEnable = VK_FALSE;
  rasterizationStateCI.rasterizerDiscardEnable = VK_FALSE;
  rasterizationStateCI.depthBiasEnable = VK_FALSE;
  rasterizationStateCI.lineWidth = 1.0f;

  // Color blend state describes how blend factors are calculated (if used)
  // We need one blend attachment state per color attachment (even if blending is not used)
  VkPipelineColorBlendAttachmentState blendAttachmentState{};
  blendAttachmentState.colorWriteMask = 0xf;
  blendAttachmentState.blendEnable = VK_FALSE;
  VkPipelineColorBlendStateCreateInfo colorBlendStateCI{};
  colorBlendStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendStateCI.attachmentCount = 1;
  colorBlendStateCI.pAttachments = &blendAttachmentState;

  // Viewport state sets the number of viewports and scissor used in this m_pipeline
  // Note: This is actually overridden by the dynamic states (see below)
  VkPipelineViewportStateCreateInfo viewportStateCI{};
  viewportStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportStateCI.viewportCount = 1;
  viewportStateCI.scissorCount = 1;

  // Enable dynamic states
  // Most states are baked into the m_pipeline, but there are still a few dynamic states that can be changed within a command m_buffer
  // To be able to change these we need do specify which dynamic states will be changed using this m_pipeline. Their actual states are set later on in the command m_buffer.
  // For this example we will set the viewport and scissor using dynamic states
  std::vector<VkDynamicState> dynamicStateEnables;
  dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
  dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);
  VkPipelineDynamicStateCreateInfo dynamicStateCI{};
  dynamicStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicStateCI.pDynamicStates = dynamicStateEnables.data();
  dynamicStateCI.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

  // Depth and stencil state containing depth and stencil compare and test operations
  // We only use depth tests and want depth tests and writes to be enabled and compare with less or equal
  VkPipelineDepthStencilStateCreateInfo depthStencilStateCI{};
  depthStencilStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencilStateCI.depthTestEnable = VK_TRUE;
  depthStencilStateCI.depthWriteEnable = VK_TRUE;
  depthStencilStateCI.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
  depthStencilStateCI.depthBoundsTestEnable = VK_FALSE;
  depthStencilStateCI.back.failOp = VK_STENCIL_OP_KEEP;
  depthStencilStateCI.back.passOp = VK_STENCIL_OP_KEEP;
  depthStencilStateCI.back.compareOp = VK_COMPARE_OP_ALWAYS;
  depthStencilStateCI.stencilTestEnable = VK_FALSE;
  depthStencilStateCI.front = depthStencilStateCI.back;

  // Multi sampling state
  // This example does not make use of multi sampling (for anti-aliasing), the state must still be set and passed to the m_pipeline
  VkPipelineMultisampleStateCreateInfo multisampleStateCI{};
  multisampleStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampleStateCI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampleStateCI.pSampleMask = nullptr;

  // Vertex input descriptions
  // Specifies the vertex input parameters for a m_pipeline

  // Vertex input binding
  // This example uses a single vertex input binding at binding point 0 (see vkCmdBindVertexBuffers)
  VkVertexInputBindingDescription vertexInputBinding{};
  vertexInputBinding.binding = 0;
  vertexInputBinding.stride = sizeof(Vertex);
  vertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  // Input attribute bindings describe shader attribute locations and m_memory layouts
  std::array<VkVertexInputAttributeDescription, 4> vertexInputAttributs{};
  // These match the following shader layout (see triangle.vert):
  //	layout (location = 0) in vec3 inPos;
  //	layout (location = 1) in vec3 inColor;
  // Attribute location 0: Position
  vertexInputAttributs[0].binding = 0;
  vertexInputAttributs[0].location = 0;
  // Position attribute is three 32 bit signed (SFLOAT) floats (R32 G32 B32)
  vertexInputAttributs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertexInputAttributs[0].offset = offsetof(Vertex, in_attrpos);
  // Attribute location 1: Color
  vertexInputAttributs[1].binding = 0;
  vertexInputAttributs[1].location = 1;
  // Color attribute is four 32 bit signed (SFLOAT) floats (R32 G32 B32 A32)
  vertexInputAttributs[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
  vertexInputAttributs[1].offset = offsetof(Vertex, in_attrcol);
  // Attribute location 2: Texture coordinate 0
  vertexInputAttributs[2].binding = 0;
  vertexInputAttributs[2].location = 2;
  // Texture coordinate attribute 0 is two 32 bit signed (SFLOAT) floats (R32 G32)
  vertexInputAttributs[2].format = VK_FORMAT_R32G32_SFLOAT;
  vertexInputAttributs[2].offset = offsetof(Vertex, in_attrcord0);
  // Attribute location 3: Texture coordinate 1
  vertexInputAttributs[3].binding = 0;
  vertexInputAttributs[3].location = 3;
  // Texture coordinate attribute 1 is two 32 bit signed (SFLOAT) floats (R32 G32)
  vertexInputAttributs[3].format = VK_FORMAT_R32G32_SFLOAT;
  vertexInputAttributs[3].offset = offsetof(Vertex, in_attrcord1);

  // Vertex input state used for m_pipeline creation
  VkPipelineVertexInputStateCreateInfo vertexInputStateCI{};
  vertexInputStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputStateCI.vertexBindingDescriptionCount = 1;
  vertexInputStateCI.pVertexBindingDescriptions = &vertexInputBinding;
  vertexInputStateCI.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributs.size());
  vertexInputStateCI.pVertexAttributeDescriptions = vertexInputAttributs.data();

  // Shaders
  std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

  // Vertex shader
  shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  // Set m_pipeline stage for this shader
  shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
  // Load binary SPIR-V shader
  shaderStages[0].module = UTILS::vulkanCreateShaderModule(m_vkData->vkDevice, "triangle.vert.spv");
  // Main entry point for the shader
  shaderStages[0].pName = "main";
  assert(shaderStages[0].module != VK_NULL_HANDLE);

  // Fragment shader
  shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  // Set m_pipeline stage for this shader
  shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  // Load binary SPIR-V shader
  shaderStages[1].module = UTILS::vulkanCreateShaderModule(m_vkData->vkDevice, "triangle.frag.spv");
  // Main entry point for the shader
  shaderStages[1].pName = "main";
  assert(shaderStages[1].module != VK_NULL_HANDLE);

  // Set m_pipeline shader stage info
  pipelineCI.stageCount = static_cast<uint32_t>(shaderStages.size());
  pipelineCI.pStages = shaderStages.data();

  // Assign the m_pipeline states to the m_pipeline creation info structure
  pipelineCI.pVertexInputState = &vertexInputStateCI;
  pipelineCI.pInputAssemblyState = &inputAssemblyStateCI;
  pipelineCI.pRasterizationState = &rasterizationStateCI;
  pipelineCI.pColorBlendState = &colorBlendStateCI;
  pipelineCI.pMultisampleState = &multisampleStateCI;
  pipelineCI.pViewportState = &viewportStateCI;
  pipelineCI.pDepthStencilState = &depthStencilStateCI;
  pipelineCI.pDynamicState = &dynamicStateCI;

  // Create rendering m_pipeline using the specified states
  VK_CHECK_RESULT(vkCreateGraphicsPipelines(m_vkData->vkDevice, m_vkData->vkPipelineCache, 1,
                                            &pipelineCI, nullptr, &m_vkPipeline));

  // Shader modules are no longer needed once the graphics m_pipeline has been created
  vkDestroyShaderModule(m_vkData->vkDevice, shaderStages[0].module, nullptr);
  vkDestroyShaderModule(m_vkData->vkDevice, shaderStages[1].module, nullptr);
}

void CVulkanShaderTexture::UpdateUniformBuffer(uint32_t index, const ShaderData& shaderData)
{
  memcpy(m_uniformBuffers[index].mapped, &shaderData, sizeof(ShaderData));
}

void CVulkanShaderTexture::UpdateVerticesBuffer(uint32_t index, const Vertices& vertices)
{
  memcpy(&this->vertices, &vertices, sizeof(Vertices));
}

} // namespace KODI::RENDERING::VULKAN
