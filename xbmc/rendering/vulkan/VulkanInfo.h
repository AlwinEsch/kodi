/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

constexpr const uint32_t REQUIRED_VK_API_VERSION{VK_API_VERSION_1_2};
constexpr const uint32_t MIN_DESCRIPTORS_PER_TYPE = 8u; // Allow up to 8 buffers by default.
constexpr const uint32_t MAX_TEXTURES = (16u * 4096u);

struct CVulkanPhysicalDeviceInfo
{
  VkPhysicalDevice device{VK_NULL_HANDLE};

  VkPhysicalDeviceProperties properties = {};
  VkPhysicalDeviceDriverProperties driverProperties = {};
  VkFormatProperties formatProperties = {};
  VkImageFormatProperties imageFormatProperties = {};
  uint64_t drmDeviceId{0};

  std::vector<VkExtensionProperties> extensions;

  VkPhysicalDeviceFeatures features = {};

  // Extended physical device features:
  bool featureSamplerYCBCRconversion = false;
  bool featureProtectedMemory = false;
  bool featureSamplerAnisotropy = false;

  std::vector<VkQueueFamilyProperties> queueFamilies;
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
