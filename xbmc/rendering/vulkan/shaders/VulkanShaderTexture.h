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

// Vertex layout used in this example
struct Vertex
{
  glm::vec3 in_attrpos;
  glm::vec2 in_attrcord0;
  glm::vec2 in_attrcord1;
};

class CVulkanDeviceQueue;
class CVulkanMemoryBuffer;

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
