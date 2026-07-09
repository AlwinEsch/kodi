/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanCommandBuffer.h"

#include "rendering/vulkan/VulkanCommandPool.h"
#include "rendering/vulkan/VulkanDeviceQueue.h"
#include "utils/log.h"

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

CVulkanCommandBuffer::CVulkanCommandBuffer(CVulkanDeviceQueue* deviceQueue,
                                           CVulkanCommandPool* commandPool,
                                           bool primary)
  : m_deviceQueue(deviceQueue),
    m_commandPool(commandPool),
    m_primary(primary)
{
  m_commandPool->IncrementCommandBufferCount();
}

CVulkanCommandBuffer::~CVulkanCommandBuffer()
{
  m_commandPool->DecrementCommandBufferCount();
}

bool CVulkanCommandBuffer::InitializeCommandBuffer()
{
  VkResult result = VK_SUCCESS;
  VkDevice device = m_deviceQueue->GetVulkanDevice();

  VkCommandBufferAllocateInfo info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = m_commandPool->GetVkCommandPool(),
      .level = m_primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY,
      .commandBufferCount = 1,
  };

  assert(m_vKCommandBuffer == static_cast<VkCommandBuffer>(VK_NULL_HANDLE));

  result = vkAllocateCommandBuffers(device, &info, &m_vKCommandBuffer);
  if (VK_SUCCESS != result)
  {
    CLog::Log(LOGERROR, "Vulkan: vkAllocateCommandBuffers() failed: {}", result);
    return false;
  }

  m_recordType = RECORD_TYPE_EMPTY;
  return true;
}

void CVulkanCommandBuffer::DeinitializeCommandBuffer()
{
  VkDevice device = m_deviceQueue->GetVulkanDevice();

  //if (submission_fence_.is_valid())
  //{
  //  assert(m_deviceQueue->GetFenceHelper()->HasPassed(submission_fence_));
  //  submission_fence_ = VulkanFenceHelper::FenceHandle();
  //}

  if (m_vKCommandBuffer != VK_NULL_HANDLE)
  {
    vkFreeCommandBuffers(device, m_commandPool->GetVkCommandPool(), 1, &m_vKCommandBuffer);
    m_vKCommandBuffer = VK_NULL_HANDLE;
  }
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
