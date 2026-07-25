/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "IVulkanShader.h"

#include "rendering/vulkan/utils/VulkanInitStructs.h"
#include "rendering/vulkan/utils/VulkanUtils.h"

namespace KODI::RENDERING::VULKAN
{

IVulkanShader::IVulkanShader(const VulkanData* vkData, CVulkanDeviceQueue* deviceQueue)
  : m_vkData(vkData),
    m_deviceQueue(deviceQueue)
{
}

VkPipelineShaderStageCreateInfo IVulkanShader::LoadShader(std::string fileName,
                                                          VkShaderStageFlagBits stage)
{
  VkPipelineShaderStageCreateInfo shaderStage{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = stage,
      .module = UTILS::vulkanCreateShaderModule(m_vkData->vkDevice, fileName),
      .pName = "main",
      .pSpecializationInfo = nullptr,
  };

  return shaderStage;
}

void IVulkanShader::UnloadShader(VkPipelineShaderStageCreateInfo shaderStage)
{
  VkShaderModule shaderModule = shaderStage.module;
  vkDestroyShaderModule(m_vkData->vkDevice, shaderModule, nullptr);
}

} // namespace KODI::RENDERING::VULKAN
