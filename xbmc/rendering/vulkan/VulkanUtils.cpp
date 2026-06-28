/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanUtils.h"

#include "VulkanInfo.h"
#include "utils/log.h"

#include <stdexcept>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

VkBool32 vulkanErrorCallback(VkDebugReportFlagsEXT flags,
                             VkDebugReportObjectTypeEXT objectType,
                             uint64_t object,
                             size_t location,
                             int32_t messageCode,
                             const char* pLayerPrefix,
                             const char* pMessage,
                             void* pUserData)
{
  int logLevel;
  if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    logLevel = LOGERROR;
  else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT ||
           flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    logLevel = LOGWARNING;
  else if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
    logLevel = LOGDEBUG;
  else if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
    logLevel = LOGINFO;
  else // For VK_DEBUG_REPORT_FLAG_BITS_MAX_ENUM_EXT and unknown flags
    logLevel = LOGINFO;

  /*!
   * @remark pLayerPrefix and pMessage should not be nullptr, but we check them
   * just in case to avoid potential crashes.
   */
  CLog::Log(logLevel, "Vulkan: {}: {}", pLayerPrefix ? pLayerPrefix : "Unknown",
            pMessage ? pMessage : "No message text");
  return VK_FALSE;
}

void LogGraphicsInfo(const CVulkanInfo& vulkanInfo)
{
  // Log the Vulkan API information
  CLog::Log(LOGINFO, "Vulkan: Logging graphics information...");
  CLog::Log(LOGINFO, "        - Available API Version: {}.{}.{}", VK_VERSION_MAJOR(vulkanInfo.availableAPIVersion),
            VK_VERSION_MINOR(vulkanInfo.availableAPIVersion), VK_VERSION_PATCH(vulkanInfo.availableAPIVersion));
  CLog::Log(LOGINFO, "        - Used API Version: {}.{}.{}",
            VK_VERSION_MAJOR(vulkanInfo.usedAPIVersion),
            VK_VERSION_MINOR(vulkanInfo.usedAPIVersion),
            VK_VERSION_PATCH(vulkanInfo.usedAPIVersion));
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
