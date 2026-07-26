/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "rendering/vulkan/VulkanData.h"

#include <memory>
#include <vector>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

namespace KODI::RENDERING::VULKAN
{

class CVulkanCommandPool;
class CVulkanFenceHelper;
class CVulkanRenderSystem;
class CVulkanMemoryBuffer;

enum DeviceQueueOption : uint32_t
{
  GRAPHICS_QUEUE_FLAG = 0x01,
  PRESENTATION_SUPPORT_QUEUE_FLAG = 0x02,
};
using DeviceQueueOptions = uint32_t;

class CVulkanDeviceQueue
{
public:
  explicit CVulkanDeviceQueue(CVulkanRenderSystem* vulkanRenderSystem);
  ~CVulkanDeviceQueue();

  bool Initialize(DeviceQueueOptions options,
                  uint32_t gpuVendorId,
                  uint32_t gpuDeviceId,
                  const std::vector<const char*>& requiredExtensions,
                  const std::vector<const char*>& optionalExtensions,
                  VkDeviceSize heapMemoryLimit,
                  bool allowProtectedMemory,
                  bool isThreadSafe);
  void Destroy();

  bool SupportsExtension(const char* extension) const;
  bool SupportsFormat(VkFormat format) const;

  std::unique_ptr<CVulkanCommandPool> CreateCommandPool();

  VkInstance vkInstance() const { return m_vkInstance; }
  VkDevice vkDevice() const { return m_vkDevice; }
  VkPhysicalDevice vkPhysicalDevice() const { return m_vkPhysicalDevice; }
  const VkPhysicalDeviceProperties& vkPhysicalDeviceProperties() const
  {
    return m_vkPhysicalDeviceProperties;
  }
  const VkPhysicalDeviceDriverProperties& vkPhysicalDeviceDriverProperties() const
  {
    return m_vkPhysicalDeviceDriverProperties;
  }
  const VkPhysicalDeviceMemoryProperties& vkPhysicalDeviceMemoryProperties() const
  {
    return m_vkPhysicalDeviceMemoryProperties;
  }
  VkQueue vkQueue() const { return m_vkQueue; }
  int32_t VulkanQueueIndex() const { return m_vkQueueIndex; }
  bool AllowProtectedMemory() const { return m_allowProtectedMemory; }
  VmaAllocator VMAAllocator() const { return m_vmaAllocator; }
  const VkPhysicalDeviceFeatures2& EnabledDeviceFeatures2() const
  {
    return m_vkEnabledDeviceFeatures2;
  }
  const VkPhysicalDeviceFeatures& EnabledDeviceFeatures() const
  {
    return m_vkEnabledDeviceFeatures2.features;
  }
  const VkPhysicalDeviceLimits& DeviceLimits() const { return m_vkPhysicalDeviceProperties.limits; }
  CVulkanFenceHelper* FenceHelper() const { return m_cleanupHelper.get(); }
  CVulkanCommandPool* CommandPool() const { return m_commandPool.get(); }

  /**
	* Get the index of a memory type that has all the requested property bits set
	*
	* @param[in] typeBits Bit mask with bits set for each memory type supported by the resource to request for (from VkMemoryRequirements)
	* @param[in] properties Bit mask of properties for the memory type to request
	* @param[out] memTypeFound (Optional) Pointer to a bool that is set to true if a matching memory type has been found
	*
	* @return Index of the requested memory type
	*
	* @throw Throws an exception if memTypeFound is null and no memory type could be found that supports the requested properties
	*/
  uint32_t GetMemoryType(uint32_t typeBits,
                         VkMemoryPropertyFlags properties,
                         VkBool32* memTypeFound = nullptr) const;


  VkCommandBuffer CreateCommandBuffer(VkCommandBufferLevel level, bool begin = false);
  VkCommandBuffer CreateCommandBuffer(VkCommandBufferLevel level,
                                      VkCommandPool pool,
                                      bool begin = false);
  void FlushCommandBuffer(VkCommandBuffer commandBuffer, bool free = true);
  void FlushCommandBuffer(VkCommandBuffer commandBuffer,
                          VkCommandPool pool,
                          VkQueue queue,
                          bool free = true);

  /**
   * @brief Creates a Vulkan buffer and allocates memory for it.
   *
   * @param[in] usageFlags The usage flags for the buffer.
   * @param[in] memoryPropertyFlags The memory property flags for the buffer.
   * @param[out] memoryData The Vulkan memory data structure, see @ref VulkanMemoryData.
   * @param[in] size The size of the buffer.
   * @param[in] data Optional pointer to the data to initialize the buffer with.
   *                 Leaves the buffer uninitialized if nullptr.
   * @return @ref VkResult indicating success or failure.
   */
  VkResult CreateBuffer(VkBufferUsageFlags usageFlags,
                        VkMemoryPropertyFlags memoryPropertyFlags,
                        VulkanMemoryData* memoryData,
                        VkDeviceSize size,
                        const void* data = nullptr);

  /**
   * @brief Destroys a Vulkan buffer and frees its associated memory.
   *
   * @param[in,out] memoryData The Vulkan memory data structure, see @ref VulkanMemoryData.
   */
  void DestroyBuffer(VulkanMemoryData* memoryData);

  /**
   * @brief Copies data from one Vulkan buffer to another.
   *
   * @param[in] src The source Vulkan memory data structure, see @ref VulkanMemoryData.
   * @param[in] dst The destination Vulkan memory data structure, see @ref VulkanMemoryData.
   * @param[in] copyRegion Optional pointer to a VkBufferCopy structure defining the region to copy.
   *                       If nullptr, the entire buffer will be copied.
   */
  void CopyBuffer(VulkanMemoryData* src, VulkanMemoryData* dst, VkBufferCopy* copyRegion = nullptr);

  /**
   * @brief Copies data from one Vulkan buffer to another using a specific command pool and queue.
   *
   * @param[in] src The source Vulkan memory data structure, see @ref VulkanMemoryData.
   * @param[in] dst The destination Vulkan memory data structure, see @ref VulkanMemoryData.
   * @param[in] commandPool The Vulkan command pool to use for the copy operation.
   * @param[in] queue The Vulkan queue to submit the copy command buffer to.
   * @param[in] copyRegion Optional pointer to a VkBufferCopy structure defining the region to copy.
   *                       If nullptr, the entire buffer will be copied.
   */
  void CopyBuffer(VulkanMemoryData* src,
                  VulkanMemoryData* dst,
                  VkCommandPool commandPool,
                  VkQueue queue,
                  VkBufferCopy* copyRegion = nullptr);

  /**
   * @brief Copies data from multiple Vulkan buffers to their corresponding destination buffers.
   *
   * This function available to copy multiple buffers in a single command buffer submission, which can
   * improve performance by reducing the number of command buffer submissions.
   *
   * @param[in] srcDstPairs A vector of pairs of source and destination Vulkan memory data structures.
   */
  void CopyBuffers(const std::vector<std::pair<VulkanMemoryData*, VulkanMemoryData*>>& srcDstPairs);

  /**
   * @brief Copies data from multiple Vulkan buffers to their corresponding destination buffers using
   * a specific command pool and queue.
   *
   * This function available to copy multiple buffers in a single command buffer submission, which can
   * improve performance by reducing the number of command buffer submissions.
   *
   * @param[in] srcDstPairs A vector of pairs of source and destination Vulkan memory data structures.
   * @param[in] commandPool The Vulkan command pool to use for the copy operation.
   * @param[in] queue The Vulkan queue to submit the copy command buffer to.
   */
  void CopyBuffers(const std::vector<std::pair<VulkanMemoryData*, VulkanMemoryData*>>& srcDstPairs,
                   VkCommandPool commandPool,
                   VkQueue queue);

  VkResult Map(VulkanMemoryData* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  void Unmap(VulkanMemoryData* data);

private:
  CVulkanDeviceQueue(const CVulkanDeviceQueue&) = delete;
  CVulkanDeviceQueue& operator=(const CVulkanDeviceQueue&) = delete;

  CVulkanRenderSystem* m_vulkanRenderSystem{nullptr};

  VkInstance m_vkInstance{VK_NULL_HANDLE};
  VkDevice m_vkDevice{VK_NULL_HANDLE};
  VkQueue m_vkQueue{VK_NULL_HANDLE};
  int32_t m_vkQueueIndex{-1};
  VkPhysicalDevice m_vkPhysicalDevice{VK_NULL_HANDLE};
  VkPhysicalDeviceProperties m_vkPhysicalDeviceProperties{};
  VkPhysicalDeviceDriverProperties m_vkPhysicalDeviceDriverProperties{};
  VkPhysicalDeviceMemoryProperties m_vkPhysicalDeviceMemoryProperties{};
  VkPhysicalDeviceFeatures2 m_vkEnabledDeviceFeatures2{};
  VkPhysicalDeviceProtectedMemoryFeatures m_vkProtectedMemoryFeatures{};
#if defined(TARGET_ANDROID) || defined(TARGET_LINUX)
  VkPhysicalDeviceSamplerYcbcrConversionFeatures m_vkSamplerYCBCRConversionFeatures{};
#endif // defined(TARGET_ANDROID) || defined(TARGET_LINUX)
  VmaAllocator m_vmaAllocator{VK_NULL_HANDLE};

  std::vector<const char*> m_enabledExtensions;
  std::unique_ptr<CVulkanFenceHelper> m_cleanupHelper;
  std::unique_ptr<CVulkanCommandPool> m_commandPool;

  bool m_allowProtectedMemory{false};
  uint32_t m_drmDeviceId{0};
};

} // namespace KODI::RENDERING::VULKAN
