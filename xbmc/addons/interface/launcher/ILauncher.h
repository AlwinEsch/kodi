/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <vector>

namespace ADDON
{
class IAddon;
}

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

enum class ChildStatus
{
  Running,
  NotStarted,
  ExitedNormal,
  StoppedBySignal,
  StoppedByUnknown,
  KilledFromOutside,
  SeqmentionFault,
};

class ILauncher
{
public:
  ILauncher(std::shared_ptr<ADDON::IAddon> addonInfo, const std::string& uuid)
    : m_addonInfo(addonInfo), m_uuid(uuid)
  {
  }
  virtual ~ILauncher() = default;

  virtual bool Launch(const std::vector<char*>& argv, bool waitForExit) = 0;
  virtual bool Kill(bool wait = true) = 0;
  virtual int64_t GetMainProcessPID() const = 0;
  virtual ChildStatus ProcessActive() = 0;
  virtual bool SupportNonBlocking() { return false; }
  virtual bool SupportNonBlocking(const std::string& path) { return false; }

  int GetExitCode() const { return m_exitCode; }
  ChildStatus GetLastChildStatus() const { return m_lastStatus; }
  const std::string& GetStackTrace() const { return m_stacktrace; }

protected:
  const std::shared_ptr<ADDON::IAddon> m_addonInfo;
  const std::string m_uuid;

  int m_exitCode = 0;
  std::atomic<ChildStatus> m_lastStatus{ChildStatus::NotStarted};
  std::string m_stacktrace;
};

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
