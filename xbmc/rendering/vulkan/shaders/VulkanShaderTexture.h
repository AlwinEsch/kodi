/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "rendering/vulkan/shaders/IVulkanShader.h"

#include <array>
#include <cstring>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#define USE_PARTICLES 1

namespace KODI::RENDERING::VULKAN
{

// We want to keep GPU and CPU busy. To do that we may start building a new command m_buffer while the previous one is still being executed
// This number defines how many frames may be worked on simultaneously at once
// Increasing this number may improve performance but will also introduce additional latency
constexpr auto MAX_CONCURRENT_FRAMES = 3;

constexpr auto PARTICLE_COUNT = 512;

// Vertex layout used in this example
struct Vertex
{
  glm::vec3 in_attrpos;
  glm::vec4 in_attrcol;
  glm::vec2 in_attrcord0;
  glm::vec2 in_attrcord1;
};

struct Uniform
{
  glm::mat4 projectionMatrix;
  glm::mat4 modelMatrix;
  glm::mat4 viewMatrix;
};

struct Texture
{
  VkSampler sampler{VK_NULL_HANDLE};
  VkImage image{VK_NULL_HANDLE};
  VkImageLayout imageLayout;
  VkDeviceMemory deviceMemory{VK_NULL_HANDLE};
  VkImageView view{VK_NULL_HANDLE};
  uint32_t width{0};
  uint32_t height{0};
  uint32_t mipLevels{0};
};

class CVulkanDeviceQueue;
class CVulkanMemoryBuffer;

class CVulkanShaderTexture : public IVulkanShader
{
public:
  CVulkanShaderTexture(const VulkanData* vulkanData, CVulkanDeviceQueue* deviceQueue);
  virtual ~CVulkanShaderTexture() = default;

  bool Create() override;
  void Destroy() override;

  VkPipeline VulkanPipeline() const override { return m_vkPipeline; }
  VkPipelineLayout VulkanPipelineLayout() const { return m_vkPipelineLayout; }
  VulkanMemoryData* GetUniformBuffer(uint32_t index) { return &m_uniformBuffers[index]; }
#if USE_PARTICLES == 1
  VulkanMemoryData* GetVertexBuffer(uint32_t index) { return &m_vertexBuffers[index]; }
  VulkanMemoryData* GetIndexBuffer(uint32_t index) { return &m_indexBuffers[index]; }
  void UpdateVerticesBuffer(uint32_t index, const Vertex* vertices);
  void UpdateIndeciesBuffer(uint32_t index, const uint32_t* indices, size_t count);
#else
  VulkanMemoryData* GetVertexBuffer() { return &m_vertices; }
  VulkanMemoryData* GetIndexBuffer() { return &m_indices; }
#endif

  void UpdateUniformBuffer(uint32_t index, const Uniform& uniform);

private:
  void CreateVertexBuffer();
  void CreateUniformBuffers();
  void SetupDescriptors();
  void CreatePipelines();

  VkPipeline m_vkPipeline{VK_NULL_HANDLE};
  VkPipelineLayout m_vkPipelineLayout{VK_NULL_HANDLE};
  VkDescriptorSetLayout m_descrSetLayout{VK_NULL_HANDLE};
  VkRenderPass m_renderPass{VK_NULL_HANDLE};
  VkDescriptorPool m_descriptorPool{VK_NULL_HANDLE};

  std::array<VulkanMemoryData, MAX_CONCURRENT_FRAMES> m_uniformBuffers;
#if USE_PARTICLES == 1
  std::array<VulkanMemoryData, MAX_CONCURRENT_FRAMES> m_vertexBuffers;
  std::array<VulkanMemoryData, MAX_CONCURRENT_FRAMES> m_indexBuffers;
#else
  VulkanMemoryData m_vertices;
  VulkanMemoryData m_indices;
#endif
};

} // namespace KODI::RENDERING::VULKAN
