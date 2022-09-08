/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_GUI_CONTROLS_RENDERING_H
#define C_API_GUI_CONTROLS_RENDERING_H

#include "../../addon_base.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  typedef void* KODI_GUI_CONTROL_HANDLE;

  // //     ATTR_DLL_EXPORT void kodi_gui_controls_radio_rendering_set_callbacks(
  // //         KODI_GUI_CONTROL_HANDLE handle,
  // //         KODI_GUI_CLIENT_HANDLE clienthandle,
  // //         bool (*createCB)(KODI_GUI_CLIENT_HANDLE, int, int, int, int, ADDON_HARDWARE_CONTEXT),
  // //         void (*renderCB)(KODI_GUI_CLIENT_HANDLE),
  // //         void (*stopCB)(KODI_GUI_CLIENT_HANDLE),
  // //         bool (*dirtyCB)(KODI_GUI_CLIENT_HANDLE)) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_radio_rendering_destroy(KODI_GUI_CONTROL_HANDLE handle)
      __INTRODUCED_IN_KODI(1);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_GUI_CONTROLS_RENDERING_H */
