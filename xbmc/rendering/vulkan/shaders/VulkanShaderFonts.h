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

#include <glm/glm.hpp>

namespace KODI::RENDERING::VULKAN
{

struct ShaderFontsVertex
{
  glm::vec3 in_attrpos;
  glm::vec4 in_attrcolor;
  glm::vec2 in_attrcord0;
};

class CVulkanDeviceQueue;

class CVulkanShaderFonts : public IVulkanShader
{
public:
  CVulkanShaderFonts(const VulkanData* vulkanData, CVulkanDeviceQueue* deviceQueue);
  virtual ~CVulkanShaderFonts() = default;

  struct VulkanUniform
  {
    glm::mat4 projectionMatrix;
    glm::mat4 modelMatrix;
    float depth;
  };

  VkPipelineLayout VulkanPipelineLayout() const
  {
    return m_vkPipelineLayout;
  }
  VkPipeline VulkanPipeline() const { return m_vkPipeline; }

  VulkanMemoryData* GetUniformBuffer(uint32_t index) { return &m_uniformBuffers[index]; }
  void UpdateUniformBuffer(uint32_t index, const VulkanUniform& uniform);

protected:
  bool CreatePipelineLayout() override;
  void DestroyPipelineLayout() override;

  bool CreatePipeline() override;
  void DestroyPipeline() override;

  bool CreateUniformBuffers() override;
  void DestroyUniformBuffers() override;

private:
  VkPipelineLayout m_vkPipelineLayout{VK_NULL_HANDLE};
  VkPipeline m_vkPipeline{VK_NULL_HANDLE};
  std::array<VulkanMemoryData, MAX_CONCURRENT_FRAMES> m_uniformBuffers{};
};

} // namespace KODI::RENDERING::VULKAN
