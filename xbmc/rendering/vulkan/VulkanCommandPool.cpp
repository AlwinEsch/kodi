/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanCommandPool.h"

#include "rendering/vulkan/VulkanCommandBuffer.h"
#include "rendering/vulkan/VulkanDeviceQueue.h"
#include "utils/log.h"

#include <cassert>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

CVulkanCommandPool::CVulkanCommandPool(CVulkanDeviceQueue* deviceQueue) : m_deviceQueue(deviceQueue)
{
}

CVulkanCommandPool::~CVulkanCommandPool()
{
  assert(m_commandBufferCount == 0u);
  assert(m_vkCommandPool == VK_NULL_HANDLE);
}

bool CVulkanCommandPool::Initialize(bool allowProtectedMemory)
{
  VkCommandPoolCreateInfo info{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = nullptr,
      //.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT |
               (allowProtectedMemory ? VK_COMMAND_POOL_CREATE_PROTECTED_BIT : 0),
      .queueFamilyIndex = m_deviceQueue->GetVulkanQueueIndex(),
  };
  VkResult result =
      vkCreateCommandPool(m_deviceQueue->GetVulkanDevice(), &info, nullptr, &m_vkCommandPool);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create command pool for per frame data. ERROR {0}",
              result);
    return false;
  }

  return true;
}

void CVulkanCommandPool::Deinitialize()
{
  assert(m_commandBufferCount == 0u);
  if (m_vkCommandPool != VK_NULL_HANDLE)
  {
    vkDestroyCommandPool(m_deviceQueue->GetVulkanDevice(), m_vkCommandPool, nullptr);
    m_vkCommandPool = VK_NULL_HANDLE;
  }
}

std::unique_ptr<CVulkanCommandBuffer> CVulkanCommandPool::CreatePrimaryCommandBuffer()
{
  auto commandBuffer = std::make_unique<CVulkanCommandBuffer>(m_deviceQueue, this, true);
  if (!commandBuffer->Initialize())
    return nullptr;

  return commandBuffer;
}

std::unique_ptr<CVulkanCommandBuffer> CVulkanCommandPool::CreateSecondaryCommandBuffer()
{
  auto commandBuffer = std::make_unique<CVulkanCommandBuffer>(m_deviceQueue, this, false);
  if (!commandBuffer->Initialize())
    return nullptr;

  return commandBuffer;
}

void CVulkanCommandPool::IncrementCommandBufferCount()
{
  ++m_commandBufferCount;
}

void CVulkanCommandPool::DecrementCommandBufferCount()
{
  assert(m_commandBufferCount > 0u);
  --m_commandBufferCount;
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
