/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#ifndef C_API_ADDONINSTANCE_WEB_H
#define C_API_ADDONINSTANCE_WEB_H

#include "../addon_base.h"

#define WEB_TYPE_ID_BROWSER 0
#define WEB_MAX_NAME_STRING_SIZE 80

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if __KODI_API__ >= 1

  typedef KODI_ADDON_INSTANCE_HDL KODI_ADDON_WEB_HDL;
  typedef void* KODI_ADDON_WEB_CONTROL_HDL;
  typedef void* WEB_KODI_CONTROL;

  //============================================================================
  /// @brief Web add-on error codes
  ///
  enum WEB_ADDON_ERROR
  {
    /// @brief if something was go complete wrong
    WEB_ADDON_ERROR_PERMANENT_FAILED = -7,

    /// @brief the command failed
    WEB_ADDON_ERROR_FAILED = -6,

    /// @brief the parameters of the method that was called are invalid for this operation
    WEB_ADDON_ERROR_INVALID_PARAMETERS = -5,

    /// @brief the command was rejected by the addon
    WEB_ADDON_ERROR_REJECTED = -4,

    /// @brief the method that KODI called is not implemented by the add-on
    WEB_ADDON_ERROR_NOT_IMPLEMENTED = -3,

    /// @brief an unknown error occurred
    WEB_ADDON_ERROR_UNKNOWN = -2,

    /// @brief no error occurred, but existing part reopened
    WEB_ADDON_ERROR_NO_ERROR_REOPEN = -1,

    /// @brief no error occurred
    WEB_ADDON_ERROR_NO_ERROR = 0,
  };
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief
  typedef enum WEB_KEY_MOD
  {
    /// @brief
    WEB_KEY_MOD_NONE = 0x0000,

    /// @brief
    WEB_KEY_MOD_SHIFT = 0x0001,

    /// @brief
    WEB_KEY_MOD_CTRL = 0x0002,

    /// @brief
    WEB_KEY_MOD_ALT = 0x0004,

    /// @brief
    WEB_KEY_MOD_META = 0x0008,

    /// @brief
    WEB_KEY_MOD_SUPER = 0x0010,

    /// @brief
    WEB_KEY_MOD_NUMLOCK = 0x0100,

    /// @brief
    WEB_KEY_MOD_CAPSLOCK = 0x0200,

    /// @brief
    WEB_KEY_MOD_SCROLLOCK = 0x0400,
  } WEB_KEY_MOD;
  //----------------------------------------------------------------------------

  typedef struct WEB_KEYBOARD_INPUT_EVENT
  {
    const char* controller_id;
    const char* feature_name;
    bool pressed;
    uint32_t unicode;
    enum WEB_KEY_MOD modifiers;
  } WEB_KEYBOARD_INPUT_EVENT;

  /*!
   * @brief Type defination structure
   */
  typedef struct WEB_ADDON_VARIOUS_TYPE
  {
    const char* strName;
    int iAddonInternalId;
    int iType;
  } WEB_ADDON_VARIOUS_TYPE;

  /*!
   * @brief Web addon gui control handle data
   */
  typedef struct WEB_ADDON_GUI_PROPS
  {
    /*!
     * @brief identify name of related control to know on next open
     */
    char strName[WEB_MAX_NAME_STRING_SIZE];

    /*!
     * @brief Used render device, NULL for OpenGL and only be used for DirectX
     */
    ADDON_HARDWARE_CONTEXT pDevice;

    /*!
     * @brief For GUI control used render positions and sizes
     */
    float fXPos;
    float fYPos;
    float fWidth;
    float fHeight;
    float fPixelRatio;
    float fFPS;

    /*!
     * @brief For GUI control used positions and sizes on skin
     */
    float fSkinXPos;
    float fSkinYPos;
    float fSkinWidth;
    float fSkinHeight;

    /*!
     * @brief If set the opened control becomes handled transparent with the
     * color value given on iBackgroundColorARGB
     */
    bool bUseTransparentBackground;

    /*!
     * @brief The wanted background color on opened control.
     *
     * If bUseTransparentBackground is false it is the background during empty
     * control (Webside not loaded)
     * If bUseTransparentBackground is true then it set the transparency color
     * of the handled control
     */
    uint32_t iBackgroundColorARGB;

    /*!
     * @brief The id's from control outside of the web GUI render control.
     * Used with OnAction to inform about next GUI item to select if inside
     * control a point comes to the end.
     */
    int iGUIItemLeft;
    int iGUIItemRight;
    int iGUIItemTop;
    int iGUIItemBottom;
    int iGUIItemBack;

    /*!
     * @brief Identifier of the control on Kodi. Required to have on callbacks,
     * must set by callback functions on WEB_KODI_CONTROL::dataAddress with this!
     */
    WEB_KODI_CONTROL pControlIdent;
  } WEB_ADDON_GUI_PROPS;

  struct ADDON_HANDLE_STRUCT
  {
    void* callerAddress; /*!< address of the caller */
    void* dataAddress; /*!< address to store data in */
    int dataIdentifier; /*!< parameter to pass back when calling the callback */
  };

  struct KODI_ADDON_ACTION_DATA;

  typedef enum WEB_ADDON_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_START_INSTANCE_V1)(const KODI_ADDON_WEB_HDL hdl);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_STOP_INSTANCE_V1)(const KODI_ADDON_WEB_HDL hdl);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_MAIN_INITIALIZE_V1)(const KODI_ADDON_WEB_HDL hdl);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_MAIN_LOOP_V1)(const KODI_ADDON_WEB_HDL hdl);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_MAIN_SHUTDOWN_V1)(const KODI_ADDON_WEB_HDL hdl);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_SET_MUTE_V1)(const KODI_ADDON_WEB_HDL hdl, bool mute);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_SET_LANGUAGE_V1)(const KODI_ADDON_WEB_HDL hdl,
                              const char* language);

  typedef enum WEB_ADDON_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_CREATE_CONTROL_V1)(const KODI_ADDON_WEB_HDL hdl,
                                                                                    const struct WEB_ADDON_GUI_PROPS* props,
                                                                                    const char* start_url,
                                                                                    WEB_KODI_CONTROL* handle);
  typedef enum WEB_ADDON_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_DESTROY_CONTROL_V1)(const KODI_ADDON_WEB_HDL hdl,
                                  const WEB_KODI_CONTROL handle,
                                  bool complete);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_CONTROL_RENDER_INIT_V1)(const KODI_ADDON_WEB_HDL hdl,
                                const WEB_KODI_CONTROL handle);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_CONTROL_RENDER_DEINIT_V1)(const KODI_ADDON_WEB_HDL hdl,
                                const WEB_KODI_CONTROL handle);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_CONTROL_RENDER_V1)(const KODI_ADDON_WEB_HDL hdl,
                                const WEB_KODI_CONTROL handle);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_CONTROL_DIRTY_V1)(const KODI_ADDON_WEB_HDL hdl,
                                const WEB_KODI_CONTROL handle);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_CONTROL_ON_INIT_V1)(const KODI_ADDON_WEB_HDL hdl,
                                  const WEB_KODI_CONTROL handle);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_CONTROL_ON_ACTION_V1)(const KODI_ADDON_WEB_HDL hdl,
                                    const WEB_KODI_CONTROL handle,
                                    const struct KODI_ADDON_ACTION_DATA* action,
                                    int* nextItem);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_CONTROL_ON_MOUSE_EVENT_V1)(const KODI_ADDON_WEB_HDL hdl,
                                        const WEB_KODI_CONTROL handle,
                                        int id,
                                        double x,
                                        double y,
                                        double offsetX,
                                        double offsetY,
                                        int state);

  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_CONTROL_OPEN_WEBSITE_V1)(const KODI_ADDON_WEB_HDL hdl,
                                      const WEB_KODI_CONTROL handle,
                                      const char* url);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_CONTROL_GET_HISTORY_V1)(const KODI_ADDON_WEB_HDL hdl,
                                      const WEB_KODI_CONTROL handle,
                                      char*** list,
                                      size_t* entries,
                                      bool behind_current);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_CONTROL_SEARCH_TEXT_V1)(const KODI_ADDON_WEB_HDL hdl,
                                      const WEB_KODI_CONTROL handle,
                                      const char* text,
                                      bool forward,
                                      bool matchCase,
                                      bool findNext);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_CONTROL_STOP_SEARCH_V1)(const KODI_ADDON_WEB_HDL hdl,
                                      const WEB_KODI_CONTROL handle,
                                      bool clearSelection);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_CONTROL_WEB_CMD_RELOAD_V1)(const KODI_ADDON_WEB_HDL hdl,
                                        const WEB_KODI_CONTROL handle);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_CONTROL_WEB_CMD_STOP_LOAD_V1)(const KODI_ADDON_WEB_HDL hdl,
                                            const WEB_KODI_CONTROL handle);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_CONTROL_WEB_CMD_NAV_BACK_V1)(const KODI_ADDON_WEB_HDL hdl,
                                          const WEB_KODI_CONTROL handle);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_CONTROL_WEB_CMD_NAV_FORWARD_V1)(const KODI_ADDON_WEB_HDL hdl,
                                              const WEB_KODI_CONTROL handle);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_CONTROL_WEB_OPEN_OWN_CONTEXT_MENU_V1)(const KODI_ADDON_WEB_HDL hdl,
                                                    const WEB_KODI_CONTROL handle);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_WEB_CONTROL_SCREEN_SIZE_CHANGE_V1)(const KODI_ADDON_WEB_HDL hdl,
                                            const WEB_KODI_CONTROL handle,
                                            float x,
                                            float y,
                                            float width,
                                            float height,
                                            bool fullscreen);

  struct KODI_ADDON_WEB_FUNC
  {
    PFN_KODI_ADDON_INSTANCE_CREATE_V1 create;
    PFN_KODI_ADDON_INSTANCE_DESTROY_V1 destroy;

    PFN_KODI_ADDON_WEB_START_INSTANCE_V1 start_instance;
    PFN_KODI_ADDON_WEB_STOP_INSTANCE_V1 stop_instance;
    PFN_KODI_ADDON_WEB_MAIN_INITIALIZE_V1 main_initialize;
    PFN_KODI_ADDON_WEB_MAIN_LOOP_V1 main_loop;
    PFN_KODI_ADDON_WEB_MAIN_SHUTDOWN_V1 main_shutdown;
    PFN_KODI_ADDON_WEB_SET_MUTE_V1 set_mute;
    PFN_KODI_ADDON_WEB_SET_LANGUAGE_V1 set_language;

    PFN_KODI_ADDON_WEB_CREATE_CONTROL_V1 create_control;
    PFN_KODI_ADDON_WEB_DESTROY_CONTROL_V1 destroy_control;
    PFN_KODI_ADDON_WEB_CONTROL_RENDER_INIT_V1 control_render_init;
    PFN_KODI_ADDON_WEB_CONTROL_RENDER_DEINIT_V1 control_render_deinit;
    PFN_KODI_ADDON_WEB_CONTROL_RENDER_V1 control_render;
    PFN_KODI_ADDON_WEB_CONTROL_DIRTY_V1 control_dirty;
    PFN_KODI_ADDON_WEB_CONTROL_ON_INIT_V1 control_on_init;
    PFN_KODI_ADDON_WEB_CONTROL_ON_ACTION_V1 control_on_action;
    PFN_KODI_ADDON_WEB_CONTROL_ON_MOUSE_EVENT_V1 control_on_mouse_event;

    PFN_KODI_ADDON_WEB_CONTROL_OPEN_WEBSITE_V1 control_open_website;
    PFN_KODI_ADDON_WEB_CONTROL_GET_HISTORY_V1 control_get_history;
    PFN_KODI_ADDON_WEB_CONTROL_SEARCH_TEXT_V1 control_search_text;
    PFN_KODI_ADDON_WEB_CONTROL_STOP_SEARCH_V1 control_stop_search;
    PFN_KODI_ADDON_WEB_CONTROL_WEB_CMD_RELOAD_V1 control_web_cmd_reload;
    PFN_KODI_ADDON_WEB_CONTROL_WEB_CMD_STOP_LOAD_V1 control_web_cmd_stop_load;
    PFN_KODI_ADDON_WEB_CONTROL_WEB_CMD_NAV_BACK_V1 control_web_cmd_nav_back;
    PFN_KODI_ADDON_WEB_CONTROL_WEB_CMD_NAV_FORWARD_V1 control_web_cmd_nav_forward;
    PFN_KODI_ADDON_WEB_CONTROL_WEB_OPEN_OWN_CONTEXT_MENU_V1 control_web_open_own_context_menu;
    PFN_KODI_ADDON_WEB_CONTROL_SCREEN_SIZE_CHANGE_V1 control_screen_size_change;
  };
  /*---AUTO_GEN_PARSE<OVERRIDE;USE_OFFSCREEN_RENDER=KODI_ADDON_WEB_FUNC(control_render_init,control_render_deinit,control_render;CWebAddonControl)>---*/

  ATTR_DLL_EXPORT void kodi_addon_web_inhibit_shutdown(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                          bool inhibit); __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_web_inhibit_screensaver(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                              bool inhibit); __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool  kodi_addon_web_is_muted(KODI_ADDON_INSTANCE_BACKEND_HDL hdl); __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void  kodi_addon_web_control_set_control_ready(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                    WEB_KODI_CONTROL handle,
                                    bool ready); __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void  kodi_addon_web_control_set_opened_address(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                      WEB_KODI_CONTROL handle,
                                      const char* title); __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void  kodi_addon_web_control_set_opened_title(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                    WEB_KODI_CONTROL handle,
                                    const char* title); __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void  kodi_addon_web_control_set_icon_url(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                WEB_KODI_CONTROL handle,
                                const char* icon); __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void  kodi_addon_web_control_set_fullscreen(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                  WEB_KODI_CONTROL handle,
                                  bool fullscreen); __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void  kodi_addon_web_control_set_loading_state(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                    WEB_KODI_CONTROL handle,
                                    bool isLoading,
                                    bool canGoBack,
                                    bool canGoForward); __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void  kodi_addon_web_control_set_tooltip(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                              WEB_KODI_CONTROL handle,
                              const char* tooltip); __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void  kodi_addon_web_control_set_status_message(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                      WEB_KODI_CONTROL handle,
                                      const char* status); __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void  kodi_addon_web_control_request_open_site_in_new_tab(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                                WEB_KODI_CONTROL handle,
                                                const char* url); __INTRODUCED_IN_KODI(1);


#endif /* __KODI_API__ >= 1 */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_ADDONINSTANCE_WEB_H */
