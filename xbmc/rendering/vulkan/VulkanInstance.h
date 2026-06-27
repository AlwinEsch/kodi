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

class CVulkanInstance
{
public:
  CVulkanInstance() = default;
  ~CVulkanInstance() = default;

  bool Create();
  void Destroy();

  VkInstance GetInstance() const { return m_instance; }

private:
  VkInstance m_instance{VK_NULL_HANDLE};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
