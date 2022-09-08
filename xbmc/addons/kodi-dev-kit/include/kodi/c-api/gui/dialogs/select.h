/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_GUI_DIALOGS_SELECT_H
#define C_API_GUI_DIALOGS_SELECT_H

#include "../../addon_base.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  ATTR_DLL_EXPORT int kodi_gui_dialogs_select_open(const char* heading,
                                                   const char* entries[],
                                                   size_t size,
                                                   int selected,
                                                   unsigned int autoclose) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_gui_dialogs_select_open_multi_select(const char* heading,
                                                                 const char* entryIDs[],
                                                                 const char* entryNames[],
                                                                 uint8_t entriesSelected[],
                                                                 size_t size,
                                                                 unsigned int autoclose)
      __INTRODUCED_IN_KODI(1);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_GUI_DIALOGS_SELECT_H */
