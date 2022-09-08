/*
 *  Copyright (C) 2017-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "GameClientInfo.h"

#include "addons/addoninfo/AddonInfo.h"
#include "games/addons/GameClient.h"

using namespace ADDON;
using namespace KODI::GAME;

namespace
{

constexpr const char* EXTENSION_SEPARATOR = "|";
constexpr const char* EXTENSION_WILDCARD = "*";

constexpr const char* GAME_PROPERTY_EXTENSIONS = "extensions";
constexpr const char* GAME_PROPERTY_SUPPORTS_VFS = "supports_vfs";
constexpr const char* GAME_PROPERTY_SUPPORTS_STANDALONE = "supports_standalone";

/*
 * \brief Convert to lower case and canonicalize with a leading "."
 */
std::string NormalizeExtension(const std::string& strExtension)
{
  std::string ext = strExtension;

  if (!ext.empty() && ext != EXTENSION_WILDCARD)
  {
    StringUtils::ToLower(ext);

    if (ext[0] != '.')
      ext.insert(0, ".");
  }

  return ext;
}

} // namespace

CGameClientInfo::CGameClientInfo(const ADDON::AddonInfoPtr& info, bool installed, bool enabled)
  : m_addonInfo(info), m_installed(installed), m_enabled(enabled)
{
  std::vector<std::string> extensions = StringUtils::Split(
      info->Type(ADDON_GAMEDLL)->GetValue(GAME_PROPERTY_EXTENSIONS).asString(), EXTENSION_SEPARATOR);
  std::transform(extensions.begin(), extensions.end(),
                 std::inserter(m_extensions, m_extensions.begin()), NormalizeExtension);

  // Check for wildcard extension
  if (m_extensions.find(EXTENSION_WILDCARD) != m_extensions.end())
  {
    m_bSupportsAllExtensions = true;
    m_extensions.clear();
  }

  m_bSupportsVFS = info->Type(ADDON_GAMEDLL)->GetValue(GAME_PROPERTY_SUPPORTS_VFS).asBoolean();
  m_bSupportsStandalone =
      info->Type(ADDON_GAMEDLL)->GetValue(GAME_PROPERTY_SUPPORTS_STANDALONE).asBoolean();

}

bool CGameClientInfo::SupportsPath() const
{
  return !m_extensions.empty() || m_bSupportsAllExtensions;
}

bool CGameClientInfo::IsExtensionValid(const std::string& strExtension) const
{
  if (strExtension.empty())
    return false;

  if (SupportsAllExtensions())
    return true;

  return m_extensions.find(NormalizeExtension(strExtension)) != m_extensions.end();
}

GameClientPtr CGameClientInfo::GetActiveGameClient()
{
  if (!m_usedGameClient)
    m_usedGameClient = std::make_shared<CGameClient>(m_addonInfo);

  return m_usedGameClient;
}

