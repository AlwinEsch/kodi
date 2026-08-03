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
#include "rendering/vulkan/VulkanData.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "utils/log.h"

#include <algorithm>

namespace KODI::RENDERING::VULKAN
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

  VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &m_vkInstance), false);

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

    VK_CHECK_RESULT(
        vkCreateDebugUtilsMessengerEXT(m_vkInstance, &createInfo, nullptr, &m_vkDebugMessenger),
        false);
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

  // Get and check the Vulkan API version supported by the Vulkan loader and driver.
  VK_CHECK_RESULT(vkEnumerateInstanceVersion(&m_vulkanInfo.availableAPIVersion), false);

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
    VK_CHECK_RESULT(vkEnumerateInstanceExtensionProperties(layer_name, &num_instance_exts, nullptr),
                    false);

    const size_t previous_extension_count = m_vulkanInfo.instanceExtensions.size();
    m_vulkanInfo.instanceExtensions.resize(previous_extension_count + num_instance_exts);

    VK_CHECK_RESULT(vkEnumerateInstanceExtensionProperties(
                        layer_name, &num_instance_exts,
                        &m_vulkanInfo.instanceExtensions.data()[previous_extension_count]),
                    false);
  }

  uint32_t numInstanceLayers = 0;
  VK_CHECK_RESULT(vkEnumerateInstanceLayerProperties(&numInstanceLayers, nullptr), false);

  m_vulkanInfo.instanceLayers.resize(numInstanceLayers);
  VK_CHECK_RESULT(
      vkEnumerateInstanceLayerProperties(&numInstanceLayers, m_vulkanInfo.instanceLayers.data()),
      false);

  m_vulkanInfo.instanceLayers.resize(numInstanceLayers);
  VK_CHECK_RESULT(
      vkEnumerateInstanceLayerProperties(&numInstanceLayers, m_vulkanInfo.instanceLayers.data()),
      false);

  return true;
}

bool CVulkanInstance::GetDeviceInfos(VkPhysicalDevice physicalDevice /* = VK_NULL_HANDLE*/)
{
  using KODI::RENDERING::VULKAN::UTILS::ErrorString;

  std::vector<VkPhysicalDevice> physicalDevices;
  if (physicalDevice == VK_NULL_HANDLE)
  {
    uint32_t count = 0;
    VK_CHECK_RESULT(vkEnumeratePhysicalDevices(m_vkInstance, &count, nullptr), false);
    if (count == 0)
    {
      CLog::Log(LOGERROR, "vkEnumeratePhysicalDevices returns zero device.");
      return false;
    }

    physicalDevices.resize(count);
    VK_CHECK_RESULT(vkEnumeratePhysicalDevices(m_vkInstance, &count, physicalDevices.data()),
                    false);
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
    VK_CHECK_RESULT(
        vkEnumerateDeviceExtensionProperties(device, nullptr /* pLayerName */, &count, nullptr),
        false);

    if (count)
    {
      info.extensions.resize(count);
      VK_CHECK_RESULT(vkEnumerateDeviceExtensionProperties(device, nullptr /* pLayerName */, &count,
                                                           info.extensions.data()),
                      false);

      CLog::Log(LOGDEBUG, "Vulkan: Device {0}", info.properties.deviceName);
      CLog::Log(LOGDEBUG, "        - Extensions:");
      for (const auto& ext : info.extensions)
      {
        CLog::Log(LOGDEBUG, "          - {0} - Version {1}", ext.extensionName, ext.specVersion);
        info.extensions2.push_back(ext.extensionName);
      }
    }

    if (info.properties.apiVersion >= REQUIRED_VK_API_VERSION)
    {
      //--------------------------------------------------------------------------------------------
      // Query the features of the physical device, including the extended dynamic state feature.

      VkPhysicalDeviceProtectedMemoryFeatures protected_memory_feature{};
      protected_memory_feature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES;
      protected_memory_feature.pNext = nullptr;

      VkPhysicalDeviceSamplerYcbcrConversionFeatures ycbcr_conversion{};
      ycbcr_conversion.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES;
      ycbcr_conversion.pNext = &protected_memory_feature;

      VkPhysicalDeviceExtendedDynamicStateFeaturesEXT dyn_state{};
      dyn_state.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
      dyn_state.pNext = &ycbcr_conversion;

      VkPhysicalDeviceExtendedDynamicState2FeaturesEXT dyn_state2{};
      dyn_state2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT;
      dyn_state2.pNext = &dyn_state;

      VkPhysicalDeviceExtendedDynamicState3FeaturesEXT dyn_state3{};
      dyn_state3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT;
      dyn_state3.pNext = &dyn_state2;

      VkPhysicalDeviceFeatures2 features_2{};
      features_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
      features_2.pNext = &dyn_state3;

      vkGetPhysicalDeviceFeatures2(device, &features_2);

      //--------------------------------------------------------------------------------------------
      // Query the driver properties of the physical device, including the DRM properties.
      // TODO: Not finished yet, but we can use this to query the driver version and other properties.

      info.driverProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;

      VkPhysicalDeviceDrmPropertiesEXT drm_properties{};
      drm_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRM_PROPERTIES_EXT;

      if (info.ExtensionSupported(VK_EXT_PHYSICAL_DEVICE_DRM_EXTENSION_NAME))
      {
        info.driverProperties.pNext = &drm_properties;
      }

      VkPhysicalDeviceProperties2 properties2 = {
          .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
          .pNext = &info.driverProperties,
          .properties = {},
      };
      vkGetPhysicalDeviceProperties2(device, &properties2);

      //--------------------------------------------------------------------------------------------
      // Store the features in the device info structure.

      info.features = features_2.features;
      info.featureSamplerYCBCRconversion = ycbcr_conversion.samplerYcbcrConversion;
      info.featureProtectedMemory = protected_memory_feature.protectedMemory;
      info.featureDeviceDRM = info.ExtensionSupported(VK_EXT_PHYSICAL_DEVICE_DRM_EXTENSION_NAME);
      info.featureExtendedDynamicState =
          info.ExtensionSupported(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME) &&
          dyn_state.extendedDynamicState;
      info.featureExtendedDynamicState2 =
          info.ExtensionSupported(VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME);
      info.featureExtendedDynamicState3 =
          info.ExtensionSupported(VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME);

      CLog::Log(LOGDEBUG, "        - Features:");
      CLog::Log(LOGDEBUG, "          - samplerYcbcrConversion - Supported: {0}",
                info.featureSamplerYCBCRconversion ? "true" : "false");
      CLog::Log(LOGDEBUG, "          - protectedMemory - Supported: {0}",
                info.featureProtectedMemory ? "true" : "false");
      CLog::Log(LOGDEBUG, "          - deviceDRM - Supported: {0}",
                info.featureDeviceDRM ? "true" : "false");

      CLog::Log(LOGDEBUG, "          - extendedDynamicState - Supported: {0}",
                info.featureExtendedDynamicState ? "true" : "false");

      CLog::Log(LOGDEBUG, "          - extendedDynamicState2 - Supported: {0}",
                info.featureExtendedDynamicState2 ? "true" : "false");
      CLog::Log(LOGDEBUG, "            - extendedDynamicState2LogicOp - Supported: {0}",
                dyn_state2.extendedDynamicState2LogicOp ? "true" : "false");
      CLog::Log(LOGDEBUG, "            - extendedDynamicState2PatchControlPoints - Supported: {0}",
                dyn_state2.extendedDynamicState2PatchControlPoints ? "true" : "false");

      CLog::Log(LOGDEBUG, "          - extendedDynamicState3 - Supported: {0}",
                info.featureExtendedDynamicState3 ? "true" : "false");
      CLog::Log(LOGDEBUG, "            - extendedDynamicState3TessellationDomainOrigin - Supported: {0}",
                dyn_state3.extendedDynamicState3TessellationDomainOrigin ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3DepthClampEnable - Supported: {0}",
                dyn_state3.extendedDynamicState3DepthClampEnable ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3PolygonMode - Supported: {0}",
                dyn_state3.extendedDynamicState3PolygonMode ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3RasterizationSamples - Supported: {0}",
                dyn_state3.extendedDynamicState3RasterizationSamples ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3SampleMask - Supported: {0}",
                dyn_state3.extendedDynamicState3SampleMask ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3AlphaToCoverageEnable - Supported: {0}",
                dyn_state3.extendedDynamicState3AlphaToCoverageEnable ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3AlphaToOneEnable - Supported: {0}",
                dyn_state3.extendedDynamicState3AlphaToOneEnable ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3LogicOpEnable - Supported: {0}",
                dyn_state3.extendedDynamicState3LogicOpEnable ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3ColorBlendEnable - Supported: {0}",
                dyn_state3.extendedDynamicState3ColorBlendEnable ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3ColorBlendEquation - Supported: {0}",
                dyn_state3.extendedDynamicState3ColorBlendEquation ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3ColorWriteMask - Supported: {0}",
                dyn_state3.extendedDynamicState3ColorWriteMask ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3RasterizationStream - Supported: {0}",
                dyn_state3.extendedDynamicState3RasterizationStream ? "true" : "false");

      CLog::Log(LOGDEBUG,
                "            - extendedDynamicState3ConservativeRasterizationMode - Supported: {0}",
                dyn_state3.extendedDynamicState3ConservativeRasterizationMode ? "true" : "false");

      CLog::Log(
          LOGDEBUG, "            - extendedDynamicState3ExtraPrimitiveOverestimationSize - Supported: {0}",
          dyn_state3.extendedDynamicState3ExtraPrimitiveOverestimationSize ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3DepthClipEnable - Supported: {0}",
                dyn_state3.extendedDynamicState3DepthClipEnable ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3SampleLocationsEnable - Supported: {0}",
                dyn_state3.extendedDynamicState3SampleLocationsEnable ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3ColorBlendAdvanced - Supported: {0}",
                dyn_state3.extendedDynamicState3ColorBlendAdvanced ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3ProvokingVertexMode - Supported: {0}",
                dyn_state3.extendedDynamicState3ProvokingVertexMode ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3LineRasterizationMode - Supported: {0}",
                dyn_state3.extendedDynamicState3LineRasterizationMode ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3LineStippleEnable - Supported: {0}",
                dyn_state3.extendedDynamicState3LineStippleEnable ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3DepthClipNegativeOneToOne - Supported: {0}",
                dyn_state3.extendedDynamicState3DepthClipNegativeOneToOne ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3ViewportWScalingEnable - Supported: {0}",
                dyn_state3.extendedDynamicState3ViewportWScalingEnable ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3ViewportSwizzle - Supported: {0}",
                dyn_state3.extendedDynamicState3ViewportSwizzle ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3CoverageToColorEnable - Supported: {0}",
                dyn_state3.extendedDynamicState3CoverageToColorEnable ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3CoverageToColorLocation - Supported: {0}",
                dyn_state3.extendedDynamicState3CoverageToColorLocation ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3CoverageModulationMode - Supported: {0}",
                dyn_state3.extendedDynamicState3CoverageModulationMode ? "true" : "false");

      CLog::Log(LOGDEBUG,
                "            - extendedDynamicState3CoverageModulationTableEnable - Supported: {0}",
                dyn_state3.extendedDynamicState3CoverageModulationTableEnable ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3CoverageModulationTable - Supported: {0}",
                dyn_state3.extendedDynamicState3CoverageModulationTable ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3CoverageReductionMode - Supported: {0}",
                dyn_state3.extendedDynamicState3CoverageReductionMode ? "true" : "false");

      CLog::Log(
          LOGDEBUG, "            - extendedDynamicState3RepresentativeFragmentTestEnable - Supported: {0}",
          dyn_state3.extendedDynamicState3RepresentativeFragmentTestEnable ? "true" : "false");

      CLog::Log(LOGDEBUG, "            - extendedDynamicState3ShadingRateImageEnable - Supported: {0}",
                dyn_state3.extendedDynamicState3ShadingRateImageEnable ? "true" : "false");
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

} // namespace KODI::RENDERING::VULKAN
