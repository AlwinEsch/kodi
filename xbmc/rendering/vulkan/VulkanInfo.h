/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

struct CVulkanPhysicalDeviceInfo
{
  VkPhysicalDevice device{VK_NULL_HANDLE};

  VkPhysicalDeviceProperties properties = {};
  VkPhysicalDeviceDriverProperties driverProperties = {};
  VkFormatProperties formatProperties = {};
  VkImageFormatProperties imageFormatProperties = {};
  uint64_t drmDeviceId{0};

  std::vector<VkExtensionProperties> extensions;
  std::vector<std::string> extensions2;

  VkPhysicalDeviceFeatures features = {};

  // Extended physical device features:
  bool featureSamplerYCBCRconversion = false;
  bool featureProtectedMemory = false;
  bool featureSamplerAnisotropy = false;
  bool featureDeviceDRM = false;
  bool featureExtendedDynamicState = false;
  // TODO: The content of this struct is not used yet, but we can use it in the future
  // to check for extended dynamic state features.
  // Within CVulkanInstance::GetBasicInfos, we can query for the extended dynamic state
  // features and set these flags accordingly.
  bool featureExtendedDynamicState2 = false;
  bool featureExtendedDynamicState3 = false;

  std::vector<VkQueueFamilyProperties> queueFamilies;

  /**
   * Check if an extension is supported by the (physical device)
   *
   * @param extension Name of the extension to check
   * @return True if the extension is supported (present in the list read at device creation time)
   */
  bool ExtensionSupported(const std::string& extension) const
  {
    return (std::find(extensions2.begin(), extensions2.end(), extension) !=
            extensions2.end());
  }
};

struct CVulkanInfo
{
  uint32_t availableAPIVersion{VK_MAKE_VERSION(1, 0, 0)};
  uint32_t usedAPIVersion{VK_MAKE_VERSION(1, 0, 0)};
  std::vector<VkExtensionProperties> instanceExtensions;
  std::vector<const char*> requiredInstanceExtensions;
  std::vector<VkLayerProperties> instanceLayers;
  uint32_t usedPhysicalDeviceIndex{0};
  std::vector<CVulkanPhysicalDeviceInfo> physicalDevices;
  bool debugUtilsEnabled{false};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
