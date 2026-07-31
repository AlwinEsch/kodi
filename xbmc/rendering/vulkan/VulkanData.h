/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

namespace KODI::RENDERING::VULKAN
{

constexpr const uint32_t REQUIRED_VK_API_VERSION{VK_API_VERSION_1_2};
constexpr const uint32_t MIN_DESCRIPTORS_PER_TYPE = 16u;
constexpr const uint32_t MAX_SANITY_LIGHTMAPS = 256;
constexpr const uint32_t MAX_TEXTURES = (16u * 4096u);
constexpr const uint32_t MAX_UNIFORM_ALLOC = 2048u;


// We want to keep GPU and CPU busy. To do that we may start building a new command m_buffer while the previous one is still being executed
// This number defines how many frames may be worked on simultaneously at once
// Increasing this number may improve performance but will also introduce additional latency
constexpr const uint32_t MAX_CONCURRENT_FRAMES = 3u;

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
   * @brief The mapped memory range for this Vulkan memory data.
   *
   * This is useful for updating the contents of the memory via a mapped pointer.
   */
  VkMappedMemoryRange mappedRange{};

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

struct VulkanUniform
{
  glm::mat4 projectionMatrix;
  glm::mat4 modelMatrix;
  float depth;
};

struct VulkanData
{
  VkInstance vkInstance{VK_NULL_HANDLE};
  VkDevice vkDevice{VK_NULL_HANDLE};
  VkPhysicalDevice vkPhysicalDevice{VK_NULL_HANDLE};
  VkSurfaceKHR vkSurface{VK_NULL_HANDLE};
  VkSurfaceFormatKHR vkSurfaceFormat{};
  VkCommandPool vkCommandPool{VK_NULL_HANDLE};
  VkPipelineCache vkPipelineCache{VK_NULL_HANDLE};
  VkRenderPass vkRenderPass{VK_NULL_HANDLE};
  VkQueue vkQueue{VK_NULL_HANDLE};
  VkSwapchainKHR vkSwapchain{VK_NULL_HANDLE};
  VkFormat vkSwapchainFormat = VK_FORMAT_UNDEFINED;

  VkDescriptorPool vkDescriptorPool{VK_NULL_HANDLE};

  VkDescriptorSetLayout vkDescriptorSetLayout_Texture{VK_NULL_HANDLE};
  VkDescriptorSetLayout vkDescriptorSetLayout_Uniform{VK_NULL_HANDLE};

  VkSampler vkPointSampler{VK_NULL_HANDLE};
  VkSampler vkLinearSampler{VK_NULL_HANDLE};
};

} // namespace KODI::RENDERING::VULKAN
