/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "IOffscreenRenderProcess.h"
#include "addons/interface/RunningProcess.h"
#include "utils/StringUtils.h"

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

namespace
{
static uint64_t nextUsableId = 0;
} /* namespace */

IOffscreenRenderProcess::IOffscreenRenderProcess()
  : m_usedId(nextUsableId++)
{
}

bool IOffscreenRenderProcess::CreateRenderHelper(CRunningProcess* process)
{
  if (m_renderHelper)
    return true;

  const std::string id = StringUtils::Format("{}-{:04}", process->GetUUID(), m_usedId);
  m_renderHelper = std::make_unique<CViewRendererOpenGL>(id);
  return true;
}

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
