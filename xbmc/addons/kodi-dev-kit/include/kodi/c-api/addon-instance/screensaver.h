/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_ADDONINSTANCE_SCREENSAVER_H
#define C_API_ADDONINSTANCE_SCREENSAVER_H

#include "../addon_base.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  typedef KODI_ADDON_INSTANCE_HDL KODI_ADDON_SCREENSAVER_HDL;

  struct KODI_ADDON_SCREENSAVER_PROPS
  {
    ADDON_HARDWARE_CONTEXT device;
    int x;
    int y;
    int width;
    int height;
    float pixelRatio;
  };

  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_SCREENSAVER_START_V1)(const KODI_ADDON_SCREENSAVER_HDL hdl);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_SCREENSAVER_STOP_V1)(const KODI_ADDON_SCREENSAVER_HDL hdl);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_SCREENSAVER_RENDER_V1)(const KODI_ADDON_SCREENSAVER_HDL hdl);

  typedef struct KODI_ADDON_SCREENSAVER_FUNC
  {
    PFN_KODI_ADDON_INSTANCE_CREATE_V1 create;
    PFN_KODI_ADDON_INSTANCE_DESTROY_V1 destroy;
    PFN_KODI_ADDON_SCREENSAVER_START_V1 start;
    PFN_KODI_ADDON_SCREENSAVER_STOP_V1 stop;
    PFN_KODI_ADDON_SCREENSAVER_RENDER_V1 render;
  } KODI_ADDON_SCREENSAVER_FUNC;
  /*---AUTO_GEN_PARSE<OVERRIDE;USE_OFFSCREEN_RENDER=KODI_ADDON_SCREENSAVER_FUNC(start,stop,render;CScreenSaver)>---*/

  ATTR_DLL_EXPORT void kodi_addon_screensaver_get_properties(
      const KODI_ADDON_INSTANCE_BACKEND_HDL hdl, struct KODI_ADDON_SCREENSAVER_PROPS* props)
      __INTRODUCED_IN_KODI(1);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_ADDONINSTANCE_SCREENSAVER_H */
