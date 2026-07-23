/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "rendering/vulkan/VulkanMemoryBuffer.h"
#include "rendering/vulkan/shaders/IVulkanShader.h"

#include <glm/glm.hpp>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

class CVulkanDeviceQueue;

class CVulkanShaderMulti : public IVulkanShader
{
public:
  CVulkanShaderMulti(const VulkanData* vulkanData,
                     CVulkanDeviceQueue* deviceQueue);
  virtual ~CVulkanShaderMulti() = default;

  bool Create(const VkPipelineCache& pipelineCache) override;
  void Destroy() override;

  VkPipeline VulkanPipeline() const override { return m_vkPipeline; }

private:
  VkPipeline m_vkPipeline{VK_NULL_HANDLE};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI