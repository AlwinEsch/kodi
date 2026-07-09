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

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

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
}

bool CVulkanSurface::InitializeSurface(CVulkanDeviceQueue* deviceQueue, SurfaceFormat format)
{
  return true;
}

void CVulkanSurface::DeinitializeSurface()
{
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
