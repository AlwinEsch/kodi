/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once
#include <string>

#include <vulkan/vulkan_core.h>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

class CVulkanSwapChain;

class CVulkanScopedWrite
{
public:
  explicit CVulkanScopedWrite(CVulkanSwapChain* swap_chain);
  CVulkanScopedWrite(CVulkanScopedWrite&& other);
  ~CVulkanScopedWrite();

  const CVulkanScopedWrite& operator=(CVulkanScopedWrite&& other);

  void Reset();

  bool Success() const { return m_success; }
  VkImage Image() const { return m_image; }
  uint32_t ImageIndex() const { return m_imageIndex; }
  VkImageLayout ImageLayout() const { return m_imageLayout; }
  VkImageUsageFlags ImageUsage() const { return m_imageUsage; }
  VkSemaphore BeginSemaphore() const { return m_beginSemaphore; }
  VkSemaphore EndSemaphore() const { return m_endSemaphore; }

private:
  CVulkanScopedWrite(const CVulkanScopedWrite&) = delete;
  CVulkanScopedWrite& operator=(const CVulkanScopedWrite&) = delete;

  CVulkanSwapChain* m_swapChain{nullptr};
  bool m_success{false};
  VkImage m_image{VK_NULL_HANDLE};
  uint32_t m_imageIndex{0};
  VkImageLayout m_imageLayout{VK_IMAGE_LAYOUT_UNDEFINED};
  VkImageUsageFlags m_imageUsage{0};
  VkSemaphore m_beginSemaphore{VK_NULL_HANDLE};
  VkSemaphore m_endSemaphore{VK_NULL_HANDLE};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI