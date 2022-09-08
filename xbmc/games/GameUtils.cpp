/*
 *  Copyright (C) 2012-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "GameUtils.h"

#include "FileItem.h"
#include "ServiceBroker.h"
#include "URL.h"
#include "addons/Addon.h"
#include "addons/AddonInstaller.h"
#include "addons/AddonManager.h"
#include "cores/RetroPlayer/savestates/ISavestate.h"
#include "cores/RetroPlayer/savestates/SavestateDatabase.h"
#include "filesystem/SpecialProtocol.h"
#include "games/GameServices.h"
#include "games/addons/GameClient.h"
#include "games/addons/GameClientController.h"
#include "games/addons/GameClientInfo.h"
#include "games/dialogs/GUIDialogSelectGameClient.h"
#include "games/dialogs/GUIDialogSelectSavestate.h"
#include "games/tags/GameInfoTag.h"
#include "messaging/helpers/DialogOKHelper.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "utils/log.h"

#include <algorithm>

using namespace KODI;
using namespace GAME;

bool CGameUtils::FillInGameClient(CFileItem& item, std::string& savestatePath)
{
  using namespace ADDON;

  if (item.GetGameInfoTag()->GetGameClient().empty())
  {
    // If the fileitem is an add-on, fall back to that
    if (item.HasAddonInfo() && item.GetAddonInfo()->Type() == ADDON::ADDON_GAMEDLL)
    {
      item.GetGameInfoTag()->SetGameClient(item.GetAddonInfo()->ID());
    }
    else
    {
      if (!CGUIDialogSelectSavestate::ShowAndGetSavestate(item.GetPath(), savestatePath))
        return false;

      if (!savestatePath.empty())
      {
        RETRO::CSavestateDatabase db;
        std::unique_ptr<RETRO::ISavestate> save = RETRO::CSavestateDatabase::AllocateSavestate();
        db.GetSavestate(savestatePath, *save);
        item.GetGameInfoTag()->SetGameClient(save->GameClientID());
      }
      else
      {
        // No game client specified, need to ask the user
        GameClientInfoVector candidates;
        GameClientInfoVector installable;
        bool bHasVfsGameClient;
        GetGameClients(item, candidates, installable, bHasVfsGameClient);

        if (candidates.empty() && installable.empty())
        {
          // if: "This game can only be played directly from a hard drive or partition. Compressed files must be extracted."
          // else: "This game isn't compatible with any available emulators."
          int errorTextId = bHasVfsGameClient ? 35214 : 35212;

          // "Failed to play game"
          KODI::MESSAGING::HELPERS::ShowOKDialogText(CVariant{35210}, CVariant{errorTextId});
        }
        else if (candidates.size() == 1 && installable.empty())
        {
          // Only 1 option, avoid prompting the user
          item.GetGameInfoTag()->SetGameClient(candidates[0]->ID());
        }
        else
        {
          std::string gameClient = CGUIDialogSelectGameClient::ShowAndGetGameClient(
              item.GetPath(), candidates, installable);

          if (!gameClient.empty())
            item.GetGameInfoTag()->SetGameClient(gameClient);
        }
      }
    }
  }

  const std::string gameClient = item.GetGameInfoTag()->GetGameClient();
  if (gameClient.empty())
    return false;

  if (Install(gameClient))
  {
    // If the addon is disabled we need to enable it
    if (!Enable(gameClient))
    {
      CLog::Log(LOGDEBUG, "Failed to enable game client {}", gameClient);
      item.GetGameInfoTag()->SetGameClient("");
    }
  }
  else
  {
    CLog::Log(LOGDEBUG, "Failed to install game client: {}", gameClient);
    item.GetGameInfoTag()->SetGameClient("");
  }

  return !item.GetGameInfoTag()->GetGameClient().empty();
}

void CGameUtils::GetGameClients(const CFileItem& file,
                                GameClientInfoVector& candidates,
                                GameClientInfoVector& installable,
                                bool& bHasVfsGameClient)
{
  using namespace ADDON;

  bHasVfsGameClient = false;

  // Try to resolve path to a local file, as not all game clients support VFS
  CURL translatedUrl(CSpecialProtocol::TranslatePath(file.GetPath()));

  // Get local and remote candidates
  GameClientInfoVector gameClients;
  if (CServiceBroker::GetGameServices().GameClientController().GetAvailableGameClients(gameClients, GameInfoSelection::INSTALLED))
    GetGameClients(gameClients, translatedUrl, candidates, bHasVfsGameClient);
  if (CServiceBroker::GetGameServices().GameClientController().GetAvailableGameClients(gameClients, GameInfoSelection::INSTALLABLE))
    GetGameClients(gameClients, translatedUrl, installable, bHasVfsGameClient);

  // Sort by name
  //! @todo Move to presentation code
  auto SortByName = [](const GameClientInfoPtr& lhs, const GameClientInfoPtr& rhs) {
    std::string lhsName = lhs->Name();
    std::string rhsName = rhs->Name();

    StringUtils::ToLower(lhsName);
    StringUtils::ToLower(rhsName);

    return lhsName < rhsName;
  };

  std::sort(candidates.begin(), candidates.end(), SortByName);
  std::sort(installable.begin(), installable.end(), SortByName);
}

void CGameUtils::GetGameClients(const GameClientInfoVector& gameClients,
                                const CURL& translatedUrl,
                                GameClientInfoVector& candidates,
                                bool& bHasVfsGameClient)
{
  bHasVfsGameClient = false;

  const std::string extension = URIUtils::GetExtension(translatedUrl.Get());

  const bool bIsLocalFile =
      (translatedUrl.GetProtocol() == "file" || translatedUrl.GetProtocol().empty());

  for (auto& client : gameClients)
  {
    // Filter by extension
    if (!client->IsExtensionValid(extension))
      continue;

    // Filter by VFS
    if (!bIsLocalFile && !client->SupportsVFS())
    {
      bHasVfsGameClient = true;
      continue;
    }

    candidates.emplace_back(client);
  }
}

bool CGameUtils::HasGameExtension(const std::string& path)
{
  using namespace ADDON;

  // Get filename from CURL so that top-level zip directories will become
  // normal paths:
  //
  //   zip://%2Fpath_to_zip_file.zip/  ->  /path_to_zip_file.zip
  //
  std::string filename = CURL(path).GetFileNameWithoutPath();

  // Get the file extension
  std::string extension = URIUtils::GetExtension(filename);
  if (extension.empty())
    return false;

  StringUtils::ToLower(extension);

  // Look for a game client that supports this extension
  GameClientInfoVector gameClients;
  if (CServiceBroker::GetGameServices().GameClientController().GetAvailableGameClients(gameClients, GameInfoSelection::ALL))
  {
    for (auto& gameClient : gameClients)
    {
      if (gameClient->IsExtensionValid(extension))
        return true;
    }
  }

  return false;
}

std::set<std::string> CGameUtils::GetGameExtensions()
{
  using namespace ADDON;

  std::set<std::string> extensions;

  GameClientInfoVector gameClients;
  if (CServiceBroker::GetGameServices().GameClientController().GetAvailableGameClients(gameClients, GameInfoSelection::ALL))
  {
    for (auto& gameClient : gameClients)
    {
      extensions.insert(gameClient->GetExtensions().begin(), gameClient->GetExtensions().end());
    }
  }

  return extensions;
}

bool CGameUtils::IsStandaloneGame(const ADDON::AddonPtr& addon)
{
  using namespace ADDON;

  switch (addon->Type())
  {
    case ADDON_GAMEDLL:
    {
      auto client = CServiceBroker::GetGameServices().GameClientController().GetGameClient(addon->ID());
      if (client)
        return client->SupportsStandalone();
      return false;
    }
    case ADDON_SCRIPT:
    {
      return addon->HasType(ADDON_GAME);
    }
    default:
      break;
  }

  return false;
}

bool CGameUtils::Install(const std::string& gameClient)
{
  // If the addon isn't installed we need to install it
  bool installed = CServiceBroker::GetAddonMgr().IsAddonInstalled(gameClient);
  if (!installed)
  {
    ADDON::AddonPtr installedAddon;
    installed = ADDON::CAddonInstaller::GetInstance().InstallModal(
        gameClient, installedAddon, ADDON::InstallModalPrompt::CHOICE_NO);
    if (!installed)
    {
      CLog::Log(LOGERROR, "Game utils: Failed to install {}", gameClient);
      // "Error"
      // "Failed to install add-on."
      MESSAGING::HELPERS::ShowOKDialogText(CVariant{257}, CVariant{35256});
    }
  }

  return installed;
}

bool CGameUtils::Enable(const std::string& gameClient)
{
  bool bSuccess = true;

  if (CServiceBroker::GetAddonMgr().IsAddonDisabled(gameClient))
    bSuccess = CServiceBroker::GetAddonMgr().EnableAddon(gameClient);

  return bSuccess;
}
