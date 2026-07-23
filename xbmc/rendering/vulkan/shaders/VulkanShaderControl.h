/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "IVulkanShader.h"

#include <memory>
#include <unordered_map>

namespace KODI::RENDERING::VULKAN
{

constexpr ShaderId VULKAN_SM_TEST = 0;
constexpr ShaderId VULKAN_SM_DEFAULT = 1;
constexpr ShaderId VULKAN_SM_TEXTURE = 2;
constexpr ShaderId VULKAN_SM_TEXTURE_LIM = 3;
constexpr ShaderId VULKAN_SM_TEXTURE_NOALPHA = 4;
constexpr ShaderId VULKAN_SM_TEXTURE_NOBLEND = 5;
constexpr ShaderId VULKAN_SM_MULTI = 6;
constexpr ShaderId VULKAN_SM_FONTS = 7;
constexpr ShaderId VULKAN_SM_FONTS_SHADER_CLIP = 8;
constexpr ShaderId VULKAN_SM_MULTI_BLENDCOLOR = 9;

constexpr ShaderId VULKAN_SM_ID_INVALID = -1;
constexpr ShaderId VULKAN_SM_OPTIONAL_ID_START = 100;

class IVulkanShader;

class CVulkanShaderControl
{
public:
  CVulkanShaderControl(const VulkanData* vulkanData, CVulkanDeviceQueue* deviceQueue);
  virtual ~CVulkanShaderControl() = default;

  bool CreateAllShaders(VkDevice device, VkPipelineLayout pipelineLayout, VkRenderPass renderPass);
  void DestroyAllShaders();

  IVulkanShader* GetShader(ShaderId shaderId) const;
  VkPipeline GetPipeline(ShaderId shaderId) const;

  ShaderId AddOptionalShader(std::unique_ptr<IVulkanShader> shader);

private:
  const VulkanData* m_vulkanData;
  CVulkanDeviceQueue* const m_deviceQueue;

  std::unordered_map<ShaderId, std::unique_ptr<IVulkanShader>> m_shaders;

  ShaderId m_nextShaderId{VULKAN_SM_OPTIONAL_ID_START};
  VkPipelineCache m_pipelineCache{VK_NULL_HANDLE};
};

} // namespace KODI::RENDERING::VULKAN
