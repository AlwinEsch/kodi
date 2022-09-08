/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "SharedMemControl.h"

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

CSharedMemControl::CSharedMemControl(const std::string& identifier,
                                     size_t size /* = DEFAULT_SHARED_MEM_SIZE*/)
  : m_identifier(identifier), m_sharedMemSize(size)
{
}

void CSharedMemControl::SetActive()
{
  m_active = true;
}

void CSharedMemControl::SetInactive()
{
  m_active = false;
}

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
