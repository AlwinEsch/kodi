/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "settings/lib/ISettingCallback.h"
#include "threads/CriticalSection.h"
#include "utils/JobManager.h"
#include "utils/Observer.h"
#include "web/WebAddon.h"
#include "windows/GUIMediaWindow.h"

namespace KODI
{

namespace GUILIB
{
class CGUIWebAddonControl;
} /* namespace GUILIB */

namespace WEB
{

class CGUIWindowWebBrowser : public CGUIWindow
{
public:
  CGUIWindowWebBrowser();
  ~CGUIWindowWebBrowser() override;

  void OnInitWindow() override;
  void OnDeinitWindow(int nextWindowID) override;
  bool OnMessage(CGUIMessage& message) override;
  bool OnAction(const CAction& action) override;
  bool OnBack(int actionID) override;
  void SetInvalid() override;
  void OnWindowLoaded() override;
  void OnWindowUnload() override;
  bool HasWebControl() const override { return true; }

  std::string SelectedAddress();
  const std::string& OpenedAddress() const;
  const std::string& OpenedTitle() const;
  const std::string& Tooltip() const;
  const std::string& StatusMessage() const;
  const std::string& IconURL() const;
  bool IsLoading() const;
  bool CanGoBack() const;
  bool CanGoForward() const;

  GUILIB::CGUIWebAddonControl* GetRunningControl();
  bool ControlDestroyAll();

private:
  bool ControlCreate(const std::string& url = "");
  bool ControlDestroy(int itemNo, bool destroyAllOthers);
  bool SelectTab(int tab);

  mutable CCriticalSection m_mutex;
  GUILIB::CGUIWebAddonControl* m_originalWebControl{nullptr};
  GUILIB::CGUIWebAddonControl* m_activeWebControl{nullptr};
  std::map<int, GUILIB::CGUIWebAddonControl*> m_controls;
  std::unique_ptr<CFileItemList> m_vecItems{new CFileItemList()};
  CGUIViewControl m_viewControl;
  int m_selectedItem{-1};
  std::atomic_uint m_nextBrowserId{0};
  std::string m_lastSearchLabel;
};

} /* namespace WEB */
} /* namespace KODI */
