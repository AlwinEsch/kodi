/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_GUI_DIALOGS_PROGRESS_H
#define C_API_GUI_DIALOGS_PROGRESS_H

#include "../../addon_base.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  typedef void* KODI_GUI_HANDLE;

  ATTR_DLL_EXPORT KODI_GUI_HANDLE kodi_gui_dialogs_progress_new_dialog() __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_dialogs_progress_delete_dialog(KODI_GUI_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_dialogs_progress_open(KODI_GUI_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_dialogs_progress_set_heading(KODI_GUI_HANDLE handle,
                                                             const char* heading)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_dialogs_progress_set_line(KODI_GUI_HANDLE handle,
                                                          unsigned int line_no,
                                                          const char* line) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_dialogs_progress_set_can_cancel(KODI_GUI_HANDLE handle,
                                                                bool can_cancel)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_progress_is_canceled(KODI_GUI_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_dialogs_progress_set_percentage(KODI_GUI_HANDLE handle,
                                                                int percentage)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT int kodi_gui_dialogs_progress_get_percentage(KODI_GUI_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_dialogs_progress_show_progress_bar(KODI_GUI_HANDLE handle,
                                                                   bool on_off)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_dialogs_progress_set_progress_max(KODI_GUI_HANDLE handle, int max)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_dialogs_progress_set_progress_advance(KODI_GUI_HANDLE handle,
                                                                      int n_steps)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_progress_abort(KODI_GUI_HANDLE handle)
      __INTRODUCED_IN_KODI(1);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_GUI_DIALOGS_PROGRESS_H */
