/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanDeviceQueue.h"

#include "rendering/vulkan/VulkanCommandPool.h"
#include "rendering/vulkan/VulkanDeviceQueue.h"
#include "rendering/vulkan/VulkanFenceHelper.h"
#include "rendering/vulkan/VulkanInstance.h"
#include "rendering/vulkan/VulkanRenderSystem.h"
#include "rendering/vulkan/utils/VulkanInitStructs.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "utils/log.h"

#include <cassert>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

using namespace KODI::RENDERING::VULKAN::UTILS;

namespace KODI::RENDERING::VULKAN
{

using KODI::RENDERING::VULKAN::UTILS::ErrorString;

CVulkanDeviceQueue::CVulkanDeviceQueue(CVulkanRenderSystem* vulkanRenderSystem)
  : m_vulkanRenderSystem(vulkanRenderSystem),
    m_vkInstance(vulkanRenderSystem->GetVulkanInstance()->GetVkInstance())

{
  assert(m_vkInstance);
}

CVulkanDeviceQueue::~CVulkanDeviceQueue()
{
  Destroy();
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
  CLog::Log(LOGINFO, "Vulkan: Initializing vulkan device.");

  VkResult result;

  m_allowProtectedMemory = allowProtectedMemory;
  m_enabledExtensions.clear();

  const CVulkanInfo& info = m_vulkanRenderSystem->GetVulkanInstance()->GetVulkanInfo();

  VkQueueFlags queueFlags = 0;
  if (options & DeviceQueueOption::GRAPHICS_QUEUE_FLAG) [[likely]]
  {
    queueFlags |= VK_QUEUE_GRAPHICS_BIT;
  }
  if (m_allowProtectedMemory) [[likely]]
  {
    queueFlags |= VK_QUEUE_PROTECTED_BIT;
  }

  // We prefer to use discrete GPU, integrated GPU is the second, and then
  // others.
  static constexpr auto kDeviceTypeScores = std::to_array<int>({
      0, // VK_PHYSICAL_DEVICE_TYPE_OTHER
      3, // VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
      4, // VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
      2, // VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU
      1, // VK_PHYSICAL_DEVICE_TYPE_CPU
  });

  int deviceIndex = -1;
  int queueIndex = -1;
  int deviceScore = -1;
  for (size_t i = 0; i < info.physicalDevices.size(); ++i)
  {
    const auto& deviceInfo = info.physicalDevices[i];
    const auto& deviceProperties = deviceInfo.properties;
    if (deviceProperties.apiVersion < info.usedAPIVersion) [[likely]]
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
        deviceProperties.deviceType > VK_PHYSICAL_DEVICE_TYPE_CPU) [[unlikely]]
    {
      CLog::Log(LOGERROR, "Vulkan: Unsupported device type: {0}", deviceProperties.deviceType);
      continue;
    }

    const VkPhysicalDevice& device = deviceInfo.device;
    bool found = false;
    for (size_t n = 0; n < deviceInfo.queueFamilies.size(); ++n)
    {
      if ((deviceInfo.queueFamilies[n].queueFlags & queueFlags) != queueFlags) [[likely]]
      {
        continue;
      }

      if (options & DeviceQueueOption::PRESENTATION_SUPPORT_QUEUE_FLAG &&
          !m_vulkanRenderSystem->GetPhysicalDevicePresentationSupport(
              device, deviceInfo.queueFamilies, n)) [[likely]]
      {
        continue;
      }

      if (kDeviceTypeScores[deviceProperties.deviceType] > deviceScore) [[likely]]
      {
        deviceIndex = i;
        queueIndex = static_cast<int>(n);
        deviceScore = kDeviceTypeScores[deviceProperties.deviceType];
        found = true;
        break;
      }
    }

    if (!found) [[likely]]
      continue;

    // Use the device, if it matches gpu info.
    if (gpuVendorId != 0 && gpuDeviceId != 0) [[likely]]
      break;

    // If the device is a discrete GPU, we will use it. Otherwise go through
    // all the devices and find the device with the highest score.
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) [[likely]]
      break;
  }

  if (deviceIndex < 0) [[unlikely]]
  {
    CLog::Log(LOGFATAL, "Vulkan: Did not find suitable device with a queue that supports graphics "
                        "and presentation.");
    return false;
  }

  m_vulkanRenderSystem->GetVulkanInstance()->SetUsedPhysicalDeviceIndex(deviceIndex);

  const auto& physicalDeviceInfo = info.physicalDevices[deviceIndex];
  m_vkPhysicalDevice = physicalDeviceInfo.device;
  m_vkPhysicalDeviceProperties = physicalDeviceInfo.properties;
  m_vkPhysicalDeviceDriverProperties = physicalDeviceInfo.driverProperties;
  m_drmDeviceId = physicalDeviceInfo.drmDeviceId;
  m_vkQueueIndex = queueIndex;

  for (const char* extension : requiredExtensions)
  {
    if (!CVulkanInstance::ValidateExtensions(extension, physicalDeviceInfo.extensions)) [[unlikely]]
    {
      CLog::Log(LOGERROR, "Vulkan: Required Vulkan extension {0} is not supported", extension);
      return false;
    }
    m_enabledExtensions.push_back(extension);
  }

  for (const char* extension : optionalExtensions)
  {
    if (!CVulkanInstance::ValidateExtensions(extension, physicalDeviceInfo.extensions)) [[unlikely]]
    {
      CLog::Log(LOGWARNING, "Vulkan: Optional Vulkan extension {0} is not supported", extension);
    }
    else
    {
      m_enabledExtensions.push_back(extension);
    }
  }

  // Disable all physical device features by default.
  // NOTE: By set of REQUIRED_VK_API_VERSION to support higher Vulkan API version, on ".pNext" value must be e.g.
  //       "VkPhysicalDeviceVulkan12Features" instead of "nullptr" and on next version the same on 12.
  //       Currently we supports API 1.1 only.
  m_vkEnabledDeviceFeatures2 = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
      .pNext = nullptr,
      .features = {},
  };

  if (physicalDeviceInfo.featureSamplerYCBCRconversion) [[likely]]
  {
    m_vkSamplerYCBCRConversionFeatures = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES,
        .pNext = nullptr,
        .samplerYcbcrConversion = VK_TRUE,
    };

    // Add VkPhysicalDeviceSamplerYcbcrConversionFeatures struct to pNext chain
    // of VkPhysicalDeviceFeatures2 to enable YCbCr sampler support.
    m_vkEnabledDeviceFeatures2.pNext = &m_vkSamplerYCBCRConversionFeatures;
  }

  if (allowProtectedMemory) [[likely]]
  {
    if (!physicalDeviceInfo.featureProtectedMemory) [[unlikely]]
    {
      CLog::Log(LOGFATAL, "Vulkan: Protected memory is not supported. Vulkan is unavailable.");
      return false;
    }

    m_vkProtectedMemoryFeatures = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES,
        .pNext = nullptr,
        .protectedMemory = VK_TRUE,
    };

    // Add VkPhysicalDeviceProtectedMemoryFeatures struct to pNext chain
    // of VkPhysicalDeviceFeatures2 to enable protected memory support.
    m_vkProtectedMemoryFeatures.pNext = m_vkEnabledDeviceFeatures2.pNext;
    m_vkEnabledDeviceFeatures2.pNext = &m_vkProtectedMemoryFeatures;
  }

  // Store Properties features, limits and properties of the physical m_device for later use
  // Device properties also contain limits and sparse properties
  vkGetPhysicalDeviceProperties(m_vkPhysicalDevice, &m_vkPhysicalDeviceProperties);
  // Query the physical device features.
  vkGetPhysicalDeviceFeatures2(m_vkPhysicalDevice, &m_vkEnabledDeviceFeatures2);
  // Memory properties are used regularly for creating all kinds of buffers
  vkGetPhysicalDeviceMemoryProperties(m_vkPhysicalDevice, &m_vkPhysicalDeviceMemoryProperties);

  // The sample uses a single graphics queue
  const float queuePriority = 0.5f;
  VkDeviceQueueCreateInfo vkQueueCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueFamilyIndex = static_cast<uint32_t>(m_vkQueueIndex),
      .queueCount = 1,
      .pQueuePriorities = &queuePriority,
  };

  if (allowProtectedMemory) [[likely]]
  {
    vkQueueCreateInfo.flags |= VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT;
  }

  VkDeviceCreateInfo vkDeviceCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &vkQueueCreateInfo,
      .enabledLayerCount = 0,
      .ppEnabledLayerNames = nullptr,
      .enabledExtensionCount = static_cast<uint32_t>(m_enabledExtensions.size()),
      .ppEnabledExtensionNames = m_enabledExtensions.data(),
      .pEnabledFeatures = &m_vkEnabledDeviceFeatures2.features,
  };

  VK_CHECK_RESULT(vkCreateDevice(m_vkPhysicalDevice, &vkDeviceCreateInfo, nullptr, &m_vkDevice),
                  false);

  if (m_allowProtectedMemory) [[likely]]
  {
    VkDeviceQueueInfo2 vkQueueInfo2 = {};
    vkQueueInfo2.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
    vkQueueInfo2.flags = VK_DEVICE_QUEUE_CREATE_PROTECTED_BIT;
    vkQueueInfo2.queueFamilyIndex = queueIndex;
    vkQueueInfo2.queueIndex = 0;
    vkGetDeviceQueue2(m_vkDevice, &vkQueueInfo2, &m_vkQueue);
  }
  else
  {
    vkGetDeviceQueue(m_vkDevice, queueIndex, 0, &m_vkQueue);
  }

  VmaVulkanFunctions vmaVulkanFunc = {};
  vmaVulkanFunc.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
  vmaVulkanFunc.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

  VmaAllocatorCreateInfo allocatorInfo = {
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
      .pTypeExternalMemoryHandleTypes = nullptr,
#endif
  };

  result = vmaCreateAllocator(&allocatorInfo, &m_vmaAllocator);
  if (result != VK_SUCCESS) [[unlikely]]
  {
    vkDestroyDevice(m_vkDevice, nullptr);
    m_vkDevice = VK_NULL_HANDLE;
    CLog::Log(LOGERROR, "Vulkan: Could not create allocator for VMA allocator. result: {0}",
              ErrorString(result));
    return false;
  }

  m_cleanupHelper = std::make_unique<CVulkanFenceHelper>(this);

  auto commandPool = std::make_unique<CVulkanCommandPool>(this);
  if (!commandPool->Initialize(m_allowProtectedMemory))
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create default command pool ({0}:{1})", __FILENAME__,
              __LINE__);
    commandPool->Destroy();
    return false;
  }
  m_commandPool = std::move(commandPool);

  return true;
}

void CVulkanDeviceQueue::Destroy()
{
  if (m_commandPool)
  {
    m_commandPool->Destroy();
    m_commandPool.reset();
  }
  if (m_cleanupHelper)
  {
    m_cleanupHelper->Destroy();
    m_cleanupHelper.reset();
  }
  if (m_vmaAllocator != VK_NULL_HANDLE)
  {
    vmaDestroyAllocator(m_vmaAllocator);
    m_vmaAllocator = VK_NULL_HANDLE;
  }
  if (m_vkDevice != VK_NULL_HANDLE)
  {
    vkDeviceWaitIdle(m_vkDevice);
  }
  if (m_vkDevice != VK_NULL_HANDLE)
  {
    vkDestroyDevice(m_vkDevice, nullptr);
    m_vkDevice = VK_NULL_HANDLE;
  }

  m_vkQueue = VK_NULL_HANDLE;
  m_vkQueueIndex = 0;
  m_vkPhysicalDevice = VK_NULL_HANDLE;
}

bool CVulkanDeviceQueue::SupportsExtension(const char* extension) const
{
  return std::ranges::any_of(m_enabledExtensions,
                             [extension](const char* p) { return std::strcmp(extension, p) == 0; });
}

bool CVulkanDeviceQueue::SupportsFormat(VkFormat format) const
{
  VkFormatProperties formatProperties{};
  vkGetPhysicalDeviceFormatProperties(m_vkPhysicalDevice, format, &formatProperties);
  if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
  {
    CLog::Log(LOGDEBUG, "Vulkan: Format {} is not supported by device {}", static_cast<int>(format),
              m_vkPhysicalDeviceProperties.deviceName);
    return false;
  }
  return true;
}

std::unique_ptr<CVulkanCommandPool> CVulkanDeviceQueue::CreateCommandPool()
{
  auto commandPool = std::make_unique<CVulkanCommandPool>(this);
  if (!commandPool->Initialize(m_allowProtectedMemory))
    return nullptr;
  return commandPool;
}

uint32_t CVulkanDeviceQueue::GetMemoryType(uint32_t typeBits,
                                           VkMemoryPropertyFlags properties,
                                           VkBool32* memTypeFound) const
{
  for (uint32_t i = 0; i < m_vkPhysicalDeviceMemoryProperties.memoryTypeCount; i++)
  {
    if ((typeBits & 1) == 1)
    {
      if ((m_vkPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) ==
          properties)
      {
        if (memTypeFound)
        {
          *memTypeFound = true;
        }
        return i;
      }
    }
    typeBits >>= 1;
  }

  if (memTypeFound)
  {
    *memTypeFound = false;
    return 0;
  }
  else
  {
    throw std::runtime_error("Could not find a matching memory type");
  }
}

VkCommandBuffer CVulkanDeviceQueue::CreateCommandBuffer(VkCommandBufferLevel level,
                                                        bool begin /* = false*/)
{
  return CreateCommandBuffer(level, m_commandPool->vkCommandPool(), begin);
}

VkCommandBuffer CVulkanDeviceQueue::CreateCommandBuffer(VkCommandBufferLevel level,
                                                        VkCommandPool pool,
                                                        bool begin /* = false*/)
{
  VkCommandBufferAllocateInfo allocInfo = vkCommandBufferAllocateInfo(pool, level, 1);
  VkCommandBuffer command_buffer{};
  VK_CHECK_RESULT(vkAllocateCommandBuffers(m_vkDevice, &allocInfo, &command_buffer),
                  VK_NULL_HANDLE);

  if (begin)
  {
    VkCommandBufferBeginInfo beginInfo = vkCommandBufferBeginInfo();
    VK_CHECK_RESULT(vkBeginCommandBuffer(command_buffer, &beginInfo), VK_NULL_HANDLE);
  }
  return command_buffer;
}

void CVulkanDeviceQueue::FlushCommandBuffer(VkCommandBuffer commandBuffer, bool free /* = true*/)
{
  FlushCommandBuffer(commandBuffer, m_commandPool->vkCommandPool(), m_vkQueue, free);
}

void CVulkanDeviceQueue::FlushCommandBuffer(VkCommandBuffer commandBuffer,
                                            VkCommandPool pool,
                                            VkQueue queue,
                                            bool free /* = true*/)
{
  using KODI::RENDERING::VULKAN::UTILS::DEFAULT_FENCE_TIMEOUT;

  if (commandBuffer == VK_NULL_HANDLE)
    return;

  VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));

  VkSubmitInfo submitInfo = vkSubmitInfo();
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  VkFenceCreateInfo fenceInfo = vkFenceCreateInfo();
  VkFence fence;
  VK_CHECK_RESULT(vkCreateFence(m_vkDevice, &fenceInfo, nullptr, &fence));
  VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, fence));
  VK_CHECK_RESULT(vkWaitForFences(m_vkDevice, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT));
  vkDestroyFence(m_vkDevice, fence, nullptr);
  if (free)
  {
    vkFreeCommandBuffers(m_vkDevice, pool, 1, &commandBuffer);
  }
}

VkResult CVulkanDeviceQueue::CreateBuffer(VkBufferUsageFlags usageFlags,
                                          VkMemoryPropertyFlags memoryPropertyFlags,
                                          VulkanMemoryData* memoryData,
                                          VkDeviceSize size,
                                          const void* data)
{
  // Create the buffer handle
  auto info = vkBufferCreateInfo(usageFlags, size, VK_SHARING_MODE_EXCLUSIVE);
  VK_CHECK_RESULT(vkCreateBuffer(m_vkDevice, &info, nullptr, &memoryData->buffer), res);

  // Create the memory backing up the buffer handle
  VkMemoryRequirements memReqs;
  vkGetBufferMemoryRequirements(m_vkDevice, memoryData->buffer, &memReqs);

  auto allocInfo = vkMemoryAllocateInfo();
  allocInfo.allocationSize = memReqs.size;
  allocInfo.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, memoryPropertyFlags);

  VkMemoryAllocateFlagsInfo allocFlagsInfo{};
  if (usageFlags & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
  {
    allocFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
    allocFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;
    allocInfo.pNext = &allocFlagsInfo;
  }
  VK_CHECK_RESULT(vkAllocateMemory(m_vkDevice, &allocInfo, nullptr, &memoryData->memory), res);

  // If a pointer to the buffer data has been passed, map the buffer and copy over the data
  if (data != nullptr)
  {
    void* mapped;
    VK_CHECK_RESULT(vkMapMemory(m_vkDevice, memoryData->memory, 0, size, 0, &mapped), res);
    memcpy(mapped, data, size);
    // If host coherency hasn't been requested, do a manual flush to make writes visible
    if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
    {
      VkMappedMemoryRange mappedRange{
          .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
          .pNext = nullptr,
          .memory = memoryData->memory,
          .offset = 0,
          .size = size,
      };
      vkFlushMappedMemoryRanges(m_vkDevice, 1, &mappedRange);
    }
    vkUnmapMemory(m_vkDevice, memoryData->memory);
  }

  // Attach the memory to the buffer object
  VK_CHECK_RESULT(vkBindBufferMemory(m_vkDevice, memoryData->buffer, memoryData->memory, 0), res);

  memoryData->size = size;
  memoryData->offset = 0;

  return VK_SUCCESS;
}

void CVulkanDeviceQueue::DestroyBuffer(VulkanMemoryData* memoryData)
{
  if (memoryData->mapped)
  {
    vkUnmapMemory(m_vkDevice, memoryData->memory);
    memoryData->mapped = nullptr;
  }
  if (memoryData->descriptorSet != VK_NULL_HANDLE)
  {
    vkFreeDescriptorSets(m_vkDevice, m_vulkanRenderSystem->vkData()->vkDescriptorPool, 1,
                         &memoryData->descriptorSet);
    memoryData->descriptorSet = VK_NULL_HANDLE;
  }
  if (memoryData->buffer)
  {
    vkDestroyBuffer(m_vkDevice, memoryData->buffer, nullptr);
    memoryData->buffer = VK_NULL_HANDLE;
  }
  if (memoryData->memory)
  {
    vkFreeMemory(m_vkDevice, memoryData->memory, nullptr);
    memoryData->memory = VK_NULL_HANDLE;
  }
}

void CVulkanDeviceQueue::CopyBuffer(VulkanMemoryData* src,
                                    VulkanMemoryData* dst,
                                    VkBufferCopy* copyRegion)
{
  CopyBuffer(src, dst, m_commandPool->vkCommandPool(), m_vkQueue, copyRegion);
}

void CVulkanDeviceQueue::CopyBuffer(VulkanMemoryData* src,
                                    VulkanMemoryData* dst,
                                    VkCommandPool commandPool,
                                    VkQueue queue,
                                    VkBufferCopy* copyRegion)
{
  assert(dst->size >= src->size);
  assert(src->buffer);
  VkCommandBuffer copyCmd = CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, commandPool, true);
  VkBufferCopy bufferCopy{};
  if (copyRegion == nullptr)
  {
    bufferCopy.size = src->size;
  }
  else
  {
    bufferCopy = *copyRegion;
  }

  vkCmdCopyBuffer(copyCmd, src->buffer, dst->buffer, 1, &bufferCopy);

  FlushCommandBuffer(copyCmd, commandPool, queue, true);
}

void CVulkanDeviceQueue::CopyBuffers(
    const std::vector<std::pair<VulkanMemoryData*, VulkanMemoryData*>>& srcDstPairs)
{
  CopyBuffers(srcDstPairs, m_commandPool->vkCommandPool(), m_vkQueue);
}

void CVulkanDeviceQueue::CopyBuffers(
    const std::vector<std::pair<VulkanMemoryData*, VulkanMemoryData*>>& srcDstPairs,
    VkCommandPool commandPool,
    VkQueue queue)
{
  VkCommandBuffer copyCmd = CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, commandPool, true);

  for (const auto& pair : srcDstPairs)
  {
    VulkanMemoryData* src = pair.first;
    VulkanMemoryData* dst = pair.second;
    assert(dst->size >= src->size);
    assert(src->buffer);

    VkBufferCopy bufferCopy{};
    bufferCopy.size = src->size;
    vkCmdCopyBuffer(copyCmd, src->buffer, dst->buffer, 1, &bufferCopy);
  }

  FlushCommandBuffer(copyCmd, commandPool, queue, true);
}

VkResult CVulkanDeviceQueue::Map(VulkanMemoryData* data, VkDeviceSize size, VkDeviceSize offset)
{
  return vkMapMemory(m_vkDevice, data->memory, offset, size, 0, &data->mapped);
}

void CVulkanDeviceQueue::Unmap(VulkanMemoryData* data)
{
  if (data->mapped)
  {
    vkUnmapMemory(m_vkDevice, data->memory);
    data->mapped = nullptr;
  }
}

} // namespace KODI::RENDERING::VULKAN
