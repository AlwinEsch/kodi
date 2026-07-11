/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "rendering/vulkan/VulkanDeviceQueue.h"

#include <vulkan/vulkan_core.h>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

class CVulkanFenceHelper
{
public:
  explicit CVulkanFenceHelper(CVulkanDeviceQueue* deviceQueue);
  ~CVulkanFenceHelper();

  void Destroy();

  VkResult GetFence(VkFence* fence);

  void PerformImmediateCleanup();

private:
  CVulkanFenceHelper(const CVulkanFenceHelper&) = delete;
  CVulkanFenceHelper& operator=(const CVulkanFenceHelper&) = delete;

  CVulkanDeviceQueue* m_deviceQueue{nullptr};
  VkFence m_vkFence{VK_NULL_HANDLE};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
