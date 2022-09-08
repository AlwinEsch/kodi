/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_GUI_CONTROLS_FADE_LABEL_H
#define C_API_GUI_CONTROLS_FADE_LABEL_H

#include "../../addon_base.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  typedef void* KODI_GUI_CONTROL_HANDLE;

  ATTR_DLL_EXPORT void kodi_gui_controls_fade_label_set_visible(KODI_GUI_CONTROL_HANDLE handle,
                                                                bool visible)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_fade_label_add_label(KODI_GUI_CONTROL_HANDLE handle,
                                                              const char* text)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_gui_controls_fade_label_get_label(KODI_GUI_CONTROL_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_fade_label_set_scrolling(KODI_GUI_CONTROL_HANDLE handle,
                                                                  bool scroll)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_fade_label_reset(KODI_GUI_CONTROL_HANDLE handle)
      __INTRODUCED_IN_KODI(1);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_GUI_CONTROLS_FADE_LABEL_H */
