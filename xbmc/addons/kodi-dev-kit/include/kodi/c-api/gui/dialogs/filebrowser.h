/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_GUI_DIALOGS_FILEBROWSER_H
#define C_API_GUI_DIALOGS_FILEBROWSER_H

#include "../../addon_base.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  ATTR_DLL_EXPORT bool kodi_gui_dialogs_file_browser_show_and_get_directory(const char* shares,
                                                                            const char* heading,
                                                                            const char* path_in,
                                                                            char** path_out,
                                                                            bool write_only)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_file_browser_show_and_get_file(const char* shares,
                                                                       const char* mask,
                                                                       const char* heading,
                                                                       const char* path_in,
                                                                       char** path_out,
                                                                       bool use_thumbs,
                                                                       bool use_file_directories)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_file_browser_show_and_get_file_from_dir(
      const char* directory,
      const char* mask,
      const char* heading,
      const char* path_in,
      char** path_out,
      bool use_thumbs,
      bool use_file_directories,
      bool single_list) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_file_browser_show_and_get_file_list(
      const char* shares,
      const char* mask,
      const char* heading,
      char*** file_list,
      size_t* entries,
      bool use_thumbs,
      bool use_file_directories) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_file_browser_show_and_get_source(
      const char* path_in,
      char** path_out,
      bool allow_network_shares,
      const char* additional_share,
      const char* type) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_file_browser_show_and_get_image(const char* shares,
                                                                        const char* heading,
                                                                        const char* path_in,
                                                                        char** path_out)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_file_browser_show_and_get_image_list(const char* shares,
                                                                             const char* heading,
                                                                             char*** file_list,
                                                                             size_t* entries)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_dialogs_file_browser_clear_file_list(char*** file_list,
                                                                     size_t entries)
      __INTRODUCED_IN_KODI(1);
  /*---AUTO_GEN_PARSE<OVERRIDE;USE_INTERNAL=kodi_gui_dialogs_file_browser_clear_file_list>---*/

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_GUI_DIALOGS_FILEBROWSER_H */
