/*
 *  Copyright (C) 2016-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "GUIDialogSelectGameClient.h"

#include "dialogs/GUIDialogSelect.h"
#include "filesystem/AddonsDirectory.h"
#include "games/addons/GameClient.h"
#include "games/addons/GameClientInfo.h"
#include "guilib/GUIComponent.h"
#include "guilib/GUIWindowManager.h"
#include "guilib/LocalizeStrings.h"
#include "guilib/WindowIDs.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "utils/log.h"

using namespace KODI;
using namespace KODI::MESSAGING;
using namespace GAME;

std::string CGUIDialogSelectGameClient::ShowAndGetGameClient(const std::string& gamePath,
                                                             const GameClientInfoVector& candidates,
                                                             const GameClientInfoVector& installables)
{
  std::string gameClient;

  LogGameClients(candidates, installables);

  std::string extension = URIUtils::GetExtension(gamePath);

  // "Select emulator for {0:s}"
  CGUIDialogSelect* dialog =
      GetDialog(StringUtils::Format(g_localizeStrings.Get(35258), extension));
  if (dialog != nullptr)
  {
    // Turn the addons into items
    CFileItemList items;
    CFileItemList installableItems;
    for (const auto& candidate : candidates)
    {
      ADDON::AddonPtr addon;
      if (!CServiceBroker::GetAddonMgr().GetAddon(candidate->ID(), addon,
                                                  ADDON::ADDON_GAMEDLL,
                                                  ADDON::OnlyEnabled::CHOICE_YES))
        continue;

      CFileItemPtr item(XFILE::CAddonsDirectory::FileItemFromAddon(addon, candidate->ID()));
      item->SetLabel2(g_localizeStrings.Get(35257)); // "Installed"
      items.Add(std::move(item));
    }
    for (const auto& installable : installables)
    {
      ADDON::AddonPtr addon;
      if (!CServiceBroker::GetAddonMgr().FindInstallableById(installable->ID(), addon))
        continue;

      CFileItemPtr item(XFILE::CAddonsDirectory::FileItemFromAddon(addon, installable->ID()));
      installableItems.Add(std::move(item));
    }
    items.Sort(SortByLabel, SortOrderAscending);
    installableItems.Sort(SortByLabel, SortOrderAscending);

    items.Append(installableItems);

    dialog->SetItems(items);

    dialog->Open();

    // If the "Get More" button has been pressed, show a list of installable addons
    if (dialog->IsConfirmed())
    {
      int selectedIndex = dialog->GetSelectedItem();

      if (0 <= selectedIndex && selectedIndex < items.Size())
      {
        gameClient = items[selectedIndex]->GetPath();

        CLog::Log(LOGDEBUG, "Select game client dialog: User selected emulator {}", gameClient);
      }
      else
      {
        CLog::Log(LOGDEBUG, "Select game client dialog: User selected invalid emulator {}",
                  selectedIndex);
      }
    }
    else
    {
      CLog::Log(LOGDEBUG, "Select game client dialog: User cancelled game client installation");
    }
  }

  return gameClient;
}

CGUIDialogSelect* CGUIDialogSelectGameClient::GetDialog(const std::string& title)
{
  CGUIDialogSelect* dialog =
      CServiceBroker::GetGUI()->GetWindowManager().GetWindow<CGUIDialogSelect>(
          WINDOW_DIALOG_SELECT);
  if (dialog != nullptr)
  {
    dialog->Reset();
    dialog->SetHeading(CVariant{title});
    dialog->SetUseDetails(true);
  }

  return dialog;
}

void CGUIDialogSelectGameClient::LogGameClients(const GameClientInfoVector& candidates,
                                                const GameClientInfoVector& installable)
{
  CLog::Log(LOGDEBUG, "Select game client dialog: Found {} candidates",
            static_cast<unsigned int>(candidates.size()));
  for (const auto& gameClient : candidates)
    CLog::Log(LOGDEBUG, "Adding {} as a candidate", gameClient->ID());

  if (!installable.empty())
  {
    CLog::Log(LOGDEBUG, "Select game client dialog: Found {} installable clients",
              static_cast<unsigned int>(installable.size()));
    for (const auto& gameClient : installable)
      CLog::Log(LOGDEBUG, "Adding {} as an installable client", gameClient->ID());
  }
}
