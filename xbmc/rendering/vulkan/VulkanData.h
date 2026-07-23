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

struct VulkanData
{
  VkSurfaceKHR vkSurface{VK_NULL_HANDLE};
  VkSurfaceFormatKHR vkSurfaceFormat{};
  VkDevice vkDevice{VK_NULL_HANDLE};
  VkPipelineCache vkPipelineCache{VK_NULL_HANDLE};
  VkRenderPass vkRenderPass{VK_NULL_HANDLE};
  //VkDescriptorPool vkDescriptorPool{VK_NULL_HANDLE};
  VkPipelineLayout vkPipelineLayout{
      VK_NULL_HANDLE}; // TODO: Remove this and use the pipeline layout from the shader instead
};

} // namespace KODI::RENDERING::VULKAN
