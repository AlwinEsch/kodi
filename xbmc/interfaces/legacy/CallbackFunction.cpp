/*
 *  Copyright (C) 2005-2013 Team XBMC
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "CallbackFunction.h"

namespace XBMCAddon
{
  Callback::~Callback() { XBMC_TRACE; deallocating(); }
}
