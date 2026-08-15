/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanScopedWrite.h"

#include "rendering/vulkan/VulkanSwapChain.h"

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

CVulkanScopedWrite::CVulkanScopedWrite(CVulkanSwapChain* swapChain) : m_swapChain(swapChain)
{
  m_success = m_swapChain->BeginWriteCurrentImage(
      &m_image, &m_imageIndex, &m_imageLayout, &m_imageUsage, &m_beginSemaphore, &m_endSemaphore);
  if (m_success) [[likely]]
  {
    assert(m_beginSemaphore != VK_NULL_HANDLE);
    assert(m_endSemaphore != VK_NULL_HANDLE);
  }
  else
  {
    assert(m_beginSemaphore == VK_NULL_HANDLE);
    assert(m_endSemaphore == VK_NULL_HANDLE);
  }
}

CVulkanScopedWrite::CVulkanScopedWrite(CVulkanScopedWrite&& other)
{
  *this = std::move(other);
}

CVulkanScopedWrite::~CVulkanScopedWrite()
{
  Reset();
}

const CVulkanScopedWrite& CVulkanScopedWrite::operator=(CVulkanScopedWrite&& other)
{
  Reset();

  std::swap(m_swapChain, other.m_swapChain);
  std::swap(m_success, other.m_success);
  std::swap(m_image, other.m_image);
  std::swap(m_imageIndex, other.m_imageIndex);
  std::swap(m_imageLayout, other.m_imageLayout);
  std::swap(m_imageUsage, other.m_imageUsage);
  std::swap(m_beginSemaphore, other.m_beginSemaphore);
  std::swap(m_endSemaphore, other.m_endSemaphore);

  return *this;
}

void CVulkanScopedWrite::Reset()
{
  if (m_success) [[likely]]
  {
    assert(m_beginSemaphore != VK_NULL_HANDLE);
    assert(m_endSemaphore != VK_NULL_HANDLE);
    m_swapChain->EndWriteCurrentImage();
  }
  else
  {
    assert(m_beginSemaphore == VK_NULL_HANDLE);
    assert(m_endSemaphore == VK_NULL_HANDLE);
  }

  m_swapChain = nullptr;
  m_success = false;
  m_image = VK_NULL_HANDLE;
  m_imageIndex = 0;
  m_imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  m_imageUsage = 0;
  m_beginSemaphore = VK_NULL_HANDLE;
  m_endSemaphore = VK_NULL_HANDLE;
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
