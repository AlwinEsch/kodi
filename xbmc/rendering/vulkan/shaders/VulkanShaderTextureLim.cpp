/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanShaderTextureLim.h"

#include "rendering/vulkan/utils/VulkanInitStructs.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "utils/log.h"

#include <array>
#include <cassert>

namespace KODI::RENDERING::VULKAN
{

using namespace KODI::RENDERING::VULKAN::UTILS;

CVulkanShaderTextureLim::CVulkanShaderTextureLim(const VulkanData* vkData,
                                                 CVulkanDeviceQueue* deviceQueue)
  : IVulkanShader(vkData, deviceQueue)
{
}

bool CVulkanShaderTextureLim::Create(const VkPipelineCache& pipelineCache)
{
  return true;
}

void CVulkanShaderTextureLim::Destroy()
{
  if (m_vkPipeline != VK_NULL_HANDLE)
  {
    // Destroy the Vulkan pipeline
    vkDestroyPipeline(m_vkData->vkDevice, m_vkPipeline, nullptr);
    m_vkPipeline = VK_NULL_HANDLE;
  }
}

} // namespace KODI::RENDERING::VULKAN
