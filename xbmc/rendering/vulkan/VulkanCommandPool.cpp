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
#include "rendering/vulkan/utils/VulkanInitStructs.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "utils/log.h"

#include <cassert>

using namespace KODI::RENDERING::VULKAN::UTILS;

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
  VkCommandPoolCreateInfo info = vkCommandPoolCreateInfo();
  info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  info.queueFamilyIndex = static_cast<uint32_t>(m_deviceQueue->VulkanQueueIndex());

  if (allowProtectedMemory) [[likely]]
  {
    info.flags |= VK_COMMAND_POOL_CREATE_PROTECTED_BIT;
  }

  VK_CHECK_RESULT(vkCreateCommandPool(m_deviceQueue->vkDevice(), &info, nullptr, &m_vkCommandPool), false);

  return true;
}

void CVulkanCommandPool::Destroy()
{
  assert(m_commandBufferCount == 0u);
  if (m_vkCommandPool != VK_NULL_HANDLE) [[likely]]
  {
    vkDestroyCommandPool(m_deviceQueue->vkDevice(), m_vkCommandPool, nullptr);
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
