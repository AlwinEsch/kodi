/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <memory>
#include <vector>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

class CVulkanCommandPool;
class CVulkanFenceHelper;
class CVulkanRenderSystem;

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

  std::unique_ptr<CVulkanCommandPool> CreateCommandPool();

  VkInstance VulkanInstance() const { return m_vkInstance; }
  VkDevice VulkanDevice() const { return m_vkDevice; }
  VkPhysicalDevice VulkanPhysicalDevice() const { return m_vkPhysicalDevice; }
  const VkPhysicalDeviceProperties& VulkanPhysicalDeviceProperties() const
  {
    return m_vkPhysicalDeviceProperties;
  }
  const VkPhysicalDeviceDriverProperties& VulkanPhysicalDeviceDriverProperties() const
  {
    return m_vkPhysicalDeviceDriverProperties;
  }
  VkQueue VulkanQueue() const { return m_vkQueue; }
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
  CVulkanFenceHelper* FenceHelper() const { return m_cleanupHelper.get(); }

private:
  CVulkanDeviceQueue(const CVulkanDeviceQueue&) = delete;
  CVulkanDeviceQueue& operator=(const CVulkanDeviceQueue&) = delete;

  std::vector<const char*> m_enabledExtensions;
  std::unique_ptr<CVulkanFenceHelper> m_cleanupHelper;

  VkInstance m_vkInstance{VK_NULL_HANDLE};
  VkDevice m_vkDevice{VK_NULL_HANDLE};
  VkQueue m_vkQueue{VK_NULL_HANDLE};
  int32_t m_vkQueueIndex{-1};
  VkPhysicalDevice m_vkPhysicalDevice{VK_NULL_HANDLE};
  VkPhysicalDeviceProperties m_vkPhysicalDeviceProperties{};
  VkPhysicalDeviceDriverProperties m_vkPhysicalDeviceDriverProperties{};
  VkPhysicalDeviceFeatures2 m_vkEnabledDeviceFeatures2{};
  VkPhysicalDeviceProtectedMemoryFeatures m_vkProtectedMemoryFeatures{};
#if defined(TARGET_ANDROID) || defined(TARGET_LINUX)
  VkPhysicalDeviceSamplerYcbcrConversionFeatures m_vkSamplerYCBCRConversionFeatures{};
#endif // defined(TARGET_ANDROID) || defined(TARGET_LINUX)
  VmaAllocator m_vmaAllocator{VK_NULL_HANDLE};

  CVulkanRenderSystem* m_vulkanRenderSystem{nullptr};
  bool m_allowProtectedMemory{false};
  uint32_t m_drmDeviceId{0};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
