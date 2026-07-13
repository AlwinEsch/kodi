/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

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

  VkCommandBuffer GetVulkanCommandBuffer() const { return m_vKCommandBuffer; }

  void TransitionImageLayout(VkImage image,
                             VkImageLayout oldLayout,
                             VkImageLayout newLayout,
                             uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                             uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED);

private:
  CVulkanCommandBuffer(const CVulkanCommandBuffer&) = delete;
  CVulkanCommandBuffer& operator=(const CVulkanCommandBuffer&) = delete;

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

  bool m_primary{true};
  CVulkanDeviceQueue* m_deviceQueue{nullptr};
  CVulkanCommandPool* m_commandPool{nullptr};
  VkCommandBuffer m_vKCommandBuffer{VK_NULL_HANDLE};
  RecordType m_recordType{RECORD_TYPE_EMPTY};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
