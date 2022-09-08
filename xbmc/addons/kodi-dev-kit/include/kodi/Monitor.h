/*
 *  Copyright (C) 2005-2021 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "AddonBase.h"
#include "c-api/monitor.h"

#ifdef __cplusplus

namespace kodi
{
namespace monitor
{

inline bool ATTR_DLL_LOCAL WaitForAbort(double timeout)
{
  return kodi::dl::api.kodi_monitor_wait_for_abort(timeout);
}

inline bool ATTR_DLL_LOCAL AbortRequested()
{
  return kodi::dl::api.kodi_monitor_abort_requested();
}

} /* namespace monitor */
} /* namespace kodi */

#endif /* __cplusplus */
