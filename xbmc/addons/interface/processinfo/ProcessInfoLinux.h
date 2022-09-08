/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/*---AUTO_GEN_PARSE<$$CORE_SYSTEM_NAME:linux>---*/

#pragma once

#include "ProcessInfo.h"

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

class CProcessInfoLinux : public CProcessInfo
{
public:
  CProcessInfoLinux(std::shared_ptr<ADDON::IAddon> addon, int64_t pid);

  ProcessInfoData Calculate() override;

private:
  static const int UPDATE_INTERVAL = 500;
  mutable unsigned int m_lastInfoTime{static_cast<unsigned int>(-UPDATE_INTERVAL)};

  uint64_t m_lastTotalCPU;
  uint64_t m_lastProcessCPU;
  int m_numProcessors{0};
  ProcessInfoData m_currentInfo;
};

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
