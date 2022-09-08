/*
 *  Copyright (C) 2005-2021 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <atomic>
#include <stdint.h>
#include <string>
#include <thread>

#include <msgpack.hpp>

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

class CRunningProcess;
class CShareProcessReceiver;
class CShareProcessTransmitter;

class CRunningThread
{
public:
  CRunningThread(int64_t identifier, CRunningProcess* const process);
  ~CRunningThread();

  bool Create(bool useThread);
  void Destroy();

  bool SendMessage(int group, int func);
  bool SendMessage(int group, int func, const msgpack::sbuffer& in);
  bool SendMessage(int group, int func, const msgpack::sbuffer& in, msgpack::sbuffer& ret);
  bool SendMessageOnlyGetReturn(int group, int func, msgpack::sbuffer& ret);

  bool ProcessOutside();

  int64_t GetThreadID() const { return m_identifier; }
  const std::string& GetID() const { return m_id; }

private:
  const bool m_mainThread;
  const int64_t m_identifier;
  const std::string m_id;
  CRunningProcess* const m_process;

  int64_t m_pid{-1};
  CShareProcessReceiver* m_shareProcessReceiver{nullptr};
  CShareProcessTransmitter* m_shareProcessTransmitter{nullptr};
};

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
