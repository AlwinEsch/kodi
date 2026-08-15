/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "rendering/vulkan/VulkanData.h"
#include "threads/CriticalSection.h"

#include <array>
#include <memory>

namespace KODI::RENDERING::VULKAN
{

enum BufferType
{
  BUFFER_TYPE_VERTEX,
  BUFFER_TYPE_INDEX,
  BUFFER_TYPE_UNIFORM,
  BUFFER_TYPE_COUNT
};

class CVulkanDeviceQueue;
class CVulkanDynamicBuffer;

class CVulkanDynamicBuffers
{
public:
  CVulkanDynamicBuffers(const VulkanData* vkData, CVulkanDeviceQueue* deviceQueue);
  virtual ~CVulkanDynamicBuffers();

  CVulkanDynamicBuffer* GetBuffer(BufferType type) { return m_buffers[type].get(); }

  bool Create();
  void Destroy();
  void BeginFrame(uint32_t indexBuffer);
  void EndFrame();
  VkMappedMemoryRange GetMappedMemoryRange() const;

private:
  const VulkanData* m_vkData;
  CVulkanDeviceQueue* const m_deviceQueue;

  std::array<std::unique_ptr<CVulkanDynamicBuffer>, BUFFER_TYPE_COUNT> m_buffers;
  uint32_t m_currentFrameIndex{0};
};

//--------------------------------------------------------------------------------------------------

class CVulkanDynamicBuffer
{
public:
  CVulkanDynamicBuffer(const VulkanData* vkData, CVulkanDeviceQueue* deviceQueue);
  virtual ~CVulkanDynamicBuffer();

  void* AllocateOffset(size_t size,
                       VkBuffer& buffer,
                       VkDeviceSize& bufferOffset,
                       bool uniform = false,
                       VkDescriptorSet* descriptorSet = nullptr);

private:
  friend class CVulkanDynamicBuffers;

  bool Create(VkDeviceSize initialSize, VkBufferUsageFlags usage, bool uniform = false);
  void Destroy();

  VkMappedMemoryRange GetMappedMemoryRange() const;

  const VulkanData* m_vkData;
  CVulkanDeviceQueue* const m_deviceQueue;

  VkBufferUsageFlags m_usage;
  VkDeviceSize m_currentSize{0};
  VkDeviceSize m_currentOffset{0};
  uint32_t m_currentFrameIndex{0};
  size_t m_dynamicAlignment{0};

  std::array<VulkanMemoryData, MAX_CONCURRENT_FRAMES> m_memData;

  mutable CCriticalSection m_criticalSection;
};

} // namespace KODI::RENDERING::VULKAN
