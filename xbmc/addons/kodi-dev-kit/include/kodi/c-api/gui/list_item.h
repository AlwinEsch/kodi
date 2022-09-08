/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_GUI_LIST_ITEM_H
#define C_API_GUI_LIST_ITEM_H

#include "../addon_base.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  typedef void* KODI_GUI_LISTITEM_HANDLE;

  ATTR_DLL_EXPORT KODI_GUI_LISTITEM_HANDLE kodi_gui_listitem_create(const char* label,
                                                                    const char* label2,
                                                                    const char* path)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_listitem_destroy(KODI_GUI_LISTITEM_HANDLE handle)
      __INTRODUCED_IN_KODI(1);

  ATTR_DLL_EXPORT char* kodi_gui_listitem_get_label(KODI_GUI_LISTITEM_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_listitem_set_label(KODI_GUI_LISTITEM_HANDLE handle,
                                                   const char* label) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_gui_listitem_get_label2(KODI_GUI_LISTITEM_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_listitem_set_label2(KODI_GUI_LISTITEM_HANDLE handle,
                                                    const char* label) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_gui_listitem_get_art(KODI_GUI_LISTITEM_HANDLE handle, const char* type)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_listitem_set_art(KODI_GUI_LISTITEM_HANDLE handle,
                                                 const char* type,
                                                 const char* image) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_gui_listitem_get_path(KODI_GUI_LISTITEM_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_listitem_set_path(KODI_GUI_LISTITEM_HANDLE handle, const char* path)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_gui_listitem_get_property(KODI_GUI_LISTITEM_HANDLE handle,
                                                       const char* key) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_listitem_set_property(KODI_GUI_LISTITEM_HANDLE handle,
                                                      const char* key,
                                                      const char* value) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_listitem_select(KODI_GUI_LISTITEM_HANDLE handle, bool select)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_listitem_is_selected(KODI_GUI_LISTITEM_HANDLE handle)
      __INTRODUCED_IN_KODI(1);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_GUI_LIST_ITEM_H */
