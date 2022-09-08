/*
 *  Copyright (C) 2012-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "GameClientController.h"

#include "ServiceBroker.h"
#include "addons/AddonManager.h"

using namespace ADDON;
using namespace KODI::GAME;

CGameClientController::~CGameClientController()
{
  Deinit();
}

bool CGameClientController::Init()
{
  CServiceBroker::GetAddonMgr().Events().Subscribe(this, &CGameClientController::OnEvent);
  Update();
  return false;
}

void CGameClientController::Deinit()
{
  CServiceBroker::GetAddonMgr().Events().Unsubscribe(this);
}

bool CGameClientController::GetAvailableGameClients(GameClientInfoVector& clients, GameInfoSelection select)
{
  std::unique_lock<CCriticalSection> lock(m_critSection);

  for (auto &addon : m_gameClients)
  {
    if (select == GameInfoSelection::ALL ||
        (select == GameInfoSelection::INSTALLED && addon->IsInstalled()) ||
        (select == GameInfoSelection::INSTALLABLE && !addon->IsInstalled()) ||
        (select == GameInfoSelection::ENABLED && addon->IsEnabled()) ||
        (select == GameInfoSelection::DISABLED && !addon->IsEnabled()))
    clients.emplace_back(addon);
  }

  return !clients.empty();
}

GameClientInfoPtr CGameClientController::GetGameClient(const std::string& addonId)
{
  GameClientInfoPtr gameClient;

  std::unique_lock<CCriticalSection> lock(m_critSection);

  auto itGameClient = std::find_if(m_gameClients.begin(), m_gameClients.end(),
    [&addonId](const auto& gameClient)
    {
      return gameClient->ID() == addonId;
    });

  if (itGameClient != m_gameClients.end())
    gameClient = *itGameClient;

  return gameClient;
}

void CGameClientController::OnEvent(const AddonEvent& event)
{
  if (typeid(event) == typeid(AddonEvents::Enabled) ||
      typeid(event) == typeid(AddonEvents::Disabled) ||
      typeid(event) == typeid(AddonEvents::ReInstalled))
  {
    if (CServiceBroker::GetAddonMgr().HasType(event.addonId, ADDON_GAMEDLL))
    {
      Update();
    }
  }
  else if (typeid(event) == typeid(AddonEvents::UnInstalled))
  {
    Update();
  }
}

void CGameClientController::Update()
{
  GameClientInfoVector gameClients;

  std::vector<std::shared_ptr<CAddonInfo>> infos;
  if (CServiceBroker::GetAddonMgr().GetAddonInfos(infos, false, ADDON_GAMEDLL))
  {
    for (const auto& info : infos)
    {
      const bool enabled = !CServiceBroker::GetAddonMgr().IsAddonDisabled(info->ID());
      gameClients.emplace_back(std::make_shared<CGameClientInfo>(info, true, enabled));
    }
  }

  std::vector<std::shared_ptr<IAddon>> installable;
  if (CServiceBroker::GetAddonMgr().GetInstallableAddons(installable, ADDON_GAMEDLL))
  {
    for (auto& gameClient : installable)
    {
      gameClients.emplace_back(std::make_shared<CGameClientInfo>(std::dynamic_pointer_cast<CAddon>(gameClient)->GetAddonInfo(), false, false));
    }
  }

  {
    std::unique_lock<CCriticalSection> lock(m_critSection);
    m_gameClients = std::move(gameClients);
  }
}
