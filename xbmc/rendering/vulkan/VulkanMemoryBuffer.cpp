/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanMemoryBuffer.h"

#include "rendering/vulkan/VulkanDeviceQueue.h"
#include "rendering/vulkan/utils/VulkanInitStructs.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "utils/log.h"

using namespace KODI::RENDERING::VULKAN::UTILS;

namespace KODI::RENDERING::VULKAN
{

/*

		VK_CHECK_RESULT(m_vulkanDevice->createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffers.vertices, vertices.size() * sizeof(Vertex), vertices.data()));
		VK_CHECK_RESULT(m_vulkanDevice->createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffers.indices, indices.size() * sizeof(uint32_t), indices.data()));
*/
CVulkanMemoryBuffer::CVulkanMemoryBuffer(CVulkanDeviceQueue* const deviceQueue)
  : m_deviceQueue(deviceQueue),
    m_vkDevice(deviceQueue->vkDevice())
{
}

CVulkanMemoryBuffer::~CVulkanMemoryBuffer()
{
  DestroyBuffer();
}

VkResult CVulkanMemoryBuffer::CreateBuffer(VkBufferUsageFlags usageFlags,
                                       VkMemoryPropertyFlags memPropFlags,
                                       VkDeviceSize size,
                                       const void* data /* = nullptr*/)
{
  // Create the buffer handle
  auto bufferCreateInfo = vkBufferCreateInfo(usageFlags, size, VK_SHARING_MODE_EXCLUSIVE);
  VK_CHECK_RESULT(vkCreateBuffer(m_vkDevice, &bufferCreateInfo, nullptr, &m_vkBuffer), res);

  VkMemoryRequirements memReq = {};
  vkGetBufferMemoryRequirements(m_vkDevice, m_vkBuffer, &memReq);

  auto allocFlagsInfo = vkMemoryAllocateFlagsInfo(VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR);
  allocFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;

  auto memAlloc = vkMemoryAllocateInfo();
  memAlloc.allocationSize = memReq.size;
  memAlloc.memoryTypeIndex = m_deviceQueue->GetMemoryType(memReq.memoryTypeBits, memPropFlags);
  memAlloc.pNext = &allocFlagsInfo;
  VK_CHECK_RESULT(vkAllocateMemory(m_vkDevice, &memAlloc, nullptr, &m_vkMemory), res);

  m_vkAlignment = memReq.alignment;
  m_vkSize = size;
  m_vkUsageFlags = usageFlags;
  m_vkMemoryPropertyFlags = memPropFlags;

  // If a pointer to the buffer data has been passed, map the buffer and copy over the data
  if (data != nullptr)
  {
    VK_CHECK_RESULT(vkMapMemory(m_vkDevice, m_vkMemory, 0, VK_WHOLE_SIZE, 0, &m_mapped), res);
    memcpy(m_mapped, data, size);
    if ((m_vkMemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
    {
      VkMappedMemoryRange mappedRange = vkMappedMemoryRange(m_vkMemory, 0, VK_WHOLE_SIZE);
      VK_CHECK_RESULT(vkFlushMappedMemoryRanges(m_vkDevice, 1, &mappedRange), res);
    }

    vkUnmapMemory(m_vkDevice, m_vkMemory);
  }

  // Initialize a default descriptor that covers the whole buffer size
  m_vkDescriptor.offset = 0;
  m_vkDescriptor.buffer = m_vkBuffer;
  m_vkDescriptor.range = size;

  VK_CHECK_RESULT(vkBindBufferMemory(m_vkDevice, m_vkBuffer, m_vkMemory, 0), res);

  return VK_SUCCESS;
}

void CVulkanMemoryBuffer::DestroyBuffer()
{
  if (m_vkBuffer != VK_NULL_HANDLE)
  {
    vkDestroyBuffer(m_vkDevice, m_vkBuffer, nullptr);
    m_vkBuffer = VK_NULL_HANDLE;
  }
  if (m_vkMemory != VK_NULL_HANDLE)
  {
    vkFreeMemory(m_vkDevice, m_vkMemory, nullptr);
    m_vkMemory = VK_NULL_HANDLE;
  }
}

VkResult CVulkanMemoryBuffer::Map(VkDeviceSize size, VkDeviceSize offset)
{
  return vkMapMemory(m_vkDevice, m_vkMemory, offset, size, 0, &m_mapped);
}

void CVulkanMemoryBuffer::Unmap()
{
  if (m_mapped)
  {
    vkUnmapMemory(m_vkDevice, m_vkMemory);
    m_mapped = nullptr;
  }
}

} // namespace KODI::RENDERING::VULKAN
