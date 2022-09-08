/*
 *  Copyright (C) 2005-2021 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "RunningThread.h"

#include "RunningProcess.h"
#include "addons/addoninfo/AddonInfo.h"
#include "addons/kodi-dev-kit/src/shared/ShareProcessReceiver.h"
#include "addons/kodi-dev-kit/src/shared/ShareProcessTransmitter.h"
#include "utils/StringUtils.h"
#include "utils/log.h"

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

CRunningThread::CRunningThread(int64_t identifier, CRunningProcess* const process)
  : m_mainThread(identifier == 0),
    m_identifier(identifier),
    m_id(StringUtils::Format("{}-ifc-{:05}", process->GetUUID().c_str(), m_identifier)),
    m_process(process)
{
  m_shareProcessReceiver = new CShareProcessReceiver(m_id + "-chlsrv", m_mainThread);
  m_shareProcessTransmitter = new CShareProcessTransmitter(m_id + "-srvchl", m_mainThread);
}

CRunningThread::~CRunningThread()
{
  Destroy();
  delete m_shareProcessReceiver;
  delete m_shareProcessTransmitter;
}

bool CRunningThread::Create(bool useThread)
{
  if (!m_shareProcessTransmitter->Create(true))
  {
    CLog::Log(LOGERROR,
              "CRunningThread::{}: Failed to create shared process memory from Kodi to addon '{}' "
              "(ERROR {})",
              __func__, m_process->GetAddon()->ID(), m_shareProcessTransmitter->GetLastErrorText());
    return false;
  }

  if (!m_shareProcessReceiver->Create(true, useThread))
  {
    CLog::Log(LOGERROR,
              "CRunningThread::{}: Failed to create shared process memory from addon to Kodi '{}' "
              "(ERROR {})",
              __func__, m_process->GetAddon()->ID(), m_shareProcessReceiver->GetLastErrorText());
    return false;
  }

  m_shareProcessReceiver->RegisterReceiver(std::bind(&CRunningProcess::FuncCall, m_process,
                                                     std::placeholders::_1, std::placeholders::_2,
                                                     std::placeholders::_3, std::placeholders::_4));

  return true;
}

void CRunningThread::Destroy()
{
  m_shareProcessTransmitter->Destroy();
  m_shareProcessReceiver->Destroy();
}

bool CRunningThread::SendMessage(int group, int func)
{
  return m_shareProcessTransmitter->SendMessage(group, func);
}

bool CRunningThread::SendMessage(int group, int func, const msgpack::sbuffer& in)
{
  return m_shareProcessTransmitter->SendMessage(group, func, in);
}

bool CRunningThread::SendMessage(int group,
                                 int func,
                                 const msgpack::sbuffer& in,
                                 msgpack::sbuffer& ret)
{
  return m_shareProcessTransmitter->SendMessage(group, func, in, ret);
}

bool CRunningThread::SendMessageOnlyGetReturn(int group, int func, msgpack::sbuffer& ret)
{
  return m_shareProcessTransmitter->SendMessageOnlyGetReturn(group, func, ret);
}

bool CRunningThread::ProcessOutside()
{
  return m_shareProcessReceiver->ProcessOutside();
}

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
