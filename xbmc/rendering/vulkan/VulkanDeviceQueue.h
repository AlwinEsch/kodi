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

class CVulkanRenderSystem;
class CVulkanCommandPool;

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
  void Deinitialize();

  bool SupportsExtension(const char* extension) const;

  std::unique_ptr<CVulkanCommandPool> CreateCommandPool();

  VkInstance GetVulkanInstance() const { return m_vkInstance; }
  VkDevice GetVulkanDevice() const { return m_vkDevice; }
  VkPhysicalDevice GetVulkanPhysicalDevice() const { return m_vkPhysicalDevice; }
  const VkPhysicalDeviceProperties& GetVulkanPhysicalDeviceProperties() const
  {
    return m_vkPhysicalDeviceProperties;
  }
  const VkPhysicalDeviceDriverProperties& GetVulkanPhysicalDeviceDriverProperties() const
  {
    return m_vkPhysicalDeviceDriverProperties;
  }
  VkQueue GetVulkanQueue() const { return m_vkQueue; }
  uint32_t GetVulkanQueueIndex() const { return m_vkQueueIndex; }
  bool AllowProtectedMemory() const { return m_allowProtectedMemory; }
  VmaAllocator GetVMAAllocator() const { return m_vmaAllocator; }
  const VkPhysicalDeviceFeatures2& GetEnabledDeviceFeatures2() const
  {
    return m_enabledDeviceFeatures2;
  }
  const VkPhysicalDeviceFeatures& GetEnabledDeviceFeatures() const
  {
    return m_enabledDeviceFeatures2.features;
  }

private:
  CVulkanDeviceQueue(const CVulkanDeviceQueue&) = delete;
  CVulkanDeviceQueue& operator=(const CVulkanDeviceQueue&) = delete;

  std::vector<const char*> m_enabledExtensions;

  VkInstance m_vkInstance{VK_NULL_HANDLE};
  VkDevice m_ownedVkDevice{VK_NULL_HANDLE};
  VkDevice m_vkDevice{VK_NULL_HANDLE};
  VkQueue m_vkQueue{VK_NULL_HANDLE};
  uint32_t m_vkQueueIndex{0};
  VkPhysicalDevice m_vkPhysicalDevice{VK_NULL_HANDLE};
  VkPhysicalDeviceProperties m_vkPhysicalDeviceProperties{};
  VkPhysicalDeviceDriverProperties m_vkPhysicalDeviceDriverProperties{};
  VkPhysicalDeviceFeatures2 m_enabledDeviceFeatures2{};
  VkPhysicalDeviceProtectedMemoryFeatures m_protectedMemoryFeatures{};
#if defined(TARGET_ANDROID) || defined(TARGET_LINUX)
  VkPhysicalDeviceSamplerYcbcrConversionFeatures m_samplerYCBCRConversionFeatures{};
#endif // defined(TARGET_ANDROID) || defined(TARGET_LINUX)
  VmaAllocator m_vmaAllocator{VK_NULL_HANDLE};

  CVulkanRenderSystem* m_vulkanRenderSystem{nullptr};
  bool m_allowProtectedMemory{false};
  uint32_t m_drmDeviceId{0};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
