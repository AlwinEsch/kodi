/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanSwapChain.h"

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

CVulkanSwapChain::CVulkanSwapChain(CVulkanDeviceQueue* deviceQueue)
  : m_deviceQueue(deviceQueue)
{

}

CVulkanSwapChain::~CVulkanSwapChain()
{

}

bool CVulkanSwapChain::InitializeSwapChain()
{
  return true;
}

void CVulkanSwapChain::DestroySwapChain()
{
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
