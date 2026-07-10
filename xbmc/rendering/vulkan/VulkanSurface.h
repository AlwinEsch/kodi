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
                 VkSurfaceKHR surface,
                 uint64_t acquireNextImageTimeoutNs = UINT64_MAX);
  ~CVulkanSurface();

  bool InitializeSurface(CVulkanDeviceQueue* deviceQueue, SurfaceFormat format);
  void DeinitializeSurface();

  bool SwapBuffers();
  bool Reshape(const VkExtent2D& size);

  CVulkanSwapChain* GetSwapChain() const { return m_swapChain.get(); }
  uint32_t GetSwapChainGeneration() const { return m_swapChainGeneration; }
  const VkExtent2D& GetImageSize() const { return m_imageSize; }
  VkImageUsageFlags GetImageUsageFlags() const { return m_vkImageUsageFlags; }
  VkSurfaceFormatKHR GetSurfaceFormat() const { return m_vkSurfaceFormat; }

private:
  CVulkanSurface(const CVulkanSurface&) = delete;
  CVulkanSurface& operator=(const CVulkanSurface&) = delete;

  bool CreateSwapChain(const VkExtent2D& size);

  const VkInstance m_vkInstance;
  VkSurfaceKHR m_vkSurface;
  const uint64_t m_acquireNextImageTimeoutNs;

  CVulkanDeviceQueue* m_deviceQueue{nullptr};
  uint32_t m_swapChainGeneration{0u};

  VkExtent2D m_imageSize;
  VkImageUsageFlags m_vkImageUsageFlags{0};
  VkSurfaceFormatKHR m_vkSurfaceFormat{};

  std::unique_ptr<CVulkanSwapChain> m_swapChain;
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
