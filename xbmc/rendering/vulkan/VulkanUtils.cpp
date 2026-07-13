/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanUtils.h"

#include "VulkanInfo.h"
#include "filesystem/File.h"
#include "filesystem/SpecialProtocol.h"
#include "utils/StringUtils.h"
#include "utils/log.h"

#include <stdexcept>

namespace
{

/**
 * @brief Finds a suitable memory type based on the given filter and properties.
 *
 * @param vk_instance The Vulkan instance.
 * @param typeFilter The filter for the memory type.
 * @param properties The required memory properties.
 * @return The index of the found memory type, or 0 if not found.
 *
 * @note This function is only used internally and should not be called directly. Use FindMemoryType instead.
 */
uint32_t internalFindMemoryType(VkInstance vk_instance,
                                VkPhysicalDevice physicalDevice,
                                int32_t typeFilter,
                                VkMemoryPropertyFlags properties)
{
  // Fall back to first available physical device if none is provided
  if (physicalDevice == VK_NULL_HANDLE)
  {
    uint32_t physical_devices_number = 1;
    VkResult result =
        vkEnumeratePhysicalDevices(vk_instance, &physical_devices_number, &physicalDevice);
    if (result != VK_SUCCESS)
    {

      CLog::Log(LOGERROR, "Vulkan: Failed to enumerate physical devices (Error code: {0})", result);
      return 0;
    }
  }

  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

  // Iterate over all memory types available on the physical device
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
  {
    // Check if the current memory type is acceptable based on the type_filter
    // The type_filter is a bitmask where each bit represents a memory type that is suitable
    if (typeFilter & (1 << i))
    {
      // Check if the memory type has all the desired property flags
      // properties is a bitmask of the required memory properties
      if ((memProperties.memoryTypes[i].propertyFlags & properties) == properties)
      {
        // Found a suitable memory type; return its index
        return i;
      }
    }
  }

  CLog::Log(LOGERROR, "Vulkan: Failed to find suitable memory type");
  return 0;
}

} // namespace

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{
namespace UTILS
{

VkBool32 vulkanErrorCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                             void* pUserData)
{
  if (!pCallbackData)
    return VK_FALSE;

  int logLevel;
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    logLevel = LOGERROR;
  else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    logLevel = LOGWARNING;
  else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    logLevel = LOGDEBUG;
  else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    logLevel = LOGINFO;
  else // For VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT and unknown flags
    logLevel = LOGINFO;

  /*!
   * @remark pLayerPrefix and pMessage should not be nullptr, but we check them
   * just in case to avoid potential crashes.
   */
  CLog::Log(logLevel, "Vulkan: {}: {}",
            pCallbackData->pMessageIdName ? pCallbackData->pMessageIdName : "Unknown",
            pCallbackData->pMessage ? pCallbackData->pMessage : "No message text");
  return VK_FALSE;
}

bool vulkanCreateBuffer(VkInstance vkInstance,
                        VkDevice device,
                        VkPhysicalDevice physicalDevice,
                        VkDeviceSize size,
                        VkBufferUsageFlags usage,
                        VkMemoryPropertyFlags properties,
                        VkBuffer& buffer,
                        VkDeviceMemory& bufferMemory)
{
  // Create the vertex buffer
  VkBufferCreateInfo bufferInfo{
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .size = size,
      .usage = usage,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 0,
      .pQueueFamilyIndices = nullptr,
  };

  if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create buffer (Function: %s)", __func__);
    return false;
  }

  // Get memory requirements
  VkMemoryRequirements memoryRequirements{};
  vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

  // Allocate memory for the buffer
  VkMemoryAllocateInfo alloc_info{
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .pNext = nullptr,
      .allocationSize = memoryRequirements.size,
      .memoryTypeIndex = internalFindMemoryType(vkInstance, physicalDevice,
                                                memoryRequirements.memoryTypeBits, properties),
  };

  if (vkAllocateMemory(device, &alloc_info, nullptr, &bufferMemory) != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to allocate buffer memory (Function: %s)", __func__);
    return false;
  }

  // Bind the buffer with the allocated memory
  if (vkBindBufferMemory(device, buffer, bufferMemory, 0) != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to bind buffer memory (Function: %s)", __func__);
    return false;
  }

  return true;
}

VkShaderModule vulkanCreateShaderModule(VkDevice device, const std::string& filename)
{
  XFILE::CFileStream file;

  std::string path = CSpecialProtocol::TranslatePath(
      KODI::UTILS::StringUtils::Format("special://xbmc/system/shaders/Vulkan/{}", filename));
  if (!file.Open(path))
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to open file {} (Function: {})", path, __func__);
    return nullptr;
  }

  std::vector<uint8_t> spirv;
  if (XFILE::CFile().LoadFile(path, spirv) <= 0)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to load file {} (Function: {})", path, __func__);
    return nullptr;
  }

  VkShaderModuleCreateInfo module_info{.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                                       .pNext = nullptr,
                                       .flags = 0,
                                       .codeSize = spirv.size(),
                                       .pCode = reinterpret_cast<uint32_t*>(spirv.data())};

  VkShaderModule shader_module;
  if (vkCreateShaderModule(device, &module_info, nullptr, &shader_module) != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create shader module (Function: {})", __func__);
    return nullptr;
  }

  return shader_module;
}

void LogGraphicsInfo(const CVulkanInfo& vulkanInfo)
{
  // Log the Vulkan API information
  CLog::Log(LOGINFO, "Vulkan: Logging graphics information...");
  CLog::Log(LOGINFO, "        - Available API Version: {}.{}.{}",
            VK_VERSION_MAJOR(vulkanInfo.availableAPIVersion),
            VK_VERSION_MINOR(vulkanInfo.availableAPIVersion),
            VK_VERSION_PATCH(vulkanInfo.availableAPIVersion));
  CLog::Log(LOGINFO, "        - Used API Version: {0}.{1}.{2}",
            VK_VERSION_MAJOR(vulkanInfo.usedAPIVersion),
            VK_VERSION_MINOR(vulkanInfo.usedAPIVersion),
            VK_VERSION_PATCH(vulkanInfo.usedAPIVersion));
  CLog::Log(LOGINFO, "        - Debug Utils Enabled: {0}",
            vulkanInfo.debugUtilsEnabled ? "Yes" : "No");
  for (const auto& deviceInfo : vulkanInfo.instanceExtensions)
  {
    bool isEnabled = std::find_if(vulkanInfo.requiredInstanceExtensions.begin(),
                                  vulkanInfo.requiredInstanceExtensions.end(),
                                  [&deviceInfo](const char* enabledExtension)
                                  {
                                    return strcmp(enabledExtension, deviceInfo.extensionName) == 0;
                                  }) != vulkanInfo.requiredInstanceExtensions.end();
    CLog::Log(LOGINFO, "        - Instance Extension: {0} (Version {1}), Is Enabled: {2}",
              deviceInfo.extensionName, deviceInfo.specVersion, isEnabled ? "Yes" : "No");
  }
  for (const auto& deviceInfo : vulkanInfo.physicalDevices)
  {
    CLog::Log(LOGINFO, "        - Physical Device: {0}", deviceInfo.properties.deviceName);
    CLog::Log(LOGINFO, "            - Supported: {0}",
              deviceInfo.properties.apiVersion >= REQUIRED_VK_API_VERSION ? "Yes" : "No");
    CLog::Log(LOGINFO, "            - Vendor ID: {0:#X}", deviceInfo.properties.vendorID);
    CLog::Log(LOGINFO, "            - Device ID: {0:#X}", deviceInfo.properties.deviceID);
    CLog::Log(LOGINFO, "            - Driver Version: {0}.{1}.{2}",
              VK_VERSION_MAJOR(deviceInfo.properties.driverVersion),
              VK_VERSION_MINOR(deviceInfo.properties.driverVersion),
              VK_VERSION_PATCH(deviceInfo.properties.driverVersion));
    CLog::Log(LOGINFO, "            - API Version: {0}.{1}.{2}",
              VK_VERSION_MAJOR(deviceInfo.properties.apiVersion),
              VK_VERSION_MINOR(deviceInfo.properties.apiVersion),
              VK_VERSION_PATCH(deviceInfo.properties.apiVersion));
    CLog::Log(LOGINFO, "            - DRM Device ID: {0:#X}", deviceInfo.drmDeviceId);
    CLog::Log(LOGINFO, "            - Queue Families: {0}", deviceInfo.queueFamilies.size());
    CLog::Log(LOGINFO, "            - Sampler YCbCr Conversion: {0}",
              deviceInfo.featureSamplerYCBCRconversion ? "Supported" : "Not supported");
    CLog::Log(LOGINFO, "            - Protected Memory: {0}",
              deviceInfo.featureProtectedMemory ? "Supported" : "Not supported");
  }
}

std::string ErrorString(VkResult errorCode)
{
  switch (errorCode)
  {
#define STR(r) \
  case VK_##r: \
    return #r
    STR(NOT_READY);
    STR(TIMEOUT);
    STR(EVENT_SET);
    STR(EVENT_RESET);
    STR(INCOMPLETE);
    STR(ERROR_OUT_OF_HOST_MEMORY);
    STR(ERROR_OUT_OF_DEVICE_MEMORY);
    STR(ERROR_INITIALIZATION_FAILED);
    STR(ERROR_DEVICE_LOST);
    STR(ERROR_MEMORY_MAP_FAILED);
    STR(ERROR_LAYER_NOT_PRESENT);
    STR(ERROR_EXTENSION_NOT_PRESENT);
    STR(ERROR_FEATURE_NOT_PRESENT);
    STR(ERROR_INCOMPATIBLE_DRIVER);
    STR(ERROR_TOO_MANY_OBJECTS);
    STR(ERROR_FORMAT_NOT_SUPPORTED);
    STR(ERROR_SURFACE_LOST_KHR);
    STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
    STR(SUBOPTIMAL_KHR);
    STR(ERROR_OUT_OF_DATE_KHR);
    STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
    STR(ERROR_VALIDATION_FAILED_EXT);
    STR(ERROR_INVALID_SHADER_NV);
#undef STR
    default:
      return "UNKNOWN_ERROR";
  }
}

} // namespace UTILS
} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
