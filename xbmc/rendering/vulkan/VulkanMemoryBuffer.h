/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <vulkan/vulkan_core.h>

namespace KODI::RENDERING::VULKAN
{

class CVulkanDeviceQueue;

class CVulkanMemoryBuffer
{
public:
  CVulkanMemoryBuffer(CVulkanDeviceQueue* const deviceQueue);
  virtual ~CVulkanMemoryBuffer();

  VkResult CreateBuffer(VkBufferUsageFlags usageFlags,
                        VkMemoryPropertyFlags memPropFlags,
                        VkDeviceSize size,
                        const void* data = nullptr);
  void DestroyBuffer();

  inline VkBuffer vkBuffer() const { return m_vkBuffer; }
  inline VkDeviceMemory vkMemory() const { return m_vkMemory; }
  inline VkDeviceSize vkSize() const { return m_vkSize; }
  VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  void Unmap();

private:
  CVulkanDeviceQueue* const m_deviceQueue;
  const VkDevice m_vkDevice{VK_NULL_HANDLE};

  VkPipeline m_vkPipeline{VK_NULL_HANDLE};
  VkPipelineLayout m_vkPipelineLayout{VK_NULL_HANDLE};
  VkRenderPass m_vkRenderPass{VK_NULL_HANDLE};

  VkBuffer m_vkBuffer{VK_NULL_HANDLE};
  VkDeviceMemory m_vkMemory{VK_NULL_HANDLE};
  VkDescriptorBufferInfo m_vkDescriptor{};
  VkDeviceSize m_vkSize{0};
  VkDeviceSize m_vkAlignment{0};

  VkBufferUsageFlags m_vkUsageFlags;
  VkMemoryPropertyFlags m_vkMemoryPropertyFlags;

  void* m_mapped{nullptr};
};

} // namespace KODI::RENDERING::VULKAN
