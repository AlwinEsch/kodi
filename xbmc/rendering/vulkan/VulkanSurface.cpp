/*
 *  Copyright (C) 2005-2024 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanSurface.h"

#include "rendering/vulkan/VulkanDeviceQueue.h"
#include "rendering/vulkan/VulkanSwapChain.h"
#include "utils/log.h"

#include <array>
#include <cassert>
#include <vector>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

namespace
{

const std::vector<VkFormat> PREFERRED_VK_FORMATS32 = {
    VK_FORMAT_B8G8R8A8_UNORM, // FORMAT_BGRA8888,
    VK_FORMAT_R8G8B8A8_UNORM, // FORMAT_RGBA8888,
};

const std::vector<VkFormat> PREFERRED_VK_FORMATS16 = {
    VK_FORMAT_R5G6B5_UNORM_PACK16, // FORMAT_RGB565,
};

} // namespace

CVulkanSurface::CVulkanSurface(VkInstance vkInstance,
                               VkSurfaceKHR surface,
                               uint64_t acquireNextImageTimeoutNs /*= UINT64_MAX*/)
  : m_vkInstance(vkInstance),
    m_vkSurface(surface),
    m_acquireNextImageTimeoutNs(acquireNextImageTimeoutNs)
{
}

CVulkanSurface::~CVulkanSurface()
{
  assert(m_swapChain == nullptr);
}

bool CVulkanSurface::InitializeSurface(CVulkanDeviceQueue* deviceQueue, SurfaceFormat format)
{
  assert(format >= 0u && format < SurfaceFormat::NUM_SURFACE_FORMATS);
  assert(deviceQueue != nullptr);

  VkResult result;
  auto vkPhysicalDevice = deviceQueue->GetVulkanPhysicalDevice();
  auto vkQueueIndex = deviceQueue->GetVulkanQueueIndex();

  m_deviceQueue = deviceQueue;

  VkBool32 presentSupport{VK_FALSE};
  result = vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, vkQueueIndex, m_vkSurface,
                                                &presentSupport);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: vkGetPhysicalDeviceSurfaceSupportKHR() failed: {}", result);
    return false;
  }
  if (!presentSupport)
  {
    CLog::Log(LOGERROR, "Vulkan: Surface not supported by present queue.");
    return false;
  }

  // Get list of supported formats.
  uint32_t formatCount = 0;
  result =
      vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, m_vkSurface, &formatCount, nullptr);
  if (VK_SUCCESS != result)
  {
    CLog::Log(LOGERROR, "Vulkan: vkGetPhysicalDeviceSurfaceFormatsKHR() failed: {}", result);
    return false;
  }

  std::vector<VkSurfaceFormatKHR> formats(formatCount);
  result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, m_vkSurface, &formatCount,
                                                formats.data());
  if (VK_SUCCESS != result)
  {
    CLog::Log(LOGERROR, "Vulkan: vkGetPhysicalDeviceSurfaceFormatsKHR() failed: {}", result);
    return false;
  }

  std::vector<VkFormat> preferredFormats;
  if (format == SurfaceFormat::FORMAT_RGBA_32)
  {
    preferredFormats = PREFERRED_VK_FORMATS32;
  }
  else
  {
    preferredFormats = PREFERRED_VK_FORMATS16;
  }

  if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
  {
    m_vkSurfaceFormat.format = preferredFormats[0];
    m_vkSurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  }
  else
  {
    bool formatSet = false;
    for (VkSurfaceFormatKHR supportedFormat : formats)
    {
      for (const auto& preferredFormat : preferredFormats)
      {
        if (supportedFormat.format == preferredFormat)
        {
          m_vkSurfaceFormat = supportedFormat;
          m_vkSurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
          formatSet = true;
          break;
        }
      }
      if (formatSet)
      {
        break;
      }
    }
    if (!formatSet)
    {
      CLog::Log(LOGERROR, "Vulkan: Format not supported.");
      return false;
    }
  }

  VkSurfaceCapabilitiesKHR surfaceCaps;
  result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(deviceQueue->GetVulkanPhysicalDevice(),
                                                     m_vkSurface, &surfaceCaps);
  if (VK_SUCCESS != result)
  {
    CLog::Log(LOGERROR, "Vulkan: vkGetPhysicalDeviceSurfaceCapabilitiesKHR() failed: {}", result);
    return false;
  }

  constexpr auto kRequiredUsageFlags =
      VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  constexpr auto kOptionalUsageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  if ((surfaceCaps.supportedUsageFlags & kRequiredUsageFlags) != kRequiredUsageFlags)
  {
    CLog::Log(LOGERROR,
              "Vulkan: Surface doesn't support necessary usage. supportedUsageFlags: 0x{:X}",
              surfaceCaps.supportedUsageFlags);
  }

  m_vkImageUsageFlags =
      (kRequiredUsageFlags | kOptionalUsageFlags) & surfaceCaps.supportedUsageFlags;

  return true;
}

void CVulkanSurface::DeinitializeSurface()
{
  if (m_swapChain)
  {
    m_swapChain->DeinitializeSwapChain();
    m_swapChain = nullptr;
  }
  if (m_vkSurface)
  {
    vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, nullptr);
    m_vkSurface = VK_NULL_HANDLE;
  }
}

bool CVulkanSurface::SwapBuffers()
{
  return m_swapChain->SwapBuffers(m_imageSize);
}

bool CVulkanSurface::Reshape(const VkExtent2D& size)
{
  return CreateSwapChain(size);
}

bool CVulkanSurface::CreateSwapChain(const VkExtent2D& size)
{
  // Get Surface Information.
  VkSurfaceCapabilitiesKHR surfaceCaps;
  VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      m_deviceQueue->GetVulkanPhysicalDevice(), m_vkSurface, &surfaceCaps);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGFATAL, "Vulkan: vkGetPhysicalDeviceSurfaceCapabilitiesKHR() failed: {}", result);
    return false;
  }

  // TODO: Implemenet for various surface transforms. For now, we just use the current transform.
  auto vkTransform = surfaceCaps.currentTransform;
  assert(vkTransform == (vkTransform & surfaceCaps.supportedTransforms));

  // For Android, the current vulkan surface size may not match the new size
  // (the current window size), in that case, we will create a swap chain with
  // the requested new size, and vulkan surface size should match the swapchain
  // images size soon.
  VkExtent2D imageSize = size;
  if (imageSize.width == 0 || imageSize.height == 0)
  {
    // If width and height of the surface are 0xFFFFFFFF, it means the surface
    // size will be determined by the extent of a swapchain targeting the
    // surface. In that case, we will use the minImageExtent for the swapchain.
    const uint32_t kUndefinedExtent = UINT32_MAX;
    if (surfaceCaps.currentExtent.width == kUndefinedExtent &&
        surfaceCaps.currentExtent.height == kUndefinedExtent)
    {
      imageSize.width = surfaceCaps.minImageExtent.width;
      imageSize.height = surfaceCaps.minImageExtent.height;
    }
    else
    {
      imageSize.width = surfaceCaps.currentExtent.width;
      imageSize.height = surfaceCaps.currentExtent.height;
    }
  }

  assert(imageSize.width >= surfaceCaps.minImageExtent.width);
  assert(imageSize.height >= surfaceCaps.minImageExtent.height);
  assert(imageSize.width <= surfaceCaps.maxImageExtent.width);
  assert(imageSize.height <= surfaceCaps.maxImageExtent.height);
  assert(imageSize.width > 0u);
  assert(imageSize.height > 0u);

  if (m_imageSize.width == imageSize.width && m_imageSize.height == imageSize.height &&
      m_swapChain->GetState() == VK_SUCCESS)
  {
    return true;
  }

  m_imageSize = imageSize;

  auto swapChain = std::make_unique<CVulkanSwapChain>(m_deviceQueue, m_acquireNextImageTimeoutNs);
  if (!swapChain->InitializeSwapChain(m_vkSurface, m_vkSurfaceFormat, m_imageSize,
                                      m_vkImageUsageFlags, std::move(m_swapChain)))
  {
    return false;
  }

  m_swapChain = std::move(swapChain);
  ++m_swapChainGeneration;

  return true;
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
