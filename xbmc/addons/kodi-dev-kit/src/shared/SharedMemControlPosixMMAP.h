/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/*---AUTO_GEN_PARSE<$$CORE_SYSTEM_NAME:linux,freebsd,android,osx,darwin_embedded>---*/

#pragma once

#include "SharedMemControl.h"

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

class CSharedMemControlPosix : public CSharedMemControl
{
public:
  CSharedMemControlPosix(const std::string& identifier, size_t size = SHARED_DEFAULT_MEM_SIZE);
  ~CSharedMemControlPosix() override;

  bool Create(bool initial) override;
  bool Destroy() override;

  int64_t GetPID() const override;

  bool Lock_Caller() override;
  void Unlock_Caller() override;

  bool Lock_Target() override;
  void Unlock_Target() override;

  bool Lock() override;
  void Unlock() override;

private:
  int m_fd{-1};
  bool m_creator{false};
  bool m_created{false};
};

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
