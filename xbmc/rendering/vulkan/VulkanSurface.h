/*
 *  Copyright (C) 2005-2024 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once
#include <memory>

#include <vulkan/vulkan_core.h>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

class CVulkanDeviceQueue;
class CVulkanSwapChain;

class CVulkanSurface
{
public:
  explicit CVulkanSurface();
  ~CVulkanSurface();

  bool InitializeSurface(CVulkanDeviceQueue* deviceQueue);
  void DestroySurface();

private:
  CVulkanSurface(const CVulkanSurface&) = delete;
  CVulkanSurface& operator=(const CVulkanSurface&) = delete;

  std::unique_ptr<CVulkanSwapChain> m_swapChain{nullptr};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
