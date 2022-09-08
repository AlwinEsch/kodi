/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "../../../src/shared/SharedGroups.h"
#include "../../../src/shared/api/addon_base.h"

namespace KODI_ADDON
{
namespace INTERNAL
{

struct AddonIfc;

class ATTR_DLL_LOCAL IMsgHdl
{
public:
  IMsgHdl(AddonIfc* ifc) : m_ifc(ifc) {}
  virtual ~IMsgHdl() = default;

  virtual bool HandleMessage(int funcGroup,
                             int func,
                             const msgpack::unpacked& in,
                             msgpack::sbuffer& out)
  {
    return false;
  }

protected:
  AddonIfc* m_ifc;
};

} /* namespace INTERNAL */
} /* namespace KODI_ADDON */
