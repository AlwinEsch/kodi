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
                 VkSurfaceKHR surface,
                 uint64_t acquireNextImageTimeoutNs = UINT64_MAX);
  ~CVulkanSurface();

  bool Initialize(CVulkanDeviceQueue* device_queue, SurfaceFormat format);
  void Destroy();

  virtual bool Reshape(
      const VkRect2D& size,
      VkSurfaceTransformFlagBitsKHR vkTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);

  bool SwapBuffers();
  bool PostSubBuffer(const VkRect2D& rect);

  CVulkanSwapChain* GetSwapChain() const { return m_swapChain.get(); }
  uint32_t GetSwapChainGeneration() const { return m_swapChainGeneration; }
  const VkRect2D& GetImageSize() const { return m_imageSize; }
  VkImageUsageFlags GetImageUsageFlags() const { return m_imageUsageFlags; }
  VkSurfaceFormatKHR GetSurfaceFormat() const { return m_surfaceFormat; }

private:
  bool CreateSwapChain(const VkRect2D& size,
                       VkSurfaceTransformFlagBitsKHR vkTransform);

  VkInstance m_vkInstance;
  VkSurfaceKHR m_vkSurface;
  VkSurfaceFormatKHR m_surfaceFormat{};
  VkCompositeAlphaFlagBitsKHR m_compositeAlpha{VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR};
  VkSurfaceTransformFlagBitsKHR m_vkTransform{VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR};
  uint64_t m_acquireNextImageTimeoutNs{UINT64_MAX};
  VkImageUsageFlags m_imageUsageFlags{0};
  VkRect2D m_imageSize{{0, 0}, {0, 0}};
  uint32_t m_swapChainGeneration{0};

  CVulkanDeviceQueue* m_deviceQueue{nullptr};
  std::unique_ptr<CVulkanSwapChain> m_swapChain;
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
