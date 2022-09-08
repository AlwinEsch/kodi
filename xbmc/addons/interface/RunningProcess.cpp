/*
 *  Copyright (C) 2005-2021 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "RunningProcess.h"

// Interface
#include "Controller.h"
#include "InstanceHandler.h"
#include "RunningThread.h"
#include "api/addon_base.h"

// Kodi
#include "ServiceBroker.h"
#include "addons/addoninfo/AddonInfo.h"
#include "addons/kodi-dev-kit/src/shared/DirectData.h"
#include "addons/kodi-dev-kit/src/shared/SharedData.h"
#include "addons/kodi-dev-kit/src/shared/api/processor.h"
#include "filesystem/File.h"
#include "utils/log.h"

#ifdef TARGET_LINUX
#include "processinfo/ProcessInfoLinux.h"
#endif
#include "launcher/LauncherDirectDll.h"
#ifdef TARGET_POSIX
#include "launcher/LauncherPosix.h"
#elif TARGET_WINDOWS
#include "launcher/LauncherWindows.h"
#else
#error No valid OS defined
#endif

using namespace std::chrono_literals;

namespace
{
#ifndef STR_HELPER
#define STR_HELPER(x) #x
#endif
#ifndef STR
#define STR(x) STR_HELPER(x)
#endif
constexpr const char* EXE_FLAG = "--addon";
constexpr const char* API_VERSION_FLAG = "--api";
constexpr const char* API_VERSION_VALUE = STR(__KDK_MAJOR__);
constexpr const char* DEBUG_FLAG = "--debug";
constexpr const char* DEVKIT_LIB_FLAG = "--dl";
constexpr const char* DEVKIT_DIR_FLAG = "--dir";
constexpr const char* DEVKIT_MAIN_SHARED_FLAG = "--main-shared";
constexpr const char* DEVKIT_DIRECT_API_FLAG = "--direct-api";
constexpr const char* DEVKIT_REPORT_PATH = "--report-path";
} // namespace

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

CRunningProcess::CRunningProcess(const std::shared_ptr<ADDON::IAddon>& addon,
                                 const std::string& uuid,
                                 bool noSandbox,
                                 const KODI_ADDON_INSTANCE_INFO* firstInstance)
  : CThread("CRunningProcess"),
    m_addon(addon),
    m_uuid(uuid),
#ifndef KODI_INHIBIT_SHARED
    m_noSandbox(noSandbox),
#else
    m_noSandbox(true),
#endif
    m_kodiInitInstancePtr(firstInstance),
    m_mainThread(m_threadIdentfier++, this),
    m_interface(this, m_noSandbox),
    m_cmdValue_exe(m_addon->Type(m_addon->MainType())->LibPath()),
    m_cmdValue_direct_api(
        StringUtils::Format("{}", static_cast<void*>(m_interface.m_directIfc.get()))),
    m_endMilliseconds(0ms)
{
  if (m_noSandbox)
    m_launcher = new CLauncherDirectDll(addon, uuid);
#ifndef KODI_INHIBIT_SHARED
  else
#ifdef TARGET_POSIX
    m_launcher = new CLauncherPosix(addon, uuid);
#elif TARGET_WINDOWS
    m_launcher = nullptr;
#else
    m_launcher = nullptr;
#endif
#endif
}

CRunningProcess::~CRunningProcess()
{
  Kill();
  delete m_launcher;
  delete m_processInfo;
}

bool CRunningProcess::MakeCommandLineValues(std::vector<char*>& argv_cstr)
{
  argv_cstr.reserve(m_argv.size() + 16);

  if (!m_noSandbox)
  {
    argv_cstr.emplace_back(
        const_cast<char*>(CServiceBroker::GetAddonIfcCtrl().GetDevkitLaunchExe().c_str()));
    argv_cstr.emplace_back(const_cast<char*>(EXE_FLAG));
  }

  argv_cstr.emplace_back(const_cast<char*>(m_cmdValue_exe.c_str()));

  if (m_noSandbox)
  {
    argv_cstr.emplace_back(const_cast<char*>(DEVKIT_DIRECT_API_FLAG));
    argv_cstr.emplace_back(const_cast<char*>(m_cmdValue_direct_api.c_str()));
  }

  argv_cstr.emplace_back(const_cast<char*>(API_VERSION_FLAG));
  argv_cstr.emplace_back(const_cast<char*>(API_VERSION_VALUE));

  argv_cstr.emplace_back(const_cast<char*>(DEVKIT_MAIN_SHARED_FLAG));
  argv_cstr.emplace_back(const_cast<char*>(m_uuid.c_str()));

  argv_cstr.emplace_back(const_cast<char*>(DEVKIT_LIB_FLAG));
  argv_cstr.emplace_back(
      const_cast<char*>(CServiceBroker::GetAddonIfcCtrl().GetDevkitLibPath().c_str()));

  argv_cstr.emplace_back(const_cast<char*>(DEVKIT_DIR_FLAG));
  argv_cstr.emplace_back(
      const_cast<char*>(CServiceBroker::GetAddonIfcCtrl().GetDevkitPath().c_str()));

  argv_cstr.emplace_back(const_cast<char*>(DEVKIT_REPORT_PATH));
  argv_cstr.emplace_back(
      const_cast<char*>(CServiceBroker::GetAddonIfcCtrl().GetDevkitReportPath().c_str()));

  argv_cstr.emplace_back(const_cast<char*>(DEBUG_FLAG));

  for (const auto& arg : m_argv)
    argv_cstr.emplace_back(const_cast<char*>(arg.c_str()));

  argv_cstr.emplace_back(nullptr);

  return true;
}

bool CRunningProcess::Launch(const std::vector<std::string>& argv, bool waitForExit)
{
  /*
   * Confirm the wanted executable is really present.
   */
  if (m_cmdValue_exe.empty() || !XFILE::CFile::Exists(m_cmdValue_exe))
  {
    CLog::Log(LOGERROR, "CRunningProcess::{}: Wanted add-on '{}' to run not present", __func__,  m_cmdValue_exe);
    return false;
  }

  /*
   * Start the addon main thread receiver.
   * NOTE This not relate to a real thread process itself only that it handle
   * addon his thread calls.
   */
  if (!m_mainThread.Create(waitForExit))
  {
    CLog::Log(LOGERROR, "CRunningProcess::{}: Failed to create process thread interface for addon",
              __func__, m_addon->ID());
    return false;
  }

  m_argv = argv;

  /*
   * Start addon from here, if wait for exit is set, as it should return direct
   * after performed work.
   *
   * This thought for helper apps where do one time by call the wanted work.
   *
   * For the normal cases the running thread becomes created where also start
   * the executable.
   */
  if (waitForExit || (m_noSandbox && m_launcher->SupportNonBlocking(m_cmdValue_exe)))
  {
    std::vector<char*> argv_cstr;
    if (!MakeCommandLineValues(argv_cstr))
    {
      CLog::Log(LOGERROR,
                "CRunningProcess::{}: Failed to parse command values for addon '{}' process",
                __func__, m_addon->ID());
      m_startFailed = true;
      m_startDone = true;
      return false;
    }

    if (!m_launcher->Launch(argv_cstr, waitForExit))
    {
      CLog::Log(LOGERROR, "CRunningProcess::{}: Failed to start addon '{}' process", __func__,
                m_addon->ID());
      m_startFailed = true;
      return false;
    }

    m_startFailed = false;
  }
  else
  {
    /*
     * Start here the thread and wait until done.
     * This wait needed to have addon usable for other Kodi places, otherwise
     * it can come that Kodi wants to call a non present place, e.g.
     * kodi_addon_create below.
     */
    CThread::Create();
    fprintf(stderr, "--------------> %i\n", ProcessActive());
    while (!m_startDone)
    {
      fprintf(stderr, "--------------> %i\n", ProcessActive());
      CThread::Sleep(100ms);
    }
  }

  return !m_startFailed;
}

bool CRunningProcess::Kill()
{
  // Check addon is running, if yes stop by normal way
  if (ProcessActive() == ChildStatus::Running)
  {
    // Inform addon of requested kill and to allow him needed work (e.g. stop process thread).
    if (!m_noSandbox)
    {
      msgpack::sbuffer in;
      msgpack::pack(in, msgChild__IN_kodi_processor_KillChild(m_uuid));
      SendMessage(funcGroup_Main, kodi_processor_KillChild, in);
    }
    else
    {
      void* hdl = m_interface.m_directIfc->to_addon.processor_h.thisClassHdl;
      if (m_interface.m_directIfc->to_addon.processor_h.kodi_kill_child)
      {
        m_interface.m_directIfc->to_addon.processor_h.kodi_kill_child(hdl, m_uuid.c_str());
      }
    }

    // Make the process kill with wait until done (if exceeds a time delay a kill becomes forced),
    m_launcher->Kill(true);
  }

  for (const auto& thread : m_runningSubThreads)
    thread->Destroy();
  m_runningSubThreads.clear();
  m_mainThread.Destroy();

  CThread::StopThread();

  return true;
}

ProcessInfoData CRunningProcess::GetProcessInfoData()
{
  static ProcessInfoData dummy;

  if (ProcessActive() != ChildStatus::Running)
  {
    return dummy;
  }

  if (!m_processInfo)
  {
#ifdef TARGET_LINUX
    m_processInfo = new CProcessInfoLinux(m_addon, m_launcher->GetMainProcessPID());
#endif
  }

  return m_processInfo->Calculate();
}

ChildStatus CRunningProcess::ProcessActive()
{
  if (m_launcher)
    return m_launcher->ProcessActive();

  return ChildStatus::NotStarted;
}

bool CRunningProcess::FuncCall(int group,
                               int func,
                               const msgpack::unpacked& in,
                               msgpack::sbuffer& out)
{
  if (group == funcGroup_Main)
  {
    switch (func)
    {
      case kodi_processor_CreateForNewThread:
      {
        msgParent__IN_kodi_processor_CreateForNewThread t = in.get().as<decltype(t)>();

        if (m_mainThread.GetID() != std::get<0>(t))
        {
          CLog::Log(LOGERROR,
                    "CRunningProcess::{}: Addon '{}' interface creation called from invalid place "
                    "'{}' (should '{}')",
                    __func__, m_addon->ID(), std::get<0>(t), m_mainThread.GetID());
          return false;
        }

        std::shared_ptr<CRunningThread> thread =
            std::make_shared<CRunningThread>(m_threadIdentfier++, this);
        if (!thread->Create(true))
        {
          CLog::Log(LOGERROR,
                    "CRunningProcess::{}: Failed to create new thread process for addon '{}'",
                    __func__, m_addon->ID());
          return false;
        }

        m_runningSubThreads.emplace(thread);

        msgpack::pack(out, msgParent_OUT_kodi_processor_CreateForNewThread(thread->GetID(),
                                                                           thread->GetThreadID()));
        return true;
      }
      default:
        return false;
    }
  }

  return m_interface.FuncCall(group, func, in, out);
}

bool CRunningProcess::SendMessage(int group, int func)
{
  // TODO Call with other thread?
  return m_mainThread.SendMessage(group, func);
}

bool CRunningProcess::SendMessage(int group, int func, const msgpack::sbuffer& in)
{
  // TODO Call with other thread?
  return m_mainThread.SendMessage(group, func, in);
}

bool CRunningProcess::SendMessage(int group,
                                  int func,
                                  const msgpack::sbuffer& in,
                                  msgpack::sbuffer& ret)
{
  // TODO Call with other thread?
  return m_mainThread.SendMessage(group, func, in, ret);
}

bool CRunningProcess::SendMessageOnlyGetReturn(int group, int func, msgpack::sbuffer& ret)
{
  // TODO Call with other thread?
  return m_mainThread.SendMessageOnlyGetReturn(group, func, ret);
}

void CRunningProcess::RegisterClass(IInterfaceClassHdl* ifcClass)
{
  std::unique_lock<std::mutex> lock(m_classRegisterMutex);
  m_activeIfcClasses.emplace(ifcClass);
}

void CRunningProcess::UnregisterClass(IInterfaceClassHdl* ifcClass)
{
  std::unique_lock<std::mutex> lock(m_classRegisterMutex);
  m_activeIfcClasses.erase(ifcClass);
}

void CRunningProcess::AddAddonInstance(IInstanceHandler* handler)
{
  // Paranoia check, if there a nullptr, then is really something wrong.
  if (handler == nullptr)
  {
    CLog::Log(LOGERROR, "CRunningProcess::{}: for Id '{}' called with empty instance handler",
              __func__, GetAddonID());
    return;
  }

  bool empty;

  {
    std::unique_lock<std::mutex> lock(m_mutex);

    empty = m_activeAddonHandlers.empty();

    // add the instance handler to the info to know used amount on addon
    m_activeAddonHandlers.insert(handler);
  }

  // Call addon create if no instances was used before, done without lock to prevent hangs if
  // target produces problems and not return and background controller also then blocked to
  // check.
  if (empty)
    m_interface.kodi_addon_base_h->kodi_addon_create_v1(handler->GetCInfo(), &m_hdl);
}

void CRunningProcess::ReleaseAddonInstance(IInstanceHandler* handler)
{
  // Paranoia check, if there a nullptr, then is really something wrong.
  if (handler == nullptr)
  {
    CLog::Log(LOGERROR, "CRunningProcess::{}: for Id '{}' called with empty instance handler",
              __func__, GetAddonID());
    return;
  }

  bool empty;

  {
    std::unique_lock<std::mutex> lock(m_mutex);

    auto presentHandler = m_activeAddonHandlers.find(handler);
    if (presentHandler == m_activeAddonHandlers.end())
      return;

    m_activeAddonHandlers.erase(presentHandler);

    empty = m_activeAddonHandlers.empty();
  }

  if (empty)
    m_interface.kodi_addon_base_h->kodi_addon_destroy_v1(m_hdl);
}

void CRunningProcess::InformStopReport(ChildStatus status)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  /*
   * Inform all running instances about uncontrolled stop by e.g. segemention
   * fault on addon.
   *
   * They can then stop his related threads and destroy related classes.
   * This also the reason about "++it;" as there can in for loop the amount
   * decreased.
   */
  for (auto it = m_activeAddonHandlers.cbegin(); it != m_activeAddonHandlers.cend();)
  {
    (*it)->StopReport(status);
    ++it;
  }
}

size_t CRunningProcess::UsedInstanceCount() const
{
  std::unique_lock<std::mutex> lock(m_mutex);
  return m_activeAddonHandlers.size();
}

const std::string& CRunningProcess::GetStackTrace() const
{
  return m_launcher ? m_launcher->GetStackTrace() : StringUtils::Empty;
}

void CRunningProcess::SetEndtime(unsigned int milliseconds)
{
  /*
   * Set the used endtime, this mostly used by addons where Kodi use as his
   * extension, e.g. PVR, audiodecoder and so on.
   *
   * Other independent running addons not use it as they stop itself.
   */
  m_endMilliseconds = std::chrono::milliseconds(milliseconds);
  m_endTimer.Set(m_endMilliseconds);
}

std::chrono::milliseconds CRunningProcess::EndTimeLeft() const
{
  return m_endTimer.GetTimeLeft();
}

bool CRunningProcess::UseEndTime() const
{
  return m_endMilliseconds > 0ms;
}

bool CRunningProcess::IsInUse() const
{
  /*
   * If no active instances are present return false and let wait timer expire
   * or in case of new instance to reset below again.
   */
  if (m_activeAddonHandlers.empty())
    return false;

  /*
   * Reset endtime to start.
   * Done on ever call (where normally only by "CController::Process()") to use
   * after addon no more used and stop his executable.
   */
  m_endTimer.Set(m_endMilliseconds);
  return true;
}

void CRunningProcess::Process()
{
  {
fprintf(stderr, "----------------> %s\n", m_addon->ID().c_str());
    /*
     * Launch addon from running thread.
     * This done for safeness, if the addon was started on another thread where
     * becomes stopped somewhere, becomes also the started executable stopped.
     */
    std::vector<char*> argv_cstr;
    if (!MakeCommandLineValues(argv_cstr))
    {
      CLog::Log(LOGERROR,
                "CRunningProcess::{}: Failed to parse command values for addon '{}' process",
                __func__, m_addon->ID());
      m_startFailed = true;
      m_startDone = true;
      return;
    }

    if (!m_launcher->Launch(argv_cstr, false))
    {
      CLog::Log(LOGERROR, "CRunningProcess::{}: Failed to start addon '{}' process", __func__,
                m_addon->ID());
      m_startFailed = true;
      m_startDone = true;
      return;
    }

    m_startDone = true;
  }

  /*
   * Do below the addon call to Kodi requests where done by his main thread.
   */
  while (!m_bStop)
  {
    if (!m_mainThread.ProcessOutside())
    {
      break;
    }
  }
}

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
