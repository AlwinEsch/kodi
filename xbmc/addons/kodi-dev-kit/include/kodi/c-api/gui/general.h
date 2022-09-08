/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_GUI_GENERAL_H
#define C_API_GUI_GENERAL_H

#include "../addon_base.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  ATTR_DLL_EXPORT void kodi_gui_lock() __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_unlock() __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT int kodi_gui_get_screen_height() __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT int kodi_gui_get_screen_width() __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT float kodi_gui_get_video_resolution() __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT int kodi_gui_get_current_window_dialog_id() __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT int kodi_gui_get_current_window_id() __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT ADDON_HARDWARE_CONTEXT kodi_gui_get_hw_context() __INTRODUCED_IN_KODI(1);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_GUI_GENERAL_H */
