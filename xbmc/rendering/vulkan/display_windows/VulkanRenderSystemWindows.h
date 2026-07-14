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
namespace WINDOWS
{

class CVulkanRenderSystemWindows : public CVulkanRenderSystem
{
public:
  CVulkanRenderSystemWindows();
  ~CVulkanRenderSystemWindows() override = default;

  bool InitRenderSystem() override;

  CVulkanInstance* GetVulkanInstance() override { return &m_vulkanInstance; }

private:
  CVulkanInstance m_vulkanInstance;
};


} // namespace WINDOWS
} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
