/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanFunctionPointers.h"

#include "utils/log.h"

#include <stdexcept>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

CVulkanFunctionPointers* GetVulkanFunctionPointers()
{
  static CVulkanFunctionPointers s_vulkanFunctionPointers;
  return &s_vulkanFunctionPointers;
}

bool CVulkanFunctionPointers::BindInstanceFunctionPointers(
    VkInstance vkInstance, const std::vector<VkExtensionProperties>& extensions)
{
  try
  {
#if defined(HAVE_WAYLAND)
    {
      auto it = std::find_if(
          extensions.begin(), extensions.end(), [](const VkExtensionProperties& prop)
          { return strcmp(prop.extensionName, VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME) == 0; });
      if (it != extensions.end())
      {
        constexpr char kvkCreateWaylandSurfaceKHR[] = "vkCreateWaylandSurfaceKHR";
        vkCreateWaylandSurfaceKHR = reinterpret_cast<PFN_vkCreateWaylandSurfaceKHR>(
            vkGetInstanceProcAddr(vkInstance, kvkCreateWaylandSurfaceKHR));
        if (!vkCreateWaylandSurfaceKHR)
          throw std::runtime_error("Failed to load vkCreateWaylandSurfaceKHR function pointer");

        constexpr char kvkGetPhysicalDeviceWaylandPresentationSupportKHR[] =
            "vkGetPhysicalDeviceWaylandPresentationSupportKHR";
        vkGetPhysicalDeviceWaylandPresentationSupportKHR =
            reinterpret_cast<PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR>(
                vkGetInstanceProcAddr(vkInstance,
                                      kvkGetPhysicalDeviceWaylandPresentationSupportKHR));
        if (!vkGetPhysicalDeviceWaylandPresentationSupportKHR)
          throw std::runtime_error(
              "Failed to load vkGetPhysicalDeviceWaylandPresentationSupportKHR function pointer");
      }
    }
#endif // defined(HAVE_WAYLAND)

#if defined(HAVE_X11)
    {
      auto it = std::find_if(
          extensions.begin(), extensions.end(), [](const VkExtensionProperties& prop)
          { return strcmp(prop.extensionName, VK_KHR_XLIB_SURFACE_EXTENSION_NAME) == 0; });
      if (it != extensions.end())
      {
        constexpr char kvkCreateXlibSurfaceKHR[] = "vkCreateXlibSurfaceKHR";
        vkCreateXlibSurfaceKHR = reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(
            vkGetInstanceProcAddr(vkInstance, kvkCreateXlibSurfaceKHR));
        if (!vkCreateXlibSurfaceKHR)
          throw std::runtime_error("Failed to load vkCreateXlibSurfaceKHR function pointer");

        constexpr char kvkGetPhysicalDeviceXlibPresentationSupportKHR[] =
            "vkGetPhysicalDeviceXlibPresentationSupportKHR";
        vkGetPhysicalDeviceXlibPresentationSupportKHR =
            reinterpret_cast<PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR>(
                vkGetInstanceProcAddr(vkInstance, kvkGetPhysicalDeviceXlibPresentationSupportKHR));
        if (!vkGetPhysicalDeviceXlibPresentationSupportKHR)
          throw std::runtime_error(
              "Failed to load vkGetPhysicalDeviceXlibPresentationSupportKHR function pointer");
      }
    }
#endif // defined(HAVE_X11)

#if defined(HAVE_GBM)
#error "GBM not implemented yet"
#endif // defined(HAVE_GBM)

#if defined(TARGET_ANDROID)
    {
      auto it = std::find_if(
          extensions.begin(), extensions.end(), [](const VkExtensionProperties& prop)
          { return strcmp(prop.extensionName, VK_KHR_ANDROID_SURFACE_EXTENSION_NAME) == 0; });
      if (it != extensions.end())
      {
        constexpr char kvkCreateAndroidSurfaceKHR[] = "vkCreateAndroidSurfaceKHR";
        vkCreateAndroidSurfaceKHR = reinterpret_cast<PFN_vkCreateAndroidSurfaceKHR>(
            vkGetInstanceProcAddr(vkInstance, kvkCreateAndroidSurfaceKHR));
        if (!vkCreateAndroidSurfaceKHR)
          throw std::runtime_error("Failed to load vkCreateAndroidSurfaceKHR function pointer");
      }
    }
#endif // defined(TARGET_ANDROID)

#if defined(TARGET_DARWIN) || defined(TARGET_DARWIN_EMBEDDED)
    {
      auto it = std::find_if(
          extensions.begin(), extensions.end(), [](const VkExtensionProperties& prop)
          { return strcmp(prop.extensionName, VK_EXT_METAL_SURFACE_EXTENSION_NAME) == 0; });
      if (it != extensions.end())
      {
        constexpr char kvkCreateMetalSurfaceEXT[] = "vkCreateMetalSurfaceEXT";
        vkCreateMetalSurfaceEXT = reinterpret_cast<PFN_vkCreateMetalSurfaceEXT>(
            vkGetInstanceProcAddr(vkInstance, kvkCreateMetalSurfaceEXT));
        if (!vkCreateMetalSurfaceEXT)
          throw std::runtime_error("Failed to load vkCreateMetalSurfaceEXT function pointer");
      }
    }
#endif // defined(TARGET_DARWIN) || defined(TARGET_DARWIN_EMBEDDED)

#if defined(TARGET_WINDOWS)
    {
      auto it = std::find_if(
          extensions.begin(), extensions.end(), [](const VkExtensionProperties& prop)
          { return strcmp(prop.extensionName, VK_KHR_WIN32_SURFACE_EXTENSION_NAME) == 0; });
      if (it != extensions.end())
      {
        constexpr char kvkCreateWin32SurfaceKHR[] = "vkCreateWin32SurfaceKHR";
        vkCreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(
            vkGetInstanceProcAddr(vkInstance, kvkCreateWin32SurfaceKHR));
        if (!vkCreateWin32SurfaceKHR)
          throw std::runtime_error("Failed to load vkCreateWin32SurfaceKHR function pointer");

        constexpr char kvkGetPhysicalDeviceWin32PresentationSupportKHR[] =
            "vkGetPhysicalDeviceWin32PresentationSupportKHR";
        vkGetPhysicalDeviceWin32PresentationSupportKHR =
            reinterpret_cast<PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR>(
                vkGetInstanceProcAddr(vkInstance, kvkGetPhysicalDeviceWin32PresentationSupportKHR));
        if (!vkGetPhysicalDeviceWin32PresentationSupportKHR)
          throw std::runtime_error(
              "Failed to load vkGetPhysicalDeviceWin32PresentationSupportKHR function pointer");
      }
    }
#endif // defined(TARGET_WINDOWS)

#ifndef NDEBUG
    {
      auto it = std::find_if(
          extensions.begin(), extensions.end(), [](const VkExtensionProperties& prop)
          { return strcmp(prop.extensionName, VK_EXT_DEBUG_REPORT_EXTENSION_NAME) == 0; });
      if (it != extensions.end())
      {
        constexpr char kvkCreateDebugUtilsMessengerEXT[] = "vkCreateDebugUtilsMessengerEXT";
        vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(vkInstance, kvkCreateDebugUtilsMessengerEXT));
        if (!vkCreateDebugUtilsMessengerEXT)
          throw std::runtime_error(
              "Failed to load vkCreateDebugUtilsMessengerEXT function pointer");

        constexpr char kvkDestroyDebugUtilsMessengerEXT[] = "vkDestroyDebugUtilsMessengerEXT";
        vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(vkInstance, kvkDestroyDebugUtilsMessengerEXT));
        if (!vkDestroyDebugUtilsMessengerEXT)
          throw std::runtime_error(
              "Failed to load vkDestroyDebugUtilsMessengerEXT function pointer");
      }
    }
#endif // NDEBUG
  }
  catch (const std::exception& e)
  {
    CLog::Log(LOGERROR, "Vulkan: Exception while binding instance function pointers: {}", e.what());
    return false;
  }

  return true;
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
