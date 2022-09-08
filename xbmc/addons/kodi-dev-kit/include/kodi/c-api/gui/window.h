/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_GUI_WINDOW_H
#define C_API_GUI_WINDOW_H

#include "../addon_base.h"
#include "input/action.h"
#include "input/action_ids.h"
#include "list_item.h"

#include <stddef.h>

#define ADDON_MAX_CONTEXT_ENTRIES 20
#define ADDON_MAX_CONTEXT_ENTRY_NAME_LENGTH 80

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  typedef void* KODI_GUI_WINDOW_HANDLE;
  typedef void* KODI_GUI_CLIENT_HANDLE;
  typedef void* KODI_GUI_CONTROL_HANDLE;

  typedef struct gui_context_menu_pair
  {
    unsigned int id;
    char name[ADDON_MAX_CONTEXT_ENTRY_NAME_LENGTH];
  } gui_context_menu_pair;

  //   typedef bool(ATTR_APIENTRYP PFN_KODI_GUI_WINDOW_INIT_V1)(KODI_GUI_CLIENT_HANDLE clienthandle);
  //   typedef bool(ATTR_APIENTRYP PFN_KODI_GUI_WINDOW_FOCUS_V1)(KODI_GUI_CLIENT_HANDLE clienthandle,
  //                                                             int control_id);
  //   typedef bool(ATTR_APIENTRYP PFN_KODI_GUI_WINDOW_CLICK_V1)(KODI_GUI_CLIENT_HANDLE clienthandle,
  //                                                             int control_id);
  //   typedef bool(ATTR_APIENTRYP PFN_KODI_GUI_WINDOW_ON_ACTION_V1)(KODI_GUI_CLIENT_HANDLE clienthandle,
  //                                                                 enum ADDON_ACTION action);
  //   typedef void(ATTR_APIENTRYP PFN_KODI_GUI_WINDOW_GET_CONTEXT_BUTTONS_V1)(
  //       KODI_GUI_CLIENT_HANDLE clienthandle,
  //       int control_id,
  //       struct gui_context_menu_pair* entries,
  //       size_t* amount);
  //   typedef bool(ATTR_APIENTRYP PFN_KODI_GUI_WINDOW_ON_CONTEXT_BUTTON_V1)(
  //       KODI_GUI_CLIENT_HANDLE clienthandle, int control_id, size_t selected);

  /* Window creation functions */
  ATTR_DLL_EXPORT KODI_GUI_WINDOW_HANDLE kodi_gui_window_create(KODI_HANDLE kodiBase,
                                                                const char* xml_filename,
                                                                const char* default_skin,
                                                                bool as_dialog,
                                                                bool is_media)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_window_destroy(KODI_GUI_WINDOW_HANDLE handle)
      __INTRODUCED_IN_KODI(1);

  //   ATTR_DLL_EXPORT void kodi_gui_window_set_callbacks(
  //       KODI_GUI_WINDOW_HANDLE handle,
  //       KODI_GUI_CLIENT_HANDLE clienthandle,
  //       PFN_KODI_GUI_WINDOW_INIT_V1 cb_init,
  //       PFN_KODI_GUI_WINDOW_FOCUS_V1 cb_focus,
  //       PFN_KODI_GUI_WINDOW_CLICK_V1 cb_click,
  //       PFN_KODI_GUI_WINDOW_ON_ACTION_V1 cb_on_action,
  //       PFN_KODI_GUI_WINDOW_GET_CONTEXT_BUTTONS_V1 cb_get_context_buttons,
  //       PFN_KODI_GUI_WINDOW_ON_CONTEXT_BUTTON_V1 cb_on_context_button) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_window_show(KODI_GUI_WINDOW_HANDLE handle) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_window_close(KODI_GUI_WINDOW_HANDLE handle) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_window_do_modal(KODI_GUI_WINDOW_HANDLE handle)
      __INTRODUCED_IN_KODI(1);

  /* Window control functions */
  ATTR_DLL_EXPORT bool kodi_gui_window_set_focus_id(KODI_GUI_WINDOW_HANDLE handle, int control_id)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT int kodi_gui_window_get_focus_id(KODI_GUI_WINDOW_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_window_set_control_label(KODI_GUI_WINDOW_HANDLE handle,
                                                         int control_id,
                                                         const char* label) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_window_set_control_visible(KODI_GUI_WINDOW_HANDLE handle,
                                                           int control_id,
                                                           bool visible) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_window_set_control_selected(KODI_GUI_WINDOW_HANDLE handle,
                                                            int control_id,
                                                            bool selected) __INTRODUCED_IN_KODI(1);

  /* Window property functions */
  ATTR_DLL_EXPORT void kodi_gui_window_set_property(KODI_GUI_WINDOW_HANDLE handle,
                                                    const char* key,
                                                    const char* value) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_window_set_property_int(KODI_GUI_WINDOW_HANDLE handle,
                                                        const char* key,
                                                        int value) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_window_set_property_bool(KODI_GUI_WINDOW_HANDLE handle,
                                                         const char* key,
                                                         bool value) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_window_set_property_double(KODI_GUI_WINDOW_HANDLE handle,
                                                           const char* key,
                                                           double value) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_gui_window_get_property(KODI_GUI_WINDOW_HANDLE handle, const char* key)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT int kodi_gui_window_get_property_int(KODI_GUI_WINDOW_HANDLE handle,
                                                       const char* key) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_window_get_property_bool(KODI_GUI_WINDOW_HANDLE handle,
                                                         const char* key) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT double kodi_gui_window_get_property_double(KODI_GUI_WINDOW_HANDLE handle,
                                                             const char* key)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_window_clear_properties(KODI_GUI_WINDOW_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_window_clear_property(KODI_GUI_WINDOW_HANDLE handle,
                                                      const char* key) __INTRODUCED_IN_KODI(1);

  /* List item functions */
  ATTR_DLL_EXPORT void kodi_gui_window_clear_item_list(KODI_GUI_WINDOW_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_window_add_list_item(KODI_GUI_WINDOW_HANDLE handle,
                                                     KODI_GUI_LISTITEM_HANDLE item,
                                                     int list_position) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_window_remove_list_item_from_position(KODI_GUI_WINDOW_HANDLE handle,
                                                                      int list_position)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_window_remove_list_item(KODI_GUI_WINDOW_HANDLE handle,
                                                        KODI_GUI_LISTITEM_HANDLE item)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT KODI_GUI_LISTITEM_HANDLE kodi_gui_window_get_list_item(
      KODI_GUI_WINDOW_HANDLE handle, int list_position) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_window_set_current_list_position(KODI_GUI_WINDOW_HANDLE handle,
                                                                 int list_position)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT int kodi_gui_window_get_current_list_position(KODI_GUI_WINDOW_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT int kodi_gui_window_get_list_size(KODI_GUI_WINDOW_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_window_set_container_property(KODI_GUI_WINDOW_HANDLE handle,
                                                              const char* key,
                                                              const char* value)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_window_set_container_content(KODI_GUI_WINDOW_HANDLE handle,
                                                             const char* value)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT int kodi_gui_window_get_current_container_id(KODI_GUI_WINDOW_HANDLE handle)
      __INTRODUCED_IN_KODI(1);

  /* Various functions */
  ATTR_DLL_EXPORT void kodi_gui_window_mark_dirty_region(KODI_GUI_WINDOW_HANDLE handle)
      __INTRODUCED_IN_KODI(1);

  /* GUI control access functions */
  ATTR_DLL_EXPORT KODI_GUI_CONTROL_HANDLE kodi_gui_window_get_control_button(
      KODI_GUI_WINDOW_HANDLE handle, int control_id) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT KODI_GUI_CONTROL_HANDLE kodi_gui_window_get_control_edit(
      KODI_GUI_WINDOW_HANDLE handle, int control_id) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT KODI_GUI_CONTROL_HANDLE kodi_gui_window_get_control_fade_label(
      KODI_GUI_WINDOW_HANDLE handle, int control_id) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT KODI_GUI_CONTROL_HANDLE kodi_gui_window_get_control_image(
      KODI_GUI_WINDOW_HANDLE handle, int control_id) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT KODI_GUI_CONTROL_HANDLE kodi_gui_window_get_control_label(
      KODI_GUI_WINDOW_HANDLE handle, int control_id) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT KODI_GUI_CONTROL_HANDLE kodi_gui_window_get_control_progress(
      KODI_GUI_WINDOW_HANDLE handle, int control_id) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT KODI_GUI_CONTROL_HANDLE kodi_gui_window_get_control_radio_button(
      KODI_GUI_WINDOW_HANDLE handle, int control_id) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT KODI_GUI_CONTROL_HANDLE kodi_gui_window_get_control_render_addon(
      KODI_GUI_WINDOW_HANDLE handle, int control_id) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT KODI_GUI_CONTROL_HANDLE kodi_gui_window_get_control_settings_slider(
      KODI_GUI_WINDOW_HANDLE handle, int control_id) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT KODI_GUI_CONTROL_HANDLE kodi_gui_window_get_control_slider(
      KODI_GUI_WINDOW_HANDLE handle, int control_id) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT KODI_GUI_CONTROL_HANDLE kodi_gui_window_get_control_spin(
      KODI_GUI_WINDOW_HANDLE handle, int control_id) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT KODI_GUI_CONTROL_HANDLE kodi_gui_window_get_control_text_box(
      KODI_GUI_WINDOW_HANDLE handle, int control_id) __INTRODUCED_IN_KODI(1);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_GUI_WINDOW_H */
