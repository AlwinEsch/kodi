/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once
#include "rendering/vulkan/VulkanInstance.h"
#include "rendering/vulkan/VulkanRenderSystem.h"

#include <wayland-client.h>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{
namespace WAYLAND
{

class CVulkanRenderSystemWayland : public CVulkanRenderSystem
{
public:
  CVulkanRenderSystemWayland();
  ~CVulkanRenderSystemWayland() override = default;

  void SetRenderSystemWaylandInfo(wl_display* display, wl_surface* surface)
  {
    m_waylandDisplay = display;
    m_waylandSurface = surface;
  }
  bool InitRenderSystem() override;

  CVulkanInstance* GetVulkanInstance() override { return &m_vulkanInstance; }
  VkSurfaceKHR GetVulkanSurface() override { return m_vulkanSurface; }

  std::vector<const char*> GetRequiredDeviceExtensions() override;
  std::vector<const char*> GetOptionalDeviceExtensions() override;

  bool GetPhysicalDevicePresentationSupport(
    VkPhysicalDevice device,
    const std::vector<VkQueueFamilyProperties>& queueFamilyProperties,
    uint32_t queueFamilyIndex) override;

private:
  CVulkanInstance m_vulkanInstance;
  wl_display* m_waylandDisplay = nullptr;
  wl_surface* m_waylandSurface = nullptr;
  VkSurfaceKHR m_vulkanSurface = nullptr;
};

} // namespace WAYLAND
} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
