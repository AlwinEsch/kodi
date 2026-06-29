/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <vulkan/vulkan_core.h>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

class CVulkanDeviceQueue
{
public:
  CVulkanDeviceQueue(VkInstance vkInstance);

  VkInstance GetVulkanInstance() const { return m_vkInstance; }

private:
  CVulkanDeviceQueue(const CVulkanDeviceQueue&) = delete;
  CVulkanDeviceQueue& operator=(const CVulkanDeviceQueue&) = delete;

  VkInstance m_vkInstance{VK_NULL_HANDLE};

};


} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI