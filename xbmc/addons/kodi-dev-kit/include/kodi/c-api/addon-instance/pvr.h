/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_ADDONINSTANCE_PVR_H
#define C_API_ADDONINSTANCE_PVR_H

#include "../addon_base.h"
#include "pvr/pvr_channel_groups.h"
#include "pvr/pvr_channels.h"
#include "pvr/pvr_defines.h"
#include "pvr/pvr_edl.h"
#include "pvr/pvr_epg.h"
#include "pvr/pvr_general.h"
#include "pvr/pvr_menu_hook.h"
#include "pvr/pvr_providers.h"
#include "pvr/pvr_recordings.h"
#include "pvr/pvr_stream.h"
#include "pvr/pvr_timers.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  typedef void* KODI_ADDON_PVR_HDL;
  typedef void* KODI_ADDON_PVR_TRANSFER_HDL;

  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_CAPABILITIES_V1)(
      KODI_ADDON_PVR_HDL hdl, struct PVR_ADDON_CAPABILITIES* capabilities);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_BACKEND_NAME_V1)(
      KODI_ADDON_PVR_HDL hdl, char* str, size_t mem_size);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_BACKEND_VERSION_V1)(
      KODI_ADDON_PVR_HDL hdl, char* str, size_t mem_size);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_BACKEND_HOSTNAME_V1)(
      KODI_ADDON_PVR_HDL hdl, char* str, size_t mem_size);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_CONNECTION_STRING_V1)(
      KODI_ADDON_PVR_HDL hdl, char* str, size_t mem_size);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_DRIVE_SPACE_V1)(
      KODI_ADDON_PVR_HDL hdl, uint64_t* total, uint64_t* used);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_CALL_SETTINGS_MENU_HOOK_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct PVR_MENUHOOK* menuhook);

  //--==----==----==----==----==----==----==----==----==----==----==----==----==
  // Channel interface functions
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_CHANNELS_AMOUNT_V1)(
      KODI_ADDON_PVR_HDL hdl, int* amount);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_CHANNELS_V1)(
      KODI_ADDON_PVR_HDL hdl, KODI_ADDON_PVR_TRANSFER_HDL handle, bool radio);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_CHANNEL_STREAM_PROPERTIES_V1)(
      KODI_ADDON_PVR_HDL hdl,
      const struct PVR_CHANNEL* channel,
      struct PVR_NAMED_VALUE* properties,
      size_t* properties_count);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_SIGNAL_STATUS_V1)(
      KODI_ADDON_PVR_HDL hdl, int channel_uid, struct PVR_SIGNAL_STATUS* signal_status);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_DESCRAMBLE_INFO_V1)(
      KODI_ADDON_PVR_HDL hdl, int channel_uid, struct PVR_DESCRAMBLE_INFO* descramble_info);

  //--==----==----==----==----==----==----==----==----==----==----==----==----==
  // Provider interface functions

  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_PROVIDERS_AMOUNT_V1)(
      KODI_ADDON_PVR_HDL hdl, int* amount);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_PROVIDERS_V1)(
      KODI_ADDON_PVR_HDL hdl, KODI_ADDON_PVR_TRANSFER_HDL handle);

  //--==----==----==----==----==----==----==----==----==----==----==----==----==
  // Channel group interface functions
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_CHANNEL_GROUPS_AMOUNT_V1)(
      KODI_ADDON_PVR_HDL hdl, int* amount);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_CHANNEL_GROUPS_V1)(
      KODI_ADDON_PVR_HDL hdl, KODI_ADDON_PVR_TRANSFER_HDL handle, bool radio);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_CHANNEL_GROUP_MEMBERS_V1)(
      KODI_ADDON_PVR_HDL hdl,
      KODI_ADDON_PVR_TRANSFER_HDL handle,
      const struct PVR_CHANNEL_GROUP* group);

  //--==----==----==----==----==----==----==----==----==----==----==----==----==
  // Channel edit interface functions
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_DELETE_CHANNEL_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct PVR_CHANNEL* channel);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_RENAME_CHANNEL_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct PVR_CHANNEL* channel);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_OPEN_DIALOG_CHANNEL_SETTINGS_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct PVR_CHANNEL* channel);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_OPEN_DIALOG_CHANNEL_ADD_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct PVR_CHANNEL* channel);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_OPEN_DIALOG_CHANNEL_SCAN_V1)(
      KODI_ADDON_PVR_HDL hdl);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_CALL_CHANNEL_MENU_HOOK_V1)(
      KODI_ADDON_PVR_HDL hdl,
      const struct PVR_MENUHOOK* menuhook,
      const struct PVR_CHANNEL* channel);

  //--==----==----==----==----==----==----==----==----==----==----==----==----==
  // EPG interface functions
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_EPG_FOR_CHANNEL_V1)(
      KODI_ADDON_PVR_HDL hdl,
      KODI_ADDON_PVR_TRANSFER_HDL handle,
      int channel_uid,
      time_t start,
      time_t end);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_IS_EPG_TAG_RECORDABLE_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct EPG_TAG* tag, bool* is_recordable);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_IS_EPG_TAG_PLAYABLE_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct EPG_TAG* tag, bool* is_playable);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_EPG_TAG_EDL_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct EPG_TAG* tag, struct PVR_EDL_ENTRY edl[], size_t* size);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_EPG_TAG_STREAM_PROPERTIES_V1)(
      KODI_ADDON_PVR_HDL hdl,
      const struct EPG_TAG* tag,
      struct PVR_NAMED_VALUE* properties,
      size_t* properties_count);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_SET_EPG_MAX_PAST_DAYS_V1)(
      KODI_ADDON_PVR_HDL hdl, int past_days);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_SET_EPG_MAX_FUTURE_DAYS_V1)(
      KODI_ADDON_PVR_HDL hdl, int future_days);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_CALL_EPG_MENU_HOOK_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct PVR_MENUHOOK* menuhook, const struct EPG_TAG* tag);

  //--==----==----==----==----==----==----==----==----==----==----==----==----==
  // Recording interface functions
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_RECORDINGS_AMOUNT_V1)(
      KODI_ADDON_PVR_HDL hdl, bool deleted, int* amount);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_RECORDINGS_V1)(
      KODI_ADDON_PVR_HDL hdl, KODI_ADDON_PVR_TRANSFER_HDL handle, bool deleted);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_DELETE_RECORDING_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct PVR_RECORDING* recording);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_UNDELETE_RECORDING_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct PVR_RECORDING* recording);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_DELETE_ALL_RECORDINGS_FROM_TRASH_V1)(
      KODI_ADDON_PVR_HDL hdl);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_RENAME_RECORDING_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct PVR_RECORDING* recording);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_SET_RECORDING_LIFETIME_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct PVR_RECORDING* recording);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_SET_RECORDING_PLAY_COUNT_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct PVR_RECORDING* recording, int count);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_SET_RECORDING_LAST_PLAYED_POSITION_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct PVR_RECORDING* recording, int last_played_position);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_RECORDING_LAST_PLAYED_POSITION_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct PVR_RECORDING* recording, int* last_played_position);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_RECORDING_EDL_V1)(
      KODI_ADDON_PVR_HDL hdl,
      const struct PVR_RECORDING* recording,
      struct PVR_EDL_ENTRY edl[],
      size_t* size);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_RECORDING_SIZE_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct PVR_RECORDING* recording, int64_t* size);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_RECORDING_STREAM_PROPERTIES_V1)(
      KODI_ADDON_PVR_HDL hdl,
      const struct PVR_RECORDING* recording,
      struct PVR_NAMED_VALUE* properties,
      size_t* properties_count);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_CALL_RECORDING_MENU_HOOK_V1)(
      KODI_ADDON_PVR_HDL hdl,
      const struct PVR_MENUHOOK* menuhook,
      const struct PVR_RECORDING* recording);

  //--==----==----==----==----==----==----==----==----==----==----==----==----==
  // Timer interface functions
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_TIMER_TYPES_V1)(
      KODI_ADDON_PVR_HDL hdl, struct PVR_TIMER_TYPE types[], size_t* types_count);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_TIMERS_AMOUNT_V1)(
      KODI_ADDON_PVR_HDL hdl, int* amount);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_GET_TIMERS_V1)(
      KODI_ADDON_PVR_HDL hdl, KODI_ADDON_PVR_TRANSFER_HDL handle);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_ADD_TIMER_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct PVR_TIMER* timer);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_DELETE_TIMER_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct PVR_TIMER* timer, bool force_delete);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_UPDATE_TIMER_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct PVR_TIMER* timer);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_CALL_TIMER_MENU_HOOK_V1)(
      KODI_ADDON_PVR_HDL hdl, const struct PVR_MENUHOOK* menuhook, const struct PVR_TIMER* timer);

  //--==----==----==----==----==----==----==----==----==----==----==----==----==
  // Powersaving interface functions
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_ON_SYSTEM_SLEEP_V1)(
      KODI_ADDON_PVR_HDL hdl);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_ON_SYSTEM_WAKE_V1)(
      KODI_ADDON_PVR_HDL hdl);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_ON_POWER_SAVING_ACTIVATED_V1)(
      KODI_ADDON_PVR_HDL hdl);
  typedef enum PVR_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_PVR_ON_POWER_SAVING_DEACTIVATED_V1)(
      KODI_ADDON_PVR_HDL hdl);

  typedef struct KODI_ADDON_PVR_FUNC
  {
    //--==----==----==----==----==----==----==----==----==----==----==----==----==
    // General interface functions
    PFN_KODI_ADDON_PVR_GET_CAPABILITIES_V1 get_capabilities;
    PFN_KODI_ADDON_PVR_GET_BACKEND_NAME_V1 get_backend_name;
    PFN_KODI_ADDON_PVR_GET_BACKEND_VERSION_V1 get_backend_version;
    PFN_KODI_ADDON_PVR_GET_BACKEND_HOSTNAME_V1 get_backend_hostname;
    PFN_KODI_ADDON_PVR_GET_CONNECTION_STRING_V1 get_connection_string;
    PFN_KODI_ADDON_PVR_GET_DRIVE_SPACE_V1 get_drive_space;
    PFN_KODI_ADDON_PVR_CALL_SETTINGS_MENU_HOOK_V1 call_settings_menu_hook;

    //--==----==----==----==----==----==----==----==----==----==----==----==----==
    // Channel interface functions
    PFN_KODI_ADDON_PVR_GET_CHANNELS_AMOUNT_V1 get_channels_amount;
    PFN_KODI_ADDON_PVR_GET_CHANNELS_V1 get_channels;
    PFN_KODI_ADDON_PVR_GET_CHANNEL_STREAM_PROPERTIES_V1 get_channel_stream_properties;
    PFN_KODI_ADDON_PVR_GET_SIGNAL_STATUS_V1 get_signal_status;
    PFN_KODI_ADDON_PVR_GET_DESCRAMBLE_INFO_V1 get_descramble_info;

    //--==----==----==----==----==----==----==----==----==----==----==----==----==
    // Provider interface functions
    PFN_KODI_ADDON_PVR_GET_PROVIDERS_AMOUNT_V1 get_providers_amount;
    PFN_KODI_ADDON_PVR_GET_PROVIDERS_V1 get_providers;

    //--==----==----==----==----==----==----==----==----==----==----==----==----==
    // Channel group interface functions
    PFN_KODI_ADDON_PVR_GET_CHANNEL_GROUPS_AMOUNT_V1 get_channel_groups_amount;
    PFN_KODI_ADDON_PVR_GET_CHANNEL_GROUPS_V1 get_channel_groups;
    PFN_KODI_ADDON_PVR_GET_CHANNEL_GROUP_MEMBERS_V1 get_channel_group_members;

    //--==----==----==----==----==----==----==----==----==----==----==----==----==
    // Channel edit interface functions
    PFN_KODI_ADDON_PVR_DELETE_CHANNEL_V1 delete_channel;
    PFN_KODI_ADDON_PVR_RENAME_CHANNEL_V1 rename_channel;
    PFN_KODI_ADDON_PVR_OPEN_DIALOG_CHANNEL_SETTINGS_V1 open_dialog_channel_settings;
    PFN_KODI_ADDON_PVR_OPEN_DIALOG_CHANNEL_ADD_V1 open_dialog_channel_add;
    PFN_KODI_ADDON_PVR_OPEN_DIALOG_CHANNEL_SCAN_V1 open_dialog_channel_scan;
    PFN_KODI_ADDON_PVR_CALL_CHANNEL_MENU_HOOK_V1 call_channel_menu_hook;

    //--==----==----==----==----==----==----==----==----==----==----==----==----==
    // EPG interface functions
    PFN_KODI_ADDON_PVR_GET_EPG_FOR_CHANNEL_V1 get_epg_for_channel;
    PFN_KODI_ADDON_PVR_IS_EPG_TAG_RECORDABLE_V1 is_epg_tag_recordable;
    PFN_KODI_ADDON_PVR_IS_EPG_TAG_PLAYABLE_V1 is_epg_tag_playable;
    PFN_KODI_ADDON_PVR_GET_EPG_TAG_EDL_V1 get_epg_tag_edl;
    PFN_KODI_ADDON_PVR_GET_EPG_TAG_STREAM_PROPERTIES_V1 get_epg_tag_stream_properties;
    PFN_KODI_ADDON_PVR_SET_EPG_MAX_PAST_DAYS_V1 set_epg_max_past_days;
    PFN_KODI_ADDON_PVR_SET_EPG_MAX_FUTURE_DAYS_V1 set_epg_max_future_days;
    PFN_KODI_ADDON_PVR_CALL_EPG_MENU_HOOK_V1 call_epg_menu_hook;

    //--==----==----==----==----==----==----==----==----==----==----==----==----==
    // Recording interface functions
    PFN_KODI_ADDON_PVR_GET_RECORDINGS_AMOUNT_V1 get_recordings_amount;
    PFN_KODI_ADDON_PVR_GET_RECORDINGS_V1 get_recordings;
    PFN_KODI_ADDON_PVR_DELETE_RECORDING_V1 delete_recording;
    PFN_KODI_ADDON_PVR_UNDELETE_RECORDING_V1 undelete_recording;
    PFN_KODI_ADDON_PVR_DELETE_ALL_RECORDINGS_FROM_TRASH_V1 delete_all_recordings_from_trash;
    PFN_KODI_ADDON_PVR_RENAME_RECORDING_V1 rename_recording;
    PFN_KODI_ADDON_PVR_SET_RECORDING_LIFETIME_V1 set_recording_lifetime;
    PFN_KODI_ADDON_PVR_SET_RECORDING_PLAY_COUNT_V1 set_recording_play_count;
    PFN_KODI_ADDON_PVR_SET_RECORDING_LAST_PLAYED_POSITION_V1 set_recording_last_played_position;
    PFN_KODI_ADDON_PVR_GET_RECORDING_LAST_PLAYED_POSITION_V1 get_recording_last_played_position;
    PFN_KODI_ADDON_PVR_GET_RECORDING_EDL_V1 get_recording_edl;
    PFN_KODI_ADDON_PVR_GET_RECORDING_SIZE_V1 get_recording_size;
    PFN_KODI_ADDON_PVR_GET_RECORDING_STREAM_PROPERTIES_V1 get_recording_stream_properties;
    PFN_KODI_ADDON_PVR_CALL_RECORDING_MENU_HOOK_V1 call_recording_menu_hook;

    //--==----==----==----==----==----==----==----==----==----==----==----==----==
    // Timer interface functions
    PFN_KODI_ADDON_PVR_GET_TIMER_TYPES_V1 get_timer_types;
    PFN_KODI_ADDON_PVR_GET_TIMERS_AMOUNT_V1 get_timers_amount;
    PFN_KODI_ADDON_PVR_GET_TIMERS_V1 get_timers;
    PFN_KODI_ADDON_PVR_ADD_TIMER_V1 add_timer;
    PFN_KODI_ADDON_PVR_DELETE_TIMER_V1 delete_timer;
    PFN_KODI_ADDON_PVR_UPDATE_TIMER_V1 update_timer;
    PFN_KODI_ADDON_PVR_CALL_TIMER_MENU_HOOK_V1 call_timer_menu_hook;

    //--==----==----==----==----==----==----==----==----==----==----==----==----==
    // Powersaving interface functions
    PFN_KODI_ADDON_PVR_ON_SYSTEM_SLEEP_V1 on_system_sleep;
    PFN_KODI_ADDON_PVR_ON_SYSTEM_WAKE_V1 on_system_wake;
    PFN_KODI_ADDON_PVR_ON_POWER_SAVING_ACTIVATED_V1 on_power_saving_activated;
    PFN_KODI_ADDON_PVR_ON_POWER_SAVING_DEACTIVATED_V1 on_power_saving_deactivated;
  } KODI_ADDON_PVR_FUNC;

  ATTR_DLL_EXPORT int kodi_addon_pvr_get_epg_max_future_days(KODI_ADDON_INSTANCE_BACKEND_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT int kodi_addon_pvr_get_epg_max_past_days(KODI_ADDON_INSTANCE_BACKEND_HDL hdl)
      __INTRODUCED_IN_KODI(1);

  ATTR_DLL_EXPORT void kodi_addon_pvr_add_menu_hook(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                                    const struct PVR_MENUHOOK* hook)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_pvr_recording_notification(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                                             const char* name,
                                                             const char* fileName,
                                                             bool on) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_pvr_connection_state_change(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                                              const char* strConnectionString,
                                                              enum PVR_CONNECTION_STATE newState,
                                                              const char* strMessage)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_pvr_epg_event_state_change(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                                             struct EPG_TAG* tag,
                                                             enum EPG_EVENT_STATE newState)
      __INTRODUCED_IN_KODI(1);

  //--==----==----==----==----==----==----==----==----==----==----==----==----==
  // Transfer functions where give data back to Kodi, e.g. GetChannels calls TransferChannelEntry
  ATTR_DLL_EXPORT void kodi_addon_pvr_transfer_channel_entry(
      KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
      const KODI_ADDON_PVR_TRANSFER_HDL handle,
      const struct PVR_CHANNEL* chan) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_pvr_transfer_provider_entry(
      KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
      const KODI_ADDON_PVR_TRANSFER_HDL handle,
      const struct PVR_PROVIDER* chanProvider) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_pvr_transfer_channel_group(
      KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
      const KODI_ADDON_PVR_TRANSFER_HDL handle,
      const struct PVR_CHANNEL_GROUP* group) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_pvr_transfer_channel_group_member(
      KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
      const KODI_ADDON_PVR_TRANSFER_HDL handle,
      const struct PVR_CHANNEL_GROUP_MEMBER* member) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_pvr_transfer_epg_entry(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                                         const KODI_ADDON_PVR_TRANSFER_HDL handle,
                                                         const struct EPG_TAG* epgentry)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_pvr_transfer_recording_entry(
      KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
      const KODI_ADDON_PVR_TRANSFER_HDL handle,
      const struct PVR_RECORDING* recording) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_pvr_transfer_timer_entry(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                                           const KODI_ADDON_PVR_TRANSFER_HDL handle,
                                                           const struct PVR_TIMER* timer)
      __INTRODUCED_IN_KODI(1);

  //--==----==----==----==----==----==----==----==----==----==----==----==----==
  // Kodi inform interface functions
  ATTR_DLL_EXPORT void kodi_addon_pvr_trigger_channel_update(KODI_ADDON_INSTANCE_BACKEND_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_pvr_trigger_providers_update(KODI_ADDON_INSTANCE_BACKEND_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_pvr_trigger_channel_groups_update(
      KODI_ADDON_INSTANCE_BACKEND_HDL hdl) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_pvr_trigger_epg_update(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                                         unsigned int iChannelUid)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_pvr_trigger_recording_update(KODI_ADDON_INSTANCE_BACKEND_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_pvr_trigger_timer_update(KODI_ADDON_INSTANCE_BACKEND_HDL hdl)
      __INTRODUCED_IN_KODI(1);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !C_API_ADDONINSTANCE_PVR_H */
