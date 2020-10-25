/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "../AddonBase.h"
#include "../c-api/addon-instance/web.h"
#include "../gui/input/Action.h"

#ifdef __cplusplus

namespace kodi
{
namespace addon
{

class CWebControl;
class CInstanceWeb;

class ATTRIBUTE_HIDDEN WebKeyboardInputEvent : public CStructHdl<WebKeyboardInputEvent, WEB_KEYBOARD_INPUT_EVENT>
{
  /*! \cond PRIVATE */
  friend class CWebControl;
  friend class CInstanceWeb;
  /*! \endcond */

public:
  std::string GetControllerId() const { return m_cStructure->controller_id; }
  std::string GetFeatureName() const { return m_cStructure->feature_name; }
  bool GetPressed() const { return m_cStructure->pressed; }
  uint32_t GetUnicode() const { return m_cStructure->unicode; }
  WEB_KEY_MOD GetModifiers() const { return m_cStructure->modifiers; }

private:
  WebKeyboardInputEvent(const WEB_KEYBOARD_INPUT_EVENT* tag) : CStructHdl(tag) {}
  WebKeyboardInputEvent(WEB_KEYBOARD_INPUT_EVENT* tag) : CStructHdl(tag) {}
  WebKeyboardInputEvent() = delete;
  WebKeyboardInputEvent(const WebKeyboardInputEvent& tag) = delete;
};

class ATTRIBUTE_HIDDEN CWebControl
{
public:
  CWebControl(KODI_HANDLE handle, int dataIdentifier)
    : m_handle(*static_cast<WEB_CONTROL_HANDLE*>(handle))
  {
    m_handle.ownHandle.callerAddress = this;
    m_handle.ownHandle.dataIdentifier = dataIdentifier;
  }

  virtual ~CWebControl() = default;

  virtual void Render() {}
  virtual bool Dirty() { return false; }
  virtual bool OnInit() { return false; }
  virtual bool OnAction(const gui::input::CAction& action, int& nextItem)
  {
    return false;
  }
  virtual bool OnMouseEvent(int id, double x, double y, double offsetX, double offsetY, int state)
  {
    return false;
  }
  virtual bool OpenWebsite(const std::string& url) { return false; }
  virtual bool GetHistory(std::vector<std::string>& historyWebsiteNames, bool behindCurrent)
  {
    return false;
  }
  virtual void SearchText(const std::string& text, bool forward, bool matchCase, bool findNext) {}
  virtual void StopSearch(bool clearSelection) {}
  virtual void Reload() {}
  virtual void StopLoad() {}
  virtual void GoBack() {}
  virtual void GoForward() {}
  virtual void OpenOwnContextMenu() {}
  virtual void ScreenSizeChange(float x, float y, float width, float height, bool fullscreen) {}

  inline void SetControlReady(bool ready)
  {
    const AddonInstance_WebAddon* data = m_handle.instanceData;
    data->toKodi->control_set_control_ready(data->toKodi->kodiInstance, m_handle.props->pControlIdent,
                                           ready);
  }

  inline void SetOpenedAddress(const std::string& title)
  {
    const AddonInstance_WebAddon* data = m_handle.instanceData;
    data->toKodi->control_set_opened_address(data->toKodi->kodiInstance,
                                            m_handle.props->pControlIdent, title.c_str());
  }

  inline void SetOpenedTitle(const std::string& title)
  {
    const AddonInstance_WebAddon* data = m_handle.instanceData;
    data->toKodi->control_set_opened_title(data->toKodi->kodiInstance, m_handle.props->pControlIdent,
                                          title.c_str());
  }

  inline void SetIconURL(const std::string& icon)
  {
    const AddonInstance_WebAddon* data = m_handle.instanceData;
    data->toKodi->control_set_icon_url(data->toKodi->kodiInstance, m_handle.props->pControlIdent,
                                      icon.c_str());
  }

  inline void SetFullscreen(bool fullscreen)
  {
    const AddonInstance_WebAddon* data = m_handle.instanceData;
    data->toKodi->control_set_fullscreen(data->toKodi->kodiInstance, m_handle.props->pControlIdent,
                                        fullscreen);
  }

  inline void SetLoadingState(bool isLoading, bool canGoBack, bool canGoForward)
  {
    const AddonInstance_WebAddon* data = m_handle.instanceData;
    data->toKodi->control_set_loading_state(data->toKodi->kodiInstance, m_handle.props->pControlIdent,
                                           isLoading, canGoBack, canGoForward);
  }

  inline void SetTooltip(const std::string& tooltip)
  {
    const AddonInstance_WebAddon* data = m_handle.instanceData;
    data->toKodi->control_set_tooltip(data->toKodi->kodiInstance, m_handle.props->pControlIdent,
                                     tooltip.c_str());
  }

  inline void SetStatusMessage(const std::string& status)
  {
    const AddonInstance_WebAddon* data = m_handle.instanceData;
    data->toKodi->control_set_status_message(data->toKodi->kodiInstance,
                                            m_handle.props->pControlIdent, status.c_str());
  }

  inline void RequestOpenSiteInNewTab(const std::string& url)
  {
    const AddonInstance_WebAddon* data = m_handle.instanceData;
    data->toKodi->control_request_open_site_in_new_tab(data->toKodi->kodiInstance,
                                                      m_handle.props->pControlIdent, url.c_str());
  }

  int GetDataIdentifier() const { return m_handle.ownHandle.dataIdentifier; }
  std::string GetName() const { return m_handle.props->strName; }
  virtual void* GetDevice() const { return m_handle.props->pDevice; }
  virtual float GetXPos() const { return m_handle.props->fXPos; }
  virtual float GetYPos() const { return m_handle.props->fYPos; }
  virtual float GetWidth() const { return m_handle.props->fWidth; }
  virtual float GetHeight() const { return m_handle.props->fHeight; }
  virtual float GetPixelRatio() const { return m_handle.props->fPixelRatio; }
  virtual float GetFPS() const { return m_handle.props->fFPS; }
  virtual float GetSkinXPos() const { return m_handle.props->fSkinXPos; }
  virtual float GetSkinYPos() const { return m_handle.props->fSkinYPos; }
  virtual float GetSkinWidth() const { return m_handle.props->fSkinWidth; }
  virtual float GetSkinHeight() const { return m_handle.props->fSkinHeight; }
  virtual bool UseTransparentBackground() const
  {
    return m_handle.props->bUseTransparentBackground;
  }
  virtual uint32_t GetBackgroundColorARGB() const { return m_handle.props->iBackgroundColorARGB; }
  int GetGUIItemLeft() const { return m_handle.props->iGUIItemLeft; }
  int GetGUIItemRight() const { return m_handle.props->iGUIItemRight; }
  int GetGUIItemTop() const { return m_handle.props->iGUIItemTop; }
  int GetGUIItemBottom() const { return m_handle.props->iGUIItemBottom; }
  int GetGUIItemBack() const { return m_handle.props->iGUIItemBack; }

private:
  friend class CInstanceWeb;

  WEB_CONTROL_HANDLE m_handle;
};

class ATTRIBUTE_HIDDEN CInstanceWeb : public IAddonInstance
{
public:
  //==========================================================================
  ///
  /// @ingroup cpp_kodi_addon_web
  /// @brief Web addon class constructor
  ///
  /// Used by an add-on that only supports web browser.
  ///
  CInstanceWeb() : IAddonInstance(ADDON_INSTANCE_WEB, GetKodiTypeVersion(ADDON_INSTANCE_WEB))
  {
    if (CAddonBase::m_interface->globalSingleInstance != nullptr)
      throw std::logic_error("kodi::addon::CInstanceWeb: Creation of more as one in single "
                             "instance way is not allowed!");

    SetAddonStruct(CAddonBase::m_interface->firstKodiInstance);
    CAddonBase::m_interface->globalSingleInstance = this;
  }
  //--------------------------------------------------------------------------

  CInstanceWeb(KODI_HANDLE instance, const std::string& kodiVersion = "")
    : IAddonInstance(ADDON_INSTANCE_WEB,
                     !kodiVersion.empty() ? kodiVersion : GetKodiTypeVersion(ADDON_INSTANCE_WEB))
  {
    if (CAddonBase::m_interface->globalSingleInstance != nullptr)
      throw std::logic_error("kodi::addon::CInstanceWeb: Creation of multiple together with single "
                             "instance way is not allowed!");

    SetAddonStruct(instance);
  }

  virtual ~CInstanceWeb() = default;

  virtual bool MainInitialize() { return false; }

  virtual void MainLoop() {}

  virtual void MainShutdown() {}

  virtual WEB_ADDON_ERROR StartInstance() { return WEB_ADDON_ERROR_NO_ERROR; }
  virtual void StopInstance() {}

  virtual void SetMute(bool mute) {}
  virtual bool SetLanguage(const char* language) { return false; }

  inline void InhibitShutdown(bool inhibit)
  {
    m_instanceData->toKodi->inhibit_shutdown(m_instanceData->toKodi->kodiInstance, inhibit);
  }

  inline void InhibitScreensaver(bool inhibit)
  {
    m_instanceData->toKodi->inhibit_screensaver(m_instanceData->toKodi->kodiInstance, inhibit);
  }

  virtual kodi::addon::CWebControl* CreateControl(const std::string& sourceName,
                                                  const std::string& startURL,
                                                  KODI_HANDLE handle)
  {
    return nullptr;
  }
  virtual bool DestroyControl(kodi::addon::CWebControl* control, bool complete) { return false; }

  inline std::string UserPath(const std::string& append = "")
  {
    std::string ret = m_instanceData->props->strUserPath;
    if (!ret.empty() && (ret.at(ret.length() - 1) == '\\' || ret.at(ret.length() - 1) == '/'))
      ret = ret.substr(0, ret.length() - 1);

    if (!append.empty())
    {
      if (append.at(0) != '\\' && append.at(0) != '/')
#ifdef TARGET_WINDOWS
        ret.append("\\");
#else
        ret.append("/");
#endif
      ret.append(append);
    }
    return ret;
  }

  inline std::string AddonLibPath(const std::string& append = "")
  {
    std::string ret = m_instanceData->props->strAddonLibPath;
    if (!ret.empty() && (ret.at(ret.length() - 1) == '\\' || ret.at(ret.length() - 1) == '/'))
      ret = ret.substr(0, ret.length() - 1);

    if (!append.empty())
    {
      if (append.at(0) != '\\' && append.at(0) != '/')
#ifdef TARGET_WINDOWS
        ret.append("\\");
#else
        ret.append("/");
#endif
      ret.append(append);
    }
    return ret;
  }

  inline std::string AddonSharePath(const std::string& append = "")
  {
    std::string ret = m_instanceData->props->strAddonSharePath;
    if (!ret.empty() && (ret.at(ret.length() - 1) == '\\' || ret.at(ret.length() - 1) == '/'))
      ret = ret.substr(0, ret.length() - 1);

    if (!append.empty())
    {
      if (append.at(0) != '\\' && append.at(0) != '/')
#ifdef TARGET_WINDOWS
        ret.append("\\");
#else
        ret.append("/");
#endif
      ret.append(append);
    }
    return ret;
  }

  inline bool IsMuted() const
  {
    return m_instanceData->toKodi->is_muted(m_instanceData->toKodi->kodiInstance);
  }

private:
  void SetAddonStruct(KODI_HANDLE instance)
  {
    if (instance == nullptr)
      throw std::logic_error("kodi::addon::CInstanceWeb: Creation with empty addon structure not "
                             "allowed, table must be given from Kodi!");

    m_instanceData = static_cast<AddonInstance_WebAddon*>(instance);
    m_instanceData->toAddon->addonInstance = this;
    m_instanceData->toAddon->main_initialize = main_initialize;
    m_instanceData->toAddon->main_loop = main_loop;
    m_instanceData->toAddon->main_shutdown = main_shutdown;
    m_instanceData->toAddon->start_instance = start_instance;
    m_instanceData->toAddon->stop_instance = stop_instance;
    m_instanceData->toAddon->set_mute = set_mute;
    m_instanceData->toAddon->set_language = set_language;
    m_instanceData->toAddon->create_control = create_control;
    m_instanceData->toAddon->destroy_control = destroy_control;
    m_instanceData->toAddon->control_render = control_render;
    m_instanceData->toAddon->control_dirty = control_dirty;
    m_instanceData->toAddon->control_on_init = control_on_init;
    m_instanceData->toAddon->control_on_action = control_on_action;
    m_instanceData->toAddon->control_on_mouse_event = control_on_mouse_event;
    m_instanceData->toAddon->control_open_website = control_open_website;
    m_instanceData->toAddon->control_get_history = control_get_history;
    m_instanceData->toAddon->control_get_history_clear = control_get_history_clear;
    m_instanceData->toAddon->control_search_text = control_search_text;
    m_instanceData->toAddon->control_stop_search = control_stop_search;
    m_instanceData->toAddon->control_web_cmd_reload = control_web_cmd_reload;
    m_instanceData->toAddon->control_web_cmd_stop_load = control_web_cmd_stop_load;
    m_instanceData->toAddon->control_web_cmd_nav_back = control_web_cmd_nav_back;
    m_instanceData->toAddon->control_web_cmd_nav_forward = control_web_cmd_nav_forward;
    m_instanceData->toAddon->control_web_open_own_context_menu = control_web_open_own_context_menu;
    m_instanceData->toAddon->control_screen_size_change = control_screen_size_change;
  }

  friend class CWebControl;

  inline static bool main_initialize(const AddonInstance_WebAddon* instance)
  {
    return static_cast<CInstanceWeb*>(instance->toAddon->addonInstance)->MainInitialize();
  }

  inline static void main_loop(const AddonInstance_WebAddon* instance)
  {
    static_cast<CInstanceWeb*>(instance->toAddon->addonInstance)->MainLoop();
  }

  inline static void main_shutdown(const AddonInstance_WebAddon* instance)
  {
    static_cast<CInstanceWeb*>(instance->toAddon->addonInstance)->MainShutdown();
  }

  inline static WEB_ADDON_ERROR start_instance(const AddonInstance_WebAddon* instance)
  {
    return static_cast<CInstanceWeb*>(instance->toAddon->addonInstance)->StartInstance();
  }

  inline static void stop_instance(const AddonInstance_WebAddon* instance)
  {
    static_cast<CInstanceWeb*>(instance->toAddon->addonInstance)->StopInstance();
  }

  inline static void set_mute(const AddonInstance_WebAddon* instance, bool mute)
  {
    static_cast<CInstanceWeb*>(instance->toAddon->addonInstance)->SetMute(mute);
  }

  inline static bool set_language(const AddonInstance_WebAddon* instance, const char* language)
  {
    return static_cast<CInstanceWeb*>(instance->toAddon->addonInstance)->SetLanguage(language);
  }

  inline static WEB_ADDON_ERROR create_control(const AddonInstance_WebAddon* instance,
                                               const WEB_ADDON_GUI_PROPS* props,
                                               const char* start_url,
                                               ADDON_HANDLE handle)
  {
    WEB_CONTROL_HANDLE controlHandle;
    controlHandle.instanceData = instance;
    controlHandle.props = props;

    CWebControl* control = static_cast<CInstanceWeb*>(instance->toAddon->addonInstance)
                               ->CreateControl(props->strName, start_url, &controlHandle);
    if (control == nullptr)
    {
      kodi::Log(ADDON_LOG_ERROR, "Failed to create web addon control instance");
      return WEB_ADDON_ERROR_FAILED;
    }

    handle->callerAddress = control;
    handle->dataIdentifier = control->GetDataIdentifier();
    return WEB_ADDON_ERROR_NO_ERROR;
  }

  inline static bool destroy_control(const AddonInstance_WebAddon* instance,
                                     const ADDON_HANDLE handle,
                                     bool complete)
  {
    return static_cast<CInstanceWeb*>(instance->toAddon->addonInstance)
        ->DestroyControl(static_cast<CWebControl*>(handle->callerAddress), complete);
  }

  inline static void control_render(const AddonInstance_WebAddon* instance,
                                    const ADDON_HANDLE handle)
  {
    static_cast<CWebControl*>(handle->callerAddress)->Render();
  }

  inline static bool control_dirty(const AddonInstance_WebAddon* instance,
                                   const ADDON_HANDLE handle)
  {
    return static_cast<CWebControl*>(handle->callerAddress)->Dirty();
  }

  inline static bool control_on_init(const AddonInstance_WebAddon* instance,
                                     const ADDON_HANDLE handle)
  {
    return static_cast<CWebControl*>(handle->callerAddress)->OnInit();
  }

  inline static bool control_on_action(const AddonInstance_WebAddon* instance,
                                       const ADDON_HANDLE handle,
                                       const addon_action_data* action,
                                       int* nextItem)
  {
    return static_cast<CWebControl*>(handle->callerAddress)
        ->OnAction(action, *nextItem);
  }

  inline static bool control_on_mouse_event(const AddonInstance_WebAddon* instance,
                                            const ADDON_HANDLE handle,
                                            int id,
                                            double x,
                                            double y,
                                            double offsetX,
                                            double offsetY,
                                            int state)
  {
    return static_cast<CWebControl*>(handle->callerAddress)
        ->OnMouseEvent(id, x, y, offsetX, offsetY, state);
  }

  inline static bool control_open_website(const AddonInstance_WebAddon* instance,
                                          const ADDON_HANDLE handle,
                                          const char* url)
  {
    return static_cast<CWebControl*>(handle->callerAddress)->OpenWebsite(url);
  }

  inline static bool control_get_history(const AddonInstance_WebAddon* instance,
                                         const ADDON_HANDLE handle,
                                         char*** list,
                                         unsigned int* entries,
                                         bool behind_current)
  {
    std::vector<std::string> historyWebsiteNames;
    bool ret = static_cast<CWebControl*>(handle->callerAddress)
                   ->GetHistory(historyWebsiteNames, behind_current);
    if (ret)
    {
      *list = static_cast<char**>(malloc(historyWebsiteNames.size() * sizeof(char*)));
      *entries = static_cast<unsigned int>(historyWebsiteNames.size());
      for (size_t i = 0; i < historyWebsiteNames.size(); ++i)
#ifdef WIN32 // To prevent warning C4996
        (*list)[i] = _strdup(historyWebsiteNames[i].c_str());
#else
        (*list)[i] = strdup(historyWebsiteNames[i].c_str());
#endif
    }
    return ret;
  }

  inline static void control_get_history_clear(const AddonInstance_WebAddon* instance,
                                               const ADDON_HANDLE handle,
                                               char** list,
                                               unsigned int entries)
  {
    for (unsigned int i = 0; i < entries; ++i)
      free(list[i]);
    free(list);
  }

  inline static void control_search_text(const AddonInstance_WebAddon* instance,
                                         const ADDON_HANDLE handle,
                                         const char* text,
                                         bool forward,
                                         bool matchCase,
                                         bool findNext)
  {
    static_cast<CWebControl*>(handle->callerAddress)
        ->SearchText(text, forward, matchCase, findNext);
  }

  inline static void control_stop_search(const AddonInstance_WebAddon* instance,
                                         const ADDON_HANDLE handle,
                                         bool clearSelection)
  {
    static_cast<CWebControl*>(handle->callerAddress)->StopSearch(clearSelection);
  }

  inline static void control_web_cmd_reload(const AddonInstance_WebAddon* instance,
                                            const ADDON_HANDLE handle)
  {
    static_cast<CWebControl*>(handle->callerAddress)->Reload();
  }

  inline static void control_web_cmd_stop_load(const AddonInstance_WebAddon* instance,
                                               const ADDON_HANDLE handle)
  {
    static_cast<CWebControl*>(handle->callerAddress)->StopLoad();
  }

  inline static void control_web_cmd_nav_back(const AddonInstance_WebAddon* instance,
                                              const ADDON_HANDLE handle)
  {
    static_cast<CWebControl*>(handle->callerAddress)->GoBack();
  }

  inline static void control_web_cmd_nav_forward(const AddonInstance_WebAddon* instance,
                                                 const ADDON_HANDLE handle)
  {
    static_cast<CWebControl*>(handle->callerAddress)->GoForward();
  }

  inline static void control_web_open_own_context_menu(const AddonInstance_WebAddon* instance,
                                                       const ADDON_HANDLE handle)
  {
    static_cast<CWebControl*>(handle->callerAddress)->OpenOwnContextMenu();
  }

  inline static void control_screen_size_change(const AddonInstance_WebAddon* instance,
                                                const ADDON_HANDLE handle,
                                                float x,
                                                float y,
                                                float width,
                                                float height,
                                                bool fullscreen)
  {
    static_cast<CWebControl*>(handle->callerAddress)
        ->ScreenSizeChange(x, y, width, height, fullscreen);
  }

  AddonInstance_WebAddon* m_instanceData;
};

} /* namespace addon */
} /* namespace kodi */

#endif /* __cplusplus */
