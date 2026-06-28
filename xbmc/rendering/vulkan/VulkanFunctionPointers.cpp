/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanFunctionPointers.h"

#include "utils/log.h"

#include <stdexcept>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

CVulkanFunctionPointers* GetVulkanFunctionPointers()
{
  static CVulkanFunctionPointers s_vulkanFunctionPointers;
  return &s_vulkanFunctionPointers;
}

bool CVulkanFunctionPointers::BindInstanceFunctionPointers(VkInstance vkInstance)
{
  try
  {
    constexpr char kvkCreateDebugReportCallbackEXT[] = "vkCreateDebugReportCallbackEXT";
    vkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
        vkGetInstanceProcAddr(vkInstance, kvkCreateDebugReportCallbackEXT));
    if (!vkCreateDebugReportCallbackEXT)
      throw std::runtime_error("Failed to load vkCreateDebugReportCallbackEXT function pointer");

    constexpr char kvkDestroyDebugReportCallbackEXT[] = "vkDestroyDebugReportCallbackEXT";
    vkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
        vkGetInstanceProcAddr(vkInstance, kvkDestroyDebugReportCallbackEXT));
    if (!vkDestroyDebugReportCallbackEXT)
      throw std::runtime_error("Failed to load vkDestroyDebugReportCallbackEXT function pointer");
  }
  catch (const std::exception& e)
  {
    CLog::Log(LOGERROR, "Vulkan: Exception while binding instance function pointers: {}", e.what());
    return false;
  }

  return true;
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
