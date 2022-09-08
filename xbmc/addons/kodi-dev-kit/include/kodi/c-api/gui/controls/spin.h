/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_GUI_CONTROLS_SPIN_H
#define C_API_GUI_CONTROLS_SPIN_H

#include "../../addon_base.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  typedef void* KODI_GUI_CONTROL_HANDLE;

  ATTR_DLL_EXPORT void kodi_gui_controls_spin_set_visible(KODI_GUI_CONTROL_HANDLE handle,
                                                          bool visible) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_spin_set_enabled(KODI_GUI_CONTROL_HANDLE handle,
                                                          bool enabled) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_spin_set_text(KODI_GUI_CONTROL_HANDLE handle,
                                                       const char* text) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_spin_reset(KODI_GUI_CONTROL_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_spin_set_type(KODI_GUI_CONTROL_HANDLE handle, int type)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_spin_add_string_label(KODI_GUI_CONTROL_HANDLE handle,
                                                               const char* label,
                                                               const char* value)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_spin_set_string_value(KODI_GUI_CONTROL_HANDLE handle,
                                                               const char* value)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_gui_controls_spin_get_string_value(KODI_GUI_CONTROL_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_spin_add_int_label(KODI_GUI_CONTROL_HANDLE handle,
                                                            const char* label,
                                                            int value) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_spin_set_int_range(KODI_GUI_CONTROL_HANDLE handle,
                                                            int start,
                                                            int end) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_spin_set_int_value(KODI_GUI_CONTROL_HANDLE handle,
                                                            int value) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT int kodi_gui_controls_spin_get_int_value(KODI_GUI_CONTROL_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_spin_set_float_range(KODI_GUI_CONTROL_HANDLE handle,
                                                              float start,
                                                              float end) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_spin_set_float_value(KODI_GUI_CONTROL_HANDLE handle,
                                                              float value) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT float kodi_gui_controls_spin_get_float_value(KODI_GUI_CONTROL_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_spin_set_float_interval(KODI_GUI_CONTROL_HANDLE handle,
                                                                 float interval)
      __INTRODUCED_IN_KODI(1);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_GUI_CONTROLS_SPIN_H */
