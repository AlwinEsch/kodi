/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "../../../include/kodi/c-api/addon_base.h"
#include "../../../src/shared/SharedData.h"

#include <stdint.h>
#include <string>
#include <thread>

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

class CShareProcessReceiver;
class CShareProcessTransmitter;

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */

namespace KODI_ADDON
{
namespace INTERNAL
{

class CAddonControl;

class ATTR_DLL_LOCAL CAddonThreadIfc
{
public:
  CAddonThreadIfc(int64_t identifier, CAddonControl* process, bool processOutside);
  ~CAddonThreadIfc();

  bool Create();
  void Destroy();
  void DestroyReceiver();

  bool SendMessage(int group, int func);
  bool SendMessage(int group, int func, const msgpack::sbuffer& in);
  bool SendMessage(int group, int func, const msgpack::sbuffer& in, msgpack::sbuffer& ret);
  bool SendMessageOnlyGetReturn(int group, int func, msgpack::sbuffer& ret);

  bool ProcessOutside();

  bool Active() const;
  const std::string& GetID() const { return m_id; }

private:
  const bool m_mainThread;
  const int64_t m_identifier;
  const std::string m_id;
  const bool m_processOutside;
  CAddonControl* const m_process;

  int64_t m_pid{-1};
  KODI::ADDONS::INTERFACE::CShareProcessReceiver* m_shareProcessReceiver{nullptr};
  KODI::ADDONS::INTERFACE::CShareProcessTransmitter* m_shareProcessTransmitter{nullptr};
};

} /* namespace INTERNAL */
} /* namespace KODI_ADDON */
