/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "ScreenSaver.h"

#include "addons/interface/api/addon-instance/screensaver.h"
#include "filesystem/SpecialProtocol.h"
#include "utils/log.h"
#include "windowing/GraphicContext.h"
#include "windowing/WinSystem.h"

using namespace ADDON;
using namespace KODI::ADDONS;

CScreenSaver::CScreenSaver(const AddonInfoPtr& addonInfo)
  : IInstanceHandler(this, ADDON_INSTANCE_SCREENSAVER, addonInfo)
{
  if (CreateInstance() != ADDON_STATUS_OK)
    CLog::Log(LOGFATAL, "Screensaver: failed to create instance for '{}' and not usable!", ID());
}

CScreenSaver::~CScreenSaver()
{
  /* Destroy the class "kodi::addon::CInstanceScreensaver" on add-on side */
  DestroyInstance();
}

bool CScreenSaver::Start()
{
  const auto ifc = m_ifc->kodi_addoninstance_screensaver_h;
  return ifc->kodi_addon_screensaver_start_v1(this, m_instance);
}

void CScreenSaver::Stop()
{
  const auto ifc = m_ifc->kodi_addoninstance_screensaver_h;
  return ifc->kodi_addon_screensaver_stop_v1(this, m_instance);
}

void CScreenSaver::Render()
{
  const auto ifc = m_ifc->kodi_addoninstance_screensaver_h;
  return ifc->kodi_addon_screensaver_render_v1(this, m_instance);
}

bool CScreenSaver::GetOffscreenRenderInfos(int& x, int& y, int& width, int& height, ADDON_HARDWARE_CONTEXT& context)
{
  const auto winSystem = CServiceBroker::GetWinSystem();
  if (!winSystem)
    return false;

  x = 0;
  y = 0;
  context = winSystem->GetHWContext();
  width = winSystem->GetGfxContext().GetWidth();
  height = winSystem->GetGfxContext().GetHeight();

  return true;
}

void CScreenSaver::GetProperties(struct KODI_ADDON_SCREENSAVER_PROPS* props)
{
  if (!props)
    return;

  const auto winSystem = CServiceBroker::GetWinSystem();
  if (!winSystem)
    return;

  props->x = 0;
  props->y = 0;
  props->device = winSystem->GetHWContext();
  props->width = winSystem->GetGfxContext().GetWidth();
  props->height = winSystem->GetGfxContext().GetHeight();
  props->pixelRatio = winSystem->GetGfxContext().GetResInfo().fPixelRatio;
}
