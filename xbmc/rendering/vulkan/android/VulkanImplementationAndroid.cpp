/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanImplementAndroid.h"

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{
namespace ANDROID
{

bool CVulkanImplementationLinux::IsAvailable() const
{
  return true;
}

bool CVulkanImplementationLinux::CreateSurface(void* window, VkSurfaceKHR& surface) const
{
  // Implementation for creating a Vulkan surface on Linux goes here.
  // This typically involves using platform-specific extensions like VK_KHR_xcb_surface or VK_KHR_wayland_surface.
  return false; // Placeholder return value; actual implementation needed.
}

} // namespace ANDROID
} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
