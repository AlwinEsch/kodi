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

  bool BindInstanceFunctionPointers(VkInstance vkInstance, const std::vector<VkExtensionProperties>& extensions);

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

#ifndef NDEBUG
  VulkanFunction<PFN_vkCreateDebugUtilsMessengerEXT> vkCreateDebugUtilsMessengerEXT;
  VulkanFunction<PFN_vkDestroyDebugUtilsMessengerEXT> vkDestroyDebugUtilsMessengerEXT;
#endif // NDEBUG
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI

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
