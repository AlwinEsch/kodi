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

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

constexpr ShaderId VULKAN_TEST_SHADER = 0;
constexpr ShaderId VULKAN_DEFAULT_SHADER = 1;
constexpr ShaderId VULKAN_TEXTURE_SHADER = 2;
constexpr ShaderId VULKAN_TEXTURE_NOALPHA_SHADER = 3;
constexpr ShaderId VULKAN_FONTS_SHADER = 4;

constexpr ShaderId SHADER_ID_INVALID = -1;
constexpr ShaderId OPTIONAL_SHADER_ID_START = 100;

class IVulkanShader;

class CVulkanShaderControl
{
public:
  CVulkanShaderControl() = default;
  virtual ~CVulkanShaderControl() = default;

  bool CreateAllShaders(VkDevice device, VkPipelineLayout pipelineLayout, VkRenderPass renderPass);
  void DestroyAllShaders();

  std::shared_ptr<IVulkanShader> GetShader(ShaderId shaderId) const;
  VkPipeline GetPipeline(ShaderId shaderId) const;

  ShaderId AddOptionalShader(const std::shared_ptr<IVulkanShader>& shader);

private:
  std::unordered_map<ShaderId, std::shared_ptr<IVulkanShader>> m_shaders;

  ShaderId m_nextShaderId{OPTIONAL_SHADER_ID_START};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
