/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <memory>
#include <string_view>
#include <vulkan/vulkan_core.h>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

using ShaderId = int;

class IVulkanShader;

struct ShaderListEntry
{
  ShaderId id;
  std::shared_ptr<IVulkanShader> (*create)(VkDevice device, VkPipelineLayout layout, VkRenderPass renderPass);
  std::string_view name;
};

template<typename Shader>
std::shared_ptr<IVulkanShader> ObjectFactory(VkDevice device, VkPipelineLayout layout, VkRenderPass renderPass)
{
  return std::make_shared<Shader>(device, layout, renderPass);
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
  IVulkanShader() = default;
  virtual ~IVulkanShader() = default;

  virtual bool Create() = 0;
  virtual void Destroy() = 0;

  virtual VkPipeline VulkanPipeline() const = 0;
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
