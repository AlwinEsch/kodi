/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanInstance.h"

#include "CompileInfo.h"
#include "VulkanFunctionPointers.h"
#include "rendering/vulkan/VulkanUtils.h"
#include "utils/log.h"

#include <algorithm>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

CVulkanInstance::~CVulkanInstance()
{
  Destroy();
}

bool CVulkanInstance::Create(const std::vector<const char*>& required_extensions,
                             const std::vector<const char*>& required_layers)
{
  using KODI::RENDERING::VULKAN::UTILS::ErrorString;

  if (m_vkInstance != VK_NULL_HANDLE)
  {
    CLog::Log(LOGERROR, "Vulkan: Instance already created");
    return false;
  }

  CLog::Log(LOGINFO, "Vulkan: Initializing vulkan instance.");

  if (!GetBasicInfos(required_layers))
    return false;

  m_vulkanInfo.usedAPIVersion = REQUIRED_VK_API_VERSION;
  m_vulkanInfo.requiredInstanceExtensions = required_extensions;

#ifndef NDEBUG
  if (ValidateExtensions(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, m_vulkanInfo.instanceExtensions))
  {
    m_vulkanInfo.debugUtilsEnabled = true;
    m_vulkanInfo.requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
#endif

#if defined(VKB_ENABLE_PORTABILITY)
  m_vulkanInfo.requiredInstanceExtensions.push_back(
      VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

  bool portabilityEnumerationAvailable = false;
  if (ValidateExtensions(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
                         m_vulkanInfo.instanceExtensions))
  {
    m_vulkanInfo.requiredInstanceExtensions.push_back(
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    portabilityEnumerationAvailable = true;
  }
#endif

#ifndef NDEBUG
  for (const char* required : m_vulkanInfo.requiredInstanceExtensions)
  {
    bool found = false;
    for (const VkExtensionProperties& ext : m_vulkanInfo.instanceExtensions)
    {
      if (strcmp(ext.extensionName, required) == 0) [[likely]]
      {
        found = true;
        break;
      }
    }
    if (!found) [[unlikely]]
    {
      CLog::Log(LOGERROR,
                "Required extension {0} missing from enumerated Vulkan extensions. "
                "vkCreateInstance will likely fail.",
                required);
    }
  }
#endif

  VkApplicationInfo app_info = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .pApplicationName = CCompileInfo::GetAppName(),
      .applicationVersion = VK_MAKE_VERSION(CCompileInfo::GetMajor(), CCompileInfo::GetMinor(), 0),
      .pEngineName = nullptr,
      .engineVersion = 0,
      .apiVersion = m_vulkanInfo.usedAPIVersion,
  };

  VkInstanceCreateInfo instanceCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .pApplicationInfo = &app_info,
      .enabledLayerCount = static_cast<uint32_t>(required_layers.size()),
      .ppEnabledLayerNames = required_layers.data(),
      .enabledExtensionCount =
          static_cast<uint32_t>(m_vulkanInfo.requiredInstanceExtensions.size()),
      .ppEnabledExtensionNames = m_vulkanInfo.requiredInstanceExtensions.data(),
  };

#if (defined(VKB_ENABLE_PORTABILITY))
  if (portabilityEnumerationAvailable)
  {
    instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  }
#endif

  VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &m_vkInstance);
  if (VK_SUCCESS != result) [[unlikely]]
  {
    CLog::Log(LOGERROR, "vkCreateInstance() failed: {0}", ErrorString(result));
    return false;
  }

  if (!GetVulkanFunctionPointers()->BindInstanceFunctionPointers(m_vkInstance,
                                                                 m_vulkanInfo.instanceExtensions))
  {
    // Logging is done in BindInstanceFunctionPointers.
    return false;
  }

#ifndef NDEBUG
  // Register our error logging function.
  if (m_vulkanInfo.debugUtilsEnabled && m_vkDebugMessenger == VK_NULL_HANDLE)
  {
    // Setting up the debug messenger
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = &UTILS::vulkanErrorCallback;
    createInfo.pUserData = nullptr; // Optional user data

    VkResult result =
        vkCreateDebugUtilsMessengerEXT(m_vkInstance, &createInfo, nullptr, &m_vkDebugMessenger);
    if (result != VK_SUCCESS) [[unlikely]]
    {
      using KODI::RENDERING::VULKAN::UTILS::ErrorString;
      CLog::Log(LOGERROR, "Vulkan: Failed to set up debug messenger: {}", ErrorString(result));
    }
  }
#endif

  if (!GetDeviceInfos()) [[unlikely]]
    return false;

  UTILS::LogGraphicsInfo(m_vulkanInfo);

  return true;
}

void CVulkanInstance::Destroy()
{
#ifndef NDEBUG
  if (m_vulkanInfo.debugUtilsEnabled && m_vkDebugMessenger != VK_NULL_HANDLE) [[likely]]
  {
    vkDestroyDebugUtilsMessengerEXT(m_vkInstance, m_vkDebugMessenger, nullptr);
    m_vkDebugMessenger = VK_NULL_HANDLE;
  }
#endif

  if (m_vkInstance != VK_NULL_HANDLE) [[likely]]
  {
    vkDestroyInstance(m_vkInstance, nullptr);
    m_vkInstance = VK_NULL_HANDLE;
  }
}

bool CVulkanInstance::GetBasicInfos(const std::vector<const char*>& requiredLayers)
{
  using KODI::RENDERING::VULKAN::UTILS::ErrorString;

  VkResult result;

  // Get and check the Vulkan API version supported by the Vulkan loader and driver.
  result = vkEnumerateInstanceVersion(&m_vulkanInfo.availableAPIVersion);
  if (result != VK_SUCCESS) [[unlikely]]
  {
    CLog::Log(LOGERROR, "vkEnumerateInstanceVersion() failed: {0}", ErrorString(result));
    return false;
  }

  if (m_vulkanInfo.availableAPIVersion < REQUIRED_VK_API_VERSION) [[unlikely]]
  {
    CLog::Log(LOGFATAL, "Vulkan API version {0}.{1}.{2} is less than required version {3}.{4}.{5}",
              VK_VERSION_MAJOR(m_vulkanInfo.availableAPIVersion),
              VK_VERSION_MINOR(m_vulkanInfo.availableAPIVersion),
              VK_VERSION_PATCH(m_vulkanInfo.availableAPIVersion),
              VK_VERSION_MAJOR(REQUIRED_VK_API_VERSION), VK_VERSION_MINOR(REQUIRED_VK_API_VERSION),
              VK_VERSION_PATCH(REQUIRED_VK_API_VERSION));
    return false;
  }

  // Query the extensions from all layers, including ones that are implicitly
  // available (identified by passing a null ptr as the layer name).
  std::vector<const char*> allRequiredLayers = requiredLayers;

  // Include the extension properties provided by the Vulkan implementation as
  // part of the enumeration.
  allRequiredLayers.emplace_back(nullptr);

  for (const char* layer_name : allRequiredLayers)
  {
    uint32_t num_instance_exts{0};
    result = vkEnumerateInstanceExtensionProperties(layer_name, &num_instance_exts, nullptr);
    if (result != VK_SUCCESS) [[unlikely]]
    {
      CLog::Log(LOGERROR, "vkEnumerateInstanceExtensionProperties({0}) failed to get size: {1}",
                (layer_name ? layer_name : "nullptr"), ErrorString(result));
      return false;
    }

    const size_t previous_extension_count = m_vulkanInfo.instanceExtensions.size();
    m_vulkanInfo.instanceExtensions.resize(previous_extension_count + num_instance_exts);

    result = vkEnumerateInstanceExtensionProperties(
        layer_name, &num_instance_exts,
        &m_vulkanInfo.instanceExtensions.data()[previous_extension_count]);
    if (result != VK_SUCCESS) [[unlikely]]
    {
      CLog::Log(LOGERROR,
                "vkEnumerateInstanceExtensionProperties({0}) failed to get properties: {1}",
                (layer_name ? layer_name : "nullptr"), ErrorString(result));
      return false;
    }
  }

  uint32_t numInstanceLayers = 0;
  result = vkEnumerateInstanceLayerProperties(&numInstanceLayers, nullptr);
  if (result != VK_SUCCESS) [[unlikely]]
  {
    CLog::Log(LOGERROR, "vkEnumerateInstanceLayerProperties(NULL) failed: {0}",
              ErrorString(result));
    return false;
  }

  m_vulkanInfo.instanceLayers.resize(numInstanceLayers);
  result =
      vkEnumerateInstanceLayerProperties(&numInstanceLayers, m_vulkanInfo.instanceLayers.data());
  if (result != VK_SUCCESS) [[unlikely]]
  {
    CLog::Log(LOGERROR, "vkEnumerateInstanceLayerProperties() failed: {0}", ErrorString(result));
    return false;
  }

  return true;
}

bool CVulkanInstance::GetDeviceInfos(VkPhysicalDevice physicalDevice /* = VK_NULL_HANDLE*/)
{
  using KODI::RENDERING::VULKAN::UTILS::ErrorString;

  std::vector<VkPhysicalDevice> physicalDevices;
  if (physicalDevice == VK_NULL_HANDLE)
  {
    uint32_t count = 0;
    VkResult result = vkEnumeratePhysicalDevices(m_vkInstance, &count, nullptr);
    if (result != VK_SUCCESS)
    {
      CLog::Log(LOGERROR, "vkEnumeratePhysicalDevices failed: {0}", ErrorString(result));
      return false;
    }

    if (!count)
    {
      CLog::Log(LOGERROR, "vkEnumeratePhysicalDevices returns zero device.");
      return false;
    }

    physicalDevices.resize(count);
    result = vkEnumeratePhysicalDevices(m_vkInstance, &count, physicalDevices.data());
    if (VK_SUCCESS != result)
    {
      CLog::Log(LOGERROR, "vkEnumeratePhysicalDevices() failed: {0}", ErrorString(result));
      return false;
    }
  }
  else
  {
    physicalDevices.push_back(physicalDevice);
  }

  m_vulkanInfo.physicalDevices.reserve(physicalDevices.size());
  for (VkPhysicalDevice device : physicalDevices)
  {
    m_vulkanInfo.physicalDevices.emplace_back();
    auto& info = m_vulkanInfo.physicalDevices.back();
    info.device = device;

    vkGetPhysicalDeviceProperties(device, &info.properties);

    uint32_t count = 0;
    VkResult result =
        vkEnumerateDeviceExtensionProperties(device, nullptr /* pLayerName */, &count, nullptr);
    if (result != VK_SUCCESS)
    {
      CLog::Log(LOGERROR, "vkEnumerateDeviceExtensionProperties failed: {0}", ErrorString(result));
    }

    info.extensions.resize(count);
    result = vkEnumerateDeviceExtensionProperties(device, nullptr /* pLayerName */, &count,
                                                  info.extensions.data());
    if (result != VK_SUCCESS)
    {
      CLog::Log(LOGERROR, "vkEnumerateDeviceExtensionProperties failed: {0}", ErrorString(result));
    }

    if (info.properties.apiVersion >= REQUIRED_VK_API_VERSION)
    {
      bool has_drm_extension = std::ranges::any_of(
          info.extensions, [](const auto& ext)
          { return strcmp(ext.extensionName, VK_EXT_PHYSICAL_DEVICE_DRM_EXTENSION_NAME) == 0; });

      info.driverProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;

      VkPhysicalDeviceDrmPropertiesEXT drm_properties{};
      drm_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRM_PROPERTIES_EXT;

      if (has_drm_extension)
      {
        info.driverProperties.pNext = &drm_properties;
      }

      VkPhysicalDeviceProperties2 properties2 = {
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
          .pNext = &info.driverProperties,
          .properties = {},
      };
      vkGetPhysicalDeviceProperties2(device, &properties2);

      VkPhysicalDeviceProtectedMemoryFeatures protected_memory_feature = {
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES,
          .pNext = nullptr,
          .protectedMemory = VK_FALSE,
      };
      VkPhysicalDeviceSamplerYcbcrConversionFeatures ycbcr_conversion_features = {
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES,
          .pNext = &protected_memory_feature,
          .samplerYcbcrConversion = VK_FALSE,
      };
      VkPhysicalDeviceFeatures2 features_2 = {
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
          .pNext = &ycbcr_conversion_features,
          .features = {},
      };

      vkGetPhysicalDeviceFeatures2(device, &features_2);

      info.features = features_2.features;
      info.featureSamplerYCBCRconversion = ycbcr_conversion_features.samplerYcbcrConversion;
      info.featureProtectedMemory = protected_memory_feature.protectedMemory;
    }

    count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
    if (count)
    {
      info.queueFamilies.resize(count);
      vkGetPhysicalDeviceQueueFamilyProperties(device, &count, info.queueFamilies.data());
    }
  }

  return true;
}

bool CVulkanInstance::ValidateExtensions(const char* extension,
                                         const std::vector<VkExtensionProperties>& available)
{
  bool found = false;
  for (auto& available_extension : available)
  {
    fprintf(stderr, "Vulkan: Available extension: %s - Required: %s\n", available_extension.extensionName,
            extension);
    if (strcmp(available_extension.extensionName, extension) == 0)
    {
      found = true;
      break;
    }
  }

  if (!found)
  {
    return false;
  }

  return true;
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
