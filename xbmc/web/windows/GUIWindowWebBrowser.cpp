/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "GUIWindowWebBrowser.h"

#include "Application.h"
#include "GUIUserMessages.h"
#include "ServiceBroker.h"
#include "dialogs/GUIDialogKaiToast.h"
#include "dialogs/GUIDialogNumeric.h"
#include "dialogs/GUIDialogOK.h"
#include "dialogs/GUIDialogSelect.h"
#include "dialogs/GUIDialogYesNo.h"
#include "guilib/GUIComponent.h"
#include "guilib/GUIMessage.h"
#include "guilib/GUIWebAddonControl.h"
#include "guilib/GUIWindowManager.h"
#include "guilib/LocalizeStrings.h"
#include "input/Key.h"
#include "messaging/ApplicationMessenger.h"
#include "settings/MediaSettings.h"
#include "settings/Settings.h"
#include "threads/SingleLock.h"
#include "utils/Observer.h"
#include "utils/StringUtils.h"
#include "utils/log.h"
#include "web/WebManager.h"

using namespace KODI::GUILIB;
using namespace KODI::WEB;

#define CONTROL_DEFAULT_WEBADDON 2
#define CONTROL_EDIT_URL 110

#define CONTROL_LIST_ACTIVE_WEBSITE_TABS 9000
#define CONTROL_BUTTON_GO_BACK 9001
#define CONTROL_BUTTON_GO_FWD 9002
#define CONTROL_BUTTON_SEARCH 9020
#define CONTROL_EDIT_SEARCH 9021

CGUIWindowWebBrowser::CGUIWindowWebBrowser()
  : CGUIWindow(WINDOW_WEB_BROWSER, "MyWebBrowser.xml")
{
  m_loadType = KEEP_IN_MEMORY;
}

CGUIWindowWebBrowser::~CGUIWindowWebBrowser()
{
}

bool CGUIWindowWebBrowser::OnBack(int actionID)
{
  return CGUIWindow::OnBack(actionID);
}

void CGUIWindowWebBrowser::OnInitWindow()
{
  CGUIWindow::OnInitWindow();

  m_viewControl.SetCurrentView(CONTROL_LIST_ACTIVE_WEBSITE_TABS);
  m_viewControl.SetFocused();
  SET_CONTROL_VISIBLE(CONTROL_LIST_ACTIVE_WEBSITE_TABS);
}

void CGUIWindowWebBrowser::OnDeinitWindow(int nextWindowID)
{
  CGUIWindow::OnDeinitWindow(nextWindowID);
}

void CGUIWindowWebBrowser::OnWindowLoaded()
{
  CSingleLock lock(m_mutex);

  CGUIWindow::OnWindowLoaded();

  m_originalWebControl = dynamic_cast<CGUIWebAddonControl*>(GetControl(CONTROL_DEFAULT_WEBADDON));
  if (!m_originalWebControl)
  {
    CLog::Log(LOGERROR, "CGUIWindowWebBrowser::{}: Needed web control not defined on skin", __func__);
    return;
  }
  m_originalWebControl->SetVisible(false);

  m_viewControl.Reset();
  m_viewControl.SetParentWindow(GetID());
  m_viewControl.AddView(GetControl(CONTROL_LIST_ACTIVE_WEBSITE_TABS));

  m_selectedItem = 0;
  m_nextBrowserId = 0;

  // Add the item to add a new site
  CFileItemPtr itemSiteAdd(new CFileItem());
  itemSiteAdd->SetProperty("type", "add");
  itemSiteAdd->SetProperty("browserid", -1);
  itemSiteAdd->SetProperty("webcontrolid", "");
  m_vecItems->Add(itemSiteAdd);

  // Add the start website in selection list
  if (!ControlCreate())
  {
    CLog::Log(LOGERROR, "CGUIWindowWebBrowser::{}: Failed to create start website control", __func__);
    return;
  }

  if (!m_controls.empty())
    m_activeWebControl = m_controls[0];
}

void CGUIWindowWebBrowser::OnWindowUnload()
{
  CSingleLock lock(m_mutex);

  m_originalWebControl = nullptr;
  m_activeWebControl = nullptr;
  for (const auto& entry : m_controls)
  {
    CGUIWebAddonControl* control = entry.second;
    CServiceBroker::GetWEBManager().UnregisterRunningWebGUIControl(control);

    RemoveControl(control);
    delete control;
  }
  m_controls.clear();
  m_vecItems->Clear();
  m_viewControl.Reset();

  CGUIWindow::OnWindowUnload();
}

bool CGUIWindowWebBrowser::OnMessage(CGUIMessage& message)
{
  CSingleLock lock(m_mutex);

  switch (message.GetMessage())
  {
    case GUI_MSG_WEB_UPDATE_ADDRESS:
    {
      if (m_selectedItem >= 0 && m_selectedItem < m_vecItems->Size() - 1)
        m_vecItems->Get(m_selectedItem)->SetLabel2(message.GetLabel());
      SET_CONTROL_LABEL2(CONTROL_EDIT_URL, message.GetLabel());
      break;
    }
    case GUI_MSG_WEB_UPDATE_NAME:
    {
      if (m_selectedItem >= 0 && m_selectedItem < m_vecItems->Size() - 1)
        m_vecItems->Get(m_selectedItem)->SetLabel(message.GetLabel());
      break;
    }
    case GUI_MSG_WEB_UPDATE_ICON:
    {
      if (m_selectedItem >= 0 && m_selectedItem < m_vecItems->Size() - 1)
        m_vecItems->Get(m_selectedItem)->SetArt("icon", message.GetLabel());
      break;
    }
    case GUI_MSG_WEB_NEW_TAB:
    {
      for (int i = 0; i < m_vecItems->Size(); ++i)
      {
        if (m_vecItems->Get(i)->GetLabel2() == message.GetLabel())
          return SelectTab(i);
      }

      if (m_controls.size() == 1 && m_controls[0]->OpenedAddress().empty())
        m_controls[0]->SetLoadAddress(message.GetLabel());
      else
        ControlCreate(message.GetLabel());
    }
    case GUI_MSG_WEB_OPEN_TAB:
    {
      for (int i = 0; i < m_vecItems->Size(); ++i)
      {
        if (m_vecItems->Get(i)->GetProperty("webcontrolid").asString() == message.GetLabel())
          return SelectTab(i);
      }
      break;
    }
    default:
      break;
  }

  return CGUIWindow::OnMessage(message);
}

bool CGUIWindowWebBrowser::SelectTab(int tab)
{
  CFileItemPtr itemToChangeTo(m_vecItems->Get(tab));
  int browserId = itemToChangeTo->GetProperty("browserid").asInteger();
  const auto& control = m_controls.find(browserId);
  if (control == m_controls.end())
  {
    CLog::Log(LOGERROR, "CGUIWindowWebBrowser::{}: Requested website control (id: '{}') not found",
              __func__, browserId);
    return false;
  }

  m_selectedItem = tab;

  if (m_activeWebControl)
    m_activeWebControl->SetVisible(false);
  m_activeWebControl = control->second;
  m_activeWebControl->SetVisible(true);

  SET_CONTROL_LABEL2(CONTROL_EDIT_URL, m_activeWebControl->OpenedAddress());

  m_viewControl.SetItems(*m_vecItems);
  m_viewControl.SetSelectedItem(m_selectedItem);
  return true;
}

#define TABS_CONTEXT_DELETE_TAB 0
#define TABS_CONTEXT_DELETE_OTHER_TABS 1
#define TABS_CONTEXT_ADD_TO_FAVORITES 2
bool CGUIWindowWebBrowser::OnAction(const CAction& action)
{
  CSingleLock lock(m_mutex);

  int focusedControl = GetFocusedControlID();
  int id = action.GetID();
  switch (id)
  {
    case ACTION_CONTEXT_MENU:
    case ACTION_MOUSE_RIGHT_CLICK:
    {
      if (focusedControl == CONTROL_BUTTON_GO_BACK || focusedControl == CONTROL_BUTTON_GO_FWD)
      {
        std::vector<std::string> historyWebsiteNames;
        m_activeWebControl->GetHistory(historyWebsiteNames,
                                       focusedControl == CONTROL_BUTTON_GO_FWD);
        if (!historyWebsiteNames.empty())
        {
          // popup the context menu
          CContextButtons choices;
          for (unsigned int i = 0; i < historyWebsiteNames.size(); ++i)
            choices.Add(i, historyWebsiteNames[focusedControl == CONTROL_BUTTON_GO_FWD
                                                   ? i
                                                   : historyWebsiteNames.size() - 1 - i]);

          int choice = CGUIDialogContextMenu::ShowAndGetChoice(choices);
          if (choice >= 0)
          {
            for (int i = 0; i <= choice; ++i)
              CServiceBroker::GetGUI()->GetWindowManager().SendMessage(
                  focusedControl == CONTROL_BUTTON_GO_FWD ? GUI_MSG_WEB_CONTROL_GO_FWD
                                                          : GUI_MSG_WEB_CONTROL_GO_BACK,
                  GetID(), CONTROL_DEFAULT_WEBADDON);
          }
        }
        return true;
      }
      else if (focusedControl == CONTROL_LIST_ACTIVE_WEBSITE_TABS)
      {
        int selected = m_viewControl.GetSelectedItem();
        if (selected < 0 || selected >= m_vecItems->Size())
          break;

        /* No context on add item */
        if (m_vecItems->Get(selected)->GetProperty("type").asString() == "add")
          break;

        CContextButtons choices;
        choices.Add(TABS_CONTEXT_DELETE_TAB, g_localizeStrings.Get(920));
        if (m_selectedItem == selected && m_controls.size() > 1)
          choices.Add(TABS_CONTEXT_DELETE_OTHER_TABS, g_localizeStrings.Get(921));
        choices.Add(TABS_CONTEXT_ADD_TO_FAVORITES, g_localizeStrings.Get(922));
        int choice = CGUIDialogContextMenu::ShowAndGetChoice(choices);
        if (choice < 0)
          break;

        CFileItemPtr item(m_vecItems->Get(selected));
        switch (choice)
        {
          case TABS_CONTEXT_DELETE_TAB:
          {
            ControlDestroy(selected, false);
            break;
          }
          case TABS_CONTEXT_DELETE_OTHER_TABS:
          {
            ControlDestroy(selected, true);
            break;
          }
          case TABS_CONTEXT_ADD_TO_FAVORITES:
          {
            CServiceBroker::GetGUI()->GetWindowManager().SendMessage(
                GUI_MSG_WEB_CONTROL_ADD_TO_FAVOURITES, GetID(), CONTROL_DEFAULT_WEBADDON);
            break;
          }
          default:
            break;
        }
        return true;
      }
      break;
    }
    case ACTION_SELECT_ITEM:
    case ACTION_MOUSE_LEFT_CLICK:
    {
      if (focusedControl == CONTROL_LIST_ACTIVE_WEBSITE_TABS)
      {
        int selected = m_viewControl.GetSelectedItem();
        if (selected < 0 || selected >= m_vecItems->Size())
          return false;

        CFileItemPtr item(m_vecItems->Get(selected));
        std::string type = item->GetProperty("type").asString();
        if (type == "add")
        {
          ControlCreate();
        }
        else if (type == "website" && m_selectedItem != selected)
        {
          int id = item->GetProperty("browserid").asInteger();
          const auto& control = m_controls.find(id);
          if (control == m_controls.end())
          {
            CLog::Log(LOGERROR, "CGUIWindowWebBrowser::{}: Requested website control (id: '{}') not found",
                      __func__, id);
            return false;
          }

          m_activeWebControl->SetVisible(false);
          m_activeWebControl = control->second;
          m_activeWebControl->SetVisible(true);

          m_selectedItem = selected;
          m_vecItems->Get(selected)->SetArt("icon", m_activeWebControl->IconURL());
          m_vecItems->Get(selected)->SetLabel(m_activeWebControl->OpenedTitle());
          m_vecItems->Get(selected)->SetLabel2(m_activeWebControl->OpenedAddress());
          SET_CONTROL_LABEL2(CONTROL_EDIT_URL, m_activeWebControl->OpenedAddress());
        }
        return true;
      }
      else if (focusedControl == CONTROL_BUTTON_SEARCH)
      {
        CGUIMessage msg(GUI_MSG_ITEM_SELECTED, GetID(), CONTROL_EDIT_SEARCH);
        OnMessage(msg);
        std::string searchLabel = msg.GetLabel();
        if (!searchLabel.empty())
        {
          m_activeWebControl->SearchText(searchLabel, true, false,
                                         m_lastSearchLabel == searchLabel);
          m_lastSearchLabel = searchLabel;
        }
        else if (!m_lastSearchLabel.empty())
        {
          m_lastSearchLabel.clear();
          m_activeWebControl->StopSearch(true);
        }
      }
    }
    default:
      break;
  }

  return CGUIWindow::OnAction(action);
}

bool CGUIWindowWebBrowser::ControlCreate(const std::string& url)
{
  std::string id = StringUtils::Format("kodi_web_window_browser_id-%05i", m_nextBrowserId);

  CGUIWebAddonControl* control = new CGUIWebAddonControl(*m_originalWebControl);
  control->SetWebControlIdString(id);
  control->SetLoadAddress(url);
  control->AllocResources();
  m_controls[m_nextBrowserId] = control;
  InsertControl(control, m_originalWebControl);

  // Add the start website in selection list
  CFileItemPtr itemSiteNew(new CFileItem());
  itemSiteNew->SetLabel(g_localizeStrings.Get(919));
  itemSiteNew->SetLabel2(url);
  itemSiteNew->SetArt("icon", "DefaultFile.png");
  itemSiteNew->SetProperty("type", "website");
  itemSiteNew->SetProperty("browserid", m_nextBrowserId++);
  itemSiteNew->SetProperty("webcontrolid", id);
  m_vecItems->AddFront(itemSiteNew, m_vecItems->Size() - 1);

  m_viewControl.SetItems(*m_vecItems);

  m_selectedItem = m_vecItems->Size() - 2;
  m_viewControl.SetSelectedItem(m_selectedItem);
  if (m_activeWebControl)
    m_activeWebControl->SetVisible(false);
  m_activeWebControl = control;
  m_activeWebControl->SetVisible(true);

  CServiceBroker::GetWEBManager().RegisterRunningWebGUIControl(control, m_selectedItem);
  return true;
}

bool CGUIWindowWebBrowser::ControlDestroyAll()
{
  CSingleLock lock(m_mutex);

  return ControlDestroy(-1, false);
}

bool CGUIWindowWebBrowser::ControlDestroy(int itemNo, bool destroyAllOthers)
{
  if (m_controls.empty())
    return false;

  if (m_activeWebControl)
    m_activeWebControl->SetVisible(false);

  if (itemNo < 0 || m_controls.size() == 1)
  {
    for (const auto& it : m_controls)
    {
      CGUIWebAddonControl* control = it.second;
      CServiceBroker::GetWEBManager().UnregisterRunningWebGUIControl(control);
      RemoveControl(control);
      delete control;
    }

    m_controls.clear();
    m_activeWebControl = nullptr;
    CServiceBroker::GetGUI()->GetWindowManager().PreviousWindow();

    m_selectedItem = 0;
    m_nextBrowserId = 0;
    m_lastSearchLabel = "";

    // Add the item to add a new site
    CFileItemPtr itemSiteAdd(new CFileItem());
    itemSiteAdd->SetProperty("type", "add");
    itemSiteAdd->SetProperty("browserid", -1);
    itemSiteAdd->SetProperty("webcontrolid", "");
    m_vecItems->Clear();
    m_vecItems->Add(itemSiteAdd);
    m_viewControl.SetItems(*m_vecItems);

    return true;
  }

  CFileItemPtr item(m_vecItems->Get(itemNo));
  if (destroyAllOthers)
  {
    CFileItemPtr itemAddSite(m_vecItems->Get(m_vecItems->Size() - 1));
    int browserId = item->GetProperty("browserid").asInteger();
    const auto& control = m_controls.find(browserId);
    if (control == m_controls.end())
    {
      CLog::Log(LOGERROR, "CGUIWindowWebBrowser::{}: Requested website control (id: '{}') not found",
                __func__, browserId);
      return false;
    }

    for (auto it = m_controls.begin(); it != m_controls.end();)
    {
      if (it->first != browserId)
      {
        CGUIWebAddonControl* control = it->second;
        CServiceBroker::GetWEBManager().UnregisterRunningWebGUIControl(control);

        it = m_controls.erase(it);
        RemoveControl(control);
        delete control;
      }
      else
        ++it;
    }

    m_activeWebControl = control->second;

    m_vecItems->Clear();
    m_vecItems->Add(item);
    m_vecItems->Add(itemAddSite);
    m_selectedItem = m_vecItems->Size() - 2;
  }
  else
  {
    if (m_selectedItem == itemNo)
    {
      int switchTobrowserId;
      if (itemNo >= m_vecItems->Size() - 2)
      {
        m_selectedItem = itemNo - 1;
        switchTobrowserId = m_vecItems->Get(itemNo - 1)->GetProperty("browserid").asInteger();
      }
      else
        switchTobrowserId = m_vecItems->Get(itemNo + 1)->GetProperty("browserid").asInteger();

      const auto& control = m_controls.find(switchTobrowserId);
      if (control == m_controls.end())
      {
        CLog::Log(LOGERROR,
                  "CGUIWindowWebBrowser::{}: Requested website control (id: '{}') to switch to not found",
                  __func__, switchTobrowserId);
        return false;
      }
      m_activeWebControl = control->second;
    }

    if (itemNo <= m_selectedItem)
    {
      --m_selectedItem;
    }

    int browserId = m_vecItems->Get(itemNo)->GetProperty("browserid").asInteger();
    const auto& control = m_controls.find(browserId);
    if (control == m_controls.end())
    {
      CLog::Log(LOGERROR,
                "CGUIWindowWebBrowser::{}: Requested website control (id: '{}') to destroy not found",
                __func__, browserId);
      return false;
    }

    CServiceBroker::GetWEBManager().UnregisterRunningWebGUIControl(control->second);

    m_vecItems->Remove(itemNo);
    RemoveControl(control->second);

    m_controls.erase(browserId);
    delete control->second;
  }

  m_viewControl.SetItems(*m_vecItems);
  m_viewControl.SetSelectedItem(m_selectedItem);
  if (m_activeWebControl)
    m_activeWebControl->SetVisible(true);

  return true;
}

void CGUIWindowWebBrowser::SetInvalid()
{
  CGUIWindow::SetInvalid();
}

KODI::GUILIB::CGUIWebAddonControl* CGUIWindowWebBrowser::GetRunningControl()
{
  CSingleLock lock(m_mutex);

  return m_activeWebControl;
}

/*! Return value functions */
//{
std::string CGUIWindowWebBrowser::SelectedAddress()
{
  CGUIMessage msg(GUI_MSG_ITEM_SELECTED, GetID(), CONTROL_EDIT_URL);
  OnMessage(msg);
  return msg.GetLabel();
}

const std::string& CGUIWindowWebBrowser::OpenedAddress() const
{
  if (m_activeWebControl)
    return m_activeWebControl->OpenedAddress();
  return StringUtils::Empty;
}

const std::string& CGUIWindowWebBrowser::OpenedTitle() const
{
  if (m_activeWebControl)
    return m_activeWebControl->OpenedTitle();
  return StringUtils::Empty;
}

const std::string& CGUIWindowWebBrowser::Tooltip() const
{
  if (m_activeWebControl)
    return m_activeWebControl->Tooltip();
  return StringUtils::Empty;
}

const std::string& CGUIWindowWebBrowser::StatusMessage() const
{
  if (m_activeWebControl)
    return m_activeWebControl->StatusMessage();
  return StringUtils::Empty;
}

const std::string& CGUIWindowWebBrowser::IconURL() const
{
  if (m_activeWebControl)
    return m_activeWebControl->IconURL();
  return StringUtils::Empty;
}

bool CGUIWindowWebBrowser::IsLoading() const
{
  if (m_activeWebControl)
    return m_activeWebControl->IsLoading();
  return false;
}

bool CGUIWindowWebBrowser::CanGoBack() const
{
  if (m_activeWebControl)
    return m_activeWebControl->CanGoBack();
  return false;
}

bool CGUIWindowWebBrowser::CanGoForward() const
{
  if (m_activeWebControl)
    return m_activeWebControl->CanGoForward();
  return false;
}
