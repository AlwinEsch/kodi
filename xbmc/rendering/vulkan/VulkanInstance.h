/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "VulkanInfo.h"
#include <vector>
#include <vulkan/vulkan_core.h>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

class CVulkanInstance
{
public:
  CVulkanInstance() = default;
  ~CVulkanInstance();

  bool Create(const std::vector<const char*>& required_extensions,
              const std::vector<const char*>& required_layers);
  void Destroy();

  VkInstance GetVkInstance() const { return m_vkInstance ; }

private:
  bool GetBasicInfos(const std::vector<const char*>& requiredLayers);
  bool GetDeviceInfos(VkPhysicalDevice physicalDevice = VK_NULL_HANDLE);

  VkInstance m_vkInstance{VK_NULL_HANDLE};

  CVulkanInfo m_vulkanInfo;

  VkDebugReportCallbackEXT m_vkReportCallback{VK_NULL_HANDLE};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
