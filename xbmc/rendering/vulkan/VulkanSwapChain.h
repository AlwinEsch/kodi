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

  bool InitializeSwapChain(VkSurfaceKHR surface,
                           const VkSurfaceFormatKHR& surfaceFormat,
                           const VkExtent2D& size,
                           VkImageUsageFlags imageUsageFlags,
                           std::unique_ptr<CVulkanSwapChain> oldSwapChain);
  void DeinitializeSwapChain();

  VkResult GetState() const
  {
    std::unique_lock lock(m_criticalSection);
    return m_vkState;
  }

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

private:
  CVulkanSwapChain(const CVulkanSwapChain&) = delete;
  CVulkanSwapChain& operator=(const CVulkanSwapChain&) = delete;

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

  bool AcquireNextSwapchainImage();
  bool GetOrCreateSemaphores(VkSemaphore& acquireSemaphore, VkSemaphore& presentSemaphore);
  VkSemaphore CreateSemaphore(VkDevice vk_device);
  void ReturnSemaphores(VkSemaphore acquireSemaphore, VkSemaphore presentSemaphore);

  CVulkanDeviceQueue* const m_deviceQueue;
  const uint64_t m_acquireNextImageTimeoutNs;

  mutable CCriticalSection m_criticalSection;

  // Images in the swap chain.
  bool m_newAcquired{true};
  bool m_destroySwapchainWillHang{false};
  std::optional<uint32_t> m_acquiredImage;
  std::vector<ImageData> m_images;
  std::deque<PendingSemaphores> m_pendingSemaphoresQueue;

  VkExtent2D m_size{0, 0};
  VkSwapchainKHR m_vkSwapChain{VK_NULL_HANDLE};
  VkImageUsageFlags m_vkImageUsageFlags{0};
  VkResult m_vkState{VK_SUCCESS};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
