/*
 *  Copyright (C) 2017-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "GameClientSubsystem.h"

#include "GameClient.h"
#include "GameClientProperties.h"
#include "addons/kodi-dev-kit/include/kodi/c-api/addon-instance/game.h"
#include "games/addons/cheevos/GameClientCheevos.h"
#include "games/addons/input/GameClientInput.h"
#include "games/addons/streams/GameClientStreams.h"

using namespace KODI;
using namespace KODI::ADDONS::INTERFACE;
using namespace GAME;

CGameClientSubsystem::CGameClientSubsystem(CGameClient& gameClient,
                                           CHdl_kodi_addoninstance_game_h& gameClientIfc,
                                           const KODI_ADDON_GAME_HDL gameClientHdl,
                                           CCriticalSection& clientAccess)
  : m_gameClient(gameClient),
    m_gameClientIfc(gameClientIfc),
    m_gameClientHdl(gameClientHdl),
    m_clientAccess(clientAccess)
{
}

CGameClientSubsystem::~CGameClientSubsystem() = default;

GameClientSubsystems CGameClientSubsystem::CreateSubsystems(
    CGameClient& gameClient,
    CHdl_kodi_addoninstance_game_h& gameClientIfc,
    const KODI_ADDON_GAME_HDL gameClientHdl,
    CCriticalSection& clientAccess)
{
  GameClientSubsystems subsystems = {};

  subsystems.Cheevos = std::make_unique<CGameClientCheevos>(gameClient, gameClientIfc, gameClientHdl);
  subsystems.Input.reset(
      new CGameClientInput(gameClient, gameClientIfc, gameClientHdl, clientAccess));
  subsystems.AddonProperties.reset(new CGameClientProperties(gameClient));
  subsystems.Streams.reset(new CGameClientStreams(gameClient));

  return subsystems;
}

void CGameClientSubsystem::DestroySubsystems(GameClientSubsystems& subsystems)
{
  subsystems.Cheevos.reset();
  subsystems.Input.reset();
  subsystems.AddonProperties.reset();
  subsystems.Streams.reset();
}

CGameClientCheevos& CGameClientSubsystem::Cheevos() const
{
  return m_gameClient.Cheevos();
}

CGameClientInput& CGameClientSubsystem::Input() const
{
  return m_gameClient.Input();
}

CGameClientProperties& CGameClientSubsystem::AddonProperties() const
{
  return m_gameClient.AddonProperties();
}

CGameClientStreams& CGameClientSubsystem::Streams() const
{
  return m_gameClient.Streams();
}
