/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_GUI_ACTION_H
#define C_API_GUI_ACTION_H

#define KODI_GUI_ACTION_MAX_AMOUNTS 6 // Must be at least 6

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  typedef void* KODI_GUI_ACTION_HANDLE;

  struct kodi_gui_action
  {
    int id;
    const char* name;

    float amount[KODI_GUI_ACTION_MAX_AMOUNTS];

    float repeat;
    unsigned int hold_time;
    unsigned int button_code;
    wchar_t unicode;
    const char* text;

    KODI_GUI_ACTION_HANDLE action_hdl;
  };

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_GUI_ACTION_H */
