/*
 *  Copyright (C) 2012-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "GameClientInfo.h"
#include "games/GameTypes.h"
#include "threads/CriticalSection.h"

#include <memory>
#include <vector>

namespace ADDON
{
struct AddonEvent;
}

namespace KODI
{
namespace GAME
{

enum class GameInfoSelection
{
  ALL,
  INSTALLABLE,
  INSTALLED,
  ENABLED,
  DISABLED
};

class CGameClientController
{
public:
  CGameClientController() = default;
  ~CGameClientController();

  bool Init();
  void Deinit();

  bool GetAvailableGameClients(GameClientInfoVector& clients, GameInfoSelection select);
  GameClientInfoPtr GetGameClient(const std::string& addonId);

protected:
  void Update();
  void OnEvent(const ADDON::AddonEvent& event);

private:
  CCriticalSection m_critSection;
  GameClientInfoVector m_gameClients;
};

} // namespace GAME
} // namespace KODI
