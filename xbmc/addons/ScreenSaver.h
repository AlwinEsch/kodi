/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "addons/kodi-dev-kit/include/kodi/c-api/addon-instance/screensaver.h"
#include "interface/InstanceHandler.h"
#include "interface/IOffscreenRenderProcess.h"

namespace KODI
{
namespace ADDONS
{

class CScreenSaver : public KODI::ADDONS::INTERFACE::IInstanceHandler,
                     public KODI::ADDONS::INTERFACE::IOffscreenRenderProcess
{
public:
  explicit CScreenSaver(const ADDON::AddonInfoPtr& addonInfo);
  ~CScreenSaver() override;

  bool Start();
  void Stop();
  void Render();

  // Addon callback functions
  void GetProperties(struct KODI_ADDON_SCREENSAVER_PROPS* props);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_screensaver_get_properties>---*/

protected:
  bool GetOffscreenRenderInfos(int& x, int& y, int& width, int& height, ADDON_HARDWARE_CONTEXT& context) override;
};

} /* namespace ADDONS */
} /* namespace KODI */
