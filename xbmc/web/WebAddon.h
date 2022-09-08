/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "addons/interface/InstanceHandler.h"
#include "addons/interface/api/addon-instance/web.h"

namespace KODI
{
namespace WEB
{

class CWebAddon;
typedef std::shared_ptr<CWebAddon> CWebAddonPtr;
#define INVALID_WEB_ADDON_ID -1

/*!
 * Interface from Kodi to a Web add-on.
 *
 * Also translates Kodi's C++ structures to the addon's C structures.
 */
class CWebAddon : public KODI::ADDONS::INTERFACE::IInstanceHandler
{
public:
  CWebAddon(const ADDON::AddonInfoPtr& addonInfo);
  ~CWebAddon() override;

  /** @name web add-on methods */
  //@{

  /*!
   * @brief Initialise the instance of this add-on.
   * @param iClientId The ID of this add-on.
   */
  ADDON_STATUS Create(int addonId);

  /*!
   * @brief Destroy the instance of this add-on.
   */
  void Destroy();

  /*!
   * @brief Destroy and recreate this add-on.
   */
  void ReCreate();

  /*!
   * @return True if this instance is initialised, false otherwise.
   */
  bool ReadyToUse() const;

  /*!
   * @return The ID of this instance.
   */
  int GetID() const;

  /*!
   * @brief From application main thread started Initialize call
   *
   * @return true if successfull initialized
   */
  bool MainInitialize();

  /*!
   * @brief From application main thread called main loop
   */
  void MainLoop();

  /*!
   * @brief From application main started shutdown call
   */
  void MainShutdown();

  /*!
   * @brief To inform addon about selected audio mute of Kodi
   */
  void SetMute(bool mute);

  /*!
   * @brief To inform addon about selected language of Kodi
   */
  void SetLanguage(const std::string& language);
  //@}

  /*!
   * GUI control related functions
   */
  //@{
  WEB_ADDON_ERROR ControlCreate(const WEB_ADDON_GUI_PROPS& props,
                                const std::string& startURL,
                                WEB_KODI_CONTROL& handle);
  bool DestroyControl(const WEB_KODI_CONTROL handle, bool complete);
  void RenderInit(KODI::ADDONS::INTERFACE::IOffscreenRenderProcess* offscreen,
              const WEB_KODI_CONTROL handle);
  void RenderDeinit(KODI::ADDONS::INTERFACE::IOffscreenRenderProcess* offscreen,
              const WEB_KODI_CONTROL handle);
  void Render(KODI::ADDONS::INTERFACE::IOffscreenRenderProcess* offscreen,
              const WEB_KODI_CONTROL handle);
  bool Dirty(const WEB_KODI_CONTROL handle);
  bool OnInit(const WEB_KODI_CONTROL handle);
  bool OnAction(const WEB_KODI_CONTROL handle, const KODI_ADDON_ACTION_DATA* action, int& nextItem);
  bool OnMouseEvent(const WEB_KODI_CONTROL handle,
                    int id,
                    double x,
                    double y,
                    double offsetX,
                    double offsetY,
                    int state);
  bool ControlGetHistory(const WEB_KODI_CONTROL handle,
                         std::vector<std::string>& historyWebsiteNames,
                         bool behindCurrent);
  void ControlSearchText(const WEB_KODI_CONTROL handle,
                         const std::string& text,
                         bool forward,
                         bool matchCase,
                         bool findNext);
  void ControlStopSearch(const WEB_KODI_CONTROL handle, bool clearSelection);
  void ControlScreenSizeChange(
      const WEB_KODI_CONTROL handle, float x, float y, float width, float height, bool fullscreen);
  //@}

  /*!
   * Internet browser related functions
   */
  //@{
  bool OpenWebsite(const WEB_KODI_CONTROL handle, const std::string& url);
  void Reload(const WEB_KODI_CONTROL handle);
  void StopLoad(const WEB_KODI_CONTROL handle);
  void GoBack(const WEB_KODI_CONTROL handle);
  void GoForward(const WEB_KODI_CONTROL handle);
  void OpenOwnContextMenu(const WEB_KODI_CONTROL handle);
  //@}

  /*!
   * @brief Callback functions from addon to kodi
   */
  //@{
  void cb_inhibit_shutdown(bool inhibit);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_web_inhibit_shutdown>---*/
  void cb_inhibit_screensaver(bool inhibit);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_web_inhibit_screensaver>---*/
  bool cb_is_muted();
  /*---AUTO_GEN_PARSE<CB:kodi_addon_web_is_muted>---*/
  void cb_control_set_control_ready(WEB_KODI_CONTROL handle, bool ready);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_web_control_set_control_ready>---*/
  void cb_control_set_opened_address(WEB_KODI_CONTROL handle, const char* address);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_web_control_set_opened_address>---*/
  void cb_control_set_opened_title(WEB_KODI_CONTROL handle, const char* title);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_web_control_set_opened_title>---*/
  void cb_control_set_icon_url(WEB_KODI_CONTROL handle, const char* icon);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_web_control_set_icon_url>---*/
  void cb_control_set_fullscreen(WEB_KODI_CONTROL handle, bool fullscreen);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_web_control_set_fullscreen>---*/
  void cb_control_set_loading_state(WEB_KODI_CONTROL handle,
                                    bool isLoading,
                                    bool canGoBack,
                                    bool canGoForward);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_web_control_set_loading_state>---*/
  void cb_control_set_tooltip(WEB_KODI_CONTROL handle, const char* tooltip);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_web_control_set_tooltip>---*/
  void cb_control_set_status_message(WEB_KODI_CONTROL handle, const char* status);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_web_control_set_status_message>---*/
  void cb_control_request_open_site_in_new_tab(WEB_KODI_CONTROL handle, const char* url);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_web_control_request_open_site_in_new_tab>---*/
  //@}

private:
  static const char* ToString(const WEB_ADDON_ERROR error);
  bool LogError(const WEB_ADDON_ERROR error, const char* strMethod) const;
  void ResetProperties(int addonId = -1);

  bool m_readyToUse{false}; /*!< true if this add-on is connected to the backend, false otherwise */
  bool m_readyToProcess{false};
  bool m_initCalled{false};
  int m_addonId{-1}; /*!< database ID of the add-on */

  CCriticalSection m_critSection; /*!< */
};

} /* namespace WEB */
} /* namespace KODI */
