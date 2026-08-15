/*
 *  Copyright (C) 2005-2018 Team Kodi
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

class CVulkanRenderPass
{
public:
  CVulkanRenderPass(VkDevice logicalDevice, VkRenderPass renderPass);
  ~CVulkanRenderPass();

  static std::unique_ptr<CVulkanRenderPass> Create(VkFormat format, VkDevice logicalDevice);

  VkRenderPass vkRenderPass() const { return m_renderPass; }

private:
  CVulkanRenderPass(const CVulkanRenderPass&) = delete;
  CVulkanRenderPass& operator=(const CVulkanRenderPass&) = delete;

  const VkRenderPass m_renderPass;
  const VkDevice m_logicalDevice;
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
