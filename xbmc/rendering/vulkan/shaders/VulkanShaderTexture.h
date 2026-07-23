/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "rendering/vulkan/shaders/IVulkanShader.h"

#include <memory>

#include <glm/glm.hpp>
#include <vector>

namespace KODI::RENDERING::VULKAN
{

class CVulkanDeviceQueue;
class CVulkanMemoryBuffer;

class CVulkanShaderTexture : public IVulkanShader
{
public:
  CVulkanShaderTexture(const VulkanData* vulkanData,
                       CVulkanDeviceQueue* deviceQueue);
  virtual ~CVulkanShaderTexture() = default;

  bool Create() override;
  void Destroy() override;

  VkPipeline VulkanPipeline() const override { return m_vkPipeline; }

private:
  VkPipeline m_vkPipeline{VK_NULL_HANDLE};
};

} // namespace KODI::RENDERING::VULKAN
