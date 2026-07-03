/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanDeviceQueue.h"

#include "VulkanInstance.h"
#include "VulkanRenderSystem.h"
#include "utils/log.h"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

CVulkanDeviceQueue::CVulkanDeviceQueue(CVulkanRenderSystem* vulkanRenderSystem)
  : m_vulkanRenderSystem(vulkanRenderSystem),
    m_vkInstance(vulkanRenderSystem->GetVulkanInstance()->GetVkInstance())
{
}

CVulkanDeviceQueue::~CVulkanDeviceQueue()
{
  Deinitialize();
}

bool CVulkanDeviceQueue::Initialize(DeviceQueueOptions options,
                                    uint32_t gpuVendorId,
                                    uint32_t gpuDeviceId,
                                    const std::vector<const char*>& requiredExtensions,
                                    const std::vector<const char*>& optionalExtensions,
                                    VkDeviceSize heapMemoryLimit,
                                    bool allowProtectedMemory,
                                    bool isThreadSafe)
{
  VkResult result = VK_SUCCESS;

  if (m_vkInstance == VK_NULL_HANDLE)
    return false;

  const CVulkanInfo& info = m_vulkanRenderSystem->GetVulkanInstance()->GetVulkanInfo();

  m_allowProtectedMemory = allowProtectedMemory;

  VkQueueFlags queueFlags = 0;
  if (options & DeviceQueueOption::GRAPHICS_QUEUE_FLAG)
  {
    queueFlags |= VK_QUEUE_GRAPHICS_BIT;
  }
  if (m_allowProtectedMemory)
  {
    queueFlags |= VK_QUEUE_PROTECTED_BIT;
  }

  // We prefer to use discrete GPU, integrated GPU is the second, and then
  // others.
  static constexpr auto kDeviceTypeScores = std::to_array<int>({
      0, // m_vkPhysicalDeviceTYPE_OTHER
      3, // m_vkPhysicalDeviceTYPE_INTEGRATED_GPU
      4, // m_vkPhysicalDeviceTYPE_DISCRETE_GPU
      2, // m_vkPhysicalDeviceTYPE_VIRTUAL_GPU
      1, // m_vkPhysicalDeviceTYPE_CPU
  });

  int deviceIndex = -1;
  int queueIndex = -1;
  int deviceScore = -1;
  for (size_t i = 0; i < info.physicalDevices.size(); ++i)
  {
    const auto& deviceInfo = info.physicalDevices[i];
    const auto& deviceProperties = deviceInfo.properties;
    if (deviceProperties.apiVersion < info.usedAPIVersion)
      continue;

    // In dual-CPU cases, we cannot detect the active GPU correctly on Linux,
    // so don't select GPU device based on the |gpu_info|.
#if !defined(TARGET_LINUX)
    // If gpu info is provided, the device should match it.
    if (gpuVendorId != 0 && gpuDeviceId != 0 &&
        (deviceProperties.vendorID != gpuVendorId || deviceProperties.deviceID != gpuDeviceId))
    {
      continue;
    }
#endif

    if (deviceProperties.deviceType < 0 ||
        deviceProperties.deviceType > VK_PHYSICAL_DEVICE_TYPE_CPU)
    {
      CLog::Log(LOGERROR, "Vulkan: Unsupported device type: {0}", deviceProperties.deviceType);
      continue;
    }

    const VkPhysicalDevice& device = deviceInfo.device;
    bool found = false;
    for (size_t n = 0; n < deviceInfo.queueFamilies.size(); ++n)
    {
      if ((deviceInfo.queueFamilies[n].queueFlags & queueFlags) != queueFlags)
      {
        continue;
      }

      if (options & DeviceQueueOption::PRESENTATION_SUPPORT_QUEUE_FLAG &&
          !m_vulkanRenderSystem->GetPhysicalDevicePresentationSupport(device,
                                                                      deviceInfo.queueFamilies, n))
      {
        continue;
      }

      if (kDeviceTypeScores[deviceProperties.deviceType] > deviceScore)
      {
        deviceIndex = i;
        queueIndex = static_cast<int>(n);
        deviceScore = kDeviceTypeScores[deviceProperties.deviceType];
        found = true;
        break;
      }
    }

    if (!found)
      continue;

    // Use the device, if it matches gpu info.
    if (gpuVendorId != 0 && gpuDeviceId != 0)
      break;

    // If the device is a discrete GPU, we will use it. Otherwise go through
    // all the devices and find the device with the highest score.
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
      break;
  }

  if (deviceIndex == -1)
  {
    CLog::Log(LOGERROR, "Vulkan: Cannot find capable device");
    return false;
  }

  const auto& physicalDeviceInfo = info.physicalDevices[deviceIndex];
  m_vkPhysicalDevice = physicalDeviceInfo.device;
  m_vkPhysicalDeviceProperties = physicalDeviceInfo.properties;
  m_vkPhysicalDeviceDriverProperties = physicalDeviceInfo.driverProperties;
  m_drmDeviceId = physicalDeviceInfo.drmDeviceId;
  m_vkQueueIndex = static_cast<uint32_t>(queueIndex);

  float queuePriority = 0.0f;
  VkDeviceQueueCreateInfo queueCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .pNext = nullptr,
      .flags = allowProtectedMemory ? VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT : 0u,
      .queueFamilyIndex = m_vkQueueIndex,
      .queueCount = 1,
      .pQueuePriorities = &queuePriority,
  };

  std::vector<const char*> enabledExtensions;
  for (const char* extension : requiredExtensions)
  {
    if (std::ranges::none_of(physicalDeviceInfo.extensions,
                             [extension](const VkExtensionProperties& p)
                             { return std::strcmp(extension, p.extensionName) == 0; }))
    {
      CLog::Log(LOGERROR, "Vulkan: Required Vulkan extension {0} is not supported", extension);
      return false;
    }
    enabledExtensions.push_back(extension);
  }

  for (const char* extension : optionalExtensions)
  {
    if (std::ranges::none_of(physicalDeviceInfo.extensions,
                             [extension](const VkExtensionProperties& p)
                             { return std::strcmp(extension, p.extensionName) == 0; }))
    {
      CLog::Log(LOGERROR, "Vulkan: Optional Vulkan extension {0} is not supported", extension);
    }
    else
    {
      enabledExtensions.push_back(extension);
    }
  }

  // Disable all physical device features by default.
  // NOTE: By set of REQUIRED_VK_API_VERSION to support higher Vulkan API version, on ".pNext" value must be e.g.
  //       "VkPhysicalDeviceVulkan12Features" instead of "nullptr" and on next version the same on 12.
  //       Currently we supports API 1.1 only.
  // clang-format off
  m_enabledDeviceFeatures2 = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
      .pNext = nullptr,
      .features = {}
  };
  // clang-format on

  if (physicalDeviceInfo.featureSamplerYCBCRconversion)
  {
    // clang-format off
    m_samplerYCBCRConversionFeatures = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES,
        .pNext = nullptr,
        .samplerYcbcrConversion = VK_TRUE
    };
    // clang-format on

    // Add VkPhysicalDeviceSamplerYcbcrConversionFeatures struct to pNext chain
    // of VkPhysicalDeviceFeatures2 to enable YCbCr sampler support.
    m_enabledDeviceFeatures2.pNext = &m_samplerYCBCRConversionFeatures;
  }

  if (allowProtectedMemory)
  {
    if (!physicalDeviceInfo.featureProtectedMemory)
    {
      CLog::Log(LOGFATAL, "Vulkan: Protected memory is not supported. Vulkan is unavailable.");
      return false;
    }

    // clang-format off
    m_protectedMemoryFeatures = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES,
        .pNext = nullptr,
        .protectedMemory = VK_TRUE
    };
    // clang-format on

    // Add VkPhysicalDeviceProtectedMemoryFeatures struct to pNext chain
    // of VkPhysicalDeviceFeatures2 to enable YCbCr sampler support.
    m_protectedMemoryFeatures.pNext = m_enabledDeviceFeatures2.pNext;
    m_enabledDeviceFeatures2.pNext = &m_protectedMemoryFeatures;
  }

  // Query the physical device features.
  vkGetPhysicalDeviceFeatures2(m_vkPhysicalDevice, &m_enabledDeviceFeatures2);

  // clang-format off
  VkDeviceCreateInfo device_create_info = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .queueCreateInfoCount = 1,
    .pQueueCreateInfos = &queueCreateInfo,
    .enabledLayerCount = 0,
    .ppEnabledLayerNames = nullptr,
    .enabledExtensionCount = enabledExtensions.size(),
    .ppEnabledExtensionNames = enabledExtensions.data(),
    .pEnabledFeatures = &m_enabledDeviceFeatures2.features
  };
  // clang-format on

  result = vkCreateDevice(m_vkPhysicalDevice, &device_create_info, nullptr, &m_ownedVkDevice);
  if (VK_SUCCESS != result)
  {
    CLog::Log(LOGERROR, "Vulkan: vkCreateDevice failed. result: {0}", result);
    return false;
  }

  m_vkDevice = m_ownedVkDevice;

  if (m_allowProtectedMemory)
  {
    VkDeviceQueueInfo2 queueInfo2 = {};
    queueInfo2.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
    queueInfo2.flags = VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT;
    queueInfo2.queueFamilyIndex = queueIndex;
    queueInfo2.queueIndex = 0;
    vkGetDeviceQueue2(m_vkDevice, &queueInfo2, &m_vkQueue);
  }
  else
  {
    vkGetDeviceQueue(m_vkDevice, queueIndex, 0, &m_vkQueue);
  }

  // clang-format off
  VmaVulkanFunctions vmaVulkanFunc = {};
  vmaVulkanFunc.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
  vmaVulkanFunc.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

  VmaAllocatorCreateInfo allocatorInfo =
  {
    .flags = 0,
    .physicalDevice = m_vkPhysicalDevice,
    .device = m_vkDevice,
    .preferredLargeHeapBlockSize = heapMemoryLimit ? heapMemoryLimit : 0,
    .pAllocationCallbacks = nullptr,
    .pDeviceMemoryCallbacks = nullptr,
    .pHeapSizeLimit = nullptr,
    .pVulkanFunctions = &vmaVulkanFunc,
    .instance = m_vkInstance,
    .vulkanApiVersion = info.usedAPIVersion,
#if VMA_EXTERNAL_MEMORY
    .pTypeExternalMemoryHandleTypes = nullptr
#endif
  };
  // clang-format on

  result = vmaCreateAllocator(&allocatorInfo, &m_vmaAllocator);
  if (result != VK_SUCCESS)
  {
    vkDestroyDevice(m_ownedVkDevice, nullptr);
    m_ownedVkDevice = VK_NULL_HANDLE;
    CLog::Log(LOGERROR, "Vulkan: Could not create allocator for VMA allocator");
    return false;
  }

  return true;
}

void CVulkanDeviceQueue::Deinitialize()
{
  if (m_vmaAllocator != VK_NULL_HANDLE)
  {
    vmaDestroyAllocator(m_vmaAllocator);
    m_vmaAllocator = VK_NULL_HANDLE;
  }
  if (m_vkDevice != VK_NULL_HANDLE)
  {
    vkDeviceWaitIdle(m_vkDevice);
  }
  if (m_ownedVkDevice != VK_NULL_HANDLE)
  {
    vkDestroyDevice(m_ownedVkDevice, nullptr);
    m_ownedVkDevice = VK_NULL_HANDLE;
    m_vkDevice = VK_NULL_HANDLE;
  }

  m_vkQueue = VK_NULL_HANDLE;
  m_vkQueueIndex = 0;
  m_vkPhysicalDevice = VK_NULL_HANDLE;
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
