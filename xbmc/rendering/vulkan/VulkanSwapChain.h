/*
 *  Copyright (C) 2005-2024 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "threads/CriticalSection.h"

#include <cassert>
#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>

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
  CVulkanSwapChain(CVulkanDeviceQueue* deviceQueue,
                   uint64_t acquireNextImageTimeoutNs = UINT64_MAX);
  ~CVulkanSwapChain();

  bool Initialize(VkSurfaceKHR surface,
                  const VkSurfaceFormatKHR& surfaceFormat,
                  const VkRect2D& size,
                  uint32_t minImageCount,
                  VkImageUsageFlags imageUsageFlags,
                  VkSurfaceTransformFlagBitsKHR preTransform,
                  VkCompositeAlphaFlagBitsKHR compositeAlpha,
                  std::unique_ptr<CVulkanSwapChain> oldSwapChain = nullptr);
  void Destroy();

  bool PostSubBuffer(const VkRect2D& rect);

  VkResult State() const { return m_state; }

  uint32_t AmmountSwapChainImages() const
  {
    // No need to lock here, as m_images is only modified in InitializeSwapChain()
    // and DeinitializeSwapChain(), which are called from the main thread only.
    return static_cast<uint32_t>(m_images.size());
  }

  uint32_t CurrentImageIndex() const
  {
    std::unique_lock lock(m_criticalSection);

    assert(m_acquiredImage.has_value());

    return *m_acquiredImage;
  }

  bool AcquireNextSwapchainImage();

  bool GetImage(uint32_t index,
                VkImage* image,
                VkImageLayout* layout,
                VkSemaphore* acquireSemaphore,
                VkSemaphore* presentSemaphore) const;

  VkSwapchainKHR GetSwapchain() const { return m_swapchain; }

private:
  CVulkanSwapChain(const CVulkanSwapChain&) = delete;
  CVulkanSwapChain& operator=(const CVulkanSwapChain&) = delete;

  bool InitializeSwapChain(VkSurfaceKHR surface,
                           const VkSurfaceFormatKHR& surfaceFormat,
                           const VkRect2D& size,
                           uint32_t minImageCount,
                           VkImageUsageFlags imageUsageFlags,
                           VkSurfaceTransformFlagBitsKHR preTransform,
                           VkCompositeAlphaFlagBitsKHR compositeAlpha,
                           std::unique_ptr<CVulkanSwapChain> oldSwapChain);
  void DestroySwapChain();

  bool InitializeSwapImages(const VkSurfaceFormatKHR& surfaceFormat);
  void DestroySwapImages();

  bool InitializeSemaphores();
  void DestroySemaphores();
  bool GetOrCreateSemaphores(VkSemaphore* acquireSemaphore, VkSemaphore* presentSemaphore);
  void ReturnSemaphores(VkSemaphore acquireSemaphore, VkSemaphore presentSemaphore);

  bool PresentBuffer(const VkRect2D& rect);

  CVulkanDeviceQueue* const m_deviceQueue;
  const uint64_t m_acquireNextImageTimeoutNs;

  // Available semaphores can be reused when waiting semaphores is over.
  struct PendingSemaphores
  {
    VkSemaphore acquireSemaphore{VK_NULL_HANDLE};
    VkSemaphore presentSemaphore{VK_NULL_HANDLE};
  };

  struct ImageData
  {
    VkImage image{VK_NULL_HANDLE};
    VkImageLayout imageLayout{VK_IMAGE_LAYOUT_UNDEFINED};
    // Semaphore used for vkAcquireNextImageKHR()
    VkSemaphore acquireSemaphore{VK_NULL_HANDLE};
    // Semaphore used for vkQueuePresentKHR()
    VkSemaphore presentSemaphore{VK_NULL_HANDLE};
  };

  mutable CCriticalSection m_criticalSection;

  std::deque<PendingSemaphores> m_pendingSemaphoresQueue;
  std::vector<ImageData> m_images;
  std::optional<uint32_t> m_acquiredImage;
  bool m_newAcquired{true};
  bool m_destroySwapchainWillHang{false};
  VkResult m_state{VK_SUCCESS};
  VkSwapchainKHR m_swapchain{VK_NULL_HANDLE};
  VkImageUsageFlags m_imageUsage{0};
  VkRect2D m_size{{0, 0}, {0, 0}};
  bool m_incrementalPresentSupported{false};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
