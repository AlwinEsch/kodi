/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanRenderSystemMetal.h"

#include <vulkan/vulkan_metal.h>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{
namespace METAL
{

CVulkanRenderSystemMetal::CVulkanRenderSystemMetal() : CVulkanRenderSystem()
{
}

bool CVulkanRenderSystemMetal::InitRenderSystem()
{
  std::vector<const char*> required_extensions = {VK_KHR_SURFACE_EXTENSION_NAME,
                                                  VK_EXT_METAL_SURFACE_EXTENSION_NAME};
  std::vector<const char*> required_layers = {};
  if (!m_vulkanInstance.Create(required_extensions, required_layers))
    return false;

  return CVulkanRenderSystem::InitRenderSystem();
}

} // namespace METAL
} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
