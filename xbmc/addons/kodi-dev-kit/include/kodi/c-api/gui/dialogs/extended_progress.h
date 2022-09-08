/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_GUI_DIALOGS_EXTENDED_PROGRESS_H
#define C_API_GUI_DIALOGS_EXTENDED_PROGRESS_H

#include "../../addon_base.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  typedef void* KODI_GUI_HANDLE;

  ATTR_DLL_EXPORT KODI_GUI_HANDLE kodi_gui_dialogs_extended_progress_new_dialog(const char* title)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_dialogs_extended_progress_delete_dialog(KODI_GUI_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_gui_dialogs_extended_progress_get_title(KODI_GUI_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_dialogs_extended_progress_set_title(KODI_GUI_HANDLE handle,
                                                                    const char* title)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_gui_dialogs_extended_progress_get_text(KODI_GUI_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_dialogs_extended_progress_set_text(KODI_GUI_HANDLE handle,
                                                                   const char* text)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_extended_progress_is_finished(KODI_GUI_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_dialogs_extended_progress_mark_finished(KODI_GUI_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT float kodi_gui_dialogs_extended_progress_get_percentage(KODI_GUI_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_dialogs_extended_progress_set_percentage(KODI_GUI_HANDLE handle,
                                                                         float percentage)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_dialogs_extended_progress_set_progress(KODI_GUI_HANDLE handle,
                                                                       int currentItem,
                                                                       int itemCount)
      __INTRODUCED_IN_KODI(1);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_GUI_DIALOGS_EXTENDED_PROGRESS_H */
