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
#include "rendering/vulkan/VulkanUtils.h"
#include "utils/log.h"

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

using KODI::RENDERING::VULKAN::UTILS::ErrorString;

CVulkanFramebuffer::CVulkanFramebuffer(VkImageView vkImageView,
                                       VkFramebuffer vkFramebuffer,
                                       std::unique_ptr<CVulkanCommandBuffer> commandBuffer)
  : m_vkImageView(vkImageView),
    m_vkFramebuffer(vkFramebuffer),
    m_commandBuffer(std::move(commandBuffer))
{
}

CVulkanFramebuffer::~CVulkanFramebuffer()
{
  //const VkDevice vk_device = m_commandBuffer->VulkanDeviceQueue()->VulkanDevice();
  //if (m_vkFramebuffer != VK_NULL_HANDLE)
  //{
  //  vkDestroyFramebuffer(vk_device, m_vkFramebuffer, nullptr);
  //}
  //if (m_vkImageView != VK_NULL_HANDLE)
  //{
  //  vkDestroyImageView(vk_device, m_vkImageView, nullptr);
  //}
  //m_commandBuffer->Destroy();
  //m_commandBuffer.reset();
}


std::unique_ptr<CVulkanFramebuffer> CVulkanFramebuffer::Create(
    CVulkanDeviceQueue* vulkanDeviceQueue,
    CVulkanCommandPool* vulkanCommandPool,
    VkRenderPass vkRenderPass,
    CVulkanSurface* vulkanSurface,
    VkImage vkImage)
{
  CVulkanSwapChain* vulkan_swap_chain = vulkanSurface->SwapChain();
  const VkDevice vk_device = vulkanDeviceQueue->VulkanDevice();

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
  VkImageView vk_image_view = VK_NULL_HANDLE;
  result = vkCreateImageView(vk_device, &vkImageViewCreateInfo, nullptr, &vk_image_view);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGFATAL, "Vulkan: Failed to create a Vulkan image view. ERROR {0}",
              ErrorString(result));
    return nullptr;
  }
  VkFramebufferCreateInfo vkFramebufferCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .renderPass = vkRenderPass,
      .attachmentCount = 1,
      .pAttachments = &vk_image_view,
      .width = static_cast<uint32_t>(vulkan_swap_chain->Size().extent.width),
      .height = static_cast<uint32_t>(vulkan_swap_chain->Size().extent.height),
      .layers = 1,
  };

  VkFramebuffer vk_framebuffer = VK_NULL_HANDLE;
  result = vkCreateFramebuffer(vk_device, &vkFramebufferCreateInfo, nullptr, &vk_framebuffer);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGFATAL, "Vulkan: Failed to create a Vulkan framebuffer. ERROR {0}",
              ErrorString(result));
    return nullptr;
  }

  auto commandBuffer =
      std::make_unique<CVulkanCommandBuffer>(vulkanDeviceQueue, vulkanCommandPool, true);
  if (!commandBuffer->Initialize())
  {
    CLog::Log(LOGFATAL, "Vulkan: Failed to initialize a Vulkan command buffer.");
    return nullptr;
  }

  return std::make_unique<CVulkanFramebuffer>(vk_image_view, vk_framebuffer,
                                              std::move(commandBuffer));
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
