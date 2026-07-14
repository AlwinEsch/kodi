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
#include "rendering/vulkan/VulkanUtils.h"
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

using KODI::RENDERING::VULKAN::UTILS::ErrorString;

namespace
{

const std::vector<VkFormat> PREFERRED_VK_FORMATS32 = {
    VK_FORMAT_R8G8B8A8_SRGB,
    VK_FORMAT_B8G8R8A8_SRGB,
    VK_FORMAT_A8B8G8R8_SRGB_PACK32,
    //VK_FORMAT_B8G8R8A8_UNORM, // FORMAT_BGRA8888,
    //VK_FORMAT_R8G8B8A8_UNORM, // FORMAT_RGBA8888,
};

const std::vector<VkFormat> PREFERRED_VK_FORMATS16 = {
    VK_FORMAT_R5G6B5_UNORM_PACK16, // FORMAT_RGB565,
};

// Minimum VkImages in a vulkan swap chain.
constexpr uint32_t MIN_IMAGE_COUNT = 3u;

} // namespace

CVulkanSurface::CVulkanSurface(VkInstance vkInstance,
                               VkSurfaceKHR vkSurface,
                               uint64_t acquireNextImageTimeoutNs /* = UINT64_MAX*/)
  : m_vkInstance(vkInstance),
    m_vkSurface(vkSurface),
    m_acquireNextImageTimeoutNs(acquireNextImageTimeoutNs)
{
}

CVulkanSurface::~CVulkanSurface()
{
  assert(!m_swapChain);
}

bool CVulkanSurface::Initialize(CVulkanDeviceQueue* deviceQueue, SurfaceFormat format)
{
  m_deviceQueue = deviceQueue;

  VkBool32 presentSupport;
  VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(m_deviceQueue->VulkanPhysicalDevice(),
                                                         m_deviceQueue->VulkanQueueIndex(),
                                                         m_vkSurface, &presentSupport);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: vkGetPhysicalDeviceSurfaceSupportKHR() failed: {}",
              ErrorString(result));
    return false;
  }
  if (!presentSupport)
  {
    CLog::Log(LOGERROR, "Vulkan: Surface not supported by present queue.");
    return false;
  }

  // Get list of supported formats.
  uint32_t formatCount = 0;
  result = vkGetPhysicalDeviceSurfaceFormatsKHR(m_deviceQueue->VulkanPhysicalDevice(),
                                                m_vkSurface, &formatCount, nullptr);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: vkGetPhysicalDeviceSurfaceFormatsKHR() failed: {}",
              ErrorString(result));
    return false;
  }

  std::vector<VkSurfaceFormatKHR> formats(formatCount);
  result = vkGetPhysicalDeviceSurfaceFormatsKHR(m_deviceQueue->VulkanPhysicalDevice(),
                                                m_vkSurface, &formatCount, formats.data());
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: vkGetPhysicalDeviceSurfaceFormatsKHR() failed: {}",
              ErrorString(result));
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

  if (formats.size() == 1 && VK_FORMAT_UNDEFINED == formats[0].format)
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
  result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_deviceQueue->VulkanPhysicalDevice(),
                                                     m_vkSurface, &surfaceCaps);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR() failed: {}",
              ErrorString(result));
    return false;
  }

  constexpr auto kRequiredUsageFlags =
      VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  constexpr auto kOptionalUsageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  if ((surfaceCaps.supportedUsageFlags & kRequiredUsageFlags) != kRequiredUsageFlags)
  {
    CLog::Log(LOGERROR,
              "Vulkan: Vulkan surface doesn't support necessary usage. supportedUsageFlags: 0x{:X}",
              surfaceCaps.supportedUsageFlags);
  }

  m_vkImageUsageFlags = (kRequiredUsageFlags | kOptionalUsageFlags) & surfaceCaps.supportedUsageFlags;

  return true;
}

void CVulkanSurface::Destroy()
{
  if (m_swapChain)
  {
    m_swapChain->Destroy();
    m_swapChain = nullptr;
  }
}

bool CVulkanSurface::Reshape(
    const VkRect2D& size,
    VkSurfaceTransformFlagBitsKHR vkTransform /* = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR*/)
{
  return CreateSwapChain(size, vkTransform);
}

bool CVulkanSurface::SwapBuffers()
{
  return PostSubBuffer(m_vkImageSize);
}

bool CVulkanSurface::PostSubBuffer(const VkRect2D& rect)
{
  bool result = m_swapChain->PostSubBuffer(rect);

  return result;
}

bool CVulkanSurface::CreateSwapChain(const VkRect2D& size,
                                     VkSurfaceTransformFlagBitsKHR vkTransform)
{
  // Get Surface Information.
  VkSurfaceCapabilitiesKHR surfaceCaps;
  VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      m_deviceQueue->VulkanPhysicalDevice(), m_vkSurface, &surfaceCaps);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGFATAL, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR() failed: {}",
              ErrorString(result));
    return false;
  }

  // TODO: Implement transform support. The current transform is the transform that the surface is
  // currently using, and it must be one of the supported transforms.
  VkSurfaceTransformFlagBitsKHR vkTransformUsed =
      vkTransform != 0 ? vkTransform : surfaceCaps.currentTransform;
  assert(vkTransformUsed == (vkTransformUsed & surfaceCaps.supportedTransforms));

  // For Android, the current vulkan surface size may not match the new size
  // (the current window size), in that case, we will create a swap chain with
  // the requested new size, and vulkan surface size should match the swapchain
  // images size soon.
  VkRect2D imageSize = size;
  if (imageSize.extent.width == 0 || imageSize.extent.height == 0)
  {
    // If width and height of the surface are 0xFFFFFFFF, it means the surface
    // size will be determined by the extent of a swapchain targeting the
    // surface. In that case, we will use the minImageExtent for the swapchain.
    const uint32_t kUndefinedExtent = UINT32_MAX;
    if (surfaceCaps.currentExtent.width == kUndefinedExtent &&
        surfaceCaps.currentExtent.height == kUndefinedExtent)
    {
      imageSize.extent.width = surfaceCaps.minImageExtent.width;
      imageSize.extent.height = surfaceCaps.minImageExtent.height;
    }
    else
    {
      imageSize.extent.width = surfaceCaps.currentExtent.width;
      imageSize.extent.height = surfaceCaps.currentExtent.height;
    }
    if (vkTransformUsed == VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR ||
        vkTransformUsed == VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR)
    {
      std::swap(imageSize.extent.width, imageSize.extent.height);
    }
  }

  assert(static_cast<uint32_t>(imageSize.extent.width) >= surfaceCaps.minImageExtent.width);
  assert(static_cast<uint32_t>(imageSize.extent.height) >= surfaceCaps.minImageExtent.height);
  assert(static_cast<uint32_t>(imageSize.extent.width) <= surfaceCaps.maxImageExtent.width);
  assert(static_cast<uint32_t>(imageSize.extent.height) <= surfaceCaps.maxImageExtent.height);
  assert(static_cast<uint32_t>(imageSize.extent.width) > 0u);
  assert(static_cast<uint32_t>(imageSize.extent.height) > 0u);

  if (m_vkImageSize.extent.width == imageSize.extent.width &&
      m_vkImageSize.extent.height == imageSize.extent.height && m_vkTransform == vkTransformUsed &&
      m_swapChain->State() == VK_SUCCESS)
  {
    return true;
  }

  m_vkImageSize = imageSize;
  m_vkTransform = vkTransformUsed;

  const VkCompositeAlphaFlagBitsKHR kCompositeAlphaBits[] = {
      VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
      VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
      VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
  };

  for (auto compositeAlphaBit : kCompositeAlphaBits)
  {
    if (surfaceCaps.supportedCompositeAlpha & compositeAlphaBit)
    {
      m_vkCompositeAlpha = compositeAlphaBit;
      break;
    }
  }

  auto swapChain = std::make_unique<CVulkanSwapChain>(m_deviceQueue, m_acquireNextImageTimeoutNs);
  // Create swap chain.
  auto minImageCount = std::max(surfaceCaps.minImageCount, MIN_IMAGE_COUNT);
  if (!swapChain->Initialize(m_vkSurface, m_vkSurfaceFormat, m_vkImageSize, minImageCount,
                             m_vkImageUsageFlags, m_vkTransform, m_vkCompositeAlpha,
                             std::move(m_swapChain)))
  {
    return false;
  }

  m_swapChain = std::move(swapChain);
  m_vkSwapchain = m_swapChain->vkSwapchain();
  ++m_swapChainGeneration;
  return true;
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
