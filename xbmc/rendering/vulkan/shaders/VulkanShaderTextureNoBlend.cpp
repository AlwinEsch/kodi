/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanShaderTextureNoBlend.h"

#include "rendering/vulkan/utils/VulkanInitStructs.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "utils/log.h"

#include <array>
#include <cassert>

namespace KODI::RENDERING::VULKAN
{

using namespace KODI::RENDERING::VULKAN::UTILS;

CVulkanShaderTextureNoBlend::CVulkanShaderTextureNoBlend(const VulkanData* vkData,
                                                         CVulkanDeviceQueue* deviceQueue)
  : IVulkanShader(vkData, deviceQueue)
{
}

} // namespace KODI::RENDERING::VULKAN
