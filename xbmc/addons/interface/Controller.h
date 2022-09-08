/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "Interface.h"
#include "processinfo/ProcessInfo.h"
#include "threads/Thread.h"

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

namespace ADDON
{
class CAddonInfo;
class CAddonMgr;
class IAddon;
} /* namespace ADDON */

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

class CRunningProcess;
class IInstanceHandler;

class CController : private CThread
{
public:
  CController(ADDON::CAddonMgr& addonMgr);
  ~CController();

  bool Init();
  void Deinit();
  void StopReport();

  bool AddonLanguageSupported(const std::shared_ptr<ADDON::IAddon>& addon) const;
  bool LaunchAddon(const std::shared_ptr<ADDON::IAddon>& addon,
                   std::vector<std::string>& argv,
                   bool waitForExit,
                   std::shared_ptr<CRunningProcess>& process);
  std::shared_ptr<CRunningProcess> GetAddonProcess(
      const std::shared_ptr<ADDON::CAddonInfo>& addonInfo,
      IInstanceHandler* handler,
      const KODI_ADDON_INSTANCE_INFO* firstInstance,
      std::shared_ptr<ADDON::IAddon>& addon);
  void ReleaseAddonProcess(const std::shared_ptr<CRunningProcess>& process,
                           IInstanceHandler* handler);
  std::shared_ptr<CRunningProcess> GetRunningAddonProcess(const std::string& addonId) const;
  std::shared_ptr<ADDON::IAddon> GetRunningAddon(const std::string& addonId) const;

  std::vector<ProcessInfoData> GetProcessInfoData();
  const std::string& GetDevkitPath() const { return m_devkitPath; }
  const std::string& GetDevkitLibPath() const { return m_devkitLib; }
  const std::string& GetDevkitLaunchExe() const { return m_devkitLaunchExe; }
  const std::string& GetDevkitReportPath() const { return m_devkitReportPath; }

private:
  void Process() override;

  bool CheckNoSandboxUsage(const std::shared_ptr<ADDON::IAddon>& addon);

  ADDON::CAddonMgr& m_addonMgr;
  const std::string m_devkitPath;
  const std::string m_devkitLib;
  const std::string m_devkitLaunchExe;
  const std::string m_devkitReportPath;
  std::atomic_bool m_running{false};
  bool m_onSystemStop{false};
  mutable std::mutex m_lock;
  std::map<std::string, std::shared_ptr<CRunningProcess>> m_runningProcesses;
};

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
