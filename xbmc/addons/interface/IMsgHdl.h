/*
 *  Copyright (C) 2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "../../addons/addoninfo/AddonInfo.h"

#include <msgpack.hpp>

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

class CInterface;
class CRunningProcess;

class IMsgHdl
{
public:
  IMsgHdl(CInterface& interface);
  virtual ~IMsgHdl() = default;

  virtual bool HandleMessage(int funcGroup,
                             int func,
                             const msgpack::unpacked& in,
                             msgpack::sbuffer& out) = 0;

protected:
  CInterface& m_interface;
  CRunningProcess* const m_process;
};

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
