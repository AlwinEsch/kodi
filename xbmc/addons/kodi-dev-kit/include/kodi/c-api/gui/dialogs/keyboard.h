/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_GUI_DIALOGS_KEYBOARD_H
#define C_API_GUI_DIALOGS_KEYBOARD_H

#include "../../addon_base.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_get_input_with_head(
      const char* text_in,
      char** text_out,
      const char* heading,
      bool allow_empty_result,
      bool hidden_input,
      unsigned int auto_close_ms) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_get_input(const char* text_in,
                                                                    char** text_out,
                                                                    bool allow_empty_result,
                                                                    unsigned int auto_close_ms)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_get_new_password_with_head(
      const char* password_in,
      char** password_out,
      const char* heading,
      bool allow_empty_result,
      unsigned int auto_close_ms) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_get_new_password(
      const char* password_in, char** password_out, unsigned int auto_close_ms)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_verify_new_password_with_head(
      char** password_out, const char* heading, bool allow_empty_result, unsigned int auto_close_ms)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_verify_new_password(
      char** password_out, unsigned int auto_close_ms) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT int kodi_gui_dialogs_keyboard_show_and_verify_password(const char* password_in,
                                                                         char** password_out,
                                                                         const char* heading,
                                                                         int retries,
                                                                         unsigned int auto_close_ms)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_get_filter(const char* text_in,
                                                                     char** text_out,
                                                                     bool searching,
                                                                     unsigned int auto_close_ms)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_send_text_to_active_keyboard(const char* text,
                                                                              bool close_keyboard)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_is_keyboard_activated() __INTRODUCED_IN_KODI(1);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_GUI_DIALOGS_KEYBOARD_H */
