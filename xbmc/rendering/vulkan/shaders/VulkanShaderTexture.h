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
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#define USE_PARTICLES 1

namespace KODI::RENDERING::VULKAN
{

constexpr auto PARTICLE_COUNT = 512;

// Vertex layout used for the texture shader. This layout is used to define the input
// attributes for the vertex shader stage of the graphics pipeline.
// It includes the position of the vertex and two sets of texture coordinates,
// which can be used for multi-texturing or other effects.
struct ShaderTextureVertex
{
  glm::vec3 in_attrpos;
  glm::vec2 in_attrcord0;
  glm::vec2 in_attrcord1;
};

class CVulkanDeviceQueue;

class CVulkanShaderTexture : public IVulkanShader
{
public:
  CVulkanShaderTexture(const VulkanData* vulkanData, CVulkanDeviceQueue* deviceQueue);
  virtual ~CVulkanShaderTexture() = default;

protected:
  bool CreatePipelineLayout() override;
  bool CreatePipeline() override;
};

} // namespace KODI::RENDERING::VULKAN
