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

namespace
{

VkPipelineStageFlags GetPipelineStageFlags2(const CVulkanDeviceQueue* deviceQueue,
                                            const VkImageLayout layout)
{
  switch (layout)
  {
    case VK_IMAGE_LAYOUT_UNDEFINED:
      return VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
    case VK_IMAGE_LAYOUT_GENERAL:
      return VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    case VK_IMAGE_LAYOUT_PREINITIALIZED:
      return VK_PIPELINE_STAGE_2_HOST_BIT;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
      return VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
      return VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
    {
      VkPipelineStageFlags flags =
          VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
      if (deviceQueue->GetEnabledDeviceFeatures().tessellationShader)
      {
        flags |= VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT |
                 VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT;
      }
      if (deviceQueue->GetEnabledDeviceFeatures().geometryShader)
      {
        flags |= VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT;
      }
      return flags;
    }
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
      return VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
    default:
      CLog::Log(LOGERROR, "Unknown layout: {}", layout);
      return 0;
  }
}

VkAccessFlags2 GetAccessMask2(const VkImageLayout layout)
{
  switch (layout)
  {
    case VK_IMAGE_LAYOUT_UNDEFINED:
      return 0;
    case VK_IMAGE_LAYOUT_GENERAL:
      CLog::Log(LOGWARNING, "VK_IMAGE_LAYOUT_GENERAL is used.");
      return VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT |
             VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT |
             VK_ACCESS_2_TRANSFER_READ_BIT | VK_ACCESS_2_SHADER_READ_BIT |
             VK_ACCESS_2_HOST_WRITE_BIT | VK_ACCESS_2_HOST_READ_BIT |
             VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT |
             VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_MEMORY_READ_BIT |
             VK_ACCESS_2_MEMORY_WRITE_BIT;
    case VK_IMAGE_LAYOUT_PREINITIALIZED:
      return VK_ACCESS_2_HOST_WRITE_BIT;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
      return VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
      return VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
      return VK_ACCESS_2_TRANSFER_READ_BIT;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
      return VK_ACCESS_2_TRANSFER_WRITE_BIT;
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
      return 0;
    default:
      CLog::Log(LOGERROR, "Unknown layout: {}", layout);
      return 0;
  }
}

} // namespace

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

void CVulkanCommandBuffer::Deinitialize()
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

void CVulkanCommandBuffer::TransitionImageLayout(VkImage image,
                                                 VkImageLayout oldLayout,
                                                 VkImageLayout newLayout,
                                                 uint32_t srcQueueFamilyIndex,
                                                 uint32_t dstQueueFamilyIndex)
{
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
  vkCmdPipelineBarrier2(m_vKCommandBuffer, &dependency_info);
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
