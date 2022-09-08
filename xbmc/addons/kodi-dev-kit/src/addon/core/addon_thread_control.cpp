/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "addon_thread_control.h"

#include "../../../include/kodi/tools/StringUtils.h"
#include "../../shared/ShareProcessReceiver.h"
#include "../../shared/ShareProcessTransmitter.h"
#include "addon_control.h"

using kodi::tools::StringUtils;
using namespace KODI::ADDONS::INTERFACE;

namespace KODI_ADDON
{
namespace INTERNAL
{

CAddonThreadIfc::CAddonThreadIfc(int64_t identifier, CAddonControl* process, bool processOutside)
  : m_mainThread(identifier == 0),
    m_identifier(identifier),
    m_id(StringUtils::Format("%s-ifc-%05i", process->GetUUID().c_str(), m_identifier)),
    m_processOutside(processOutside),
    m_process(process)
{
  m_shareProcessReceiver = new CShareProcessReceiver(m_id + "-srvchl", m_mainThread);
  m_shareProcessTransmitter = new CShareProcessTransmitter(m_id + "-chlsrv", m_mainThread);

  fprintf(stderr, "DEBUG: Started thread class with id '%s'\n", m_id.c_str());
}

CAddonThreadIfc::~CAddonThreadIfc()
{
  Destroy();
  delete m_shareProcessReceiver;
  delete m_shareProcessTransmitter;
}

bool CAddonThreadIfc::Create()
{
  if (!m_shareProcessTransmitter->Create(false))
  {
    fprintf(stderr,
            "FATAL: Failed to create shared process memory from addon to Kodi (id '%s') (Last "
            "error: %s)\n",
            m_id.c_str(), m_shareProcessTransmitter->GetLastErrorText().c_str());
    return false;
  }

  if (!m_shareProcessReceiver->Create(false, !m_processOutside))
  {
    fprintf(stderr,
            "FATAL: Failed to create shared process memory from Kodi to addon (id '%s') (Last "
            "error: %s)\n",
            m_id.c_str(), m_shareProcessReceiver->GetLastErrorText().c_str());
    return false;
  }

  m_shareProcessReceiver->RegisterReceiver(std::bind(&CAddonControl::FuncCall, m_process,
                                                     std::placeholders::_1, std::placeholders::_2,
                                                     std::placeholders::_3, std::placeholders::_4));

  return true;
}

void CAddonThreadIfc::Destroy()
{
  m_shareProcessTransmitter->Destroy();
  m_shareProcessReceiver->Destroy();
}

void CAddonThreadIfc::DestroyReceiver()
{
  m_shareProcessReceiver->Destroy();
}

bool CAddonThreadIfc::SendMessage(int group, int func)
{
  return m_shareProcessTransmitter->SendMessage(group, func);
}

bool CAddonThreadIfc::SendMessage(int group, int func, const msgpack::sbuffer& in)
{
  return m_shareProcessTransmitter->SendMessage(group, func, in);
}

bool CAddonThreadIfc::SendMessage(int group,
                                  int func,
                                  const msgpack::sbuffer& in,
                                  msgpack::sbuffer& ret)
{
  return m_shareProcessTransmitter->SendMessage(group, func, in, ret);
}

bool CAddonThreadIfc::SendMessageOnlyGetReturn(int group, int func, msgpack::sbuffer& ret)
{
  return m_shareProcessTransmitter->SendMessageOnlyGetReturn(group, func, ret);
}

bool CAddonThreadIfc::ProcessOutside()
{
  if (!m_processOutside)
    return false;

  return m_shareProcessReceiver->ProcessOutside();
}

bool CAddonThreadIfc::Active() const
{
  return m_shareProcessTransmitter->m_active || m_shareProcessReceiver->m_active;
}

} /* namespace INTERNAL */
} /* namespace KODI_ADDON */
