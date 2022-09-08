/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_GUI_DIALOGS_NUMERIC_H
#define C_API_GUI_DIALOGS_NUMERIC_H

#include "../../addon_base.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  ATTR_DLL_EXPORT bool kodi_gui_dialogs_numeric_show_and_verify_new_password(char** password)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT int kodi_gui_dialogs_numeric_show_and_verify_password(const char* password,
                                                                        const char* heading,
                                                                        int retries)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_numeric_show_and_verify_input(const char* verify_in,
                                                                      char** verify_out,
                                                                      const char* heading,
                                                                      bool verify_input)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_numeric_show_and_get_time(struct tm* time,
                                                                  const char* heading)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_numeric_show_and_get_date(struct tm* date,
                                                                  const char* heading)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_numeric_show_and_get_ip_address(const char* ip_address_in,
                                                                        char** ip_address_out,
                                                                        const char* heading)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_numeric_show_and_get_number(const char* number_in,
                                                                    char** number_out,
                                                                    const char* heading,
                                                                    unsigned int auto_close_ms)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_numeric_show_and_get_seconds(const char* time_in,
                                                                     char** time_out,
                                                                     const char* heading)
      __INTRODUCED_IN_KODI(1);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_GUI_DIALOGS_NUMERIC_H */
