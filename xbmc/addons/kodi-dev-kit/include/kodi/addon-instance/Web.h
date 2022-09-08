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

#include <vector>

namespace kodi
{
namespace addon
{

class CInstanceWeb;

struct WEB_CONTROL_HANDLE
{
  KODI_ADDON_WEB_HDL kodi;
  struct WEB_ADDON_GUI_PROPS props;
  CInstanceWeb* addonWebBase;
};

class CWebControl;
class CInstanceWeb;

using WebControlHdl = KODI_HANDLE;

class ATTR_DLL_LOCAL WebKeyboardInputEvent
  : public CStructHdl<WebKeyboardInputEvent, WEB_KEYBOARD_INPUT_EVENT>
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

class ATTR_DLL_LOCAL CWebControl
{
public:
  CWebControl(kodi::addon::CInstanceWeb* instance,
              const kodi::addon::WebControlHdl handle)
    : m_instance(instance), m_handle(*static_cast<WEB_CONTROL_HANDLE*>(handle))
  {
  }

  virtual ~CWebControl() = default;

  virtual bool RenderInit() { return false; }
  virtual void RenderDeinit() {}
  virtual void Render() {}
  virtual bool Dirty() { return false; }
  virtual bool OnInit() { return false; }
  virtual bool OnAction(const gui::input::CAction& action, int& nextItem) { return false; }
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
    kodi_addon_web_control_set_control_ready(m_handle.kodi, m_handle.props.pControlIdent,
                                             ready);
  }

  inline void SetOpenedAddress(const std::string& title)
  {
    kodi_addon_web_control_set_opened_address(m_handle.kodi, m_handle.props.pControlIdent,
                                              title.c_str());
  }

  inline void SetOpenedTitle(const std::string& title)
  {
    kodi_addon_web_control_set_opened_title(m_handle.kodi, m_handle.props.pControlIdent,
                                            title.c_str());
  }

  inline void SetIconURL(const std::string& icon)
  {
    kodi_addon_web_control_set_icon_url(m_handle.kodi, m_handle.props.pControlIdent, icon.c_str());
  }

  inline void SetFullscreen(bool fullscreen)
  {
    kodi_addon_web_control_set_fullscreen(m_handle.kodi, m_handle.props.pControlIdent, fullscreen);
  }

  inline void SetLoadingState(bool isLoading, bool canGoBack, bool canGoForward)
  {
    kodi_addon_web_control_set_loading_state(m_handle.kodi, m_handle.props.pControlIdent, isLoading,
                                             canGoBack, canGoForward);
  }

  inline void SetTooltip(const std::string& tooltip)
  {
    kodi_addon_web_control_set_tooltip(m_handle.kodi, m_handle.props.pControlIdent,
                                       tooltip.c_str());
  }

  inline void SetStatusMessage(const std::string& status)
  {
    kodi_addon_web_control_set_status_message(m_handle.kodi, m_handle.props.pControlIdent,
                                              status.c_str());
  }

  inline void RequestOpenSiteInNewTab(const std::string& url)
  {
    kodi_addon_web_control_request_open_site_in_new_tab(m_handle.kodi, m_handle.props.pControlIdent,
                                                        url.c_str());
  }

  std::string GetName() const { return m_handle.props.strName; }
  virtual void* GetDevice() const { return m_handle.props.pDevice; }
  virtual float GetXPos() const { return m_handle.props.fXPos; }
  virtual float GetYPos() const { return m_handle.props.fYPos; }
  virtual float GetWidth() const { return m_handle.props.fWidth; }
  virtual float GetHeight() const { return m_handle.props.fHeight; }
  virtual float GetPixelRatio() const { return m_handle.props.fPixelRatio; }
  virtual float GetFPS() const { return m_handle.props.fFPS; }
  virtual float GetSkinXPos() const { return m_handle.props.fSkinXPos; }
  virtual float GetSkinYPos() const { return m_handle.props.fSkinYPos; }
  virtual float GetSkinWidth() const { return m_handle.props.fSkinWidth; }
  virtual float GetSkinHeight() const { return m_handle.props.fSkinHeight; }
  virtual bool UseTransparentBackground() const
  {
    return m_handle.props.bUseTransparentBackground;
  }
  virtual uint32_t GetBackgroundColorARGB() const { return m_handle.props.iBackgroundColorARGB; }
  int GetGUIItemLeft() const { return m_handle.props.iGUIItemLeft; }
  int GetGUIItemRight() const { return m_handle.props.iGUIItemRight; }
  int GetGUIItemTop() const { return m_handle.props.iGUIItemTop; }
  int GetGUIItemBottom() const { return m_handle.props.iGUIItemBottom; }
  int GetGUIItemBack() const { return m_handle.props.iGUIItemBack; }

private:
  CInstanceWeb* m_instance{nullptr};

  WEB_CONTROL_HANDLE m_handle;
};

class ATTR_DLL_LOCAL CInstanceWeb : public IAddonInstance
{
public:
  //==========================================================================
  ///
  /// @ingroup cpp_kodi_addon_web
  /// @brief Web addon class constructor
  ///
  /// Used by an add-on that only supports web browser.
  ///
  CInstanceWeb() = default;
  //--------------------------------------------------------------------------

  CInstanceWeb(const kodi::addon::IInstanceInfo& instance)
    : IAddonInstance(instance)
  {
  }

  virtual ~CInstanceWeb() = default;

  virtual bool MainInitialize() { return false; }

  virtual void MainLoop() {}

  virtual void MainShutdown() {}

  virtual WEB_ADDON_ERROR StartInstance() { return WEB_ADDON_ERROR_NO_ERROR; }
  virtual void StopInstance() {}

  virtual void SetMute(bool mute) {}
  virtual bool SetLanguage(const std::string& language) { return false; }

  inline void InhibitShutdown(bool inhibit) { kodi_addon_web_inhibit_shutdown(m_kodi, inhibit); }

  inline void InhibitScreensaver(bool inhibit)
  {
    kodi_addon_web_inhibit_screensaver(m_kodi, inhibit);
  }

  virtual WEB_ADDON_ERROR CreateControl(const std::string& sourceName,
                                        const std::string& startURL,
                                        const kodi::addon::WebControlHdl controlHdl,
                                        kodi::addon::CWebControl*& control)
  {
    return WEB_ADDON_ERROR_NOT_IMPLEMENTED;
  }

  virtual enum WEB_ADDON_ERROR DestroyControl(kodi::addon::CWebControl* control, bool complete) { return WEB_ADDON_ERROR_NOT_IMPLEMENTED; }

  inline bool IsMuted() const { return kodi_addon_web_is_muted(m_kodi); }

private:
  void SetAddonStruct(KODI_ADDON_INSTANCE_STRUCT* instance) override
  {
    instance->hdl = this;
    instance->web->main_initialize = main_initialize;
    instance->web->main_loop = main_loop;
    instance->web->main_shutdown = main_shutdown;
    instance->web->start_instance = start_instance;
    instance->web->stop_instance = stop_instance;
    instance->web->set_mute = set_mute;
    instance->web->set_language = set_language;
    instance->web->create_control = create_control;
    instance->web->destroy_control = destroy_control;
    instance->web->control_render_init = control_render_init;
    instance->web->control_render_deinit = control_render_deinit;
    instance->web->control_render = control_render;
    instance->web->control_dirty = control_dirty;
    instance->web->control_on_init = control_on_init;
    instance->web->control_on_action = control_on_action;
    instance->web->control_on_mouse_event = control_on_mouse_event;
    instance->web->control_open_website = control_open_website;
    instance->web->control_get_history = control_get_history;
    instance->web->control_search_text = control_search_text;
    instance->web->control_stop_search = control_stop_search;
    instance->web->control_web_cmd_reload = control_web_cmd_reload;
    instance->web->control_web_cmd_stop_load = control_web_cmd_stop_load;
    instance->web->control_web_cmd_nav_back = control_web_cmd_nav_back;
    instance->web->control_web_cmd_nav_forward = control_web_cmd_nav_forward;
    instance->web->control_web_open_own_context_menu = control_web_open_own_context_menu;
    instance->web->control_screen_size_change = control_screen_size_change;
  }

  friend class CWebControl;

  inline static bool main_initialize(const KODI_ADDON_WEB_HDL hdl)
  {
    return static_cast<CInstanceWeb*>(hdl)->MainInitialize();
  }

  inline static void main_loop(const KODI_ADDON_WEB_HDL hdl)
  {
    static_cast<CInstanceWeb*>(hdl)->MainLoop();
  }

  inline static void main_shutdown(const KODI_ADDON_WEB_HDL hdl)
  {
    static_cast<CInstanceWeb*>(hdl)->MainShutdown();
  }

  inline static WEB_ADDON_ERROR start_instance(const KODI_ADDON_WEB_HDL hdl)
  {
    return static_cast<CInstanceWeb*>(hdl)->StartInstance();
  }

  inline static void stop_instance(const KODI_ADDON_WEB_HDL hdl)
  {
    static_cast<CInstanceWeb*>(hdl)->StopInstance();
  }

  inline static void set_mute(const KODI_ADDON_WEB_HDL hdl, bool mute)
  {
    static_cast<CInstanceWeb*>(hdl)->SetMute(mute);
  }

  inline static bool set_language(const KODI_ADDON_WEB_HDL hdl, const char* language)
  {
    return static_cast<CInstanceWeb*>(hdl)->SetLanguage(language);
  }

  inline static WEB_ADDON_ERROR create_control(const KODI_ADDON_WEB_HDL hdl,
                                               const WEB_ADDON_GUI_PROPS* props,
                                               const char* start_url,
                                               KODI_ADDON_WEB_CONTROL_HDL* handle)
  {
    CInstanceWeb* const instance = static_cast<CInstanceWeb*>(hdl);

    WEB_CONTROL_HANDLE controlHandle;
    controlHandle.kodi = instance->m_kodi;
    controlHandle.props = *props; // Copy as can be deleted after this call
    controlHandle.addonWebBase = instance;

    kodi::addon::CWebControl* control = nullptr;
    WEB_ADDON_ERROR err = instance->CreateControl(props->strName, start_url, &controlHandle, control);
    if ((err == WEB_ADDON_ERROR_NO_ERROR || err == WEB_ADDON_ERROR_NO_ERROR)  && control == nullptr)
    {
      kodi::Log(ADDON_LOG_ERROR, "Failed to create web addon control instance");
      return WEB_ADDON_ERROR_PERMANENT_FAILED;
    }

    *handle = control;
    return err;
  }

  inline static WEB_ADDON_ERROR destroy_control(const KODI_ADDON_WEB_HDL hdl,
                                     const WEB_KODI_CONTROL handle,
                                     bool complete)
  {
    return static_cast<CInstanceWeb*>(hdl)->DestroyControl(static_cast<CWebControl*>(handle),
                                                           complete);
  }

  inline static bool control_render_init(const KODI_ADDON_WEB_HDL hdl, const WEB_KODI_CONTROL handle)
  {
    return static_cast<CWebControl*>(handle)->RenderInit();
  }

  inline static void control_render_deinit(const KODI_ADDON_WEB_HDL hdl, const WEB_KODI_CONTROL handle)
  {
    static_cast<CWebControl*>(handle)->RenderDeinit();
  }

  inline static void control_render(const KODI_ADDON_WEB_HDL hdl, const WEB_KODI_CONTROL handle)
  {
    static_cast<CWebControl*>(handle)->Render();
  }

  inline static bool control_dirty(const KODI_ADDON_WEB_HDL hdl, const WEB_KODI_CONTROL handle)
  {
    return static_cast<CWebControl*>(handle)->Dirty();
  }

  inline static bool control_on_init(const KODI_ADDON_WEB_HDL hdl, const WEB_KODI_CONTROL handle)
  {
    return static_cast<CWebControl*>(handle)->OnInit();
  }

  inline static bool control_on_action(const KODI_ADDON_WEB_HDL hdl,
                                       const WEB_KODI_CONTROL handle,
                                       const KODI_ADDON_ACTION_DATA* action,
                                       int* nextItem)
  {
    return static_cast<CWebControl*>(handle)->OnAction(action, *nextItem);
  }

  inline static bool control_on_mouse_event(const KODI_ADDON_WEB_HDL hdl,
                                            const WEB_KODI_CONTROL handle,
                                            int id,
                                            double x,
                                            double y,
                                            double offsetX,
                                            double offsetY,
                                            int state)
  {
    return static_cast<CWebControl*>(handle)->OnMouseEvent(id, x, y, offsetX, offsetY, state);
  }

  inline static bool control_open_website(const KODI_ADDON_WEB_HDL hdl,
                                          const WEB_KODI_CONTROL handle,
                                          const char* url)
  {
    return static_cast<CWebControl*>(handle)->OpenWebsite(url);
  }

  inline static bool control_get_history(const KODI_ADDON_WEB_HDL hdl,
                                         const WEB_KODI_CONTROL handle,
                                         char*** list,
                                         size_t* entries,
                                         bool behind_current)
  {
    std::vector<std::string> historyWebsiteNames;
    bool ret = static_cast<CWebControl*>(handle)->GetHistory(historyWebsiteNames, behind_current);
    if (ret)
    {
      *list = static_cast<char**>(malloc(historyWebsiteNames.size() * sizeof(char*)));
      *entries = historyWebsiteNames.size();
      for (size_t i = 0; i < historyWebsiteNames.size(); ++i)
#ifdef WIN32 // To prevent warning C4996
        (*list)[i] = _strdup(historyWebsiteNames[i].c_str());
#else
        (*list)[i] = strdup(historyWebsiteNames[i].c_str());
#endif
    }
    return ret;
  }

  inline static void control_get_history_clear(const KODI_ADDON_WEB_HDL hdl,
                                               const WEB_KODI_CONTROL handle,
                                               char** list,
                                               unsigned int entries)
  {
    for (unsigned int i = 0; i < entries; ++i)
      free(list[i]);
    free(list);
  }

  inline static void control_search_text(const KODI_ADDON_WEB_HDL hdl,
                                         const WEB_KODI_CONTROL handle,
                                         const char* text,
                                         bool forward,
                                         bool matchCase,
                                         bool findNext)
  {
    static_cast<CWebControl*>(handle)->SearchText(text, forward, matchCase, findNext);
  }

  inline static void control_stop_search(const KODI_ADDON_WEB_HDL hdl,
                                         const WEB_KODI_CONTROL handle,
                                         bool clearSelection)
  {
    static_cast<CWebControl*>(handle)->StopSearch(clearSelection);
  }

  inline static void control_web_cmd_reload(const KODI_ADDON_WEB_HDL hdl,
                                            const WEB_KODI_CONTROL handle)
  {
    static_cast<CWebControl*>(handle)->Reload();
  }

  inline static void control_web_cmd_stop_load(const KODI_ADDON_WEB_HDL hdl,
                                               const WEB_KODI_CONTROL handle)
  {
    static_cast<CWebControl*>(handle)->StopLoad();
  }

  inline static void control_web_cmd_nav_back(const KODI_ADDON_WEB_HDL hdl,
                                              const WEB_KODI_CONTROL handle)
  {
    static_cast<CWebControl*>(handle)->GoBack();
  }

  inline static void control_web_cmd_nav_forward(const KODI_ADDON_WEB_HDL hdl,
                                                 const WEB_KODI_CONTROL handle)
  {
    static_cast<CWebControl*>(handle)->GoForward();
  }

  inline static void control_web_open_own_context_menu(const KODI_ADDON_WEB_HDL hdl,
                                                       const WEB_KODI_CONTROL handle)
  {
    static_cast<CWebControl*>(handle)->OpenOwnContextMenu();
  }

  inline static void control_screen_size_change(const KODI_ADDON_WEB_HDL hdl,
                                                const WEB_KODI_CONTROL handle,
                                                float x,
                                                float y,
                                                float width,
                                                float height,
                                                bool fullscreen)
  {
    static_cast<CWebControl*>(handle)->ScreenSizeChange(x, y, width, height, fullscreen);
  }
};

} /* namespace addon */
} /* namespace kodi */

#endif /* __cplusplus */
