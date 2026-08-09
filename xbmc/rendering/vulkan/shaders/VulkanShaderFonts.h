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

typedef enum FontsPipelineType
{
  FONTS_TYPE_SCISSOR_CLIP = 0,
  FONTS_TYPE_SHADER_CLIP = 1,
  FONTS_TYPE_SIZE = 2
} FontsPipelineType;

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

  struct VulkanUniformScissorClip
  {
    glm::mat4 matrix;
    float depth;
  };

  struct VulkanUniformShaderClip
  {
    glm::mat4 matrix;
    glm::vec4 shaderClip;
    glm::vec4 cordStep;
    float depth;
  };

  VkPipelineLayout VulkanPipelineLayout(FontsPipelineType type) const
  {
    return m_vkPipelineLayout;
  }
  VkPipeline VulkanPipeline(FontsPipelineType type) const { return m_vkPipeline[type]; }

  VulkanMemoryData* GetUniformBuffer(uint32_t index) { return &m_uniformBuffers[index]; }
  void UpdateUniformBuffer(uint32_t index, const VulkanUniformScissorClip& uniform);
  void UpdateUniformBuffer(uint32_t index, const VulkanUniformShaderClip& uniform);

protected:
  bool CreatePipelineLayout() override;
  void DestroyPipelineLayout() override;

  bool CreatePipeline() override;
  void DestroyPipeline() override;

  bool CreateUniformBuffers() override;
  void DestroyUniformBuffers() override;

private:
  VkPipelineLayout m_vkPipelineLayout{};
  std::array<VkPipeline, FONTS_TYPE_SIZE> m_vkPipeline{};
  std::array<VulkanMemoryData, MAX_CONCURRENT_FRAMES> m_uniformBuffers{};
};

} // namespace KODI::RENDERING::VULKAN
