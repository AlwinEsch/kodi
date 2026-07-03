/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanRenderSystemWayland.h"

#include "utils/log.h"

#include <vulkan/vulkan_wayland.h>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{
namespace WAYLAND
{

CVulkanRenderSystemWayland::CVulkanRenderSystemWayland() : CVulkanRenderSystem()
{
}

bool CVulkanRenderSystemWayland::InitRenderSystem()
{
  // Check if Wayland display and surface are set, should be set by the windowing
  // system before calling this function.
  if (!m_waylandDisplay || !m_waylandSurface)
  {
    CLog::Log(LOGERROR, "Vulkan: Rendersystem Wayland display or surface is not set");
    return false;
  }

  std::vector<const char*> required_extensions = {VK_KHR_SURFACE_EXTENSION_NAME,
                                                  VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME};
  std::vector<const char*> required_layers = {};
  if (!m_vulkanInstance.Create(required_extensions, required_layers))
    return false;

  // clang-format off
  VkWaylandSurfaceCreateInfoKHR createInfo = {
    .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
    .pNext = nullptr,
    .flags = 0,
    .display = m_waylandDisplay,
    .surface = m_waylandSurface
  };
  // clang-format on

  VkResult result = vkCreateWaylandSurfaceKHR(m_vulkanInstance.GetVkInstance(), &createInfo, NULL,
                                              &m_vulkanSurface);
  if (VK_SUCCESS != result)
  {
    CLog::Log(LOGERROR, "Vulkan: vkCreateWaylandSurfaceKHR() failed: {0}", result);
    return false;
  }

  return CVulkanRenderSystem::InitRenderSystem();
}

std::vector<const char*> CVulkanRenderSystemWayland::GetRequiredDeviceExtensions()
{
  return {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
}

std::vector<const char*> CVulkanRenderSystemWayland::GetOptionalDeviceExtensions()
{
  return {};
}

bool CVulkanRenderSystemWayland::GetPhysicalDevicePresentationSupport(
    VkPhysicalDevice device,
    const std::vector<VkQueueFamilyProperties>& queueFamilyProperties,
    uint32_t queueFamilyIndex)
{
  return vkGetPhysicalDeviceWaylandPresentationSupportKHR(device, queueFamilyIndex,
                                                          m_waylandDisplay);
}

} // namespace WAYLAND
} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
