/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DynamicBuffers.h"

#include "rendering/vulkan/VulkanDeviceQueue.h"
#include "rendering/vulkan/utils/VulkanInitStructs.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "utils/log.h"

namespace KODI::RENDERING::VULKAN
{

constexpr const uint32_t INITIAL_DYNAMIC_VERTEX_BUFFER_SIZE_KB = 256;
constexpr const uint32_t INITIAL_DYNAMIC_INDEX_BUFFER_SIZE_KB = 1024;
constexpr const uint32_t INITIAL_DYNAMIC_UNIFORM_BUFFER_SIZE_KB = 256;

CVulkanDynamicBuffers::CVulkanDynamicBuffers(const VulkanData* vkData,
                                             CVulkanDeviceQueue* deviceQueue)
  : m_vkData(vkData),
    m_deviceQueue(deviceQueue)
{
  for (auto& buffer : m_buffers)
  {
    buffer = std::make_unique<CVulkanDynamicBuffer>(vkData, deviceQueue);
  }
}

CVulkanDynamicBuffers::~CVulkanDynamicBuffers()
{
  Destroy();
}

bool CVulkanDynamicBuffers::Create()
{
  if (!m_buffers[BUFFER_TYPE_VERTEX]->Create(INITIAL_DYNAMIC_VERTEX_BUFFER_SIZE_KB * 1024,
                                             VK_BUFFER_USAGE_VERTEX_BUFFER_BIT))
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create vertex buffer ({0}:{1})", __FILENAME__, __LINE__);
    return false;
  }
  if (!m_buffers[BUFFER_TYPE_INDEX]->Create(INITIAL_DYNAMIC_INDEX_BUFFER_SIZE_KB * 1024,
                                            VK_BUFFER_USAGE_INDEX_BUFFER_BIT))
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create index buffer ({0}:{1})", __FILENAME__, __LINE__);
    return false;
  }
  if (!m_buffers[BUFFER_TYPE_UNIFORM]->Create(INITIAL_DYNAMIC_UNIFORM_BUFFER_SIZE_KB * 1024,
                                              VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT))
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create uniform buffer ({0}:{1})", __FILENAME__,
              __LINE__);
    return false;
  }
  return true;
}

void CVulkanDynamicBuffers::Destroy()
{
  for (auto& buffer : m_buffers)
  {
    buffer->Destroy();
  }
}

void CVulkanDynamicBuffers::BeginFrame(uint32_t indexBuffer)
{
  m_currentFrameIndex = indexBuffer;

  for (auto& buffer : m_buffers)
  {
    buffer->m_currentFrameIndex = m_currentFrameIndex;
    buffer->m_memData[m_currentFrameIndex].offset = 0;
  }
}

void CVulkanDynamicBuffers::EndFrame()
{
  std::array<VkMappedMemoryRange, BUFFER_TYPE_COUNT> ranges = {};
  for (size_t i = 0; i < BUFFER_TYPE_COUNT; ++i)
  {
    ranges[i] = m_buffers[i]->GetMappedMemoryRange();
    m_buffers[i]->m_currentOffset = 0;
  }
  vkFlushMappedMemoryRanges(m_vkData->vkDevice, BUFFER_TYPE_COUNT, ranges.data());
}

//--------------------------------------------------------------------------------------------------

CVulkanDynamicBuffer::CVulkanDynamicBuffer(const VulkanData* vkData,
                                           CVulkanDeviceQueue* deviceQueue)
  : m_vkData(vkData),
    m_deviceQueue(deviceQueue)
{
}

CVulkanDynamicBuffer::~CVulkanDynamicBuffer()
{
  Destroy();
}

bool CVulkanDynamicBuffer::Create(VkDeviceSize initialSize,
                                  VkBufferUsageFlags usage,
                                  VkDescriptorSet* descriptorSet)
{
  m_usage = usage;

  for (auto& memData : m_memData)
  {
    VK_CHECK_RESULT(m_deviceQueue->CreateBuffer(m_usage,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                &memData, initialSize),
                    false);

    // We map the buffer once, so we can update it without having to map it again
    VK_CHECK_RESULT(
        vkMapMemory(m_vkData->vkDevice, memData.memory, 0, initialSize, 0, (void**)&memData.mapped),
        false);

    if (descriptorSet)
    {
      VkDescriptorSetAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
      allocInfo.descriptorPool = m_vkData->vkDescriptorPool;
      allocInfo.descriptorSetCount = 1;
      allocInfo.pSetLayouts = &m_vkData->vkDescriptorSetLayout_Uniform;

      /// Allocate a descriptor set for the uniform buffer
      /// @note @ref vkFreeDescriptorSets becomes called within the @ref CVulkanDeviceQueue::DestroyBuffer.
      VK_CHECK_RESULT(
          vkAllocateDescriptorSets(m_vkData->vkDevice, &allocInfo, &memData.descriptorSet), false);

      // The buffer's information is passed using a m_descriptor info structure
      VkDescriptorBufferInfo bufferInfo{};
      bufferInfo.buffer = memData.buffer;
      bufferInfo.offset = 0;
      bufferInfo.range = sizeof(VulkanUniform);

      VkWriteDescriptorSet uboWrite{};
      uboWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      uboWrite.dstSet = memData.descriptorSet;
      uboWrite.dstBinding = 0;
      uboWrite.dstArrayElement = 0;
      uboWrite.descriptorCount = 1;
      uboWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
      uboWrite.pBufferInfo = &bufferInfo;

      vkUpdateDescriptorSets(m_vkData->vkDevice, 1, &uboWrite, 0, nullptr);
    }
  }

  m_currentSize = initialSize;
  m_currentOffset = 0;
  m_currentFrameIndex = 0;

  return true;
}

void CVulkanDynamicBuffer::Destroy()
{
  for (auto& memData : m_memData)
  {
    m_deviceQueue->DestroyBuffer(&memData);
  }
}

void* CVulkanDynamicBuffer::AllocateOffset(size_t size,
                                           VkBuffer& buffer,
                                           VkDeviceSize& bufferOffset,
                                           VkDescriptorSet* descriptorSet)
{
  std::unique_lock lock(m_criticalSection);

  if (m_currentOffset + size > m_currentSize)
  {
    // Not enough space in the current buffer, need to resize
    const VkDeviceSize newSize = std::max(m_currentSize * 2, m_currentOffset + size);
    Destroy();
    Create(newSize, m_usage, descriptorSet);
  }

  buffer = m_memData[m_currentFrameIndex].buffer;
  bufferOffset = m_currentOffset;
  m_currentOffset += size;
  return m_memData[m_currentFrameIndex].mapped + bufferOffset;
}

VkMappedMemoryRange CVulkanDynamicBuffer::GetMappedMemoryRange() const
{
  return {
      VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
      nullptr,
      m_memData[m_currentFrameIndex].memory,
      0,
      m_currentSize,
  };
}

} // namespace KODI::RENDERING::VULKAN
