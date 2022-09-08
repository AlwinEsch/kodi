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
#include <thread>
#include <vector>

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

class CSharedMemControl;

class CShareProcessReceiver
{
public:
  CShareProcessReceiver(const std::string& identifier,
                        bool mainThread,
                        size_t size = SHARED_DEFAULT_MEM_SIZE);
  ~CShareProcessReceiver();

  bool Create(bool initial, bool useThread);
  void Destroy();
  void RegisterReceiver(SharedProcess func);
  bool ProcessOutside();

  const std::string& GetIdentifier() const { return m_identifier; }
  const std::string& GetLastErrorText() const;

  std::atomic_bool m_active{false};

protected:
  void Process();

private:
  bool HandleMessage(int group, int func, const msgpack::unpacked& in, msgpack::sbuffer& out);

  const std::string m_identifier;
  const bool m_mainThread;
  CSharedMemControl* const m_sharedMem;

  bool m_useThread{false};
  std::thread m_thread;
  std::atomic_bool m_running{false};
  std::vector<SharedProcess> m_receivers;
};

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
