/*
 *  Copyright (C) 2005-2013 Team XBMC
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <string>
#include <vector>

#include "DBusUtil.h"

class CDBusReserve
{
public:
  CDBusReserve();
 ~CDBusReserve();

  bool AcquireDevice(const std::string &device);
  bool ReleaseDevice(const std::string &device);

private:
  CDBusConnection m_conn;
  std::vector<std::string> m_devs;
};

