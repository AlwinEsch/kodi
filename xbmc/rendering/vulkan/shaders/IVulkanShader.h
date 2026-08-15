/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "rendering/vulkan/VulkanData.h"

#include <array>
#include <memory>
#include <string>

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
  IVulkanShader(const VulkanData* vkData, CVulkanDeviceQueue* deviceQueue);
  virtual ~IVulkanShader();

  bool Create();
  void Destroy();

  virtual void vkPipeline(VkPipeline& pipeline, VkPipelineLayout& layout, int type) {}
  virtual VulkanMemoryData* GetUniformBuffer(uint32_t index)
  {
    fprintf(stderr, "GetUniformBuffer not implemented\n");
    return nullptr;
  }

protected:
  virtual bool CreatePipelineLayout() { return true; }
  virtual void DestroyPipelineLayout() {}

  virtual bool CreatePipeline() { return true; }
  virtual void DestroyPipeline() {}

  virtual bool CreateUniformBuffers() { return true; }
  virtual void DestroyUniformBuffers() {}

  VkPipelineShaderStageCreateInfo LoadShader(std::string fileName, VkShaderStageFlagBits stage);
  void UnloadShader(VkPipelineShaderStageCreateInfo shaderStage);

  const VulkanData* m_vkData;
  CVulkanDeviceQueue* const m_deviceQueue;

  std::array<VulkanMemoryData, MAX_CONCURRENT_FRAMES> m_vertexBuffers;
  std::array<VulkanMemoryData, MAX_CONCURRENT_FRAMES> m_indexBuffers;
};

} // namespace KODI::RENDERING::VULKAN
