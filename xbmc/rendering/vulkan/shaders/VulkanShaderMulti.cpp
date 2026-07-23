/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanShaderMulti.h"

#include "rendering/vulkan/utils/VulkanUtils.h"
#include "utils/log.h"

#include <cassert>

namespace KODI::RENDERING::VULKAN
{

using KODI::RENDERING::VULKAN::UTILS::ErrorString;

CVulkanShaderMulti::CVulkanShaderMulti(const VulkanData* vkData,
                                       CVulkanDeviceQueue* deviceQueue)
  : IVulkanShader(vkData, deviceQueue)
{
}

bool CVulkanShaderMulti::Create()
{

  return true;
}

void CVulkanShaderMulti::Destroy()
{
  if (m_vkPipeline != VK_NULL_HANDLE)
  {
    // Destroy the Vulkan pipeline
    vkDestroyPipeline(m_vkData->vkDevice, m_vkPipeline, nullptr);
    m_vkPipeline = VK_NULL_HANDLE;
  }
}

} // namespace KODI::RENDERING::VULKAN
