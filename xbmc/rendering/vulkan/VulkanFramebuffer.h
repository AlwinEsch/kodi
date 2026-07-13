/*
 *  Copyright (C) 2005-2026 Team Kodi
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
class CVulkanCommandPool;
class CVulkanDeviceQueue;
class CVulkanSurface;

class CVulkanFramebuffer
{
public:
  CVulkanFramebuffer(VkImageView vkImageView,
                     VkFramebuffer vkFramebuffer,
                     std::unique_ptr<CVulkanCommandBuffer> commandBuffer);
  ~CVulkanFramebuffer();

  static std::unique_ptr<CVulkanFramebuffer> Create(CVulkanDeviceQueue* vulkanDeviceQueue,
                                                    CVulkanCommandPool* vulkanCommandPool,
                                                    VkRenderPass vkRenderPass,
                                                    CVulkanSurface* vulkanSurface,
                                                    VkImage vkImage);

  VkImageView vkImageView() const { return m_vkImageView; }
  VkFramebuffer vkFramebuffer() const { return m_vkFramebuffer; }
  CVulkanCommandBuffer* CommandBuffer() const { return m_commandBuffer.get(); }

private:
  CVulkanFramebuffer(const CVulkanFramebuffer&) = delete;
  CVulkanFramebuffer& operator=(const CVulkanFramebuffer&) = delete;

  const VkImageView m_vkImageView;
  const VkFramebuffer m_vkFramebuffer;
  const std::unique_ptr<CVulkanCommandBuffer> m_commandBuffer;
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
