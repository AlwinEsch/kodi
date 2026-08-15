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

class CVulkanDeviceQueue;

class CVulkanShaderMulti : public IVulkanShader
{
public:
  CVulkanShaderMulti(const VulkanData* vulkanData,
                     CVulkanDeviceQueue* deviceQueue);
  virtual ~CVulkanShaderMulti() = default;

private:
};

} // namespace KODI::RENDERING::VULKAN