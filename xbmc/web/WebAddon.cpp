/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "WebAddon.h"

#include "Application.h"
#include "ServiceBroker.h"
#include "WebManager.h"
#include "application/ApplicationVolumeHandling.h"
#include "commons/Exception.h"
#include "filesystem/Directory.h"
#include "filesystem/SpecialProtocol.h"
#include "guilib/GUIWebAddonControl.h"
#include "messaging/ApplicationMessenger.h"
#include "settings/AdvancedSettings.h"
#include "settings/Settings.h"
#include "utils/URIUtils.h"
#include "utils/log.h"

using namespace ADDON;
using namespace KODI::ADDONS::INTERFACE;
using namespace KODI::MESSAGING;
using namespace KODI::WEB;

CWebAddon::CWebAddon(const AddonInfoPtr& addonInfo)
  : IInstanceHandler(this, ADDON_INSTANCE_WEB, addonInfo)
{
}

CWebAddon::~CWebAddon()
{
  Destroy();
}

void CWebAddon::ResetProperties(int addonId /* = -1 */)
{
  m_readyToUse = false;
  m_addonId = addonId;
}

ADDON_STATUS CWebAddon::Create(int addonId)
{
  /* ensure that a previous instance is destroyed */
  Destroy();

  /* reset all properties to defaults */
  ResetProperties(addonId);

  /* initialise the add-on */
  CLog::Log(LOGDEBUG, "CWebAddon::{}: - creating add-on instance '{}'", __func__, Name());

  ADDON_STATUS status = CreateInstance();
  if (status != ADDON_STATUS_OK)
    return status;

  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  WEB_ADDON_ERROR webstatus = ifc->kodi_addon_web_start_instance_v1(m_instance);
  if (webstatus != WEB_ADDON_ERROR_NO_ERROR)
    return ADDON_STATUS_PERMANENT_FAILURE;

  m_readyToUse = true;

  return ADDON_STATUS_OK;
}

void CWebAddon::Destroy()
{
  /* reset 'ready to use' to false */
  if (!m_readyToUse)
    return;
  m_readyToUse = false;

  CLog::Log(LOGDEBUG, "CWebAddon::{}: - destroying add-on '{}'", __func__, ID());

  /* destroy the add-on */
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  ifc->kodi_addon_web_stop_instance_v1(m_instance);
  DestroyInstance();

  /* reset all properties to defaults */
  ResetProperties();
}

void CWebAddon::ReCreate()
{
  int iAddonID(m_addonId);
  Destroy();

  /* recreate the instance */
  Create(iAddonID);
}

bool CWebAddon::MainInitialize()
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  m_readyToProcess = ifc->kodi_addon_web_main_initialize_v1(m_instance);
  m_initCalled = true;
  return m_readyToProcess;
}

void CWebAddon::MainLoop()
{
  // Init addon on this to confirm addon is loaded after Kodi's start
  if (!m_initCalled)
    MainInitialize();

  // Web browser main loop calls if init was OK
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  if (m_readyToProcess)
    ifc->kodi_addon_web_main_loop_v1(m_instance);
}

void CWebAddon::MainShutdown()
{
  // Shutdown the web browser addon
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  if (m_readyToProcess)
    ifc->kodi_addon_web_main_shutdown_v1(m_instance);
  m_readyToProcess = false;
}

bool CWebAddon::ReadyToUse() const
{
  return m_readyToUse;
}

int CWebAddon::GetID() const
{
  return m_addonId;
}

WEB_ADDON_ERROR CWebAddon::ControlCreate(const WEB_ADDON_GUI_PROPS& props,
                                         const std::string& startURL,
                                         WEB_KODI_CONTROL& handle)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  WEB_ADDON_ERROR err = ifc->kodi_addon_web_create_control_v1(m_instance, &props, startURL.c_str(), &handle);
  return err;
}

bool CWebAddon::DestroyControl(const WEB_KODI_CONTROL handle, bool complete)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  return ifc->kodi_addon_web_destroy_control_v1(m_instance, handle, complete);
}

void CWebAddon::RenderInit(KODI::ADDONS::INTERFACE::IOffscreenRenderProcess* offscreen,
                       const WEB_KODI_CONTROL handle)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  ifc->kodi_addon_web_control_render_init_v1(offscreen, m_instance, handle);
}

void CWebAddon::RenderDeinit(KODI::ADDONS::INTERFACE::IOffscreenRenderProcess* offscreen,
                       const WEB_KODI_CONTROL handle)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  ifc->kodi_addon_web_control_render_deinit_v1(offscreen, m_instance, handle);
}

void CWebAddon::Render(KODI::ADDONS::INTERFACE::IOffscreenRenderProcess* offscreen,
                       const WEB_KODI_CONTROL handle)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  ifc->kodi_addon_web_control_render_v1(offscreen, m_instance, handle);
}

bool CWebAddon::OpenWebsite(const WEB_KODI_CONTROL handle, const std::string& url)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  return ifc->kodi_addon_web_control_open_website_v1(m_instance, handle, url.c_str());
}

void CWebAddon::Reload(const WEB_KODI_CONTROL handle)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  ifc->kodi_addon_web_control_web_cmd_reload_v1(m_instance, handle);
}

void CWebAddon::StopLoad(const WEB_KODI_CONTROL handle)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  ifc->kodi_addon_web_control_web_cmd_stop_load_v1(m_instance, handle);
}

void CWebAddon::GoBack(const WEB_KODI_CONTROL handle)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  ifc->kodi_addon_web_control_web_cmd_nav_back_v1(m_instance, handle);
}

void CWebAddon::GoForward(const WEB_KODI_CONTROL handle)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  ifc->kodi_addon_web_control_web_cmd_nav_forward_v1(m_instance, handle);
}

void CWebAddon::OpenOwnContextMenu(const WEB_KODI_CONTROL handle)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  ifc->kodi_addon_web_control_web_open_own_context_menu_v1(m_instance, handle);
}

bool CWebAddon::Dirty(const WEB_KODI_CONTROL handle)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  return ifc->kodi_addon_web_control_dirty_v1(m_instance, handle);
}

bool CWebAddon::OnInit(const WEB_KODI_CONTROL handle)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  return ifc->kodi_addon_web_control_on_init_v1(m_instance, handle);
}

bool CWebAddon::OnAction(const WEB_KODI_CONTROL handle,
                         const KODI_ADDON_ACTION_DATA* action,
                         int& nextItem)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  return ifc->kodi_addon_web_control_on_action_v1(m_instance, handle, action, &nextItem);
}

bool CWebAddon::OnMouseEvent(const WEB_KODI_CONTROL handle,
                             int id,
                             double x,
                             double y,
                             double offsetX,
                             double offsetY,
                             int state)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  return ifc->kodi_addon_web_control_on_mouse_event_v1(m_instance, handle, id, x, y, offsetX,
                                                       offsetY, state);
}

bool CWebAddon::ControlGetHistory(const WEB_KODI_CONTROL handle,
                                  std::vector<std::string>& historyWebsiteNames,
                                  bool behindCurrent)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;

  char** historyList = nullptr;
  size_t listSize = 0;
  bool ret = ifc->kodi_addon_web_control_get_history_v1(m_instance, handle, &historyList, &listSize,
                                                        behindCurrent);
  if (ret && historyList)
  {
    for (size_t i = 0; i < listSize; ++i)
    {
      if (historyList[i])
      {
        historyWebsiteNames.emplace_back(historyList[i]);
        free(historyList[i]);
      }
    }
    free(historyList);
  }
  return ret;
}

void CWebAddon::ControlSearchText(const WEB_KODI_CONTROL handle,
                                  const std::string& text,
                                  bool forward,
                                  bool matchCase,
                                  bool findNext)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  ifc->kodi_addon_web_control_search_text_v1(m_instance, handle, text.c_str(), forward, matchCase,
                                             findNext);
}

void CWebAddon::ControlStopSearch(const WEB_KODI_CONTROL handle, bool clearSelection)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  ifc->kodi_addon_web_control_stop_search_v1(m_instance, handle, clearSelection);
}

void CWebAddon::ControlScreenSizeChange(
    const WEB_KODI_CONTROL handle, float x, float y, float width, float height, bool fullscreen)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  ifc->kodi_addon_web_control_screen_size_change_v1(m_instance, handle, x, y, width, height,
                                                    fullscreen);
}

void CWebAddon::SetMute(bool mute)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  ifc->kodi_addon_web_set_mute_v1(m_instance, mute);
}

void CWebAddon::SetLanguage(const std::string& language)
{
  const auto ifc = m_ifc->kodi_addoninstance_web_h;
  ifc->kodi_addon_web_set_language_v1(m_instance, language.c_str());
}

bool CWebAddon::LogError(const WEB_ADDON_ERROR error, const char* strMethod) const
{
  if (error != WEB_ADDON_ERROR_NO_ERROR)
  {
    CLog::Log(LOGERROR, "CWebAddon::{} - addon '{}' returned an error: {}", __func__, strMethod,
              ID(), ToString(error));
    return false;
  }
  return true;
}

const char* CWebAddon::ToString(const WEB_ADDON_ERROR error)
{
  switch (error)
  {
    case WEB_ADDON_ERROR_NO_ERROR:
      return "no error";
    case WEB_ADDON_ERROR_NOT_IMPLEMENTED:
      return "not implemented";
    case WEB_ADDON_ERROR_REJECTED:
      return "rejected by the addon";
    case WEB_ADDON_ERROR_INVALID_PARAMETERS:
      return "invalid parameters for this method";
    case WEB_ADDON_ERROR_FAILED:
      return "the command failed";
    case WEB_ADDON_ERROR_UNKNOWN:
    default:
      return "unknown error";
  }
}

/*!
  * @brief Callback functions from addon to kodi
  */
//@{

void CWebAddon::cb_inhibit_shutdown(bool inhibit)
{
  CServiceBroker::GetAppMessenger()->PostMsg(TMSG_INHIBITIDLESHUTDOWN, inhibit);
}

void CWebAddon::cb_inhibit_screensaver(bool inhibit)
{
  CServiceBroker::GetAppMessenger()->PostMsg(TMSG_INHIBITSCREENSAVER, inhibit);
}

bool CWebAddon::cb_is_muted()
{
  return (g_application.IsMuted() || g_application.GetVolumeRatio() <= CApplicationVolumeHandling::VOLUME_MINIMUM);
}

void CWebAddon::cb_control_set_control_ready(WEB_KODI_CONTROL handle, bool ready)
{
  CWebAddonControl* control = static_cast<CWebAddonControl*>(handle);
  assert(control);
  control->SetControlReady(ready);
}

void CWebAddon::cb_control_set_opened_address(WEB_KODI_CONTROL handle, const char* address)
{
  CWebAddonControl* control = static_cast<CWebAddonControl*>(handle);
  assert(control);
  assert(address);
  control->SetOpenedAddress(address);
}

void CWebAddon::cb_control_set_opened_title(WEB_KODI_CONTROL handle, const char* title)
{
  CWebAddonControl* control = static_cast<CWebAddonControl*>(handle);
  assert(control);
  assert(title);
  control->SetOpenedTitle(title);
}

void CWebAddon::cb_control_set_icon_url(WEB_KODI_CONTROL handle, const char* icon)
{
  CWebAddonControl* control = static_cast<CWebAddonControl*>(handle);
  assert(control);
  assert(icon);
  control->SetIconURL(icon);
}

void CWebAddon::cb_control_set_fullscreen(WEB_KODI_CONTROL handle, bool fullscreen)
{
  CWebAddonControl* control = static_cast<CWebAddonControl*>(handle);
  assert(control);

  ///TODO!!!!
}

void CWebAddon::cb_control_set_loading_state(WEB_KODI_CONTROL handle,
                                             bool isLoading,
                                             bool canGoBack,
                                             bool canGoForward)
{
  CWebAddonControl* control = static_cast<CWebAddonControl*>(handle);
  assert(control);
  control->SetLoadingState(isLoading, canGoBack, canGoForward);
}

void CWebAddon::cb_control_set_tooltip(WEB_KODI_CONTROL handle, const char* tooltip)
{
  CWebAddonControl* control = static_cast<CWebAddonControl*>(handle);
  assert(control);
  assert(tooltip);
  control->SetTooltip(tooltip);
}

void CWebAddon::cb_control_set_status_message(WEB_KODI_CONTROL handle, const char* status)
{
  CWebAddonControl* control = static_cast<CWebAddonControl*>(handle);
  assert(control);
  assert(status);
  control->SetStatusMessage(status);
}

void CWebAddon::cb_control_request_open_site_in_new_tab(WEB_KODI_CONTROL handle, const char* url)
{
  CWebAddonControl* control = static_cast<CWebAddonControl*>(handle);
  assert(control);
  assert(url);
  control->RequestOpenSiteInNewTab(url);
}
//@}
