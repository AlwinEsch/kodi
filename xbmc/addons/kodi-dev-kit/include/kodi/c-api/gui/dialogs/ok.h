/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_GUI_DIALOGS_OK_H
#define C_API_GUI_DIALOGS_OK_H

#include "../../addon_base.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  ATTR_DLL_EXPORT void kodi_gui_dialogs_ok_show_and_get_input_single_text(const char* heading,
                                                                          const char* text)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_dialogs_ok_show_and_get_input_line_text(const char* heading,
                                                                        const char* line0,
                                                                        const char* line1,
                                                                        const char* line2)
      __INTRODUCED_IN_KODI(1);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_GUI_DIALOGS_OK_H */
