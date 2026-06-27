/*
 *  Copyright (C) 2017-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "WinSystemWayland.h"
#ifdef TARGET_WEBOS
#include "WinSystemWaylandWebOS.h"
#endif
#include "rendering/vulkan/VulkanRenderSystem.h"

namespace KODI
{
namespace WINDOWING
{
namespace WAYLAND
{

#ifdef TARGET_WEBOS
using CWinSystemWaylandImpl = CWinSystemWaylandWebOS;
#else
using CWinSystemWaylandImpl = CWinSystemWayland;
#endif

class CWinSystemWaylandVulkan : public CWinSystemWaylandImpl, public CVulkanRenderSystem
{
public:
  static void Register();
  static std::unique_ptr<CWinSystemBase> CreateWinSystem();

  CRenderSystemBase *GetRenderSystem() override { return this; }
  bool InitWindowSystem() override;
  bool DestroyWindowSystem() override;

  bool CreateNewWindow(const std::string& name, bool fullScreen, RESOLUTION_INFO& res) override;
  bool DestroyWindow() override;
  void SetDirtyRegions(const CDirtyRegionList& dirtyRegions) override
  {
    //m_eglContext.SetDamagedRegions(dirtyRegions);
  }
  int GetBufferAge() override { return 0/*m_eglContext.GetBufferAge()*/; }

  bool BindTextureUploadContext() override;
  bool UnbindTextureUploadContext() override;
  bool HasContext() override;


protected:
  //bool CreateContext() override;
  void SetContextSize(CSizeInt size) override;
  void SetVSyncImpl(bool enable) override;
  void PresentRenderImpl(bool rendered) override;
};

} // namespace WAYLAND
} // namespace WINDOWING
} // namespace KODI
