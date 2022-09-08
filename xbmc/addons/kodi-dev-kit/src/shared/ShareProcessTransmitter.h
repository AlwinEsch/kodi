/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "SharedData.h"

#include <atomic>
#include <string>

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

class CSharedMemControl;

class CShareProcessTransmitter
{
public:
  CShareProcessTransmitter(const std::string& identifier,
                           bool mainThread,
                           size_t size = SHARED_DEFAULT_MEM_SIZE);
  ~CShareProcessTransmitter();

  bool Create(bool initial);
  void Destroy();

  bool SendMessage(int group, int func);
  bool SendMessage(int group, int func, const msgpack::sbuffer& in);
  bool SendMessage(int group, int func, const msgpack::sbuffer& in, msgpack::sbuffer& ret);
  bool SendMessageOnlyGetReturn(int group, int func, msgpack::sbuffer& ret);

  const std::string& GetIdentifier() const { return m_identifier; }
  const std::string& GetLastErrorText() const;
  int64_t GetPID() const;

  std::atomic_bool m_active{false};
  std::atomic_bool m_created{false};

private:
  const std::string m_identifier;
  const bool m_mainThread;
  CSharedMemControl* const m_sharedMem;
};

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
