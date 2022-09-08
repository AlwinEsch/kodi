/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_MONITOR_H
#define C_API_MONITOR_H

#include "addon_base.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  ATTR_DLL_EXPORT bool kodi_monitor_wait_for_abort(double timeout) __INTRODUCED_IN_KODI(1);
  /*---AUTO_GEN_PARSE<OVERRIDE;USE_INTERNAL=kodi_monitor_wait_for_abort>---*/

  ATTR_DLL_EXPORT bool kodi_monitor_abort_requested() __INTRODUCED_IN_KODI(1);
  /*---AUTO_GEN_PARSE<OVERRIDE;USE_INTERNAL=kodi_monitor_abort_requested>---*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !C_API_MONITOR_H */
