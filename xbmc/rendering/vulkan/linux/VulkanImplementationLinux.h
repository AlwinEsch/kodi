/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "rendering/vulkan/VulkanImplementation.h"
#include "rendering/vulkan/VulkanInstance.h"

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{
namespace LINUX
{

class CVulkanImplementationLinux : public IVulkanImplementation
{
public:
  CVulkanImplementationLinux() = default;
  ~CVulkanImplementationLinux() override = default;

  bool IsAvailable() const override;
  bool CreateSurface(void* window, VkSurfaceKHR& surface) const override;
  CVulkanInstance* GetVulkanInstance() override { return &m_instance; }

private:
  CVulkanInstance m_instance;
};

} // namespace LINUX
} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
