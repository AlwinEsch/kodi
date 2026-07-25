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

namespace KODI::RENDERING::VULKAN
{

// We want to keep GPU and CPU busy. To do that we may start building a new command m_buffer while the previous one is still being executed
// This number defines how many frames may be worked on simultaneously at once
// Increasing this number may improve performance but will also introduce additional latency
constexpr auto MAX_CONCURRENT_FRAMES = 3;

constexpr auto PARTICLE_COUNT = 512;

// Vertex layout used in this example
//struct Vertex
//{
//  float position[3];
//  float color[3];
//  float in_attrcord0[2];
//  float in_attrcord1[2];
//};

struct Vertex
{
  glm::vec3 in_attrpos;
  glm::vec4 in_attrcol;
  glm::vec2 in_attrcord0;
  glm::vec2 in_attrcord1;
};

// Vertex m_buffer and attributes
struct Vertices
{
  VkDeviceMemory memory{VK_NULL_HANDLE}; // Handle to the device memory for this buffer
  VkBuffer buffer{VK_NULL_HANDLE}; // Handle to the Vulkan buffer object that the memory is bound to
};

// Index buffer
struct Indices
{
  VkDeviceMemory memory{VK_NULL_HANDLE};
  VkBuffer buffer{VK_NULL_HANDLE};
  uint32_t count{0};
};

// Uniform buffer block object
struct UniformBuffer
{
  VkDeviceMemory memory{VK_NULL_HANDLE};
  VkBuffer buffer{VK_NULL_HANDLE};
  // The descriptor set stores the resources bound to the binding points in a shader
  // It connects the binding points of the different shaders with the buffers and images used for those bindings
  VkDescriptorSet descriptorSet{VK_NULL_HANDLE};
  // We keep a pointer to the mapped buffer, so we can easily update its contents via a memcpy
  uint8_t* mapped{nullptr};
};

// For simplicity we use the same uniform block layout as in the shader:
//
//	layout(set = 0, binding = 0) uniform UBO
//	{
//		mat4 projectionMatrix;
//		mat4 modelMatrix;
//		mat4 viewMatrix;
//	} ubo;
//
// This way we can just memcopy the ubo data to the ubo
// Note: You should use data types that align with the GPU in order to avoid manual padding (vec4, mat4)
struct ShaderData
{
  glm::mat4 projectionMatrix;
  glm::mat4 modelMatrix;
  glm::mat4 viewMatrix;
};

struct ParticleBuffer
{
  VkBuffer buffer{VK_NULL_HANDLE};
  VkDeviceMemory memory{VK_NULL_HANDLE};
  // Store the mapped address of the particle data for reuse
  void* mappedMemory{nullptr};
  // Size of the particle buffer in bytes
  size_t size{0};
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
  VkPipelineLayout VulkanPipelineLayout() const { return m_pipelineLayout; }
  UniformBuffer* GetUniformBuffer(uint32_t index) { return &m_uniformBuffers[index]; }
  void UpdateUniformBuffer(uint32_t index, const ShaderData& shaderData);
  void UpdateVerticesBuffer(uint32_t index, const Vertex& vertices);

  Vertices* GetVertexBuffer() { return &vertices; }
  Indices* GetIndexBuffer() { return &indices; }

private:
  VkPipeline m_vkPipeline{VK_NULL_HANDLE};
  VkPipelineLayout m_pipelineLayout{VK_NULL_HANDLE};
  Vertices vertices;
  Indices indices;

  void prepareParticles();
  void createVertexBuffer();
  void createUniformBuffers();
  void createDescriptorSetLayout();
  void createDescriptorPool();
  void createDescriptorSets();
  void createPipelines();

  // We use one UBO per frame, so we can have a frame overlap and make sure that uniforms aren't updated while still in use
  std::vector<UniformBuffer> m_uniformBuffers;
  std::vector<Vertices> m_particles{};

  VkDescriptorSetLayout m_descriptorSetLayout{VK_NULL_HANDLE};
  VkRenderPass m_renderPass{VK_NULL_HANDLE};
  VkDescriptorPool m_descriptorPool{VK_NULL_HANDLE};

  std::array<ParticleBuffer, MAX_CONCURRENT_FRAMES> m_particleBuffers;
};

} // namespace KODI::RENDERING::VULKAN
