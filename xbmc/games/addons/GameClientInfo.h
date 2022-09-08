/*
 *  Copyright (C) 2017-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "addons/addoninfo/AddonInfo.h"
#include "games/GameTypes.h"

#include <memory>
#include <set>
#include <string>
#include <vector>

namespace KODI
{
namespace GAME
{

class CGameClientInfo
{
public:
  CGameClientInfo(const ADDON::AddonInfoPtr& info, bool installed, bool enabled);

  std::string ID() const { return m_addonInfo->ID(); }
  std::string Name() const { return m_addonInfo->Name(); }
  bool SupportsStandalone() const { return m_bSupportsStandalone; }
  bool SupportsPath() const;
  bool SupportsVFS() const { return m_bSupportsVFS; }
  const std::set<std::string>& GetExtensions() const { return m_extensions; }
  bool SupportsAllExtensions() const { return m_bSupportsAllExtensions; }
  bool IsExtensionValid(const std::string& strExtension) const;
  bool IsInstalled() const { return m_installed; }
  bool IsEnabled() const { return m_enabled; }

  GameClientPtr GetActiveGameClient();

private:
  // Game API xml parameters
  bool m_bSupportsVFS;
  bool m_bSupportsStandalone;
  std::set<std::string> m_extensions;
  bool m_bSupportsAllExtensions;

  GameClientPtr m_usedGameClient;

  const ADDON::AddonInfoPtr m_addonInfo;
  const bool m_installed;
  const bool m_enabled;
};

} // namespace GAME
} // namespace KODI
