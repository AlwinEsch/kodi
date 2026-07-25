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

bool CVulkanShaderTexture::Create()
{
  createVertexBuffer();
  CreateUniformBuffers();
  CreateDescriptorSets();
  CreatePipelines();
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
  //// One m_buffer per concurrent frame, so we can update one frame while the other is still rendering
  //m_particles.resize(PARTICLE_COUNT);
  //for (auto& buffer : m_particleBuffers)
  //{
  //  buffer.size = m_particles.size() * sizeof(Vertex);

  //  VK_CHECK_RESULT(m_vulkanDevice->createBuffer(
  //      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
  //      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_vertexBuffer, vertices.m_size() * sizeof(Vertex)));


  //  VK_CHECK_RESULT(m_deviceQueue->CreateBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
  //                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
  //                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  //                                              &buffer, m_particles.size(), m_particles.data());

  //  // Map the memory and store the pointer for reuse
  //  VK_CHECK_RESULT(
  //      vkMapMemory(m_vkData->vkDevice, buffer.memory, 0, buffer.size, 0, &buffer.mapped));
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
  indices.size = static_cast<uint32_t>(m_indexBuffer.size());
  uint32_t indexBufferSize = indices.size * sizeof(uint32_t);

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

void CVulkanShaderTexture::CreateUniformBuffers()
{
  m_uniformBuffers.resize(MAX_CONCURRENT_FRAMES);
  for (auto& buffer : m_uniformBuffers)
  {
    VK_CHECK_RESULT(m_deviceQueue->CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                &buffer, sizeof(Uniform), &m_uniformData));
  }
}

void CVulkanShaderTexture::CreateDescriptorSets()
{
  auto dev = m_vkData->vkDevice;

  // Pool
  std::vector<VkDescriptorPoolSize> poolSizes = {
      vkDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_CONCURRENT_FRAMES),
      vkDescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_CONCURRENT_FRAMES),
  };
  auto descriptorPoolInfo = vkDescriptorPoolCreateInfo(poolSizes, MAX_CONCURRENT_FRAMES);
  VK_CHECK_RESULT(vkCreateDescriptorPool(dev, &descriptorPoolInfo, nullptr, &m_descriptorPool));

  // Layout
  std::vector<VkDescriptorSetLayoutBinding> bindings{
      // Binding 0 : Vertex shader uniform m_buffer
      {
          .binding = 0,
          .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
          .descriptorCount = 1,
          .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
          .pImmutableSamplers = nullptr,
      },
      // Binding 1 : Fragment shader image sampler
      {
          .binding = 1,
          .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
          .descriptorCount = 1,
          .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
          .pImmutableSamplers = nullptr,
      },
  };
  auto descrLayout = vkDescriptorSetLayoutCreateInfo(bindings.data(), bindings.size());
  VK_CHECK_RESULT(vkCreateDescriptorSetLayout(dev, &descrLayout, nullptr, &m_descrSetLayout));

  // Allocate one descriptor set per frame from the global descriptor pool
  auto allocInfo = vkDescriptorSetAllocateInfo(m_descriptorPool, &m_descrSetLayout, 1);
  for (uint32_t i = 0; i < MAX_CONCURRENT_FRAMES; i++)
  {
    VK_CHECK_RESULT(vkAllocateDescriptorSets(dev, &allocInfo, &m_uniformBuffers[i].descriptorSet));

    // The m_buffer's information is passed using a descriptor info structure
    VkDescriptorBufferInfo bufferInfo{
        .buffer = m_uniformBuffers[i].buffer,
        .offset = 0,
        .range = VK_WHOLE_SIZE,
    };

    // Update the descriptor set determining the shader binding points
    // For every binding point used in a shader there needs to be one
    // descriptor set matching that binding point
    std::vector<VkWriteDescriptorSet> descSets = {
        {
            // Binding 0 : Uniform buffer
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = m_uniformBuffers[i].descriptorSet,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImageInfo = nullptr,
            .pBufferInfo = &bufferInfo,
            .pTexelBufferView = nullptr,
        },
        //{
        //    // Binding 1 : Fragment shader texture sampler
        //    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        //    .pNext = nullptr,
        //    .dstSet = m_uniformBuffers[i].descriptorSet,
        //    .dstBinding = 1,
        //    .dstArrayElement = 0,
        //    .descriptorCount = 1,
        //    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        //    .pImageInfo = &m_textureDescriptor,
        //    .pBufferInfo = nullptr,
        //    .pTexelBufferView = nullptr,
        //},
    };
    vkUpdateDescriptorSets(dev, static_cast<uint32_t>(descSets.size()), descSets.data(), 0,
                           nullptr);
  }
}

void CVulkanShaderTexture::CreatePipelines()
{
  // Layout
  auto layoutInfo = vkPipelineLayoutCreateInfo(&m_descrSetLayout, 1);
  VK_CHECK_RESULT(
      vkCreatePipelineLayout(m_vkData->vkDevice, &layoutInfo, nullptr, &m_vkPipelineLayout));

  // Pipeline
  std::vector<VkDynamicState> enables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  auto dynState = vkPipelineDynamicStateCreateInfo(enables.data(), enables.size());
  auto inputAssembly = vkPipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
  auto blendAttachmentState = vkPipelineColorBlendAttachmentState(0xf, VK_FALSE);
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
      vkVertexInputAttrDescr(0, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, in_attrcol)),
      vkVertexInputAttrDescr(0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, in_attrcord0)),
      vkVertexInputAttrDescr(0, 3, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, in_attrcord1)),
  };

  // Vertex input state used for m_pipeline creation
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
    return;
  }

  auto pipelineCI = vkPipelineCreateInfo(m_vkPipelineLayout, m_renderPass);
  pipelineCI.stageCount = static_cast<uint32_t>(shaderStages.size());
  pipelineCI.pStages = shaderStages.data();
  pipelineCI.pVertexInputState = &inputState;
  pipelineCI.pInputAssemblyState = &inputAssembly;
  pipelineCI.pRasterizationState = &raster;
  pipelineCI.pColorBlendState = &colorBlendState;
  pipelineCI.pMultisampleState = &multisampleState;
  pipelineCI.pViewportState = &viewportState;
  pipelineCI.pDepthStencilState = &depthStencilState;
  pipelineCI.pDynamicState = &dynState;
  VK_CHECK_RESULT(vkCreateGraphicsPipelines(m_vkData->vkDevice, m_vkData->vkPipelineCache, 1,
                                            &pipelineCI, nullptr, &m_vkPipeline));

  // Shader modules are no longer needed once the graphics m_pipeline has been created
  UnloadShader(shaderStages[0]);
  UnloadShader(shaderStages[1]);
}

void CVulkanShaderTexture::UpdateUniformBuffer(uint32_t index, const Uniform& uniformData)
{
  memcpy(m_uniformBuffers[index].mapped, &uniformData, sizeof(Uniform));
}

void CVulkanShaderTexture::UpdateVerticesBuffer(uint32_t index, const Vertex& verticesData)
{
  memcpy(m_particleBuffers[index].mapped, &verticesData, sizeof(Vertex));
}

} // namespace KODI::RENDERING::VULKAN
