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

const std::vector<Vertex> defaultVertexBuffer{
    {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}}};

const std::vector<uint32_t> defaultIndexBuffer{0, 1, 2, 2, 3, 0};

} // namespace

CVulkanShaderTexture::CVulkanShaderTexture(const VulkanData* vkData,
                                           CVulkanDeviceQueue* deviceQueue)
  : IVulkanShader(vkData, deviceQueue)
{
}

bool CVulkanShaderTexture::Create()
{
  CreateVertexBuffer();
  CreateUniformBuffers();
  SetupDescriptors();
  CreatePipelines();
  return true;
}

void CVulkanShaderTexture::Destroy()
{
  auto dev = m_vkData->vkDevice;

  if (m_vkPipeline != VK_NULL_HANDLE)
  {
    vkDestroyPipeline(dev, m_vkPipeline, nullptr);
    m_vkPipeline = VK_NULL_HANDLE;
  }
  if (m_vkPipelineLayout != VK_NULL_HANDLE)
  {
    vkDestroyPipelineLayout(dev, m_vkPipelineLayout, nullptr);
    m_vkPipelineLayout = VK_NULL_HANDLE;
  }
  if (m_descrSetLayout != VK_NULL_HANDLE)
  {
    vkDestroyDescriptorSetLayout(dev, m_descrSetLayout, nullptr);
    m_descrSetLayout = VK_NULL_HANDLE;
  }
  if (m_descriptorPool != VK_NULL_HANDLE)
  {
    vkDestroyDescriptorPool(dev, m_descriptorPool, nullptr);
    m_descriptorPool = VK_NULL_HANDLE;
  }
  for (auto& buffer : m_uniformBuffers)
  {
    m_deviceQueue->DestroyBuffer(&buffer);
    buffer = {};
  }

#if USE_PARTICLES == 1
  for (auto& buffer : m_vertexBuffers)
  {
    m_deviceQueue->DestroyBuffer(&buffer);
    buffer = {};
  }
  for (auto& buffer : m_indexBuffers)
  {
    m_deviceQueue->DestroyBuffer(&buffer);
    buffer = {};
  }
#else
  m_deviceQueue->DestroyBuffer(&m_vertices);
  m_deviceQueue->DestroyBuffer(&m_indices);
#endif
}

void CVulkanShaderTexture::CreateVertexBuffer()
{
#if USE_PARTICLES == 1
  for (auto& buffer : m_vertexBuffers)
  {
    VK_CHECK_RESULT(m_deviceQueue->CreateBuffer(
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &buffer,
        sizeof(Vertex) * defaultVertexBuffer.size(), defaultVertexBuffer.data()));
    // We map the m_buffer once, so we can update it without having to map it again
    VK_CHECK_RESULT(vkMapMemory(m_vkData->vkDevice, buffer.memory, 0,
                                sizeof(Vertex) * defaultVertexBuffer.size(), 0,
                                (void**)&buffer.mapped));
  }

  for (auto& buffer : m_indexBuffers)
  {
    VK_CHECK_RESULT(m_deviceQueue->CreateBuffer(
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &buffer,
        64 * sizeof(uint32_t) * defaultIndexBuffer.size()));
    // We map the m_buffer once, so we can update it without having to map it again
    VK_CHECK_RESULT(vkMapMemory(m_vkData->vkDevice, buffer.memory, 0,
                                sizeof(uint32_t) * defaultIndexBuffer.size(), 0,
                                (void**)&buffer.mapped));

    memcpy(buffer.mapped, defaultIndexBuffer.data(), sizeof(uint32_t) * defaultIndexBuffer.size());
  }
#else
  const uint32_t vertexBufferSize =
      static_cast<uint32_t>(defaultVertexBuffer.size()) * sizeof(Vertex);
  const uint32_t indexBufferSize =
      static_cast<uint32_t>(defaultIndexBuffer.size()) * sizeof(uint32_t);

  struct
  {
    VulkanMemoryData vertices;
    VulkanMemoryData indices;
  } stagingBuffers{};

  // Host visible source buffers (staging)
  VK_CHECK_RESULT(m_deviceQueue->CreateBuffer(
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      &stagingBuffers.vertices, vertexBufferSize, defaultVertexBuffer.data()));
  VK_CHECK_RESULT(m_deviceQueue->CreateBuffer(
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      &stagingBuffers.indices, indexBufferSize, defaultIndexBuffer.data()));

  // Device local destination buffers
  VK_CHECK_RESULT(m_deviceQueue->CreateBuffer(
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_vertices, vertexBufferSize, nullptr));
  VK_CHECK_RESULT(m_deviceQueue->CreateBuffer(
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_indices, indexBufferSize, nullptr));

  // Copy from host do device
  std::vector<std::pair<VulkanMemoryData*, VulkanMemoryData*>> srcDstPairs = {
      //{&stagingBuffers.vertices, &m_vertices},
      {&stagingBuffers.indices, &m_indices},
  };
  m_deviceQueue->CopyBuffers(srcDstPairs);

  // Clean up
  m_deviceQueue->DestroyBuffer(&stagingBuffers.vertices);
  m_deviceQueue->DestroyBuffer(&stagingBuffers.indices);
#endif
}

void CVulkanShaderTexture::CreateUniformBuffers()
{
  // Create the buffers
  for (auto& buffer : m_uniformBuffers)
  {
    VK_CHECK_RESULT(m_deviceQueue->CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                &buffer, sizeof(Uniform), nullptr));

    // Bind m_memory to m_buffer
    VK_CHECK_RESULT(vkBindBufferMemory(m_vkData->vkDevice, buffer.buffer, buffer.memory, 0));
    // We map the m_buffer once, so we can update it without having to map it again
    VK_CHECK_RESULT(vkMapMemory(m_vkData->vkDevice, buffer.memory, 0, sizeof(Uniform), 0,
                                (void**)&buffer.mapped));
  }
}

void CVulkanShaderTexture::SetupDescriptors()
{
  auto dev = m_vkData->vkDevice;

  std::vector<VkDescriptorPoolSize> descriptorTypeCounts{
      {
          .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
          .descriptorCount = MAX_CONCURRENT_FRAMES,
      },
      {
          .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
          .descriptorCount = MAX_CONCURRENT_FRAMES,
      },
  };

  // Create the global m_descriptor pool
  // All descriptors used in this example are allocated from this pool
  VkDescriptorPoolCreateInfo descriptorPoolCI{};
  descriptorPoolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptorPoolCI.pNext = nullptr;
  descriptorPoolCI.poolSizeCount = static_cast<uint32_t>(descriptorTypeCounts.size());
  descriptorPoolCI.pPoolSizes = descriptorTypeCounts.data();
  // Set the max. number of descriptor sets that can be requested from this pool (requesting beyond this limit will result in an error)
  // Our sample will create one set per uniform buffer per frame
  descriptorPoolCI.maxSets = MAX_CONCURRENT_FRAMES;
  VK_CHECK_RESULT(vkCreateDescriptorPool(dev, &descriptorPoolCI, nullptr, &m_descriptorPool));

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

  // Allocate one descriptor set per frame from the global m_descriptor pool
  auto allocInfo = vkDescriptorSetAllocateInfo(m_descriptorPool, &m_descrSetLayout, 1);
  for (uint32_t i = 0; i < MAX_CONCURRENT_FRAMES; i++)
  {
    VK_CHECK_RESULT(vkAllocateDescriptorSets(dev, &allocInfo, &m_uniformBuffers[i].descriptorSet));

    // The m_buffer's information is passed using a m_descriptor info structure
    VkDescriptorBufferInfo bufferInfo{
        .buffer = m_uniformBuffers[i].buffer,
        .offset = 0,
        .range = sizeof(Uniform),
    };

    // Update the m_descriptor set determining the shader binding points
    // For every binding point used in a shader there needs to be one
    // m_descriptor set matching that binding point
    std::vector<VkWriteDescriptorSet> descSets = {
        {
            // Binding 0 : Uniform m_buffer
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
#if 0
        // Done within the texture class, so we don't need to do it here
        {
            // Binding 1 : Fragment shader texture sampler
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = m_uniformBuffers[i].descriptorSet,
            .dstBinding = 1,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = &m_textureDescriptor,
            .pBufferInfo = nullptr,
            .pTexelBufferView = nullptr,
        },
#endif
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

  auto pipeline = vkPipelineCreateInfo(m_vkPipelineLayout, m_renderPass);
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
                                            &pipeline, nullptr, &m_vkPipeline));

  // Shader modules are no longer needed once the graphics m_pipeline has been created
  UnloadShader(shaderStages[0]);
  UnloadShader(shaderStages[1]);
}

void CVulkanShaderTexture::UpdateUniformBuffer(uint32_t index, const Uniform& uniformData)
{
  memcpy(m_uniformBuffers[index].mapped, &uniformData, sizeof(Uniform));
}

#if USE_PARTICLES == 1
void CVulkanShaderTexture::UpdateVerticesBuffer(uint32_t index, const Vertex* vertices)
{
  memcpy(m_vertexBuffers[index].mapped, vertices, sizeof(Vertex) * defaultVertexBuffer.size());
}

void CVulkanShaderTexture::UpdateIndeciesBuffer(uint32_t index, const uint32_t* indices, size_t count)
{
  memcpy(m_indexBuffers[index].mapped, indices, sizeof(uint32_t) * count);
}
#endif

} // namespace KODI::RENDERING::VULKAN
