/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanSwapChain.h"

#include "VulkanDeviceQueue.h"
#include "VulkanInfo.h"
#include "VulkanUtils.h"
#include "utils/log.h"

#include <array>

namespace
{
// Minimum VkImages in a vulkan swap chain.
constexpr uint32_t VK_MIN_IMAGE_COUNT = 3u;
}; // namespace

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

CVulkanSwapChain::CVulkanSwapChain(CVulkanDeviceQueue* deviceQueue,
                                   uint64_t acquireNextImageTimeoutNs /* = UINT64_MAX*/)
  : m_deviceQueue(deviceQueue),
    m_acquireNextImageTimeoutNs(acquireNextImageTimeoutNs)
{
}

CVulkanSwapChain::~CVulkanSwapChain()
{
}

// WARNING: This function must be called from main thread only, otherwise it will cause a deadlock.
bool CVulkanSwapChain::InitializeSwapChain(VkSurfaceKHR surface,
                                           const VkSurfaceFormatKHR& surfaceFormat,
                                           const VkExtent2D& size,
                                           VkImageUsageFlags imageUsageFlags,
                                           std::unique_ptr<CVulkanSwapChain> oldSwapChain)
{
  VkDevice device = m_deviceQueue->GetVulkanDevice();
  VkResult result = VK_SUCCESS;

  VkSurfaceCapabilitiesKHR surfaceCaps;
  if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_deviceQueue->GetVulkanPhysicalDevice(), surface,
                                                &surfaceCaps) != VK_SUCCESS)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to get surface capabilities");
    return false;
  }

  // Initialize the swap chain.

  const std::array<VkCompositeAlphaFlagBitsKHR, 4> kCompositeAlphaBits = {
      VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
      VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
      VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
  };

  VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  for (auto compositeAlphaBit : kCompositeAlphaBits)
  {
    if (surfaceCaps.supportedCompositeAlpha & compositeAlphaBit)
    {
      compositeAlpha = compositeAlphaBit;
      break;
    }
  }

  // Figure out a suitable surface transform.
  VkSurfaceTransformFlagBitsKHR preTransform;
  if (surfaceCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
  {
    preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  }
  else
  {
    preTransform = surfaceCaps.currentTransform;
  }

  auto minImageCount = std::max(surfaceCaps.minImageCount, VK_MIN_IMAGE_COUNT);
  auto swapChain = std::make_unique<CVulkanSwapChain>(m_deviceQueue);

  VkSwapchainCreateInfoKHR info{
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .pNext = VK_NULL_HANDLE,
      .flags = 0,
      .surface = surface,
      .minImageCount = minImageCount,
      .imageFormat = surfaceFormat.format,
      .imageColorSpace = surfaceFormat.colorSpace,
      .imageExtent = size,
      .imageArrayLayers = 1,
      .imageUsage = imageUsageFlags,
      .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 0,
      .pQueueFamilyIndices = VK_NULL_HANDLE,
      .preTransform = preTransform,
      .compositeAlpha = compositeAlpha,
      .presentMode = VK_PRESENT_MODE_FIFO_KHR,
      .clipped = VK_TRUE,
      .oldSwapchain = VK_NULL_HANDLE,
  };

  if (oldSwapChain) [[likely]]
  {
    // TODO: Implement multiple threads support.
    std::unique_lock lock(oldSwapChain->m_criticalSection);

    info.oldSwapchain = oldSwapChain->m_vkSwapChain;
    m_pendingSemaphoresQueue = std::move(oldSwapChain->m_pendingSemaphoresQueue);
    oldSwapChain->m_pendingSemaphoresQueue.clear();
  }

  VkSwapchainKHR newSwapChain{VK_NULL_HANDLE};
  result = vkCreateSwapchainKHR(device, &info, nullptr, &newSwapChain);

  if (oldSwapChain) [[likely]]
  {
    oldSwapChain->DeinitializeSwapChain();
  }

  if (result != VK_SUCCESS) [[unlikely]]
  {
    CLog::Log(LOGFATAL, "Vulkan: vkCreateSwapchainKHR() failed: {}", result);
    return false;
  }

  // Initialize the swap chain images.

  uint32_t imageCount = 0;
  result = vkGetSwapchainImagesKHR(device, newSwapChain, &imageCount, nullptr);
  if (result != VK_SUCCESS) [[unlikely]]
  {
    CLog::Log(LOGFATAL, "Vulkan: vkGetSwapchainImagesKHR(nullptr) failed: {}", result);
  }

  std::vector<VkImage> images(imageCount);
  result = vkGetSwapchainImagesKHR(device, newSwapChain, &imageCount, images.data());
  if (result != VK_SUCCESS) [[unlikely]]
  {
    CLog::Log(LOGFATAL, "Vulkan: vkGetSwapchainImagesKHR(images) failed: {}", result);
  }

  m_images.resize(imageCount);
  for (uint32_t i = 0; i < imageCount; ++i)
  {
    auto& image_data = m_images[i];
    image_data.image = images[i];
  }

  m_size = info.imageExtent;
  m_vkSwapChain = newSwapChain;
  m_vkImageUsageFlags = imageUsageFlags;

  return true;
}

// WARNING: This function must be called from main thread only, otherwise it will cause a deadlock.
void CVulkanSwapChain::DeinitializeSwapChain()
{
  VkDevice device = m_deviceQueue->GetVulkanDevice();

  if (!m_pendingSemaphoresQueue.empty())
  {
    // TODO: Implement multiple threads support where pending semaphores then becomes usable.
    for (auto& pendingSemaphores : m_pendingSemaphoresQueue)
    {
      vkDestroySemaphore(device, pendingSemaphores.acquireSemaphore, nullptr);
      vkDestroySemaphore(device, pendingSemaphores.presentSemaphore, nullptr);
    }
    m_pendingSemaphoresQueue.clear();
  }

  for (auto& image : m_images)
  {
    vkDestroySemaphore(device, image.acquireSemaphore, nullptr);
    vkDestroySemaphore(device, image.presentSemaphore, nullptr);
  }
  m_images.clear();

  if (m_vkSwapChain != VK_NULL_HANDLE)
  {
    // vkDestroySwapchainKHR() will hang on X11, after resuming from hibernate.
    // It is because a Xserver issue. To workaround it, we will not call
    // vkDestroySwapchainKHR(), if the problem is detected. When the problem is
    // detected, we will consider it as context lost, so the GPU process will
    // tear down all resources, and a new GPU process will be created. So it is OK
    // to leak this swapchain.
    //
    // Note: This info and check is taken from Chromium's Vulkan implementation,
    // see https://source.chromium.org/chromium/chromium/src/+/main:gpu/vulkan/vulkan_swap_chain.cc
    if (!m_destroySwapchainWillHang)
      vkDestroySwapchainKHR(device, m_vkSwapChain, nullptr);
    m_vkSwapChain = VK_NULL_HANDLE;
  }
}

bool CVulkanSwapChain::AcquireNextSwapchainImage()
{
  VkDevice device = m_deviceQueue->GetVulkanDevice();

  VkSemaphore acquireSemaphore = VK_NULL_HANDLE;
  VkSemaphore presentSemaphore = VK_NULL_HANDLE;
  if (!GetOrCreateSemaphores(acquireSemaphore, presentSemaphore))
    return false;

  uint32_t nextImage;
  VkResult result = vkAcquireNextImageKHR(device, m_vkSwapChain, m_acquireNextImageTimeoutNs,
                                          acquireSemaphore, VK_NULL_HANDLE, &nextImage);

  if (result == VK_TIMEOUT) [[unlikely]]
  {
    CLog::Log(LOGFATAL, "Vulkan: vkAcquireNextImageKHR() hangs.");
    vkDestroySemaphore(device, acquireSemaphore, nullptr);
    vkDestroySemaphore(device, presentSemaphore, nullptr);
    m_vkState = VK_ERROR_SURFACE_LOST_KHR;
    m_destroySwapchainWillHang = true;
    return false;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) [[unlikely]]
  {
    CLog::Log(LOGFATAL, "Vulkan: vkAcquireNextImageKHR() failed: {}", result);
    vkDestroySemaphore(device, acquireSemaphore, nullptr);
    vkDestroySemaphore(device, presentSemaphore, nullptr);
    m_vkState = result;
    return false;
  }

  m_acquiredImage.emplace(nextImage);
  m_newAcquired = true;

  // For the previous use of the image, |currentImageData.acquireSemaphore|
  // has been wait on for the compositing work last time,
  // and |currentImageData.presentSemaphore| has been wait on by present
  // engine for presenting the image last time, so those two semaphores should
  // be free for reusing when |num_images() * 2| frames are passed, because it
  // is impossible there are more than |num_images() * 2| frames are in flight.
  auto& currentImageData = m_images[nextImage];
  ReturnSemaphores(currentImageData.acquireSemaphore, currentImageData.presentSemaphore);
  currentImageData.acquireSemaphore = acquireSemaphore;
  currentImageData.presentSemaphore = presentSemaphore;

  return true;
}

bool CVulkanSwapChain::GetOrCreateSemaphores(VkSemaphore& acquireSemaphore,
                                             VkSemaphore& presentSemaphore)
{
  // When pending semaphores are more than |num_images() * 2|, we will
  // assume the semaphores at the front of the queue has been signaled
  // and can be reused (because it is impossible there are more than
  // |num_images() * 2| frames are in flight). Otherwise, new semaphores
  // will be created.
  if (m_pendingSemaphoresQueue.size() >= AmmountSwapChainImages() * 2) [[likely]]
  {
    const auto& semaphores = m_pendingSemaphoresQueue.front();
    assert(semaphores.acquireSemaphore != VK_NULL_HANDLE);
    assert(semaphores.presentSemaphore != VK_NULL_HANDLE);

    m_pendingSemaphoresQueue.pop_front();
    acquireSemaphore = semaphores.acquireSemaphore;
    presentSemaphore = semaphores.presentSemaphore;
    return true;
  }

  VkDevice device = m_deviceQueue->GetVulkanDevice();
  acquireSemaphore = CreateSemaphore(device);
  if (acquireSemaphore == VK_NULL_HANDLE)
    return false;

  presentSemaphore = CreateSemaphore(device);
  if (presentSemaphore == VK_NULL_HANDLE)
  {
    // Failed to get or create semaphores, release resources.
    vkDestroySemaphore(device, acquireSemaphore, nullptr);
    return false;
  }

  return true;
}

VkSemaphore CVulkanSwapChain::CreateSemaphore(VkDevice vk_device)
{
  // Generic semaphore creation structure.
  constexpr VkSemaphoreCreateInfo semaphoreCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
  };

  VkSemaphore vkSemaphore = VK_NULL_HANDLE;
  auto result = vkCreateSemaphore(vk_device, &semaphoreCreateInfo, nullptr, &vkSemaphore);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGFATAL, "Vulkan: vkCreateSemaphore() failed: {}", result);
  }

  return vkSemaphore;
}

void CVulkanSwapChain::ReturnSemaphores(VkSemaphore acquireSemaphore, VkSemaphore presentSemaphore)
{
  assert(acquireSemaphore != VK_NULL_HANDLE && presentSemaphore != VK_NULL_HANDLE);

  if (acquireSemaphore == VK_NULL_HANDLE)
  {
    return;
  }

  m_pendingSemaphoresQueue.push_back({acquireSemaphore, presentSemaphore});
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
