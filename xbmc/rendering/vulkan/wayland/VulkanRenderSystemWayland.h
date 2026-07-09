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

/**
 * @brief A Vulkan render system implementation for Wayland
 */
class CVulkanRenderSystemWayland : public CVulkanRenderSystem
{
public:
  CVulkanRenderSystemWayland();
  ~CVulkanRenderSystemWayland() override = default;

  /**
   * @brief Sets the Wayland display and surface information for the Vulkan render system.
   *
   * @param[in] display The Wayland display.
   * @param[in] surface The Wayland surface.
   * @param[in] width The width of the surface.
   * @param[in] height The height of the surface.
   */
  void SetRenderSystemWaylandInfo(wl_display* display,
                                  wl_surface* surface,
                                  uint32_t width,
                                  uint32_t height)
  {
    m_waylandDisplay = display;
    m_waylandSurface = surface;
    m_size.width = width;
    m_size.height = height;
  }

  /**
   * @brief Initializes the Vulkan render system.
   *
   * Called from @ref CApplication::InitWindow() after the Wayland display
   * and surface have been set.
   *
   * @return True if initialization was successful, false otherwise.
   *
   * @remark This virtual function based on @ref CRenderSystemBase.
   */
  bool InitRenderSystem() override;

  /**
   * @brief Destroys the Vulkan render system.
   *
   * @return True if destruction was successful, false otherwise.
   *
   * @remark This virtual function based on @ref CRenderSystemBase.
   */
  bool DestroyRenderSystem() override;

  /**@}*/

  /**
   * @brief Gets the Vulkan instance.
   *
   * @return A pointer to the Vulkan instance.
   *
   * @remark This virtual function based on @ref CVulkanRenderSystem.
   */
  CVulkanInstance* GetVulkanInstance() override { return &m_vulkanInstance; }

  /**
   * @brief Gets the Vulkan surface.
   *
   * @return The Vulkan surface.
   *
   * @remark This virtual function based on @ref CVulkanRenderSystem
   */
  VkSurfaceKHR GetVulkanSurface() override { return m_vulkanSurface; }

  /**
   * @brief Gets the required device extensions.
   *
   * @return A vector of required device extensions.
   *
   * @remark This virtual function based on @ref CVulkanRenderSystem.
   */
  std::vector<const char*> GetRequiredDeviceExtensions() override;

  /**
   * @brief Gets the optional device extensions.
   *
   * @return A vector of optional device extensions.
   *
   * @remark This virtual function based on @ref CVulkanRenderSystem.
   */
  std::vector<const char*> GetOptionalDeviceExtensions() override;

  /**
   * @brief Gets the presentation support for a physical device.
   *
   * @param[in] device The Vulkan physical device.
   * @param[in] queueFamilyProperties The properties of the queue families.
   * @param[in] queueFamilyIndex The index of the queue family.
   *
   * @return True if the device supports presentation, false otherwise.
   *
   * @remark This virtual function based on @ref CVulkanRenderSystem.
   */
  bool GetPhysicalDevicePresentationSupport(
      VkPhysicalDevice device,
      const std::vector<VkQueueFamilyProperties>& queueFamilyProperties,
      uint32_t queueFamilyIndex) override;

private:
  CVulkanInstance m_vulkanInstance;
  wl_display* m_waylandDisplay{nullptr};
  wl_surface* m_waylandSurface{nullptr};
  VkSurfaceKHR m_vulkanSurface{nullptr};
};

} // namespace WAYLAND
} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
