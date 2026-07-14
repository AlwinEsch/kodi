/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "rendering/vulkan/VulkanFenceHelper.h"

#include <vulkan/vulkan_core.h>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

class CVulkanCommandPool;
class CVulkanDeviceQueue;

class CVulkanCommandBuffer
{
public:
  CVulkanCommandBuffer(CVulkanDeviceQueue* deviceQueue,
                       CVulkanCommandPool* commandPool,
                       bool primary);
  ~CVulkanCommandBuffer();

  bool Initialize();
  void Destroy();

  VkCommandBuffer GetVulkanCommandBuffer() const { return m_vkCommandBuffer; }

  bool Submit(uint32_t numWaitSemaphores,
              VkSemaphore* waitSemaphores,
              uint32_t numSignalSemaphores,
              VkSemaphore* signalSemaphores,
              bool allowProtectedMemory = false);

  void Wait(uint64_t timeout);

  void TransitionImageLayout(VkImage image,
                             VkImageLayout oldLayout,
                             VkImageLayout newLayout,
                             uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                             uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED);

private:
  CVulkanCommandBuffer(const CVulkanCommandBuffer&) = delete;
  CVulkanCommandBuffer& operator=(const CVulkanCommandBuffer&) = delete;

  void PostExecution();
  void ResetIfDirty();

  enum RecordType
  {
    // Nothing has been recorded yet.
    RECORD_TYPE_EMPTY,

    // Recorded for single use, will be reset upon submission.
    RECORD_TYPE_SINGLE_USE,

    // Recording for multi use, once submitted it can't be modified until reset.
    RECORD_TYPE_MULTI_USE,

    // Recorded for multi-use, can no longer be modified unless reset.
    RECORD_TYPE_RECORDED,

    // Dirty, should be cleared before use. This assumes its externally
    // synchronized and the command buffer is no longer in use.
    RECORD_TYPE_DIRTY,
  };

  bool m_recording = false;
  CVulkanDeviceQueue* m_deviceQueue{nullptr};
  CVulkanCommandPool* m_commandPool{nullptr};
  bool m_primary{true};
  VkCommandBuffer m_vkCommandBuffer{VK_NULL_HANDLE};
  RecordType m_recordType{RECORD_TYPE_EMPTY};
  CVulkanFenceHelper::CFenceHandle m_submissionFence;
};

class CVulkanCommandBufferScoped
{
public:
  CVulkanCommandBufferScoped(
      CVulkanCommandBuffer& commandBuffer,
      VkCommandBufferUsageFlags usageFlags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
  ~CVulkanCommandBufferScoped();

  VkCommandBuffer GetVulkanCommandBuffer() const
  { return m_handle;
  }

private:
  VkCommandBufferUsageFlags m_usageFlags{VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};
  VkCommandBuffer m_handle{VK_NULL_HANDLE};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
