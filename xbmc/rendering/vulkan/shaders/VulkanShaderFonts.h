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

class CVulkanDeviceQueue;

enum FontsRenderType
{
  FONTS_TYPE_SCISSOR_CLIP = 0,
  FONTS_TYPE_SHADER_CLIP,
  FONTS_TYPE_MAX
};

class CVulkanShaderFonts : public IVulkanShader
{
public:
  CVulkanShaderFonts(const VulkanData* vulkanData, CVulkanDeviceQueue* deviceQueue);
  virtual ~CVulkanShaderFonts() = default;

  struct VulkanUniform
  {
    glm::mat4 projModelMatrix;
    float depth;
  };

  struct ClipPushConstants_Scissor
  {
    glm::mat4 viewMatrix;
  };

  struct ClipPushConstants_Shader
  {
    glm::mat4 viewMatrix;
    glm::vec4 shaderClip;
    glm::vec4 cordStep;
  };

  union ClipPushConstants
  {
    ClipPushConstants_Scissor scissor;
    ClipPushConstants_Shader shader;
  };

  VkPipelineLayout VulkanPipelineLayout(FontsRenderType type) const
  {
    return m_vkPipelineLayout[type];
  }
  VkPipeline VulkanPipeline(FontsRenderType type) const { return m_vkPipeline[type]; }

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
  std::array<VkPipelineLayout, FONTS_TYPE_MAX> m_vkPipelineLayout{VK_NULL_HANDLE};
  std::array<VkPipeline, FONTS_TYPE_MAX> m_vkPipeline{VK_NULL_HANDLE};
  std::array<VulkanMemoryData, MAX_CONCURRENT_FRAMES> m_uniformBuffers{};
};

} // namespace KODI::RENDERING::VULKAN
