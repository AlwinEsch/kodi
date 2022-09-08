/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "SharedData.h"

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

class CSharedMemControl
{
public:
  CSharedMemControl(const std::string& identifier, size_t size = SHARED_DEFAULT_MEM_SIZE);
  virtual ~CSharedMemControl() = default;

  virtual bool Create(bool initial) = 0;
  virtual bool Destroy() = 0;
  virtual int64_t GetPID() const = 0;

  virtual bool Lock_Caller() = 0;
  virtual void Unlock_Caller() = 0;

  virtual bool Lock_Target() = 0;
  virtual void Unlock_Target() = 0;

  virtual bool Lock() = 0;
  virtual void Unlock() = 0;

  void SetActive();
  void SetInactive();

  const std::string& GetIdentifier() const { return m_identifier; }
  apiShareData* GetSharedMem() { return m_sharedMem; }
  size_t GetSharedMemSize() const { return m_sharedMemSize; }
  const std::string& GetLastErrorText() const { return m_errorText; }

protected:
  apiShareData* m_sharedMem = nullptr;
  std::string m_errorText;

private:
  const std::string m_identifier;
  const size_t m_sharedMemSize;
  bool m_active{false};
};

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
