/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "rendering/vulkan/VulkanData.h"
#include "rendering/vulkan/shaders/IVulkanShader.h"

#include <array>
#include <cstring>

#include <glm/glm.hpp>

namespace KODI::RENDERING::VULKAN
{

constexpr auto PARTICLE_COUNT = 512;

typedef enum TexturePipelineType {
  TEXTURE_TYPE_BLEND = 0,
  TEXTURE_TYPE_NO_BLEND = 1,
  TEXTURE_TYPE_SIZE = 2
} TexturePipelineType;

class CVulkanDeviceQueue;

class CVulkanShaderTexture : public IVulkanShader
{
public:
  CVulkanShaderTexture(const VulkanData* vulkanData, CVulkanDeviceQueue* deviceQueue);
  virtual ~CVulkanShaderTexture() = default;

  struct Vertex
  {
    glm::vec3 in_attrpos;
    glm::vec2 in_attrcord0;
    glm::vec2 in_attrcord1;
  };

  struct Uniform
  {
    glm::mat4 projectionMatrix;
    glm::mat4 modelMatrix;
    float depth;
  };

  void vkPipeline(VkPipeline& pipeline, VkPipelineLayout& layout, int type) override
  {
    pipeline = m_vkPipeline[type];
    layout = m_vkPipelineLayout[type];
  }

  VulkanMemoryData* GetUniformBuffer(uint32_t index) override { return &m_uniformBuffers[index]; }
  void UpdateUniformBuffer(uint32_t index, const Uniform& uniform);

protected:
  bool CreatePipelineLayout() override;
  void DestroyPipelineLayout() override;

  bool CreatePipeline() override;
  void DestroyPipeline() override;

  bool CreateUniformBuffers() override;
  void DestroyUniformBuffers() override;

private:
  std::array<VkPipelineLayout, TEXTURE_TYPE_SIZE> m_vkPipelineLayout{};
  std::array<VkPipeline, TEXTURE_TYPE_SIZE> m_vkPipeline{};
  std::array<VulkanMemoryData, MAX_CONCURRENT_FRAMES> m_uniformBuffers{};
};

} // namespace KODI::RENDERING::VULKAN
