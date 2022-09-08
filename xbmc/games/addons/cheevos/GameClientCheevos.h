/*
 *  Copyright (C) 2020-2021 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "addons/interface/api/addon-instance/game.h"

#include <stddef.h> /* size_t */
#include <string>

namespace KODI
{
namespace RETRO
{
enum class RConsoleID;
}

namespace GAME
{

class CGameClient;

class CGameClientCheevos
{
public:
  CGameClientCheevos(CGameClient& gameClient,
                     KODI::ADDONS::INTERFACE::CHdl_kodi_addoninstance_game_h& gameClientIfc,
                     const KODI_ADDON_GAME_HDL gameClientHdl);

  bool RCGenerateHashFromFile(std::string& hash,
                              RETRO::RConsoleID consoleID,
                              const std::string& filePath);
  bool RCGetGameIDUrl(std::string& url, const std::string& hash);
  bool RCGetPatchFileUrl(std::string& url,
                         const std::string& username,
                         const std::string& token,
                         unsigned int gameID);
  bool RCPostRichPresenceUrl(std::string& url,
                             std::string& postData,
                             const std::string& username,
                             const std::string& token,
                             unsigned gameID,
                             const std::string& richPresence);
  void RCEnableRichPresence(const std::string& script);
  void RCGetRichPresenceEvaluation(std::string& evaluation, RETRO::RConsoleID consoleID);
  // When the game is reset, the runtime should also be reset
  void RCResetRuntime();

private:
  CGameClient& m_gameClient;
  KODI::ADDONS::INTERFACE::CHdl_kodi_addoninstance_game_h& m_gameClientIfc;
  const KODI_ADDON_GAME_HDL m_gameClientHdl;
};
} // namespace GAME
} // namespace KODI
