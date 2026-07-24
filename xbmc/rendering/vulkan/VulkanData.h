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

/**
 * @brief Structure to hold Vulkan memory data.
 *
 * Becomes useful when allocating memory for Vulkan resources,
 * as it encapsulates the memory handle, size, and offset.
 */
struct VulkanMemoryData
{
  VkBuffer buffer{VK_NULL_HANDLE};
  VkDeviceMemory memory{VK_NULL_HANDLE};
  VkDeviceSize size{0};
  VkDeviceSize offset{0};
  void* mapped{nullptr};
};

struct VulkanData
{
  VkSurfaceKHR vkSurface{VK_NULL_HANDLE};
  VkSurfaceFormatKHR vkSurfaceFormat{};
  VkDevice vkDevice{VK_NULL_HANDLE};
  VkCommandPool vkCommandPool{VK_NULL_HANDLE};
  VkPipelineCache vkPipelineCache{VK_NULL_HANDLE};
  VkRenderPass vkRenderPass{VK_NULL_HANDLE};
  VkQueue vkQueue{VK_NULL_HANDLE};
  //VkDescriptorPool vkDescriptorPool{VK_NULL_HANDLE};
};

} // namespace KODI::RENDERING::VULKAN
