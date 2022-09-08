/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_GUI_CONTROLS_PROGRESS_H
#define C_API_GUI_CONTROLS_PROGRESS_H

#include "../../addon_base.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  typedef void* KODI_GUI_CONTROL_HANDLE;

  ATTR_DLL_EXPORT void kodi_gui_controls_progress_set_visible(KODI_GUI_CONTROL_HANDLE handle,
                                                              bool visible) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_progress_set_percentage(KODI_GUI_CONTROL_HANDLE handle,
                                                                 float percent)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT float kodi_gui_controls_progress_get_percentage(KODI_GUI_CONTROL_HANDLE handle)
      __INTRODUCED_IN_KODI(1);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_GUI_CONTROLS_PROGRESS_H */
