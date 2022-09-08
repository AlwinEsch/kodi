/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <memory>

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

struct ProcessInfoData
{
  std::shared_ptr<ADDON::IAddon> m_addon;
  int64_t m_pid{-1};
  int m_usedThreadsAmount{-1};
  int m_usedCPUPercentage{0};
  uint64_t m_usedPhysicalMemoryAmount{0};
  uint64_t m_usedVirtualMemoryAmount{0};
  uint64_t m_usedSharedMemoryAmount{0};
};

class CProcessInfo
{
public:
  CProcessInfo(std::shared_ptr<ADDON::IAddon> addon, int64_t pid) : m_addon(addon), m_pid(pid) {}
  virtual ~CProcessInfo() = default;

  virtual ProcessInfoData Calculate() = 0;

protected:
  std::shared_ptr<ADDON::IAddon> m_addon;
  int64_t m_pid;
};

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
