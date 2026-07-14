/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanSwapChain.h"

#include "rendering/vulkan/VulkanDeviceQueue.h"
#include "rendering/vulkan/VulkanFenceHelper.h"
#include "rendering/vulkan/VulkanInfo.h"
#include "rendering/vulkan/VulkanUtils.h"
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

using KODI::RENDERING::VULKAN::UTILS::ErrorString;

namespace
{

VkSemaphore CreateSemaphore(VkDevice vkDevice)
{
  // Generic semaphore creation structure.
  constexpr VkSemaphoreCreateInfo semaphoreCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
  };

  VkSemaphore vkSemaphore = VK_NULL_HANDLE;
  auto result = vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, nullptr, &vkSemaphore);
  if (result != VK_SUCCESS)
  {
    CLog::Log(LOGFATAL, "Vulkan: vkCreateSemaphore() failed: {}", ErrorString(result));
  }

  return vkSemaphore;
}

} // namespace

CVulkanSwapChain::CVulkanSwapChain(CVulkanDeviceQueue* deviceQueue,
                                   uint64_t acquireNextImageTimeoutNs /* = UINT64_MAX*/)
  : m_deviceQueue(deviceQueue),
    m_acquireNextImageTimeoutNs(acquireNextImageTimeoutNs)
{
}

CVulkanSwapChain::~CVulkanSwapChain()
{
}

bool CVulkanSwapChain::Initialize(VkSurfaceKHR surface,
                                  const VkSurfaceFormatKHR& surfaceFormat,
                                  const VkRect2D& size,
                                  uint32_t minImageCount,
                                  VkImageUsageFlags imageUsageFlags,
                                  VkSurfaceTransformFlagBitsKHR preTransform,
                                  VkCompositeAlphaFlagBitsKHR compositeAlpha,
                                  std::unique_ptr<CVulkanSwapChain> oldSwapChain)
{
  m_incrementalPresentSupported =
      m_deviceQueue->SupportsExtension(VK_KHR_INCREMENTAL_PRESENT_EXTENSION_NAME);

  return InitializeSwapChain(surface, surfaceFormat, size, minImageCount, imageUsageFlags,
                             preTransform, compositeAlpha, std::move(oldSwapChain)) &&
         InitializeSwapImages(surfaceFormat) && InitializeSemaphores();
}

void CVulkanSwapChain::Destroy()
{
  assert(!m_isWriting);

  DestroySemaphores();
  DestroySwapImages();
  DestroySwapChain();
}

bool CVulkanSwapChain::InitializeSwapChain(VkSurfaceKHR surface,
                                           const VkSurfaceFormatKHR& surfaceFormat,
                                           const VkRect2D& size,
                                           uint32_t minImageCount,
                                           VkImageUsageFlags imageUsageFlags,
                                           VkSurfaceTransformFlagBitsKHR preTransform,
                                           VkCompositeAlphaFlagBitsKHR compositeAlpha,
                                           std::unique_ptr<CVulkanSwapChain> oldSwapChain)
{
  VkDevice device = m_deviceQueue->VulkanDevice();
  VkResult result = VK_SUCCESS;

  VkSwapchainCreateInfoKHR info{
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .pNext = nullptr,
      .flags = 0,
      .surface = surface,
      .minImageCount = minImageCount,
      .imageFormat = surfaceFormat.format,
      .imageColorSpace = surfaceFormat.colorSpace,
      .imageExtent = size.extent,
      .imageArrayLayers = 1,
      .imageUsage = imageUsageFlags,
      .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 0,
      .pQueueFamilyIndices = nullptr,
      .preTransform = preTransform,
      .compositeAlpha = compositeAlpha,
      .presentMode = VK_PRESENT_MODE_FIFO_KHR,
      .clipped = true,
      .oldSwapchain = VK_NULL_HANDLE,
  };

  if (oldSwapChain) [[likely]]
  {
    // TODO: Implement multiple threads support.
    std::unique_lock lock(oldSwapChain->m_criticalSection);

    info.oldSwapchain = oldSwapChain->m_swapchain;
    m_pendingSemaphoresQueue = std::move(oldSwapChain->m_pendingSemaphoresQueue);
    oldSwapChain->m_pendingSemaphoresQueue.clear();
  }

  VkSwapchainKHR newSwapChain = VK_NULL_HANDLE;
  result = vkCreateSwapchainKHR(device, &info, nullptr, &newSwapChain);

  if (oldSwapChain) [[likely]]
  {
    oldSwapChain->Destroy();
  }

  if (result != VK_SUCCESS) [[unlikely]]
  {
    CLog::Log(LOGFATAL, "Vulkan: Failed to create swapchain: {}", ErrorString(result));
    return false;
  }

  m_swapchain = newSwapChain;
  m_size = size;
  m_imageUsage = imageUsageFlags;

  return true;
}

void CVulkanSwapChain::DestroySwapChain()
{
  if (m_swapchain != VK_NULL_HANDLE)
  {
    vkDestroySwapchainKHR(m_deviceQueue->VulkanDevice(), m_swapchain, nullptr);
    m_swapchain = VK_NULL_HANDLE;
  }
}

bool CVulkanSwapChain::InitializeSwapImages(const VkSurfaceFormatKHR& surfaceFormat)
{
  VkDevice device = m_deviceQueue->VulkanDevice();
  VkResult result = VK_SUCCESS;

  uint32_t imageCount;
  result = vkGetSwapchainImagesKHR(device, m_swapchain, &imageCount, nullptr);
  if (result != VK_SUCCESS) [[unlikely]]
  {
    CLog::Log(LOGFATAL, "Vulkan: Failed to get swapchain images count, Error: {}",
              ErrorString(result));
    return false;
  }

  std::vector<VkImage> swapchain_images(imageCount);
  result = vkGetSwapchainImagesKHR(device, m_swapchain, &imageCount, swapchain_images.data());
  if (result != VK_SUCCESS) [[unlikely]]
  {
    CLog::Log(LOGFATAL, "Vulkan: Failed to get swapchain images data, Error: {}",
              ErrorString(result));
    return false;
  }

  m_images.resize(imageCount);
  for (uint32_t i = 0; i < imageCount; ++i)
  {
    auto& image_data = m_images[i];
    image_data.image = swapchain_images[i];
    image_data.acquireSemaphore = VK_NULL_HANDLE;
    image_data.presentSemaphore = VK_NULL_HANDLE;
  }
  return true;
}

void CVulkanSwapChain::DestroySwapImages()
{
  VkDevice device = m_deviceQueue->VulkanDevice();
  for (auto& image : m_images)
  {
    vkDestroySemaphore(device, image.acquireSemaphore, nullptr);
    vkDestroySemaphore(device, image.presentSemaphore, nullptr);
  }
  m_images.clear();
}

bool CVulkanSwapChain::InitializeSemaphores()
{
  return AcquireNextSwapchainImage();
}

void CVulkanSwapChain::DestroySemaphores()
{
  VkDevice device = m_deviceQueue->VulkanDevice();

  if (!m_pendingSemaphoresQueue.empty()) [[unlikely]]
  {
    for (auto& pendingSemaphores : m_pendingSemaphoresQueue)
    {
      vkDestroySemaphore(device, pendingSemaphores.acquireSemaphore, nullptr);
      vkDestroySemaphore(device, pendingSemaphores.presentSemaphore, nullptr);
    }
    m_pendingSemaphoresQueue.clear();
  }
}

bool CVulkanSwapChain::PostSubBuffer(const VkRect2D& rect)
{
  if (!PresentBuffer(rect)) [[unlikely]]
  {
    return false;
  }

  if (!AcquireNextSwapchainImage()) [[unlikely]]
  {
    return false;
  }

  return true;
}

bool CVulkanSwapChain::GetImage(uint32_t index,
                                VkImage* image,
                                VkImageLayout* layout,
                                VkSemaphore* acquireSemaphore,
                                VkSemaphore* presentSemaphore) const
{
  std::unique_lock lock(m_criticalSection);
  if (index >= m_images.size())
  {
    CLog::Log(LOGERROR, "Vulkan: Invalid swapchain image index: {}", index);
    return false;
  }
  const auto& image_data = m_images[index];
  *image = image_data.image;
  *layout = image_data.imageLayout;
  *acquireSemaphore = image_data.acquireSemaphore;
  *presentSemaphore = image_data.presentSemaphore;
  return true;
}

bool CVulkanSwapChain::BeginWriteCurrentImage(VkImage* image,
                                              uint32_t* image_index,
                                              VkImageLayout* image_layout,
                                              VkImageUsageFlags* image_usage,
                                              VkSemaphore* begin_semaphore,
                                              VkSemaphore* end_semaphore)
{
  std::unique_lock lock(m_criticalSection);

  //DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  assert(image);
  assert(image_index);
  assert(image_layout);
  assert(image_usage);
  assert(begin_semaphore);
  assert(end_semaphore);
  assert(!m_isWriting);

  if (m_state != VK_SUCCESS) [[unlikely]]
  {
    return false;
  }

  if (!m_newAcquired) [[unlikely]]
  {
    return false;
  }

  auto& currentImageData = m_images[*m_acquiredImage];

  if (!m_newAcquired) [[unlikely]]
  {
    // In this case, {Begin,End}WriteCurrentImage has been called, but
    // PostSubBuffer() is not call, so |acquire_semaphore| has been wait on for
    // the previous write request, release it with FenceHelper.
    m_deviceQueue->FenceHelper()->EnqueueSemaphoreCleanupForSubmittedWork(
        currentImageData.acquireSemaphore);
    // Use |end_semaphore| from previous write as |begin_semaphore| for the new
    // write request, and create a new semaphore for |end_semaphore|.
    currentImageData.acquireSemaphore = currentImageData.presentSemaphore;
    currentImageData.presentSemaphore = CreateSemaphore(m_deviceQueue->VulkanDevice());
    if (currentImageData.presentSemaphore == VK_NULL_HANDLE) [[unlikely]]
    {
      return false;
    }
  }

  *image = currentImageData.image;
  *image_index = *m_acquiredImage;
  *image_layout = currentImageData.imageLayout;
  *image_usage = m_imageUsage;
  *begin_semaphore = currentImageData.acquireSemaphore;
  *end_semaphore = currentImageData.presentSemaphore;
  m_isWriting = true;

  return true;
}

void CVulkanSwapChain::EndWriteCurrentImage()
{
  std::unique_lock lock(m_criticalSection);

  //DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  assert(m_isWriting);
  assert(m_acquiredImage);

  auto& currentImageData = m_images[*m_acquiredImage];
  currentImageData.imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  m_isWriting = false;
  m_newAcquired = false;
}

bool CVulkanSwapChain::GetOrCreateSemaphores(VkSemaphore* acquire_semaphore,
                                             VkSemaphore* present_semaphore)
{
  // When pending semaphores are more than |num_images() * 2|, we will
  // assume the semaphores at the front of the queue has been signaled
  // and can be reused (because it is impossible there are more than
  // |num_images() * 2| frames are in flight). Otherwise, new semaphores
  // will be created.
  if (m_pendingSemaphoresQueue.size() >= m_images.size() * 2) [[likely]]
  {
    const auto& semaphores = m_pendingSemaphoresQueue.front();
    assert(semaphores.acquireSemaphore != VK_NULL_HANDLE);
    assert(semaphores.presentSemaphore != VK_NULL_HANDLE);
    m_pendingSemaphoresQueue.pop_front();
    *acquire_semaphore = semaphores.acquireSemaphore;
    *present_semaphore = semaphores.presentSemaphore;
    return true;
  }

  VkDevice device = m_deviceQueue->VulkanDevice();

  // Generic semaphore creation structure.
  constexpr VkSemaphoreCreateInfo semaphoreCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
  };

  VkResult result;

  result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, acquire_semaphore);
  if (result != VK_SUCCESS || *acquire_semaphore == VK_NULL_HANDLE)
  {
    CLog::Log(LOGFATAL, "Vulkan: Failed to create acquire semaphore: {}", ErrorString(result));
    return false;
  }

  result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, present_semaphore);
  if (result != VK_SUCCESS || *present_semaphore == VK_NULL_HANDLE)
  {
    CLog::Log(LOGFATAL, "Vulkan: Failed to create present semaphore: {}", ErrorString(result));
    vkDestroySemaphore(device, *acquire_semaphore, nullptr);
    return false;
  }

  return true;
}

void CVulkanSwapChain::ReturnSemaphores(VkSemaphore acquireSemaphore, VkSemaphore presentSemaphore)
{
  assert((acquireSemaphore != VK_NULL_HANDLE) == (presentSemaphore != VK_NULL_HANDLE));

  if (acquireSemaphore == VK_NULL_HANDLE)
    return;

  m_pendingSemaphoresQueue.push_back({acquireSemaphore, presentSemaphore});
}

bool CVulkanSwapChain::AcquireNextSwapchainImage()
{
  VkDevice device = m_deviceQueue->VulkanDevice();

  VkSemaphore acquireSemaphore = VK_NULL_HANDLE;
  VkSemaphore presentSemaphore = VK_NULL_HANDLE;
  if (!GetOrCreateSemaphores(&acquireSemaphore, &presentSemaphore))
    return false;

  uint32_t next_image;
  VkResult result = vkAcquireNextImageKHR(device, m_swapchain, m_acquireNextImageTimeoutNs,
                                          acquireSemaphore, VK_NULL_HANDLE, &next_image);
  if (result == VK_TIMEOUT) [[unlikely]]
  {
    CLog::Log(LOGERROR, "vkAcquireNextImageKHR() hangs.");
    vkDestroySemaphore(device, acquireSemaphore, nullptr);
    vkDestroySemaphore(device, presentSemaphore, nullptr);
    m_state = VK_ERROR_SURFACE_LOST_KHR;
    m_destroySwapchainWillHang = true;
    return false;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) [[unlikely]]
  {
    CLog::Log(LOGFATAL, "vkAcquireNextImageKHR() failed: {}", ErrorString(result));
    vkDestroySemaphore(device, acquireSemaphore, nullptr);
    vkDestroySemaphore(device, presentSemaphore, nullptr);
    m_state = result;
    return false;
  }

  m_acquiredImage.emplace(next_image);
  m_newAcquired = true;

  // For the previous use of the image, |currentImageData.acquire_semaphore|
  // has been wait on for the compositing work last time,
  // and |currentImageData.present_semaphore| has been wait on by present
  // engine for presenting the image last time, so those two semaphores should
  // be free for reusing when |num_images() * 2| frames are passed, because it
  // is impossible there are more than |num_images() * 2| frames are in flight.
  auto& currentImageData = m_images[next_image];
  ReturnSemaphores(currentImageData.acquireSemaphore, currentImageData.presentSemaphore);
  currentImageData.acquireSemaphore = acquireSemaphore;
  currentImageData.presentSemaphore = presentSemaphore;

  return true;
}

bool CVulkanSwapChain::PresentBuffer(const VkRect2D& rect)
{
  assert(m_state == VK_SUCCESS);
  assert(m_acquiredImage.has_value());

  auto& currentImageData = m_images[*m_acquiredImage];
  assert(currentImageData.presentSemaphore != VK_NULL_HANDLE);

  VkRectLayerKHR rect_layer = {
      .offset = {rect.offset.x, rect.offset.y},
      .extent = {rect.extent.width, rect.extent.height},
      .layer = 0,
  };

  VkPresentRegionKHR present_region = {
      .rectangleCount = 1,
      .pRectangles = &rect_layer,
  };

  VkPresentRegionsKHR present_regions = {
      .sType = VK_STRUCTURE_TYPE_PRESENT_REGIONS_KHR,
      .pNext = nullptr,
      .swapchainCount = 1,
      .pRegions = &present_region,
  };

  VkPresentInfoKHR present_info = {
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext = m_incrementalPresentSupported ? &present_regions : nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &currentImageData.presentSemaphore,
      .swapchainCount = 1,
      .pSwapchains = &m_swapchain,
      .pImageIndices = &m_acquiredImage.value(),
      .pResults = nullptr,
  };

  VkQueue queue = m_deviceQueue->VulkanQueue();
  auto result = vkQueuePresentKHR(queue, &present_info);
  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) [[unlikely]]
  {
    CLog::Log(LOGFATAL, "vkQueuePresentKHR() failed: {}", ErrorString(result));
    m_state = result;
    return false;
  }
  if (result == VK_SUBOPTIMAL_KHR)
  {
    CLog::Log(LOGWARNING, "Swapchain is suboptimal.");
  }

  m_acquiredImage.reset();

  return true;
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
