/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "Controller.h"

#include "AppParams.h"
#include "Application.h"
#include "CompileInfo.h"
#include "ServiceBroker.h"
#include "addons/interface/RunningProcess.h"
#include "addons/interface/gui/GUIDialogCrashReporter.h"
#include "filesystem/Directory.h"
#include "filesystem/File.h"
#include "filesystem/SpecialProtocol.h"
#include "settings/AdvancedSettings.h"
#include "settings/SettingsComponent.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "utils/log.h"

#include <regex>

using namespace std::chrono_literals;

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

CController::CController(ADDON::CAddonMgr& addonMgr)
  : CThread("KODI::ADDONS::INTERFACE::CController"),
    m_addonMgr(addonMgr),
    m_devkitPath(CSpecialProtocol::TranslatePath("special://xbmcbinaddons/kodi.binary.devkit/")),
    m_devkitLib(CSpecialProtocol::TranslatePath(
        "special://xbmcbinaddons/kodi.binary.devkit/" DEVKIT_LIBRARY)),
    m_devkitLaunchExe(CSpecialProtocol::TranslatePath(
        "special://xbmcbinaddons/kodi.binary.devkit/" DEVKIT_LAUNCH_EXE)),
    m_devkitReportPath(CSpecialProtocol::TranslatePath("special://temp"))
{
}

CController::~CController()
{
  Deinit();
}

bool CController::Init()
{
  using namespace XFILE;

  if (!CFile::Exists(m_devkitLib, false))
  {
    CLog::Log(LOGFATAL, "Needed DevKit library {} not present!", m_devkitLib);
    return false;
  }
  if (!CFile::Exists(m_devkitLaunchExe, false))
  {
    CLog::Log(LOGFATAL, "Needed DevKit help launch executable {} not present!", m_devkitLaunchExe);
    return false;
  }

  CThread::Create();
  m_running = true;
  m_onSystemStop = false;
  return true;
}

void CController::Deinit()
{
  if (m_running)
  {
    m_running = false;
    CThread::StopThread();
  }
}

void CController::StopReport()
{
  std::unique_lock<std::mutex> lock(m_lock);
  m_onSystemStop = true;
}

bool CController::AddonLanguageSupported(const std::shared_ptr<ADDON::IAddon>& addon) const
{
  if (addon->Type(addon->Type())->Language() != ADDON::AddonLanguage::Unknown)
    return true;
  return false;
}

bool CController::LaunchAddon(const std::shared_ptr<ADDON::IAddon>& addon,
                              std::vector<std::string>& argv,
                              bool waitForExit,
                              std::shared_ptr<CRunningProcess>& process)
{
  std::shared_ptr<CRunningProcess> newProcess;
  {
    std::unique_lock<std::mutex> lock(m_lock);
    newProcess = std::make_shared<CRunningProcess>(
        addon, "kodi-subproc-" + StringUtils::CreateUUID(), CheckNoSandboxUsage(addon), nullptr);
    m_runningProcesses.emplace(addon->ID(), newProcess);
  }

  if (!newProcess->Launch(argv, waitForExit))
  {
    return false;
  }

  if (waitForExit)
  {
    std::unique_lock<std::mutex> lock(m_lock);
    m_runningProcesses.erase(newProcess->GetAddonID());
  }

  process = newProcess;
  return true;
}

std::shared_ptr<CRunningProcess> CController::GetAddonProcess(
    const std::shared_ptr<ADDON::CAddonInfo>& addonInfo,
    IInstanceHandler* handler,
    const KODI_ADDON_INSTANCE_INFO* firstInstance,
    std::shared_ptr<ADDON::IAddon>& addon)
{

  std::shared_ptr<CRunningProcess> newProcess;

  {
    std::unique_lock<std::mutex> lock(m_lock);

    const auto& addonInstances = m_runningProcesses.find(addonInfo->ID());
    if (addonInstances != m_runningProcesses.end())
    {
      newProcess = addonInstances->second;
      addon = newProcess->GetAddon();
    }
    else
    {
      addon = std::make_shared<ADDON::CAddon>(addonInfo, addonInfo->MainType());
      newProcess =
          std::make_shared<CRunningProcess>(addon, "kodi-subproc-" + StringUtils::CreateUUID(),
                                            CheckNoSandboxUsage(addon), firstInstance);
      std::vector<std::string> argv;
      if (!newProcess->Launch(argv, false))
      {
        return nullptr;
      }

      m_runningProcesses.emplace(addonInfo->ID(), newProcess);
    }
  }

  newProcess->SetEndtime(3000);
  newProcess->AddAddonInstance(handler);

  if (!addon)
  {
    CLog::Log(LOGFATAL, "CController:{}: Tried to get add-on '{}' who not available!", __func__,
              addonInfo->ID());
  }

  return newProcess;
}

void CController::ReleaseAddonProcess(const std::shared_ptr<CRunningProcess>& process,
                                      IInstanceHandler* handler)
{
  std::unique_lock<std::mutex> lock(m_lock);

  const auto& addon = m_runningProcesses.find(process->GetAddonID());
  if (addon == m_runningProcesses.end())
    return;

  process->ReleaseAddonInstance(handler);

  if (process->UsedInstanceCount() > 0)
    return;

  if (!process->UseEndTime())
  {
    process->Kill();

    m_runningProcesses.erase(addon);
  }
}

std::shared_ptr<CRunningProcess> CController::GetRunningAddonProcess(
    const std::string& addonId) const
{
  std::unique_lock<std::mutex> lock(m_lock);

  const auto& addonInstances = m_runningProcesses.find(addonId);
  if (addonInstances != m_runningProcesses.end())
    return addonInstances->second;

  return nullptr;
}

std::shared_ptr<ADDON::IAddon> CController::GetRunningAddon(const std::string& addonId) const
{
  const std::shared_ptr<CRunningProcess> process = GetRunningAddonProcess(addonId);
  if (process)
    return process->GetAddon();

  return nullptr;
}

std::vector<ProcessInfoData> CController::GetProcessInfoData()
{
  std::vector<ProcessInfoData> data;

  std::unique_lock<std::mutex> lock(m_lock);

  for (const auto& process : m_runningProcesses)
  {
    data.emplace_back(process.second->GetProcessInfoData());
  }

  return data;
}

void CController::Process()
{
  while (!m_bStop)
  {
    CThread::Sleep(500ms);

    std::unique_lock<std::mutex> lock(m_lock);

    for (auto it = m_runningProcesses.cbegin(); it != m_runningProcesses.cend();)
    {
      /*
       * Check the addon stopped itself, e.g. by normal way or with a crash.
       */
      const ChildStatus status = it->second->ProcessActive();
      if (status != ChildStatus::Running)
      {
        const auto addonId = it->second->GetAddonID();

        CLog::Log(LOGFATAL, "CController::{}: Addon '{}' process was uncontrolled stopped!",
                  __func__, addonId);
        it->second->InformStopReport(status);
        it->second->Kill();

        const auto uuid = it->second->GetUUID();
        const auto stacktrace = it->second->GetStackTrace();

        m_runningProcesses.erase(it++);

        /*
         * Check for case user interrupt Kodi with a e.g. Strg+C
         * and not start a GUI in this case!
         */
        if (!m_onSystemStop)
        {
          // Show Dialog with a non blocking call and to have this thread further available
          MESSAGING::HELPERS::ShowReportCrashDialog(std::move(addonId), std::move(uuid), std::move(stacktrace));
        }
        continue;
      }

      /*
       * Check run of addon, if it is ununsed a amount of time do a stop of it.
       * This mainly relates to addons where processed by Kodi, e.g. PVR, audiodecoder
       * and so on related addons.
       */
      if (it->second->UseEndTime() && !it->second->IsInUse())
      {
        if (it->second->EndTimeLeft() == 0ms)
        {
          CLog::Log(LOGDEBUG,
                    "CController::{}: Addon '{}' process was inactive and becomes stopped",
                    __func__, it->second->GetAddonID());
          it->second->Kill();
          m_runningProcesses.erase(it++);
          continue;
        }
      }

      it++;
    }
  }
}

bool CController::CheckNoSandboxUsage(const std::shared_ptr<ADDON::IAddon>& addon)
{
  if (CServiceBroker::GetAppParams()->IsNoSandbox())
    return true;

  const std::string path = addon->Type(addon->MainType())->LibPath();
  if (URIUtils::IsSharedLibrarySuffix(path))
  {
    const auto advanced = CServiceBroker::GetSettingsComponent()->GetAdvancedSettings();
    return !advanced->m_addonsSimulateBinaryLibAsExe;
  }

  return false;
}

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
