/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "DynamicDll.h"
#include "ILauncher.h"
#include "threads/Thread.h"

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

class DllAddonInterface
{
public:
  virtual ~DllAddonInterface() = default;
  virtual int main(int argc, char* argv[]) = 0;
  virtual int non_blocking_main_start(int argc, char* argv[]) = 0;
  virtual void non_blocking_main_stop() = 0;
};

class DllAddon : public DllDynamic, public DllAddonInterface
{
public:
  DECLARE_DLL_WRAPPER_TEMPLATE(DllAddon)
  DEFINE_METHOD2(int, main, (int p1, char* p2[]))
  DEFINE_METHOD2(int, non_blocking_main_start, (int p1, char* p2[]))
  DEFINE_METHOD0(void, non_blocking_main_stop)
  bool main_available() { return m_main != nullptr; }
  bool non_blocking_main_start_available() { return m_non_blocking_main_start != nullptr; }
  BEGIN_METHOD_RESOLVE()
  RESOLVE_METHOD_RENAME_OPTIONAL(main, main)
  RESOLVE_METHOD_RENAME_OPTIONAL(non_blocking_main_start, non_blocking_main_start)
  RESOLVE_METHOD_RENAME_OPTIONAL(non_blocking_main_stop, non_blocking_main_stop)
  END_METHOD_RESOLVE()
};

class CLauncherDirectDll : public ILauncher, private CThread
{
public:
  CLauncherDirectDll(std::shared_ptr<ADDON::IAddon> addon, const std::string& uuid);
  ~CLauncherDirectDll();

  bool Launch(const std::vector<char*>& argv, bool waitForExit) override;
  bool Kill(bool wait) override;
  int64_t GetMainProcessPID() const override { return static_cast<int64_t>(m_pid); }
  ChildStatus ProcessActive() override;
  bool SupportNonBlocking() override;
  bool SupportNonBlocking(const std::string& path) override;

private:
  std::string GetDllPath(const std::string& libPath);

  void Process() override;

  std::atomic_bool m_startDone{false};
  std::vector<char*> m_argv;
  DllAddon* m_pDll = nullptr;
  pid_t m_pid = -1;
};

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
