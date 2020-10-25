/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "utils/Observer.h"
#include "web/WebAddon.h"
#include "web/WebAddonControl.h"
#include "web/WebEvent.h"

#include <deque>
#include <map>
#include <vector>

namespace KODI
{

namespace GUILIB
{
class CGUIWebAddonControl;
} /* namespace GUILIB */

namespace WEB
{

class CWebDatabase;
class CWebGUIInfo;

class CWebManager : public Observable, public ADDON::CAddonDllInformer
{
public:
  CWebManager();
  ~CWebManager() override;

  void Init();
  void Deinit();

  bool MainInitialize();
  void MainLoop();
  void MainShutdown();

  inline bool IsInitialising() const { return GetState() == ManagerState::Starting; }

  inline bool IsStarted() const { return GetState() == ManagerState::Started; }

  inline bool IsStopping() const { return GetState() == ManagerState::Stopping; }

  inline bool IsStopped() const { return GetState() == ManagerState::Stopped; }

  bool HasCreatedAddons() const;
  bool HasBrowser() const;
  bool IsWebWindowActive() const;
  bool IsWebWindow(int windowId) const;

  void SetLanguage(const std::string& language);
  void SetMute(bool muted);

  bool GetFavourites(const std::string& path, CFileItemList& items);
  bool GetRunningWebsites(const std::string& path, CFileItemList& items);
  const CWebAddonPtr& GetActiveWebBrowser() const { return m_activeWebbrowser; }

  void ExecuteItem(CFileItem& item);

  CEventStream<WebEvent>& Events() { return m_events; }
  void PublishEvent(WebEvent state);

  /*!
   * To handle controls on addon itself
   *
   * Kodi's web gui control for the same part can be used on several
   * places. Every of them use then the single defined on addon from
   * here.
   *
   * A control on addon becomes identified by a string, with this
   * can be the same used on different places.
   */
  class CAddonControl
  {
  public:
    CAddonControl(CWebManager* manager) : m_manager(manager) {}

    std::shared_ptr<CWebAddonControl> GetControl(const std::string id);
    void Clear();

  private:
    std::map<std::string, CWebAddonControl*> m_runningWebAddonControls;
    CWebManager* m_manager;

    void RegisterControl(CWebAddonControl* control, const std::string id);

    /* Have UnregisterWebAddonControl(...) as friend part for CWebAddonControl */
    friend class CWebAddonControl;
    void UnregisterControl(const std::string id);
  } m_addonControl;

  void RegisterRunningWebGUIControl(GUILIB::CGUIWebAddonControl* control, int tabId);
  void UnregisterRunningWebGUIControl(GUILIB::CGUIWebAddonControl* control);

protected:
  void OnEvent(const ADDON::AddonEvent& event);
  bool IsInUse(const std::string& id) override;

private:
  enum class ManagerState
  {
    Error = 0,
    Stopped,
    Starting,
    Stopping,
    Interrupted,
    Started
  };

  ManagerState GetState() const;
  void SetState(ManagerState state);

  void UpdateAddons();
  void UpdateAddon(const ADDON::AddonInfoPtr& addon, bool enabled);
  bool GetAddon(int addonId, CWebAddonPtr& addon) const;
  int GetAddonId(const ADDON::AddonInfoPtr& addon) const;
  bool IsKnown(const ADDON::AddonInfoPtr& addon) const;
  bool IsCreatedAddon(const ADDON::AddonInfoPtr& addon) const;
  bool StopAddon(const ADDON::AddonInfoPtr& addon, bool restart);
  void Clear(void);

  mutable CCriticalSection m_critSection;

  CEventSource<WebEvent> m_events;
  mutable CCriticalSection m_managerStateMutex;
  ManagerState m_managerState = ManagerState::Stopped;

  std::map<std::string, int> m_addonNameIds;
  std::map<int, CWebAddonPtr> m_addonMap;
  std::vector<GUILIB::CGUIWebAddonControl*> m_runningGUIControls;
  CWebAddonPtr m_activeWebbrowser;
};

} /* namespace WEB */
} /* namespace KODI */
