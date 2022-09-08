/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/*---AUTO_GEN_PARSE<$$CORE_SYSTEM_NAME:linux,freebsd,android,osx,darwin_embedded>---*/

#pragma once

#include "ILauncher.h"

#include <atomic>

#define HANDLE_EINTR(x) \
  ({ \
    decltype(x) eintr_wrapper_result; \
    do \
    { \
      eintr_wrapper_result = (x); \
    } while (eintr_wrapper_result == -1 && errno == EINTR); \
    eintr_wrapper_result; \
  })

#define RAW_CHECK(condition) \
  do \
  { \
    if (!(condition)) \
      CLog::Log(LOGERROR, "Check failed: " #condition); \
  } while (0)

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

class CLauncherPosix : public ILauncher
{
public:
  CLauncherPosix(std::shared_ptr<ADDON::IAddon> addon, const std::string& uuid);
  ~CLauncherPosix() = default;

  bool Launch(const std::vector<char*>& argv, bool waitForExit) override;
  bool Kill(bool wait) override;
  int64_t GetMainProcessPID() const override { return static_cast<int64_t>(m_pid); }
  ChildStatus ProcessActive() override;

private:
  enum PIPE_FILE_DESCRIPTERS
  {
    READ_FD  = 0,
    WRITE_FD = 1
  };

  void ProcessStatus(int status);

  std::atomic<pid_t> m_pid{-1};
  int m_childToParent[2]{-1};
};

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
