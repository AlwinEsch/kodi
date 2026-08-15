/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "rendering/vulkan/VulkanDeviceQueue.h"

#include <vulkan/vulkan_core.h>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

class CVulkanFenceHelper
{
public:
  explicit CVulkanFenceHelper(CVulkanDeviceQueue* deviceQueue);
  ~CVulkanFenceHelper();

  void Destroy();

  class CFenceHandle
  {
  public:
    CFenceHandle();
    CFenceHandle(const CFenceHandle& other);
    CFenceHandle& operator=(const CFenceHandle& other);

    bool IsValid() const { return m_fence != VK_NULL_HANDLE; }

  private:
    friend class CVulkanFenceHelper;
    CFenceHandle(VkFence fence, uint64_t generationId);

    VkFence m_fence{VK_NULL_HANDLE};
    uint64_t m_generationId{0};
  };

  VkResult GetFence(VkFence* fence);
  CFenceHandle EnqueueFence(VkFence fence);
  bool Wait(CFenceHandle handle, uint64_t timeoutInNanoseconds = UINT64_MAX);
  bool HasPassed(CFenceHandle handle);

  void PerformImmediateCleanup();
  void EnqueueSemaphoreCleanupForSubmittedWork(VkSemaphore semaphore);
  void EnqueueSemaphoresCleanupForSubmittedWork(std::vector<VkSemaphore> semaphores);

private:
  CVulkanFenceHelper(const CVulkanFenceHelper&) = delete;
  CVulkanFenceHelper& operator=(const CVulkanFenceHelper&) = delete;

  CVulkanDeviceQueue* m_deviceQueue{nullptr};
  VkFence m_vkFence{VK_NULL_HANDLE};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
