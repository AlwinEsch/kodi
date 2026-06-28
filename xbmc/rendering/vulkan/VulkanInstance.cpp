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
#include "VulkanUtils.h"
#include "utils/log.h"

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
  if (m_vkInstance != VK_NULL_HANDLE)
  {
    CLog::Log(LOGERROR, "Vulkan instance already created");
    return false;
  }

  if (!GetBasicInfos(required_layers))
    return false;

  m_vulkanInfo.usedAPIVersion = REQUIRED_VK_API_VERSION;
  m_vulkanInfo.enabledInstanceExtensions = required_extensions;

  for (const VkExtensionProperties& ext_property : m_vulkanInfo.instanceExtensions)
  {
    if (strcmp(ext_property.extensionName, VK_EXT_DEBUG_REPORT_EXTENSION_NAME) == 0)
    {
      m_vulkanInfo.debugReportEnabled = true;
      m_vulkanInfo.enabledInstanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }
  }

  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = CCompileInfo::GetAppName();
  app_info.apiVersion = m_vulkanInfo.usedAPIVersion;

  VkInstanceCreateInfo instance_create_info = {
      VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, // sType
      nullptr, // pNext
      0, // flags
      &app_info, // pApplicationInfo
      static_cast<uint32_t>(required_layers.size()),
      // enableLayerCount
      required_layers.data(), // ppEnabledLayerNames
      static_cast<uint32_t>(m_vulkanInfo.enabledInstanceExtensions.size()),
      // enabledExtensionCount
      m_vulkanInfo.enabledInstanceExtensions.data(),
      // ppEnabledExtensionNames
  };

  VkResult result = vkCreateInstance(&instance_create_info, nullptr, &m_vkInstance);
  if (VK_SUCCESS != result)
  {
    CLog::Log(LOGERROR, "vkCreateInstance() failed: {0}", result);
    return false;
  }

  if (!GetVulkanFunctionPointers()->BindInstanceFunctionPointers(m_vkInstance))
  {
    return false;
  }

  // Register our error logging function.
  if (m_vulkanInfo.debugReportEnabled)
  {
    VkDebugReportCallbackCreateInfoEXT cb_create_info = {};
    cb_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;

    cb_create_info.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT |
                           VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                           VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;
    cb_create_info.pfnCallback = &vulkanErrorCallback;
    result =
        vkCreateDebugReportCallbackEXT(m_vkInstance, &cb_create_info, nullptr, &m_vkReportCallback);
    if (VK_SUCCESS != result)
    {
      m_vkReportCallback = VK_NULL_HANDLE;
      CLog::Log(LOGERROR, "vkCreateDebugReportCallbackEXT failed: {0}", result);
      return false;
    }
  }

  LogGraphicsInfo(m_vulkanInfo);

  return true;
}

void CVulkanInstance::Destroy()
{
  if (m_vulkanInfo.debugReportEnabled && m_vkReportCallback != VK_NULL_HANDLE)
  {
    vkDestroyDebugReportCallbackEXT(m_vkInstance, m_vkReportCallback, nullptr);
    m_vkReportCallback = VK_NULL_HANDLE;
  }

  if (m_vkInstance != VK_NULL_HANDLE)
  {
    vkDestroyInstance(m_vkInstance, nullptr);
    m_vkInstance = VK_NULL_HANDLE;
  }
}

bool CVulkanInstance::GetBasicInfos(const std::vector<const char*>& requiredLayers)
{
  VkResult result;

  // Get and check the Vulkan API version supported by the Vulkan loader and driver.
  result = vkEnumerateInstanceVersion(&m_vulkanInfo.availableAPIVersion);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "vkEnumerateInstanceVersion() failed: {0}", result);
    return false;
  }

  if (m_vulkanInfo.availableAPIVersion < REQUIRED_VK_API_VERSION)
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
    if (result != VK_SUCCESS)
    {
      CLog::Log(LOGERROR, "vkEnumerateInstanceExtensionProperties({0}) failed: {1}",
                (layer_name ? layer_name : "nullptr"), result);
      return false;
    }

    const size_t previous_extension_count = m_vulkanInfo.instanceExtensions.size();
    m_vulkanInfo.instanceExtensions.resize(previous_extension_count + num_instance_exts);

    result = vkEnumerateInstanceExtensionProperties(
        layer_name, &num_instance_exts,
        &m_vulkanInfo.instanceExtensions.data()[previous_extension_count]);
    if (result != VK_SUCCESS)
    {
      CLog::Log(LOGERROR, "vkEnumerateInstanceExtensionProperties({0}) failed: {1}",
                (layer_name ? layer_name : "nullptr"), result);
      return false;
    }
  }

  for (const VkExtensionProperties& ext_property : m_vulkanInfo.instanceExtensions)
  {
    if (strcmp(ext_property.extensionName, VK_EXT_DEBUG_REPORT_EXTENSION_NAME) == 0)
    {
      m_vulkanInfo.debugReportEnabled = true;
      m_vulkanInfo.enabledInstanceExtensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }
  }

  uint32_t num_instance_layers = 0;
  result = vkEnumerateInstanceLayerProperties(&num_instance_layers, nullptr);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "vkEnumerateInstanceLayerProperties(NULL) failed: {0}", result);
    return false;
  }

  m_vulkanInfo.instanceLayers.resize(num_instance_layers);
  result =
      vkEnumerateInstanceLayerProperties(&num_instance_layers, m_vulkanInfo.instanceLayers.data());
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "vkEnumerateInstanceLayerProperties() failed: {0}", result);
    return false;
  }

  return true;
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
