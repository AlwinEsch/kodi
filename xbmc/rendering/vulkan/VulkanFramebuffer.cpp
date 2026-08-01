/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanFramebuffer.h"

#include "rendering/vulkan/VulkanCommandBuffer.h"
#include "rendering/vulkan/VulkanCommandPool.h"
#include "rendering/vulkan/VulkanDeviceQueue.h"
#include "rendering/vulkan/VulkanSurface.h"
#include "rendering/vulkan/VulkanSwapChain.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "utils/log.h"

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

using KODI::RENDERING::VULKAN::UTILS::ErrorString;

CVulkanFramebuffer::CVulkanFramebuffer(VkDevice vkDevice)
  : m_vkDevice(vkDevice)
{
}

CVulkanFramebuffer::~CVulkanFramebuffer()
{
  assert(m_vkFramebuffer == VK_NULL_HANDLE);
  assert(m_vkImageView == VK_NULL_HANDLE);
  assert(!m_commandBuffer);
}

bool CVulkanFramebuffer::Create(
    CVulkanDeviceQueue* vulkanDeviceQueue,
    CVulkanCommandPool* vulkanCommandPool,
    VkRenderPass vkRenderPass,
    CVulkanSurface* vulkanSurface,
    VkImage vkImage)
{
  CVulkanSwapChain* vulkan_swap_chain = vulkanSurface->SwapChain();
  const VkDevice vk_device = vulkanDeviceQueue->vkDevice();
  VkImageViewCreateInfo vkImageViewCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .image = vkImage,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = vulkanSurface->vkSurfaceFormat().format,
      .components =
          {
              .r = VK_COMPONENT_SWIZZLE_IDENTITY,
              .g = VK_COMPONENT_SWIZZLE_IDENTITY,
              .b = VK_COMPONENT_SWIZZLE_IDENTITY,
              .a = VK_COMPONENT_SWIZZLE_IDENTITY,
          },
      .subresourceRange =
          {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
  };

  VkResult result;
  VK_CHECK_RESULT(vkCreateImageView(vk_device, &vkImageViewCreateInfo, nullptr, &m_vkImageView), false);
  VkFramebufferCreateInfo vkFramebufferCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .renderPass = vkRenderPass,
      .attachmentCount = 1,
      .pAttachments = &m_vkImageView,
      .width = static_cast<uint32_t>(vulkan_swap_chain->Size().extent.width),
      .height = static_cast<uint32_t>(vulkan_swap_chain->Size().extent.height),
      .layers = 1,
  };

  result = vkCreateFramebuffer(vk_device, &vkFramebufferCreateInfo, nullptr, &m_vkFramebuffer);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGFATAL, "Vulkan: Failed to create a Vulkan framebuffer. ERROR {0} (FILE {1} LINE {2})",
              ErrorString(result), __FILE__, __LINE__);
    Destroy();
    return false;
  }

  auto commandBuffer =
      std::make_unique<CVulkanCommandBuffer>(vulkanDeviceQueue, vulkanCommandPool, true);
  if (!commandBuffer->Initialize())
  {
    CLog::Log(LOGFATAL, "Vulkan: Failed to initialize a Vulkan command buffer. (FILE {0} LINE {1})", __FILE__, __LINE__);
    Destroy();
    return false;
  }

  m_commandBuffer = std::move(commandBuffer);

  return true;
}

void CVulkanFramebuffer::Destroy()
{
  if (m_commandBuffer)
  {
    m_commandBuffer->Destroy();
    m_commandBuffer.reset();
  }
  if (m_vkFramebuffer != VK_NULL_HANDLE)
  {
    vkDestroyFramebuffer(m_vkDevice, m_vkFramebuffer, nullptr);
    m_vkFramebuffer = VK_NULL_HANDLE;
  }
  if (m_vkImageView != VK_NULL_HANDLE)
  {
    vkDestroyImageView(m_vkDevice, m_vkImageView, nullptr);
    m_vkImageView = VK_NULL_HANDLE;
  }
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
