/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "rendering/vulkan/VulkanData.h"

#include <memory>
#include <string_view>

#include <vulkan/vulkan_core.h>

namespace KODI::RENDERING::VULKAN
{

using ShaderId = int;

class IVulkanShader;
class CVulkanDeviceQueue;

struct ShaderListEntry
{
  ShaderId id;
  std::unique_ptr<IVulkanShader> (*create)(const VulkanData* vkData,
                                           CVulkanDeviceQueue* deviceQueue);
  std::string_view name;
};

template<typename Shader>
std::unique_ptr<IVulkanShader> ObjectFactory(const VulkanData* vkData,
                                             CVulkanDeviceQueue* deviceQueue)
{
  return std::make_unique<Shader>(vkData, deviceQueue);
}

enum class VulkanShaderType
{
  VERTEX_SHADER,
  FRAGMENT_SHADER,
  GEOMETRY_SHADER,
  TESSELLATION_CONTROL_SHADER,
  TESSELLATION_EVALUATION_SHADER,
  COMPUTE_SHADER
};

class IVulkanShader
{
public:
  IVulkanShader(const VulkanData* vkData, CVulkanDeviceQueue* deviceQueue)
    : m_vkData(vkData),
      m_deviceQueue(deviceQueue)
  {
  }

  virtual ~IVulkanShader() = default;

  virtual bool Create() = 0;
  virtual void Destroy() = 0;

  virtual VkPipeline VulkanPipeline() const = 0;

protected:
  const VulkanData* m_vkData;
  CVulkanDeviceQueue* const m_deviceQueue;
};

} // namespace KODI::RENDERING::VULKAN
