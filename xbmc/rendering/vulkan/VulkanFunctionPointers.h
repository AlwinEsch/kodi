/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <vector>

#include <vulkan/vulkan_core.h>
#if defined(HAVE_WAYLAND)
#include <vulkan/vulkan_wayland.h>
#elif defined(HAVE_X11)
#include <vulkan/vulkan_xlib.h>
#elif defined(HAVE_GBM)
#error "GBM not implemented yet"
#elif defined(TARGET_ANDROID)
#include <vulkan/vulkan_android.h>
#elif defined(TARGET_DARWIN) || defined(TARGET_DARWIN_EMBEDDED)
#include <vulkan/vulkan_metal.h>
#elif defined(TARGET_WINDOWS)
#include <vulkan/vulkan_win32.h>
#endif

// Generic helper definitions for inline function support
//@{
#ifdef _MSC_VER
#define ATTR_FORCEINLINE __forceinline
#elif defined(__GNUC__)
#define ATTR_FORCEINLINE inline __attribute__((__always_inline__))
#elif defined(__CLANG__)
#if __has_attribute(__always_inline__)
#define ATTR_FORCEINLINE inline __attribute__((__always_inline__))
#else
#define ATTR_FORCEINLINE inline
#endif
#else
#define ATTR_FORCEINLINE inline
#endif
//@}

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

struct CVulkanFunctionPointers;
CVulkanFunctionPointers* GetVulkanFunctionPointers();

struct CVulkanFunctionPointers
{
  CVulkanFunctionPointers() = default;
  ~CVulkanFunctionPointers() = default;

  bool BindInstanceFunctionPointers(VkInstance vkInstance,
                                    const std::vector<VkExtensionProperties>& extensions);

  template<typename T>
  class VulkanFunction;
  template<typename R, typename... Args>
  class VulkanFunction<R(VKAPI_PTR*)(Args...)>
  {
  public:
    using Fn = R(VKAPI_PTR*)(Args...);

    explicit operator bool() const { return !!fn_; }

    R operator()(Args... args) const { return fn_(args...); }

    Fn get() const { return fn_; }

    void OverrideForTesting(Fn fn) { fn_ = fn; }

  private:
    friend CVulkanFunctionPointers;

    Fn operator=(Fn fn)
    {
      fn_ = fn;
      return fn_;
    }

    Fn fn_ = nullptr;
  };

#if defined(HAVE_WAYLAND)
  VulkanFunction<PFN_vkCreateWaylandSurfaceKHR> vkCreateWaylandSurfaceKHR;
  VulkanFunction<PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR>
      vkGetPhysicalDeviceWaylandPresentationSupportKHR;
#endif // defined(HAVE_WAYLAND)

#if defined(HAVE_X11)
  VulkanFunction<PFN_vkCreateXlibSurfaceKHR> vkCreateXlibSurfaceKHR;
  VulkanFunction<PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR>
      vkGetPhysicalDeviceXlibPresentationSupportKHR;
#endif // defined(HAVE_X11)

#if defined(HAVE_GBM)
#error "GBM not implemented yet"
#endif // defined(HAVE_GBM)

#if defined(TARGET_ANDROID)
  VulkanFunction<PFN_vkCreateAndroidSurfaceKHR> vkCreateAndroidSurfaceKHR;
#endif // defined(TARGET_ANDROID)

#if defined(TARGET_DARWIN) || defined(TARGET_DARWIN_EMBEDDED)
  VulkanFunction<PFN_vkCreateMetalSurfaceEXT> vkCreateMetalSurfaceEXT;
#endif // defined(TARGET_DARWIN) || defined(TARGET_DARWIN_EMBEDDED)

#if defined(TARGET_WINDOWS)
  VulkanFunction<PFN_vkCreateWin32SurfaceKHR> vkCreateWin32SurfaceKHR;
  VulkanFunction<PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR>
      vkGetPhysicalDeviceWin32PresentationSupportKHR;
#endif // defined(TARGET_WINDOWS)

#ifndef NDEBUG
  VulkanFunction<PFN_vkCreateDebugUtilsMessengerEXT> vkCreateDebugUtilsMessengerEXT;
  VulkanFunction<PFN_vkDestroyDebugUtilsMessengerEXT> vkDestroyDebugUtilsMessengerEXT;
#endif // NDEBUG
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI

#if defined(HAVE_WAYLAND)
ATTR_FORCEINLINE VkResult
vkCreateWaylandSurfaceKHR(VkInstance instance,
                          const VkWaylandSurfaceCreateInfoKHR* pCreateInfo,
                          const VkAllocationCallbacks* pAllocator,
                          VkSurfaceKHR* pSurface);

ATTR_FORCEINLINE VkBool32 vkGetPhysicalDeviceWaylandPresentationSupportKHR(
    VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, struct wl_display* display);
#endif // defined(HAVE_WAYLAND)

#if defined(HAVE_X11)
ATTR_FORCEINLINE VkResult vkCreateXlibSurfaceKHR(VkInstance instance,
                                                 const VkXlibSurfaceCreateInfoKHR* pCreateInfo,
                                                 const VkAllocationCallbacks* pAllocator,
                                                 VkSurfaceKHR* pSurface);

ATTR_FORCEINLINE VkBool32 vkGetPhysicalDeviceXlibPresentationSupportKHR(
    VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, Display* dpy, VisualID visualID);
#endif // defined(HAVE_X11)

#if defined(HAVE_GBM)
#error "GBM not implemented yet"
#endif // defined(HAVE_GBM)

#if defined(TARGET_ANDROID)
ATTR_FORCEINLINE VkResult
vkCreateAndroidSurfaceKHR(VkInstance instance,
                          const VkAndroidSurfaceCreateInfoKHR* pCreateInfo,
                          const VkAllocationCallbacks* pAllocator,
                          VkSurfaceKHR* pSurface);
#endif // defined(TARGET_ANDROID)

#if defined(TARGET_DARWIN) || defined(TARGET_DARWIN_EMBEDDED)
ATTR_FORCEINLINE VkResult vkCreateMetalSurfaceEXT(VkInstance instance,
                                                  const VkMetalSurfaceCreateInfoEXT* pCreateInfo,
                                                  const VkAllocationCallbacks* pAllocator,
                                                  VkSurfaceKHR* pSurface);
#endif // defined(TARGET_DARWIN) || defined(TARGET_DARWIN_EMBEDDED)

#if defined(TARGET_WINDOWS)
ATTR_FORCEINLINE VkResult vkCreateWin32SurfaceKHR(VkInstance instance,
                                                  const VkWin32SurfaceCreateInfoKHR* pCreateInfo,
                                                  const VkAllocationCallbacks* pAllocator,
                                                  VkSurfaceKHR* pSurface);

ATTR_FORCEINLINE VkBool32 vkGetPhysicalDeviceWin32PresentationSupportKHR(
    VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex);
#endif // defined(TARGET_WINDOWS)

#ifndef NDEBUG
ATTR_FORCEINLINE VkResult
vkCreateDebugUtilsMessengerEXT(VkInstance instance,
                               const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                               const VkAllocationCallbacks* pAllocator,
                               VkDebugUtilsMessengerEXT* pMessenger)
{
  using KODI::RENDERING::VULKAN::GetVulkanFunctionPointers;
  return GetVulkanFunctionPointers()->vkCreateDebugUtilsMessengerEXT(instance, pCreateInfo,
                                                                     pAllocator, pMessenger);
}

ATTR_FORCEINLINE void vkDestroyDebugUtilsMessengerEXT(VkInstance instance,
                                                      VkDebugUtilsMessengerEXT messenger,
                                                      const VkAllocationCallbacks* pAllocator)
{
  using KODI::RENDERING::VULKAN::GetVulkanFunctionPointers;
  return GetVulkanFunctionPointers()->vkDestroyDebugUtilsMessengerEXT(instance, messenger,
                                                                      pAllocator);
}
#endif // NDEBUG
