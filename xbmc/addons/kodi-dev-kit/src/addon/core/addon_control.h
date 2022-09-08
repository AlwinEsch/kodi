/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "../../../include/kodi/c-api/addon_base.h"
#include "../../shared/DirectData.h"
#include "../../shared/Instances.h"
#define BACKWARD_HAS_DW 1
#include "../third_party/backward-cpp/backward.hpp"
#include "IMsgHdl.h"
#include "addon_thread_control.h"

#include <array>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>

namespace KODI_ADDON
{
namespace INTERNAL
{

class CAddonControl;
class IOffsceenRenderBase;

struct ATTR_DLL_LOCAL AddonIfc
{
  unsigned int api;
  std::string identifier;
  std::unique_ptr<CAddonControl> control{nullptr};
  bool viaMainThread{false};
  bool noReceive{false};
  bool killCalled{false};
  KODI::ADDONS::INTERFACE::UnionAddonFunctions functions{};
  std::condition_variable m_cv;

  // Data for case addon used without shared mem, not as independent app and direct to Kodi
  KODI::ADDONS::INTERFACE::DirectAddonIfc* direct{nullptr};
  bool direct_used{false};

  // Handle where contains first instance handle pointer in Kodi for addons where accessed
  // by Kodi, e.g. PVR.
  // This always set with first used instace pointer in Kodi, but only be used by C++ API and
  // if CAddonBase and IAddonInstance in same child.
  // On other addons where works itself, e.g. service or video addon, is it unused and 0.
  //   KODI_ADDON_INSTANCE_INFO* kodiFirstInstance{nullptr};

  static AddonIfc* g_ifc;
};

class ATTR_DLL_LOCAL CAddonControl
{
public:
  CAddonControl(AddonIfc* ifc);
  ~CAddonControl();

  bool Create();
  std::shared_ptr<CAddonThreadIfc> GetThreadIfc();
  bool ProcessOutside();
  bool FuncCall(int group, int func, const msgpack::unpacked& in, msgpack::sbuffer& out);

  const std::string& GetUUID() const { return m_ifc->identifier; }

  std::shared_ptr<IOffsceenRenderBase> GetOffscreenRenderBase();

private:
  AddonIfc* const m_ifc;

  const std::thread::id m_mainThreadId;
  const std::shared_ptr<CAddonThreadIfc> m_mainThread;
  std::set<std::shared_ptr<CAddonThreadIfc>> m_runningSubThreads;
  std::set<std::shared_ptr<CAddonThreadIfc>> m_unusedThreads;
  std::array<IMsgHdl*, KODI::ADDONS::INTERFACE::funcGroup_Max> m_handlers{nullptr};

  std::shared_ptr<IOffsceenRenderBase> m_offsceenRenderBase;

  std::mutex m_mutex;

  static void kodi_kill_child(void* thisClassHdl, const char* uuid);

  backward::StackTrace m_stackTrace;
  backward::SignalHandling m_signalHandling;
};

} /* namespace INTERNAL */
} /* namespace KODI_ADDON */
