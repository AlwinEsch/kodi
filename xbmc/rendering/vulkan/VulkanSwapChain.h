/*
 *  Copyright (C) 2005-2024 Team Kodi
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

class CVulkanDeviceQueue;

class CVulkanSwapChain
{
public:
  CVulkanSwapChain(CVulkanDeviceQueue* deviceQueue);
  ~CVulkanSwapChain();

  bool InitializeSwapChain();
  void DestroySwapChain();

private:
  CVulkanSwapChain(const CVulkanSwapChain&) = delete;
  CVulkanSwapChain& operator=(const CVulkanSwapChain&) = delete;

  CVulkanDeviceQueue* m_deviceQueue{nullptr};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
