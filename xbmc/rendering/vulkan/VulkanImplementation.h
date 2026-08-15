/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <vulkan/vulkan_core.h>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

class CVulkanInstance;

class IVulkanImplementation
{
public:
  virtual ~IVulkanImplementation() = default;

  virtual bool IsAvailable() const = 0;
  virtual bool CreateSurface(void* window, VkSurfaceKHR& surface) const = 0;

  virtual CVulkanInstance* GetVulkanInstance() = 0;
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
