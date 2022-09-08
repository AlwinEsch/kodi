/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "addons/interface/renderer/IRenderHelper.h"
#include "addons/kodi-dev-kit/src/shared/SharedGL.h"
#include "addons/kodi-dev-kit/include/kodi/c-api/addon_base.h"

#include <memory>

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

class CRunningProcess;

class IOffscreenRenderProcess
{
public:
  IOffscreenRenderProcess();
  virtual ~IOffscreenRenderProcess() = default;
  virtual bool GetOffscreenRenderInfos(int& x, int& y, int& width, int& height, ADDON_HARDWARE_CONTEXT& context) = 0;

  uint64_t GetUsedId() const { return m_usedId; }
  void SetAddonOffscreenHdl(uint64_t hdl) { m_addonHdl = hdl; }
  uint64_t GetAddonOffscreenHdl() const { return m_addonHdl; }
  bool CreateRenderHelper(CRunningProcess* process);
  IRenderHelper* GetRenderHelper() { return m_renderHelper.get(); }

private:
  const uint64_t m_usedId;
  uint64_t m_addonHdl{0};

  std::unique_ptr<IRenderHelper> m_renderHelper;
};

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
