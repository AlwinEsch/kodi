/*
 *  Copyright (C) 2005-2024 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <memory>
#include <optional>

#include <vulkan/vulkan_core.h>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

class CVulkanDeviceQueue;
class CVulkanSwapChain;

// Minimum bit depth of surface.
enum SurfaceFormat : uint32_t
{
  FORMAT_RGBA_32,
  FORMAT_RGB_16,

  NUM_SURFACE_FORMATS,
  DEFAULT_SURFACE_FORMAT = FORMAT_RGBA_32
};

class CVulkanSurface
{
public:
  CVulkanSurface(VkInstance vkInstance,
                 VkSurfaceKHR vkSurface,
                 uint64_t acquireNextImageTimeoutNs = UINT64_MAX);
  virtual ~CVulkanSurface();

  bool Initialize(CVulkanDeviceQueue* device_queue, SurfaceFormat format);
  void Destroy();

  virtual bool Reshape(
      const VkRect2D& size,
      VkSurfaceTransformFlagBitsKHR vkTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);

  bool SwapBuffers();
  bool PostSubBuffer(const VkRect2D& rect);

  CVulkanSwapChain* SwapChain() const { return m_swapChain.get(); }
  CVulkanDeviceQueue* DeviceQueue() const { return m_deviceQueue; }
  uint32_t SwapChainGeneration() const { return m_swapChainGeneration; }
  const VkRect2D& vkImageSize() const { return m_vkImageSize; }
  VkImageUsageFlags vkImageUsageFlags() const { return m_vkImageUsageFlags; }
  VkSwapchainKHR vkSwapchain() const { return m_vkSwapchain; }
  VkSurfaceFormatKHR vkSurfaceFormat() const { return m_vkSurfaceFormat; }

private:
  bool CreateSwapChain(const VkRect2D& size,
                       VkSurfaceTransformFlagBitsKHR vkTransform);

  VkInstance m_vkInstance;
  VkSurfaceKHR m_vkSurface;
  VkSurfaceFormatKHR m_vkSurfaceFormat{};
  VkCompositeAlphaFlagBitsKHR m_vkCompositeAlpha{VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR};
  VkSurfaceTransformFlagBitsKHR m_vkTransform{VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR};
  VkSwapchainKHR m_vkSwapchain{VK_NULL_HANDLE};
  uint64_t m_acquireNextImageTimeoutNs{UINT64_MAX};
  VkImageUsageFlags m_vkImageUsageFlags{0};
  VkRect2D m_vkImageSize{{0, 0}, {0, 0}};
  uint32_t m_swapChainGeneration{0};

  CVulkanDeviceQueue* m_deviceQueue{nullptr};
  std::unique_ptr<CVulkanSwapChain> m_swapChain;
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
