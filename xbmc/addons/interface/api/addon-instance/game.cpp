/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/* File autogenerated, see xbmc/addons/kodi-dev-kit/tools/code-generator/code_generator.py */

#include "game.h"

// Shared API
#include "addons/kodi-dev-kit/src/shared/api/addon-instance/game.h"

// Kodi
#include "addons/interface/RunningProcess.h"
#include "games/addons/GameClient.h"
#include "utils/log.h"

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1_END>---*/

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_2>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_2_END>---*/

void CHdl_kodi_addoninstance_game_h::InitDirect(directFuncToKodi_addoninstance_game_h* ifcToKodi,
                                                directFuncToAddon_addoninstance_game_h* ifcToAddon)
{
  ifcToKodi->thisClassHdl = this;
  m_ifcToAddon = ifcToAddon;
  ifcToKodi->kodi_addon_game_close_v1 = kodi_addon_game_close_v1;
  ifcToKodi->kodi_addon_game_open_stream_v1 = kodi_addon_game_open_stream_v1;
  ifcToKodi->kodi_addon_game_get_stream_buffer_v1 = kodi_addon_game_get_stream_buffer_v1;
  ifcToKodi->kodi_addon_game_add_stream_data_v1 = kodi_addon_game_add_stream_data_v1;
  ifcToKodi->kodi_addon_game_release_stream_buffer_v1 = kodi_addon_game_release_stream_buffer_v1;
  ifcToKodi->kodi_addon_game_close_stream_v1 = kodi_addon_game_close_stream_v1;
  ifcToKodi->kodi_addon_game_hw_get_proc_address_v1 = kodi_addon_game_hw_get_proc_address_v1;
  ifcToKodi->kodi_addon_game_input_event_v1 = kodi_addon_game_input_event_v1;
}

bool CHdl_kodi_addoninstance_game_h::HandleMessage(int funcGroup,
                                                   int func,
                                                   const msgpack::unpacked& in,
                                                   msgpack::sbuffer& out)
{
  if (funcGroup != funcGroup_addoninstance_game_h)
    return false;

  switch (func)
  {
    /* NOTE: Ignored as direct api only is set! */
    default:
      CLog::Log(LOGERROR,
                "CHdl_kodi_addoninstance_game_h::{}: addon called with unknown function id '{}' on "
                "group 'addoninstance_game_h'",
                __func__, func);
  }

  return false;
}

// Function calls from Kodi to addon

KODI_ADDON_GAME_HDL CHdl_kodi_addoninstance_game_h::kodi_addon_game_create_v1(
    KODI_ADDON_INSTANCE_BACKEND_HDL kodi_hdl)
{
  // Original API call: typedef KODI_ADDON_GAME_HDL(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_CREATE_V1)(KODI_ADDON_INSTANCE_BACKEND_HDL kodi_hdl);

  return m_ifcToAddon->kodi_addon_game_create_v1(m_ifcToAddon->thisClassHdl, kodi_hdl);
}

void CHdl_kodi_addoninstance_game_h::kodi_addon_game_destroy_v1(KODI_ADDON_GAME_HDL hdl)
{
  // Original API call: typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_DESTROY_V1)(KODI_ADDON_GAME_HDL hdl);

  m_ifcToAddon->kodi_addon_game_destroy_v1(m_ifcToAddon->thisClassHdl, hdl);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_load_game_v1(
    const KODI_ADDON_GAME_HDL hdl, const char* url)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_LOAD_GAME_V1)(const KODI_ADDON_GAME_HDL hdl, const char* url);

  return m_ifcToAddon->kodi_addon_game_load_game_v1(m_ifcToAddon->thisClassHdl, hdl, url);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_load_game_special_v1(
    const KODI_ADDON_GAME_HDL hdl, enum SPECIAL_GAME_TYPE type, const char** urls, size_t urlCount)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_LOAD_GAME_SPECIAL_V1)(const KODI_ADDON_GAME_HDL hdl, enum SPECIAL_GAME_TYPE type, const char** urls, size_t urlCount);

  return m_ifcToAddon->kodi_addon_game_load_game_special_v1(m_ifcToAddon->thisClassHdl, hdl, type,
                                                            urls, urlCount);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_load_standalone_v1(
    const KODI_ADDON_GAME_HDL hdl)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_LOAD_STANDALONE_V1)(const KODI_ADDON_GAME_HDL hdl);

  return m_ifcToAddon->kodi_addon_game_load_standalone_v1(m_ifcToAddon->thisClassHdl, hdl);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_unload_game_v1(
    const KODI_ADDON_GAME_HDL hdl)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_UNLOAD_GAME_V1)(const KODI_ADDON_GAME_HDL hdl);

  return m_ifcToAddon->kodi_addon_game_unload_game_v1(m_ifcToAddon->thisClassHdl, hdl);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_get_game_timing_v1(
    const KODI_ADDON_GAME_HDL hdl, struct game_system_timing* timing_info)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_GET_GAME_TIMING_V1)(const KODI_ADDON_GAME_HDL hdl, struct game_system_timing* timing_info);

  return m_ifcToAddon->kodi_addon_game_get_game_timing_v1(m_ifcToAddon->thisClassHdl, hdl,
                                                          timing_info);
}

enum GAME_REGION CHdl_kodi_addoninstance_game_h::kodi_addon_game_get_region_v1(
    const KODI_ADDON_GAME_HDL hdl)
{
  // Original API call: typedef enum GAME_REGION(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_GET_REGION_V1)(const KODI_ADDON_GAME_HDL hdl);

  return m_ifcToAddon->kodi_addon_game_get_region_v1(m_ifcToAddon->thisClassHdl, hdl);
}

bool CHdl_kodi_addoninstance_game_h::kodi_addon_game_requires_game_loop_v1(
    const KODI_ADDON_GAME_HDL hdl)
{
  // Original API call: typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_REQUIRES_GAME_LOOP_V1)(const KODI_ADDON_GAME_HDL hdl);

  return m_ifcToAddon->kodi_addon_game_requires_game_loop_v1(m_ifcToAddon->thisClassHdl, hdl);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_run_frame_v1(
    const KODI_ADDON_GAME_HDL hdl)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_RUN_FRAME_V1)(const KODI_ADDON_GAME_HDL hdl);

  return m_ifcToAddon->kodi_addon_game_run_frame_v1(m_ifcToAddon->thisClassHdl, hdl);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_reset_v1(
    const KODI_ADDON_GAME_HDL hdl)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_RESET_V1)(const KODI_ADDON_GAME_HDL hdl);

  return m_ifcToAddon->kodi_addon_game_reset_v1(m_ifcToAddon->thisClassHdl, hdl);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_hw_context_reset_v1(
    const KODI_ADDON_GAME_HDL hdl)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_HW_CONTEXT_RESET_V1)(const KODI_ADDON_GAME_HDL hdl);

  return m_ifcToAddon->kodi_addon_game_hw_context_reset_v1(m_ifcToAddon->thisClassHdl, hdl);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_hw_context_destroy_v1(
    const KODI_ADDON_GAME_HDL hdl)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_HW_CONTEXT_DESTROY_V1)(const KODI_ADDON_GAME_HDL hdl);

  return m_ifcToAddon->kodi_addon_game_hw_context_destroy_v1(m_ifcToAddon->thisClassHdl, hdl);
}

bool CHdl_kodi_addoninstance_game_h::kodi_addon_game_has_feature_v1(const KODI_ADDON_GAME_HDL hdl,
                                                                    const char* controller_id,
                                                                    const char* feature_name)
{
  // Original API call: typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_HAS_FEATURE_V1)(const KODI_ADDON_GAME_HDL hdl, const char* controller_id, const char* feature_name);

  return m_ifcToAddon->kodi_addon_game_has_feature_v1(m_ifcToAddon->thisClassHdl, hdl,
                                                      controller_id, feature_name);
}

struct game_input_topology* CHdl_kodi_addoninstance_game_h::kodi_addon_game_get_topology_v1(
    const KODI_ADDON_GAME_HDL hdl)
{
  // Original API call: typedef struct game_input_topology*(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_GET_TOPOLOGY_V1)(const KODI_ADDON_GAME_HDL hdl);

  return m_ifcToAddon->kodi_addon_game_get_topology_v1(m_ifcToAddon->thisClassHdl, hdl);
}

void CHdl_kodi_addoninstance_game_h::kodi_addon_game_free_topology_v1(
    const KODI_ADDON_GAME_HDL hdl, struct game_input_topology* topology)
{
  // Original API call: typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_FREE_TOPOLOGY_V1)(const KODI_ADDON_GAME_HDL hdl, struct game_input_topology* topology);

  m_ifcToAddon->kodi_addon_game_free_topology_v1(m_ifcToAddon->thisClassHdl, hdl, topology);
}

void CHdl_kodi_addoninstance_game_h::kodi_addon_game_set_controller_layouts_v1(
    const KODI_ADDON_GAME_HDL hdl,
    const struct game_controller_layout* controllers,
    size_t controller_count)
{
  // Original API call: typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_SET_CONTROLLER_LAYOUTS_V1)(const KODI_ADDON_GAME_HDL hdl, const struct game_controller_layout* controllers, size_t controller_count);

  m_ifcToAddon->kodi_addon_game_set_controller_layouts_v1(m_ifcToAddon->thisClassHdl, hdl,
                                                          controllers, controller_count);
}

bool CHdl_kodi_addoninstance_game_h::kodi_addon_game_enable_keyboard_v1(
    const KODI_ADDON_GAME_HDL hdl, bool enable, const char* controller_id)
{
  // Original API call: typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_ENABLE_KEYBOARD_V1)(const KODI_ADDON_GAME_HDL hdl, bool enable, const char* controller_id);

  return m_ifcToAddon->kodi_addon_game_enable_keyboard_v1(m_ifcToAddon->thisClassHdl, hdl, enable,
                                                          controller_id);
}

bool CHdl_kodi_addoninstance_game_h::kodi_addon_game_enable_mouse_v1(const KODI_ADDON_GAME_HDL hdl,
                                                                     bool enable,
                                                                     const char* controller_id)
{
  // Original API call: typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_ENABLE_MOUSE_V1)(const KODI_ADDON_GAME_HDL hdl, bool enable, const char* controller_id);

  return m_ifcToAddon->kodi_addon_game_enable_mouse_v1(m_ifcToAddon->thisClassHdl, hdl, enable,
                                                       controller_id);
}

bool CHdl_kodi_addoninstance_game_h::kodi_addon_game_connect_controller_v1(
    const KODI_ADDON_GAME_HDL hdl,
    bool connect,
    const char* port_address,
    const char* controller_id)
{
  // Original API call: typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_CONNECT_CONTROLLER_V1)(const KODI_ADDON_GAME_HDL hdl, bool connect, const char* port_address, const char* controller_id);

  return m_ifcToAddon->kodi_addon_game_connect_controller_v1(m_ifcToAddon->thisClassHdl, hdl,
                                                             connect, port_address, controller_id);
}

bool CHdl_kodi_addoninstance_game_h::kodi_addon_game_input_event_v1(
    const KODI_ADDON_GAME_HDL hdl, const struct game_input_event* event)
{
  // Original API call: typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_INPUT_EVENT_V1)(const KODI_ADDON_GAME_HDL hdl, const struct game_input_event* event);

  return m_ifcToAddon->kodi_addon_game_input_event_v1(m_ifcToAddon->thisClassHdl, hdl, event);
}

size_t CHdl_kodi_addoninstance_game_h::kodi_addon_game_serialize_size_v1(
    const KODI_ADDON_GAME_HDL hdl)
{
  // Original API call: typedef size_t(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_SERIALIZE_SIZE_V1)(const KODI_ADDON_GAME_HDL hdl);

  return m_ifcToAddon->kodi_addon_game_serialize_size_v1(m_ifcToAddon->thisClassHdl, hdl);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_serialize_v1(
    const KODI_ADDON_GAME_HDL hdl, uint8_t* data, size_t size)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_SERIALIZE_V1)(const KODI_ADDON_GAME_HDL hdl, uint8_t* data, size_t size);

  return m_ifcToAddon->kodi_addon_game_serialize_v1(m_ifcToAddon->thisClassHdl, hdl, data, size);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_deserialize_v1(
    const KODI_ADDON_GAME_HDL hdl, const uint8_t* data, size_t size)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_DESERIALIZE_V1)(const KODI_ADDON_GAME_HDL hdl, const uint8_t* data, size_t size);

  return m_ifcToAddon->kodi_addon_game_deserialize_v1(m_ifcToAddon->thisClassHdl, hdl, data, size);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_cheat_reset_v1(
    const KODI_ADDON_GAME_HDL hdl)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_CHEAT_RESET_V1)(const KODI_ADDON_GAME_HDL hdl);

  return m_ifcToAddon->kodi_addon_game_cheat_reset_v1(m_ifcToAddon->thisClassHdl, hdl);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_get_memory_v1(
    const KODI_ADDON_GAME_HDL hdl, enum GAME_MEMORY type, uint8_t** data, size_t* size)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_GET_MEMORY_V1)(const KODI_ADDON_GAME_HDL hdl, enum GAME_MEMORY type, uint8_t** data, size_t* size);

  return m_ifcToAddon->kodi_addon_game_get_memory_v1(m_ifcToAddon->thisClassHdl, hdl, type, data,
                                                     size);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_set_cheat_v1(
    const KODI_ADDON_GAME_HDL hdl, unsigned int index, bool enabled, const char* code)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_SET_CHEAT_V1)(const KODI_ADDON_GAME_HDL hdl, unsigned int index, bool enabled, const char* code);

  return m_ifcToAddon->kodi_addon_game_set_cheat_v1(m_ifcToAddon->thisClassHdl, hdl, index, enabled,
                                                    code);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_rc_generate_hash_from_file_v1(
    const KODI_ADDON_GAME_HDL hdl, char** hash, unsigned int console_id, const char* file_path)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_RC_GENERATE_HASH_FROM_FILE_V1)(const KODI_ADDON_GAME_HDL hdl, char** hash, unsigned int console_id, const char* file_path);

  return m_ifcToAddon->kodi_addon_game_rc_generate_hash_from_file_v1(
      m_ifcToAddon->thisClassHdl, hdl, hash, console_id, file_path);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_rc_get_game_id_url_v1(
    const KODI_ADDON_GAME_HDL hdl, char** url, const char* hash)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_RC_GET_GAME_ID_URL_V1)(const KODI_ADDON_GAME_HDL hdl, char** url, const char* hash);

  return m_ifcToAddon->kodi_addon_game_rc_get_game_id_url_v1(m_ifcToAddon->thisClassHdl, hdl, url,
                                                             hash);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_rc_get_patch_file_url_v1(
    const KODI_ADDON_GAME_HDL hdl,
    char** url,
    const char* username,
    const char* token,
    unsigned int game_id)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_RC_GET_PATCH_FILE_URL_V1)(const KODI_ADDON_GAME_HDL hdl, char** url, const char* username, const char* token, unsigned int game_id);

  return m_ifcToAddon->kodi_addon_game_rc_get_patch_file_url_v1(m_ifcToAddon->thisClassHdl, hdl,
                                                                url, username, token, game_id);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_rc_post_rich_presence_url_v1(
    const KODI_ADDON_GAME_HDL hdl,
    char** url,
    char** post_data,
    const char* username,
    const char* token,
    unsigned int game_id,
    const char* rich_presence)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_RC_POST_RICH_PRESENCE_URL_V1)(const KODI_ADDON_GAME_HDL hdl, char** url, char** post_data, const char* username, const char* token, unsigned int game_id, const char* rich_presence);

  return m_ifcToAddon->kodi_addon_game_rc_post_rich_presence_url_v1(
      m_ifcToAddon->thisClassHdl, hdl, url, post_data, username, token, game_id, rich_presence);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_rc_enable_rich_presence_v1(
    const KODI_ADDON_GAME_HDL hdl, const char* script)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_RC_ENABLE_RICH_PRESENCE_V1)(const KODI_ADDON_GAME_HDL hdl, const char* script);

  return m_ifcToAddon->kodi_addon_game_rc_enable_rich_presence_v1(m_ifcToAddon->thisClassHdl, hdl,
                                                                  script);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_rc_get_rich_presence_evaluation_v1(
    const KODI_ADDON_GAME_HDL hdl, char** evaluation, unsigned int console_id)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_RC_GET_RICH_PRESENCE_EVALUATION_V1)(const KODI_ADDON_GAME_HDL hdl, char** evaluation, unsigned int console_id);

  return m_ifcToAddon->kodi_addon_game_rc_get_rich_presence_evaluation_v1(
      m_ifcToAddon->thisClassHdl, hdl, evaluation, console_id);
}

enum GAME_ERROR CHdl_kodi_addoninstance_game_h::kodi_addon_game_rc_reset_runtime_v1(
    const KODI_ADDON_GAME_HDL hdl)
{
  // Original API call: typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_RC_RESET_RUNTIME_V1)(const KODI_ADDON_GAME_HDL hdl);

  return m_ifcToAddon->kodi_addon_game_rc_reset_runtime_v1(m_ifcToAddon->thisClassHdl, hdl);
}

// Callbacks from addon to Kodi

void CHdl_kodi_addoninstance_game_h::kodi_addon_game_close_v1(void* thisClassHdl,
                                                              KODI_ADDON_INSTANCE_BACKEND_HDL hdl)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_addoninstance_game_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_ADDON_GAME_CLOSE>---*/
  // AUTOGENERATED - Used by class "KODI::GAME::CGameClient" with "void cb_close_game()" on "games/addons/GameClient.h".

  KODI::GAME::CGameClient* cb = static_cast<KODI::GAME::CGameClient*>(hdl);
  cb->cb_close_game();
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_ADDON_GAME_CLOSE_END>---*/
}

KODI_GAME_STREAM_HANDLE CHdl_kodi_addoninstance_game_h::kodi_addon_game_open_stream_v1(
    void* thisClassHdl,
    KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
    const struct game_stream_properties* properties)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_addoninstance_game_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return nullptr;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_ADDON_GAME_OPEN_STREAM>---*/
  // AUTOGENERATED - Used by class "KODI::GAME::CGameClient" with "KODI_GAME_STREAM_HANDLE cb_open_stream(const game_stream_properties* properties)" on "games/addons/GameClient.h".

  KODI::GAME::CGameClient* cb = static_cast<KODI::GAME::CGameClient*>(hdl);
  return cb->cb_open_stream(properties);
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_ADDON_GAME_OPEN_STREAM_END>---*/
}

bool CHdl_kodi_addoninstance_game_h::kodi_addon_game_get_stream_buffer_v1(
    void* thisClassHdl,
    KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
    KODI_GAME_STREAM_HANDLE game_hdl,
    unsigned int width,
    unsigned int height,
    struct game_stream_buffer* buffer)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_addoninstance_game_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return false;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_ADDON_GAME_GET_STREAM_BUFFER>---*/
  // AUTOGENERATED - Used by class "KODI::GAME::CGameClient" with "bool cb_get_stream_buffer(KODI_GAME_STREAM_HANDLE stream, unsigned int width, unsigned int height, game_stream_buffer* buffer)" on "games/addons/GameClient.h".

  KODI::GAME::CGameClient* cb = static_cast<KODI::GAME::CGameClient*>(hdl);
  return cb->cb_get_stream_buffer(game_hdl, width, height, buffer);
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_ADDON_GAME_GET_STREAM_BUFFER_END>---*/
}

void CHdl_kodi_addoninstance_game_h::kodi_addon_game_add_stream_data_v1(
    void* thisClassHdl,
    KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
    KODI_GAME_STREAM_HANDLE game_hdl,
    const struct game_stream_packet* packet)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_addoninstance_game_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_ADDON_GAME_ADD_STREAM_DATA>---*/
  // AUTOGENERATED - Used by class "KODI::GAME::CGameClient" with "void cb_add_stream_data(KODI_GAME_STREAM_HANDLE stream, const game_stream_packet* packet)" on "games/addons/GameClient.h".

  KODI::GAME::CGameClient* cb = static_cast<KODI::GAME::CGameClient*>(hdl);
  cb->cb_add_stream_data(game_hdl, packet);
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_ADDON_GAME_ADD_STREAM_DATA_END>---*/
}

void CHdl_kodi_addoninstance_game_h::kodi_addon_game_release_stream_buffer_v1(
    void* thisClassHdl,
    KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
    KODI_GAME_STREAM_HANDLE game_hdl,
    struct game_stream_buffer* buffer)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_addoninstance_game_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_ADDON_GAME_RELEASE_STREAM_BUFFER>---*/
  // AUTOGENERATED - Used by class "KODI::GAME::CGameClient" with "void cb_release_stream_buffer(KODI_GAME_STREAM_HANDLE stream, game_stream_buffer* buffer)" on "games/addons/GameClient.h".

  KODI::GAME::CGameClient* cb = static_cast<KODI::GAME::CGameClient*>(hdl);
  cb->cb_release_stream_buffer(game_hdl, buffer);
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_ADDON_GAME_RELEASE_STREAM_BUFFER_END>---*/
}

void CHdl_kodi_addoninstance_game_h::kodi_addon_game_close_stream_v1(
    void* thisClassHdl, KODI_ADDON_INSTANCE_BACKEND_HDL hdl, KODI_GAME_STREAM_HANDLE game_hdl)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_addoninstance_game_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_ADDON_GAME_CLOSE_STREAM>---*/
  // AUTOGENERATED - Used by class "KODI::GAME::CGameClient" with "void cb_close_stream(KODI_GAME_STREAM_HANDLE stream)" on "games/addons/GameClient.h".

  KODI::GAME::CGameClient* cb = static_cast<KODI::GAME::CGameClient*>(hdl);
  cb->cb_close_stream(game_hdl);
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_ADDON_GAME_CLOSE_STREAM_END>---*/
}

game_proc_address_t CHdl_kodi_addoninstance_game_h::kodi_addon_game_hw_get_proc_address_v1(
    void* thisClassHdl, KODI_ADDON_INSTANCE_BACKEND_HDL hdl, const char* symbol)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_addoninstance_game_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return nullptr;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_ADDON_GAME_HW_GET_PROC_ADDRESS>---*/
  // AUTOGENERATED - Used by class "KODI::GAME::CGameClient" with "game_proc_address_t cb_hw_get_proc_address(const char* sym)" on "games/addons/GameClient.h".

  KODI::GAME::CGameClient* cb = static_cast<KODI::GAME::CGameClient*>(hdl);
  return cb->cb_hw_get_proc_address(symbol);
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_ADDON_GAME_HW_GET_PROC_ADDRESS_END>---*/
}

bool CHdl_kodi_addoninstance_game_h::kodi_addon_game_input_event_v1(
    void* thisClassHdl, KODI_ADDON_INSTANCE_BACKEND_HDL hdl, const struct game_input_event* event)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_addoninstance_game_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return false;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_ADDON_GAME_INPUT_EVENT>---*/
  // AUTOGENERATED - Used by class "KODI::GAME::CGameClient" with "bool cb_input_event(const game_input_event* event)" on "games/addons/GameClient.h".

  KODI::GAME::CGameClient* cb = static_cast<KODI::GAME::CGameClient*>(hdl);
  return cb->cb_input_event(event);
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_ADDON_GAME_INPUT_EVENT_END>---*/
}

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3_END>---*/

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
