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
#include "addons/binary-addons/BinaryAddonBase.h"
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
using namespace KODI::MESSAGING;
using namespace KODI::WEB;

CWebAddon::CWebAddon(const AddonInfoPtr& addonInfo)
  : IAddonInstanceHandler(ADDON_INSTANCE_WEB, addonInfo)
{
  // Create all interface parts independent to make API changes easier if
  // something is added
  m_struct.props = new AddonProps_WebAddon();
  m_struct.toKodi = new AddonToKodiFuncTable_WebAddon();
  m_struct.toAddon = new KodiToAddonFuncTable_WebAddon();

  ResetProperties();
}

CWebAddon::~CWebAddon()
{
  Destroy();

  delete m_struct.props;
  delete m_struct.toKodi;
  delete m_struct.toAddon;
}

void CWebAddon::ResetProperties(int addonId /* = -1 */)
{
  m_readyToUse = false;
  m_addonId = addonId;

  m_strUserPath = CSpecialProtocol::TranslatePath(Profile());
  m_strAddonLibPath = CSpecialProtocol::TranslatePath(URIUtils::GetDirectory(Addon()->LibPath()));
  m_strAddonSharePath = CSpecialProtocol::TranslatePath(Path());

  m_struct.props->strUserPath = m_strUserPath.c_str();
  m_struct.props->strAddonLibPath = m_strAddonLibPath.c_str();
  m_struct.props->strAddonSharePath = m_strAddonSharePath.c_str();
  m_struct.toKodi->kodiInstance = this;
  m_struct.toKodi->inhibit_shutdown = cb_inhibit_shutdown;
  m_struct.toKodi->inhibit_screensaver = cb_inhibit_screensaver;
  m_struct.toKodi->is_muted = cb_is_muted;
  m_struct.toKodi->control_set_control_ready = cb_control_set_control_ready;
  m_struct.toKodi->control_set_opened_address = cb_control_set_opened_address;
  m_struct.toKodi->control_set_opened_title = cb_control_set_opened_title;
  m_struct.toKodi->control_set_icon_url = cb_control_set_icon_url;
  m_struct.toKodi->control_set_fullscreen = cb_control_set_fullscreen;
  m_struct.toKodi->control_set_loading_state = cb_control_set_loading_state;
  m_struct.toKodi->control_set_tooltip = cb_control_set_tooltip;
  m_struct.toKodi->control_set_status_message = cb_control_set_status_message;
  m_struct.toKodi->control_request_open_site_in_new_tab = cb_control_request_open_site_in_new_tab;
}

ADDON_STATUS CWebAddon::Create(int addonId)
{
  /* ensure that a previous instance is destroyed */
  Destroy();

  /* reset all properties to defaults */
  ResetProperties(addonId);

  /* initialise the add-on */
  CLog::Log(LOGDEBUG, "CWebAddon::{}: - creating add-on instance '{}'", __func__, Name());

  ADDON_STATUS status = CreateInstance(&m_struct);
  if (status != ADDON_STATUS_OK)
    return status;

  WEB_ADDON_ERROR webstatus = m_struct.toAddon->start_instance(&m_struct);
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
  m_struct.toAddon->stop_instance(&m_struct);
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
  if (!m_struct.toAddon->main_initialize)
    return false;

  m_readyToProcess = m_struct.toAddon->main_initialize(&m_struct);
  m_initCalled = true;
  return m_readyToProcess;
}

void CWebAddon::MainLoop()
{
  // Init addon on this to confirm addon is loaded after Kodi's start
  if (!m_initCalled)
    MainInitialize();

  // Web browser main loop calls if init was OK
  if (m_struct.toAddon->main_loop && m_readyToProcess)
    m_struct.toAddon->main_loop(&m_struct);
}

void CWebAddon::MainShutdown()
{
  // Shutdown the web browser addon
  if (m_struct.toAddon->main_shutdown && m_readyToProcess)
    m_struct.toAddon->main_shutdown(&m_struct);
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
                                         ADDON_HANDLE handle)
{
  return m_struct.toAddon->create_control(&m_struct, &props, startURL.c_str(), handle);
}

bool CWebAddon::DestroyControl(const ADDON_HANDLE handle, bool complete)
{
  return m_struct.toAddon->destroy_control(&m_struct, handle, complete);
}

void CWebAddon::Render(const ADDON_HANDLE handle)
{
  m_struct.toAddon->control_render(&m_struct, handle);
}

bool CWebAddon::OpenWebsite(const ADDON_HANDLE handle, const std::string& url)
{
  return m_struct.toAddon->control_open_website(&m_struct, handle, url.c_str());
}

void CWebAddon::Reload(const ADDON_HANDLE handle)
{
  m_struct.toAddon->control_web_cmd_reload(&m_struct, handle);
}

void CWebAddon::StopLoad(const ADDON_HANDLE handle)
{
  m_struct.toAddon->control_web_cmd_stop_load(&m_struct, handle);
}

void CWebAddon::GoBack(const ADDON_HANDLE handle)
{
  m_struct.toAddon->control_web_cmd_nav_back(&m_struct, handle);
}

void CWebAddon::GoForward(const ADDON_HANDLE handle)
{
  m_struct.toAddon->control_web_cmd_nav_forward(&m_struct, handle);
}

void CWebAddon::OpenOwnContextMenu(const ADDON_HANDLE handle)
{
  m_struct.toAddon->control_web_open_own_context_menu(&m_struct, handle);
}

bool CWebAddon::Dirty(const ADDON_HANDLE handle)
{
  return m_struct.toAddon->control_dirty(&m_struct, handle);
}

bool CWebAddon::OnInit(const ADDON_HANDLE handle)
{
  return m_struct.toAddon->control_on_init(&m_struct, handle);
}

bool CWebAddon::OnAction(
    const ADDON_HANDLE handle, const addon_action_data* action, int& nextItem)
{
  return m_struct.toAddon->control_on_action(&m_struct, handle, action, &nextItem);
}

bool CWebAddon::OnMouseEvent(const ADDON_HANDLE handle,
                             int id,
                             double x,
                             double y,
                             double offsetX,
                             double offsetY,
                             int state)
{
  return m_struct.toAddon->control_on_mouse_event(&m_struct, handle, id, x, y, offsetX, offsetY,
                                                  state);
}

bool CWebAddon::ControlGetHistory(const ADDON_HANDLE handle,
                                  std::vector<std::string>& historyWebsiteNames,
                                  bool behindCurrent)
{
  char** historyList = nullptr;
  unsigned int listSize = 0;
  bool ret = m_struct.toAddon->control_get_history(&m_struct, handle, &historyList, &listSize,
                                                   behindCurrent);
  if (ret && historyList)
  {
    for (unsigned int i = 0; i < listSize; ++i)
      historyWebsiteNames.push_back(historyList[i]);
    m_struct.toAddon->control_get_history_clear(&m_struct, handle, historyList, listSize);
  }
  return ret;
}

void CWebAddon::ControlSearchText(
    const ADDON_HANDLE handle, const std::string& text, bool forward, bool matchCase, bool findNext)
{
  m_struct.toAddon->control_search_text(&m_struct, handle, text.c_str(), forward, matchCase,
                                        findNext);
}

void CWebAddon::ControlStopSearch(const ADDON_HANDLE handle, bool clearSelection)
{
  m_struct.toAddon->control_stop_search(&m_struct, handle, clearSelection);
}

void CWebAddon::ControlScreenSizeChange(
    const ADDON_HANDLE handle, float x, float y, float width, float height, bool fullscreen)
{
  m_struct.toAddon->control_screen_size_change(&m_struct, handle, x, y, width, height, fullscreen);
}

void CWebAddon::SetMute(bool mute)
{
  m_struct.toAddon->set_mute(&m_struct, mute);
}

void CWebAddon::SetLanguage(const std::string& language)
{
  m_struct.toAddon->set_language(&m_struct, language.c_str());
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

void CWebAddon::cb_inhibit_shutdown(KODI_HANDLE kodiInstance, bool inhibit)
{
  CApplicationMessenger::GetInstance().PostMsg(TMSG_INHIBITIDLESHUTDOWN, inhibit);
}

void CWebAddon::cb_inhibit_screensaver(KODI_HANDLE kodiInstance, bool inhibit)
{
  CApplicationMessenger::GetInstance().PostMsg(TMSG_INHIBITSCREENSAVER, inhibit);
}

bool CWebAddon::cb_is_muted(KODI_HANDLE kodiInstance)
{
  return (g_application.IsMuted() || g_application.GetVolumeRatio() <= VOLUME_MINIMUM);
}

void CWebAddon::cb_control_set_control_ready(KODI_HANDLE kodiInstance,
                                             WEB_KODI_CONTROL handle,
                                             bool ready)
{
  CWebAddonControl* control = static_cast<CWebAddonControl*>(handle);
  if (!control)
  {
    CLog::Log(LOGERROR, "CWebAddon::{} - invalid handler data", __func__);
    return;
  }

  control->SetControlReady(ready);
}

void CWebAddon::cb_control_set_opened_address(KODI_HANDLE kodiInstance,
                                              WEB_KODI_CONTROL handle,
                                              const char* address)
{
  CWebAddonControl* control = static_cast<CWebAddonControl*>(handle);
  if (!control || !address)
  {
    CLog::Log(LOGERROR, "CWebAddon::{} - invalid handler data", __func__);
    return;
  }

  control->SetOpenedAddress(address);
}

void CWebAddon::cb_control_set_opened_title(KODI_HANDLE kodiInstance,
                                            WEB_KODI_CONTROL handle,
                                            const char* title)
{
  CWebAddonControl* control = static_cast<CWebAddonControl*>(handle);
  if (!control || !title)
  {
    CLog::Log(LOGERROR, "CWebAddon::{} - invalid handler data", __func__);
    return;
  }

  control->SetOpenedTitle(title);
}

void CWebAddon::cb_control_set_icon_url(KODI_HANDLE kodiInstance,
                                        WEB_KODI_CONTROL handle,
                                        const char* icon)
{
  CWebAddonControl* control = static_cast<CWebAddonControl*>(handle);
  if (!control || !icon)
  {
    CLog::Log(LOGERROR, "CWebAddon::{} - invalid handler data", __func__);
    return;
  }

  control->SetIconURL(icon);
}

void CWebAddon::cb_control_set_fullscreen(KODI_HANDLE kodiInstance,
                                          WEB_KODI_CONTROL handle,
                                          bool fullscreen)
{
  CWebAddonControl* control = static_cast<CWebAddonControl*>(handle);
  if (!control)
  {
    CLog::Log(LOGERROR, "CWebAddon::{} - invalid handler data", __func__);
    return;
  }

  ///TODO!!!!
}

void CWebAddon::cb_control_set_loading_state(KODI_HANDLE kodiInstance,
                                             WEB_KODI_CONTROL handle,
                                             bool isLoading,
                                             bool canGoBack,
                                             bool canGoForward)
{
  CWebAddonControl* control = static_cast<CWebAddonControl*>(handle);
  if (!control)
  {
    CLog::Log(LOGERROR, "CWebAddon::{} - invalid handler data", __func__);
    return;
  }

  control->SetLoadingState(isLoading, canGoBack, canGoForward);
}

void CWebAddon::cb_control_set_tooltip(KODI_HANDLE kodiInstance,
                                       WEB_KODI_CONTROL handle,
                                       const char* tooltip)
{
  CWebAddonControl* control = static_cast<CWebAddonControl*>(handle);
  if (!control)
  {
    CLog::Log(LOGERROR, "CWebAddon::{} - invalid handler data", __func__);
    return;
  }

  control->SetTooltip(tooltip);
}

void CWebAddon::cb_control_set_status_message(KODI_HANDLE kodiInstance,
                                              WEB_KODI_CONTROL handle,
                                              const char* status)
{
  CWebAddonControl* control = static_cast<CWebAddonControl*>(handle);
  if (!control)
  {
    CLog::Log(LOGERROR, "CWebAddon::{} - invalid handler data", __func__);
    return;
  }

  control->SetStatusMessage(status);
}

void CWebAddon::cb_control_request_open_site_in_new_tab(KODI_HANDLE kodiInstance,
                                                        WEB_KODI_CONTROL handle,
                                                        const char* url)
{
  CWebAddonControl* control = static_cast<CWebAddonControl*>(handle);
  if (!control)
  {
    CLog::Log(LOGERROR, "CWebAddon::{} - invalid handler data", __func__);
    return;
  }

  control->RequestOpenSiteInNewTab(url);
}
//@}
