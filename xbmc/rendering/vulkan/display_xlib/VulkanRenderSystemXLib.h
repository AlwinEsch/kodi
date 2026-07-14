/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "rendering/vulkan/VulkanInstance.h"
#include "rendering/vulkan/VulkanRenderSystem.h"

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{
namespace XLIB
{

class CVulkanRenderSystemXLib : public CVulkanRenderSystem
{
public:
  CVulkanRenderSystemXLib();
  ~CVulkanRenderSystemXLib() override = default;

  bool InitRenderSystem() override;

  CVulkanInstance* GetVulkanInstance() override { return &m_vulkanInstance; }

private:
  CVulkanInstance m_vulkanInstance;
};

} // namespace XLIB
} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
