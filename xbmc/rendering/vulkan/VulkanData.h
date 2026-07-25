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
  /**
   * @brief The Vulkan buffer handle.
   *
   * This is the actual buffer object that can be used for rendering or compute operations.
   */
  VkBuffer buffer{VK_NULL_HANDLE};

  /**
   * @brief The Vulkan device memory handle.
   */
  VkDeviceMemory memory{VK_NULL_HANDLE};

  /**
   * @brief The descriptor set stores the resources bound to the binding points in a shader.
   *
   * It connects the binding points of the different shaders with the buffers and images
   * used for those bindings.
   */
  VkDescriptorSet descriptorSet{VK_NULL_HANDLE};

  /**
   * @brief The size of the allocated memory.
   *
   * This is useful for knowing how much memory is available for the resource.
   */
  VkDeviceSize size{0};

  /**
   * @brief The offset within the buffer where the memory starts.
   *
   * This is useful when a single buffer is used for multiple resources,
   * allowing each resource to have its own offset within the buffer.
   */
  VkDeviceSize offset{0};

  /**
   * @brief We keep a pointer to the mapped buffer,
   * so we can easily update its contents via a memcpy.
   */
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
