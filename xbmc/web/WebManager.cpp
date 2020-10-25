/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "WebManager.h"

#include "GUIUserMessages.h"
#include "ServiceBroker.h"
#include "addons/AddonManager.h"
#include "guilib/GUIComponent.h"
#include "guilib/GUIWebAddonControl.h"
#include "guilib/GUIWindowManager.h"
#include "utils/log.h"
#include "web/WebDatabase.h"
#include "windows/GUIWindowWebBrowser.h"

using namespace ADDON;
using namespace KODI::GUILIB;
using namespace KODI::WEB;

CWebManager::CWebManager(void) : m_addonControl(this)
{
}

CWebManager::~CWebManager(void)
{
  Deinit();
}

void CWebManager::Init()
{
  // initial check for enabled addons
  // if at least one pvr addon is enabled, PVRManager start up
  CJobManager::GetInstance().Submit([this] {
    SetState(ManagerState::Starting);

    CServiceBroker::GetAddonMgr().Events().Subscribe(this, &CWebManager::OnEvent);
    Deinit();
    UpdateAddons();

    SetState(ManagerState::Started);
  });
}

void CWebManager::Deinit()
{
  if (m_managerState == ManagerState::Stopped)
    return;

  // Before system becomes deinitialized and GUI, audio manager ... destroyed
  // make sure there comes no stream from addon
  SetMute(true);

  SetState(ManagerState::Stopping);

  MainShutdown();
  Clear();

  SetState(ManagerState::Stopped);

  CServiceBroker::GetAddonMgr().Events().Unsubscribe(this);
}

bool CWebManager::MainInitialize()
{
  CSingleLock lock(m_critSection);

  if (!m_activeWebbrowser)
    return false;
  return m_activeWebbrowser->MainInitialize();
}

void CWebManager::MainLoop()
{
  CSingleLock lock(m_critSection);

  if (m_activeWebbrowser)
    m_activeWebbrowser->MainLoop();
}

void CWebManager::MainShutdown()
{
  CSingleLock lock(m_critSection);

  CGUIWindowWebBrowser* runningWindow =
      CServiceBroker::GetGUI()->GetWindowManager().GetWindow<CGUIWindowWebBrowser>(
          WINDOW_WEB_BROWSER);
  if (runningWindow)
  {
    runningWindow->ControlDestroyAll();
  }

  m_addonControl.Clear();

  if (m_activeWebbrowser)
    m_activeWebbrowser->MainShutdown();
}

bool CWebManager::HasCreatedAddons() const
{
  CSingleLock lock(m_critSection);

  for (auto& addon : m_addonMap)
  {
    if (addon.second->ReadyToUse())
    {
      return true;
    }
  }

  return false;
}

bool CWebManager::HasBrowser() const
{
  CSingleLock lock(m_critSection);

  return m_activeWebbrowser != nullptr;
}

bool CWebManager::IsWebWindowActive() const
{
  return IsWebWindow(CServiceBroker::GetGUI()->GetWindowManager().GetActiveWindow());
}

bool CWebManager::IsWebWindow(int windowId) const
{
  return (windowId == WINDOW_WEB_BROWSER || windowId == WINDOW_WEB_BROWSER_FULLSCREEN);
}

bool CWebManager::GetFavourites(const std::string& path, CFileItemList& items)
{
  CWebDatabase database;
  if (!database.Open())
    return false;

  std::vector<FavouriteEntryPtr> entries;
  if (!database.GetFavourites(entries))
    return false;

  for (const auto& entry : entries)
  {
    CFileItemPtr pFileItem(new CFileItem());
    pFileItem->SetLabel(entry->GetName());
    pFileItem->SetLabel2(entry->GetURL());
    pFileItem->SetProperty("dateadded",
                           CVariant{entry->GetDateTimeAdded().GetAsLocalizedDateTime(true, false)});
    pFileItem->SetArt("icon", entry->GetIcon());
    pFileItem->SetPath("web://sites/favourites/" + entry->GetURL());
    items.Add(pFileItem);
  }

  return true;
}

bool CWebManager::GetRunningWebsites(const std::string& path, CFileItemList& items)
{
  for (const auto& control : m_runningGUIControls)
  {
    CFileItemPtr pFileItem(new CFileItem());
    pFileItem->SetLabel(control->OpenedTitle());
    pFileItem->SetLabel2(control->OpenedAddress());
    pFileItem->SetArt("icon", control->IconURL());
    pFileItem->SetProperty("browseridstring", CVariant{control->WebControlIdString()});
    pFileItem->SetProperty("width", CVariant{control->GetWidth()});
    pFileItem->SetProperty("height", CVariant{control->GetHeight()});
    pFileItem->SetPath("web://sites/running/" + control->WebControlIdString());
    items.Add(pFileItem);
  }

  return true;
}

void CWebManager::SetLanguage(const std::string& language)
{
  CSingleLock lock(m_critSection);

  for (const auto& addon : m_addonMap)
    addon.second->SetLanguage(language);
}

void CWebManager::SetMute(bool muted)
{
  CSingleLock lock(m_critSection);

  if (m_activeWebbrowser)
    m_activeWebbrowser->SetMute(muted);
}

void CWebManager::ExecuteItem(CFileItem& item)
{
  std::string url = item.GetPath();
  if (StringUtils::StartsWith(url, "web://sites/favourites/"))
  {
    StringUtils::Replace(url, "web://sites/favourites/", "");

    CServiceBroker::GetGUI()->GetWindowManager().ActivateWindow(WINDOW_WEB_BROWSER);
    CGUIMessage msg(GUI_MSG_WEB_NEW_TAB, 0, WINDOW_WEB_BROWSER);
    msg.SetLabel(url);
    CServiceBroker::GetGUI()->GetWindowManager().SendMessage(msg);
  }
  else if (StringUtils::StartsWith(url, "web://sites/running/"))
  {
    StringUtils::Replace(url, "web://sites/running/", "");

    CServiceBroker::GetGUI()->GetWindowManager().ActivateWindow(WINDOW_WEB_BROWSER);
    CGUIMessage msg(GUI_MSG_WEB_OPEN_TAB, 0, WINDOW_WEB_BROWSER);
    msg.SetLabel(url);
    CServiceBroker::GetGUI()->GetWindowManager().SendMessage(msg);
  }
}

void CWebManager::PublishEvent(WebEvent event)
{
  m_events.Publish(event);
}

void CWebManager::RegisterRunningWebGUIControl(CGUIWebAddonControl* control, int tabId)
{
  m_runningGUIControls.insert(m_runningGUIControls.begin() + tabId, control);
  m_events.Publish(WEB::ManagerChanged);
}

void CWebManager::UnregisterRunningWebGUIControl(CGUIWebAddonControl* control)
{
  for (auto it = m_runningGUIControls.begin(); it != m_runningGUIControls.end(); ++it)
  {
    if (*it == control)
    {
      m_runningGUIControls.erase(it);
      break;
    }
  }
  m_events.Publish(WEB::ManagerChanged);
}

void CWebManager::OnEvent(const AddonEvent& event)
{
}

bool CWebManager::IsInUse(const std::string& id)
{
  CSingleLock lock(m_critSection);

  const auto& itAddon = std::find_if(
      m_addonMap.begin(), m_addonMap.end(),
      [&id](const std::pair<int, CWebAddonPtr>& addon) { return addon.second->ID() == id; });
  if (itAddon != m_addonMap.end() && (*itAddon).second.use_count() > 1)
    return true;
  return false;
}

//------------------------------------------------------------------------------
// private:

CWebManager::ManagerState CWebManager::GetState(void) const
{
  CSingleLock lock(m_managerStateMutex);

  return m_managerState;
}

void CWebManager::SetState(CWebManager::ManagerState state)
{
  ObservableMessage observableMsg(ObservableMessageNone);

  {
    CSingleLock lock(m_managerStateMutex);
    if (m_managerState == state)
      return;

    m_managerState = state;

    WebEvent event;
    switch (state)
    {
      case ManagerState::Error:
        event = WEB::ManagerError;
        break;
      case ManagerState::Stopped:
        event = WEB::ManagerStopped;
        break;
      case ManagerState::Starting:
        event = WEB::ManagerStarting;
        break;
      case ManagerState::Stopping:
        event = WEB::ManagerStopped;
        break;
      case ManagerState::Interrupted:
        event = WEB::ManagerInterrupted;
        break;
      case ManagerState::Started:
        event = WEB::ManagerStarted;
        break;
      default:
        return;
    }
    m_events.Publish(event);
  }

  if (observableMsg != ObservableMessageNone)
  {
    SetChanged();
    NotifyObservers(observableMsg);
  }
}

void CWebManager::UpdateAddons()
{
  std::vector<AddonInfoPtr> addonInfos;
  CServiceBroker::GetAddonMgr().GetAddonInfos(addonInfos, false, ADDON_WEB_MANAGER);
  if (addonInfos.empty())
    return;

  for (auto& addon : addonInfos)
  {
    bool enabled = !CServiceBroker::GetAddonMgr().IsAddonDisabled(addon->ID());
    UpdateAddon(addon, enabled);
  }
}

void CWebManager::UpdateAddon(const AddonInfoPtr& addonInfo, bool enabled)
{
  if (enabled && (!IsKnown(addonInfo) || !IsCreatedAddon(addonInfo)))
  {
    std::hash<std::string> hasher;
    int iAddonId = static_cast<int>(hasher(addonInfo->ID()));
    if (iAddonId < 0)
      iAddonId = -iAddonId;

    CWebAddonPtr webAddon;
    ADDON_STATUS status = ADDON_STATUS_UNKNOWN;
    if (IsKnown(addonInfo))
    {
      GetAddon(iAddonId, webAddon);
      status = webAddon->Create(iAddonId);
    }
    else
    {
      webAddon = std::make_shared<CWebAddon>(addonInfo);
      if (!webAddon)
      {
        CLog::Log(LOGERROR, "CWebManager::{} - severe error, incorrect add-on type", __func__);
        return;
      }

      status = webAddon.get()->Create(iAddonId);

      // register the add-on
      if (m_addonMap.find(iAddonId) == m_addonMap.end())
      {
        m_addonMap.insert(std::make_pair(iAddonId, webAddon));
        m_addonNameIds.insert(make_pair(addonInfo->ID(), iAddonId));
      }
    }

    if (status != ADDON_STATUS_OK)
    {
      CLog::Log(LOGERROR, "CWebManager::{} - failed to create add-on {}, status = {}", __func__,
                addonInfo->Name(), status);
      if (status == ADDON_STATUS_PERMANENT_FAILURE)
      {
        CServiceBroker::GetAddonMgr().DisableAddon(addonInfo->ID(),
                                                   AddonDisabledReason::PERMANENT_FAILURE);
      }
    }
    else if (webAddon)
    {
      if (m_activeWebbrowser == nullptr)
        m_activeWebbrowser = webAddon;
    }
  }
  else if (IsCreatedAddon(addonInfo))
  {
    // stop add-on if it's no longer enabled, restart add-on if it's still enabled
    StopAddon(addonInfo, enabled);
  }
}

bool CWebManager::GetAddon(int addonId, CWebAddonPtr& addon) const
{
  bool bReturn(false);
  if (addonId <= INVALID_WEB_ADDON_ID)
    return bReturn;

  CSingleLock lock(m_critSection);

  const auto& entry = m_addonMap.find(addonId);
  if (entry != m_addonMap.end())
  {
    addon = entry->second;
    bReturn = true;
  }

  return bReturn;
}

int CWebManager::GetAddonId(const AddonInfoPtr& addon) const
{
  CSingleLock lock(m_critSection);

  for (const auto& entry : m_addonMap)
    if (entry.second->ID() == addon->ID())
      return entry.first;

  return -1;
}

bool CWebManager::IsKnown(const AddonInfoPtr& addon) const
{
  // database IDs start at 1
  return GetAddonId(addon) > 0;
}

bool CWebManager::IsCreatedAddon(const AddonInfoPtr& addon) const
{
  CSingleLock lock(m_critSection);

  for (const auto& entry : m_addonMap)
    if (entry.second->ID() == addon->ID())
      return entry.second->ReadyToUse();
  return false;
}

bool CWebManager::StopAddon(const AddonInfoPtr& addon, bool restart)
{
  CSingleLock lock(m_critSection);

  int iId = GetAddonId(addon);
  CWebAddonPtr mappedAddon;
  if (GetAddon(iId, mappedAddon))
  {
    if (restart)
      mappedAddon->ReCreate();
    else
    {
      const auto& entry = m_addonMap.find(iId);
      if (entry != m_addonMap.end())
        m_addonMap.erase(entry);

      mappedAddon->Destroy();
    }
    return true;
  }

  return false;
}

void CWebManager::Clear(void)
{
  CSingleLock lock(m_critSection);

  for (const auto& control : m_runningGUIControls)
  {
    control->SetWebAddonInvalid();
  }

  m_activeWebbrowser.reset();
  m_addonNameIds.clear();
  m_addonMap.clear();
}

//------------------------------------------------------------------------------
// class CWebManager::CAddonControl

void CWebManager::CAddonControl::RegisterControl(CWebAddonControl* control, const std::string id)
{
  m_runningWebAddonControls.insert(std::pair<std::string, CWebAddonControl*>(id, control));
}

void CWebManager::CAddonControl::UnregisterControl(const std::string id)
{
  m_runningWebAddonControls.erase(id);
}

WebAddonControlPtr CWebManager::CAddonControl::GetControl(const std::string id)
{
  const auto& entry = m_runningWebAddonControls.find(id);
  if (entry != m_runningWebAddonControls.end())
    return entry->second->shared_from_this();

  CWebAddonPtr addon = m_manager->GetActiveWebBrowser();
  if (!addon)
    return nullptr;

  WebAddonControlPtr control = std::make_shared<CWebAddonControl>(id, addon);
  RegisterControl(control.get(), id);
  return control;
}

void CWebManager::CAddonControl::Clear()
{
  m_runningWebAddonControls.clear();
}
