/*
 *  Copyright (C) 2026 Team Kodi
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

class CVulkanCommandBuffer;
class CVulkanDeviceQueue;

class CVulkanCommandPool
{
public:
  CVulkanCommandPool(CVulkanDeviceQueue* deviceQueue);
  ~CVulkanCommandPool();

  bool Initialize(bool allowProtectedMemory);
  void Deinitialize();

  std::unique_ptr<CVulkanCommandBuffer> CreatePrimaryCommandBuffer();
  std::unique_ptr<CVulkanCommandBuffer> CreateSecondaryCommandBuffer();

  VkCommandPool GetVkCommandPool() { return m_vkCommandPool; }

private:
  CVulkanCommandPool(const CVulkanCommandPool&) = delete;
  CVulkanCommandPool& operator=(const CVulkanCommandPool&) = delete;

  friend class CVulkanCommandBuffer;

  void IncrementCommandBufferCount();
  void DecrementCommandBufferCount();

  CVulkanDeviceQueue* m_deviceQueue{nullptr};
  VkCommandPool m_vkCommandPool{VK_NULL_HANDLE};
  uint32_t m_commandBufferCount{0};
  bool m_useProtectedMemory{false};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI