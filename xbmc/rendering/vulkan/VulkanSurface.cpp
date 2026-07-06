/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanSurface.h"

#include "VulkanSwapChain.h"

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

CVulkanSurface::CVulkanSurface()
{
}

CVulkanSurface::~CVulkanSurface()
{
}

bool CVulkanSurface::InitializeSurface(CVulkanDeviceQueue* deviceQueue)
{
  return true;
}

void CVulkanSurface::DestroySurface()
{

}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
