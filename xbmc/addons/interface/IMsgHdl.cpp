/*
 *  Copyright (C) 2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "IMsgHdl.h"

#include "RunningProcess.h"

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

IMsgHdl::IMsgHdl(CInterface& interface) : m_interface(interface), m_process(interface.m_process)
{
}

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
