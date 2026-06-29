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
  CLog::Log(LOGINFO, "        - Available API Version: {}.{}.{}",
            VK_VERSION_MAJOR(vulkanInfo.availableAPIVersion),
            VK_VERSION_MINOR(vulkanInfo.availableAPIVersion),
            VK_VERSION_PATCH(vulkanInfo.availableAPIVersion));
  CLog::Log(LOGINFO, "        - Used API Version: {0}.{1}.{2}",
            VK_VERSION_MAJOR(vulkanInfo.usedAPIVersion),
            VK_VERSION_MINOR(vulkanInfo.usedAPIVersion),
            VK_VERSION_PATCH(vulkanInfo.usedAPIVersion));
  CLog::Log(LOGINFO, "        - Debug Report Enabled: {0}",
            vulkanInfo.debugReportEnabled ? "Yes" : "No");
  for (const auto& deviceInfo : vulkanInfo.instanceExtensions)
  {
    bool isEnabled = std::find_if(vulkanInfo.enabledInstanceExtensions.begin(),
                                  vulkanInfo.enabledInstanceExtensions.end(),
                                  [&deviceInfo](const char* enabledExtension)
                                  {
                                    return strcmp(enabledExtension, deviceInfo.extensionName) == 0;
                                  }) != vulkanInfo.enabledInstanceExtensions.end();
    CLog::Log(LOGINFO, "        - Instance Extension: {0} (Version {1}), Is Enabled: {2}",
                deviceInfo.extensionName, deviceInfo.specVersion, isEnabled ? "Yes" : "No");
  }
  for (const auto& deviceInfo : vulkanInfo.physicalDevices)
  {
    CLog::Log(LOGINFO, "        - Physical Device: {0}", deviceInfo.properties.deviceName);
    CLog::Log(LOGINFO, "            - Vendor ID: {0:#X}", deviceInfo.properties.vendorID);
    CLog::Log(LOGINFO, "            - Device ID: {0:#X}", deviceInfo.properties.deviceID);
    CLog::Log(LOGINFO, "            - Driver Version: {0}.{1}.{2}",
              VK_VERSION_MAJOR(deviceInfo.properties.driverVersion),
              VK_VERSION_MINOR(deviceInfo.properties.driverVersion),
              VK_VERSION_PATCH(deviceInfo.properties.driverVersion));
    CLog::Log(LOGINFO, "            - DRM Device ID: {0:#X}", deviceInfo.drmDeviceId);
    CLog::Log(LOGINFO, "            - Queue Families: {0}", deviceInfo.queueFamilies.size());
    CLog::Log(LOGINFO, "            - Sampler YCbCr Conversion: {0}",
              deviceInfo.featureSamplerYCBCRconversion ? "Supported" : "Not supported");
    CLog::Log(LOGINFO, "            - Protected Memory: {0}",
              deviceInfo.featureProtectedMemory ? "Supported" : "Not supported");
  }
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
