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
#include "rendering/vulkan/VulkanUtils.h"
#include "utils/log.h"

#include <cassert>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

using namespace KODI::RENDERING::VULKAN::UTILS;

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

bool CVulkanCommandBuffer::Initialize()
{
  VkResult result = VK_SUCCESS;
  VkDevice device = m_deviceQueue->VulkanDevice();

  VkCommandBufferAllocateInfo info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = m_commandPool->vkCommandPool(),
      .level = m_primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY,
      .commandBufferCount = 1,
  };

  assert(m_vkCommandBuffer == static_cast<VkCommandBuffer>(VK_NULL_HANDLE));

  result = vkAllocateCommandBuffers(device, &info, &m_vkCommandBuffer);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: vkAllocateCommandBuffers() failed: {}", result);
    return false;
  }

  m_recordType = RECORD_TYPE_EMPTY;
  return true;
}

void CVulkanCommandBuffer::Destroy()
{
  VkDevice device = m_deviceQueue->VulkanDevice();

  if (m_submissionFence.IsValid())
  {
    assert(m_deviceQueue->FenceHelper()->HasPassed(m_submissionFence));
    m_submissionFence = CVulkanFenceHelper::CFenceHandle();
  }

  if (m_vkCommandBuffer != VK_NULL_HANDLE)
  {
    vkFreeCommandBuffers(device, m_commandPool->vkCommandPool(), 1, &m_vkCommandBuffer);
    m_vkCommandBuffer = VK_NULL_HANDLE;
  }
}

bool CVulkanCommandBuffer::Submit(uint32_t numWaitSemaphores,
                                  VkSemaphore* waitSemaphores,
                                  uint32_t numSignalSemaphores,
                                  VkSemaphore* signalSemaphores,
                                  bool allowProtectedMemory /*= false*/)
{
  std::vector<VkPipelineStageFlags> wait_dst_stage_mask(numWaitSemaphores,
                                                        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

  VkProtectedSubmitInfo protected_submit_info = {};
  protected_submit_info.sType = VK_STRUCTURE_TYPE_PROTECTED_SUBMIT_INFO;
  protected_submit_info.protectedSubmit = allowProtectedMemory;

  VkSubmitInfo submit_info = {};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.pNext = &protected_submit_info;
  submit_info.waitSemaphoreCount = numWaitSemaphores;
  submit_info.pWaitSemaphores = waitSemaphores;
  submit_info.pWaitDstStageMask = wait_dst_stage_mask.data();
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &m_vkCommandBuffer;
  submit_info.signalSemaphoreCount = numSignalSemaphores;
  submit_info.pSignalSemaphores = signalSemaphores;

  VkResult result = VK_SUCCESS;

  VkFence fence;
  result = m_deviceQueue->FenceHelper()->GetFence(&fence);
  if (VK_SUCCESS != result)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create fence, ERROR: {}", ErrorString(result));
    return false;
  }

  result = vkQueueSubmit(m_deviceQueue->VulkanQueue(), 1, &submit_info, fence);
  if (VK_SUCCESS != result)
  {
    vkDestroyFence(m_deviceQueue->VulkanDevice(), fence, nullptr);
    m_submissionFence = CVulkanFenceHelper::CFenceHandle();
  }
  else
  {
    m_submissionFence = m_deviceQueue->FenceHelper()->EnqueueFence(fence);
  }

  PostExecution();
  if (VK_SUCCESS != result)
  {
    CLog::Log(LOGERROR, "Vulkan: vkQueueSubmit() failed: {}", ErrorString(result));
    return false;
  }

  return true;
}

void CVulkanCommandBuffer::TransitionImageLayout(VkImage image,
                                                 VkImageLayout oldLayout,
                                                 VkImageLayout newLayout,
                                                 uint32_t srcQueueFamilyIndex,
                                                 uint32_t dstQueueFamilyIndex)
{
  VkImageMemoryBarrier barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .pNext = nullptr,
      .srcAccessMask = GetAccessMask(oldLayout),
      .dstAccessMask = GetAccessMask(newLayout),

      // Specify the old and new layouts of the image
      .oldLayout = oldLayout, // Current layout of the image
      .newLayout = newLayout, // Target layout of the image

      // We are not changing the ownership between queues
      .srcQueueFamilyIndex = srcQueueFamilyIndex,
      .dstQueueFamilyIndex = dstQueueFamilyIndex,

      // Specify the image to be affected by this barrier
      .image = image,

      // Define the subresource range (which parts of the image are affected)
      .subresourceRange =
          {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, // Affects the color aspect of the image
              .baseMipLevel = 0, // Start at mip level 0
              .levelCount = 1, // Number of mip levels affected
              .baseArrayLayer = 0, // Start at array layer 0
              .layerCount = 1, // Number of array layers affected
          },

  };
  // Specify the pipeline stages and access masks for the barrier
  auto stageFlagsOld = GetPipelineStageFlags(m_deviceQueue, oldLayout);
  auto stageFlagsNew = GetPipelineStageFlags(m_deviceQueue, newLayout);
  vkCmdPipelineBarrier(m_vkCommandBuffer, stageFlagsOld, stageFlagsNew, 0, 0, nullptr, 0, nullptr,
                       1, &barrier);

#if 0
  // Initialize the VkImageMemoryBarrier2 structure
  VkImageMemoryBarrier2 image_barrier{
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
      .pNext = nullptr,

      // Specify the pipeline stages and access masks for the barrier
      .srcStageMask =
          GetPipelineStageFlags2(m_deviceQueue, oldLayout), // Source pipeline stage mask
      .srcAccessMask = GetAccessMask2(oldLayout), // Source access mask
      .dstStageMask =
          GetPipelineStageFlags2(m_deviceQueue, newLayout), // Destination pipeline stage mask
      .dstAccessMask = GetAccessMask2(newLayout), // Destination access mask

      // Specify the old and new layouts of the image
      .oldLayout = oldLayout, // Current layout of the image
      .newLayout = newLayout, // Target layout of the image

      // We are not changing the ownership between queues
      .srcQueueFamilyIndex = srcQueueFamilyIndex,
      .dstQueueFamilyIndex = dstQueueFamilyIndex,

      // Specify the image to be affected by this barrier
      .image = image,

      // Define the subresource range (which parts of the image are affected)
      .subresourceRange = {
          .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, // Affects the color aspect of the image
          .baseMipLevel = 0, // Start at mip level 0
          .levelCount = 1, // Number of mip levels affected
          .baseArrayLayer = 0, // Start at array layer 0
          .layerCount = 1 // Number of array layers affected
      }};

  // Initialize the VkDependencyInfo structure
  VkDependencyInfo dependency_info{
      .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
      .pNext = nullptr,
      .dependencyFlags = 0, // No special dependency flags
      .memoryBarrierCount = 0, // No memory barriers
      .pMemoryBarriers = nullptr, // No memory barriers
      .bufferMemoryBarrierCount = 0, // No buffer memory barriers
      .pBufferMemoryBarriers = nullptr, // No buffer memory barriers
      .imageMemoryBarrierCount = 1, // Number of image memory barriers
      .pImageMemoryBarriers = &image_barrier // Pointer to the image memory barrier(s)
  };

  // Record the pipeline barrier into the command buffer
  vkCmdPipelineBarrier2(m_vkCommandBuffer, &dependency_info);
#endif
}

void CVulkanCommandBuffer::PostExecution()
{
  if (m_recordType == RECORD_TYPE_SINGLE_USE)
  {
    // Clear upon next use.
    m_recordType = RECORD_TYPE_DIRTY;
  }
  else if (m_recordType == RECORD_TYPE_MULTI_USE)
  {
    // Can no longer record new items unless marked as clear.
    m_recordType = RECORD_TYPE_RECORDED;
  }
}

void CVulkanCommandBuffer::ResetIfDirty()
{
  assert(!m_recording);

  if (m_recordType == RECORD_TYPE_DIRTY)
  {
    // Block if command buffer is still in use. This can be externally avoided
    // using the asynchronous SubmissionFinished() function.
    Wait(UINT64_MAX);
    VkResult result = vkResetCommandBuffer(m_vkCommandBuffer, 0);
    if (result != VK_SUCCESS)
    {
      CLog::Log(LOGERROR, "Vulkan: vkResetCommandBuffer() failed: {}", ErrorString(result));
    }
    else
    {
      m_recordType = RECORD_TYPE_EMPTY;
    }
  }
}

void CVulkanCommandBuffer::Wait(uint64_t timeout)
{
  if (!m_submissionFence.IsValid())
    return;

  m_deviceQueue->FenceHelper()->Wait(m_submissionFence, timeout);
}

CVulkanCommandBufferScoped::CVulkanCommandBufferScoped(CVulkanCommandBuffer& commandBuffer,
                                                       VkCommandBufferUsageFlags usageFlags)
  : m_usageFlags(usageFlags)
{
  assert(&commandBuffer != nullptr);
  m_handle = commandBuffer.GetVulkanCommandBuffer();
  assert(m_handle != VK_NULL_HANDLE);

  VkCommandBufferBeginInfo begin_info = {};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = m_usageFlags;
  VkResult result = vkBeginCommandBuffer(m_handle, &begin_info);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: vkBeginCommandBuffer() failed: {}", ErrorString(result));
  }
}

CVulkanCommandBufferScoped::~CVulkanCommandBufferScoped()
{
  VkResult result = vkEndCommandBuffer(m_handle);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: vkEndCommandBuffer() failed: {}", ErrorString(result));
  }
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
