/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "rendering/vulkan/shaders/IVulkanShader.h"

#include <glm/glm.hpp>

namespace KODI::RENDERING::VULKAN
{

struct ShaderFontsVertex
{
  glm::vec3 in_attrpos;
  glm::vec4 in_attrcolor;
  glm::vec2 in_attrcord0;
  glm::vec2 in_attrcord1;
};

class CVulkanDeviceQueue;

class CVulkanShaderFonts : public IVulkanShader
{
public:
  CVulkanShaderFonts(const VulkanData* vulkanData,
                     CVulkanDeviceQueue* deviceQueue);
  virtual ~CVulkanShaderFonts() = default;

protected:
  bool CreatePipelineLayout() override;
  bool CreatePipeline() override;
};

} // namespace KODI::RENDERING::VULKAN