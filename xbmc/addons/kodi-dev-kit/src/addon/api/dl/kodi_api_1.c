/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/* File autogenerated, see xbmc/addons/kodi-dev-kit/tools/code-generator/code_generator.py */

/* API_HASH<d1996da312bf6097c97b57be284d0116222467f37305491e23c51e4f8d435411> */

#include "../../../../include/kodi/c-api/dl/kodi_api_1.h"
#include "include/kodi/c-api/addon_base.h"
#include "include/kodi/c-api/audio_engine.h"
#include "include/kodi/c-api/filesystem.h"
#include "include/kodi/c-api/general.h"
#include "include/kodi/c-api/gui/dialogs/context_menu.h"
#include "include/kodi/c-api/gui/dialogs/filebrowser.h"
#include "include/kodi/c-api/gui/dialogs/keyboard.h"
#include "include/kodi/c-api/gui/dialogs/numeric.h"
#include "include/kodi/c-api/gui/dialogs/ok.h"
#include "include/kodi/c-api/gui/dialogs/select.h"
#include "include/kodi/c-api/gui/dialogs/text_viewer.h"
#include "include/kodi/c-api/gui/dialogs/yes_no.h"
#include "include/kodi/c-api/gui/general.h"
#include "include/kodi/c-api/monitor.h"
#include "include/kodi/c-api/network.h"

ATTR_DL_DLL_EXPORT bool load_kodi_api_1(struct kodi_api_1* api)
{
  if (api == NULL)
  {
    return false;
  }

  // addon_base.h
  api->kodi_addon_get_addon_path = &kodi_addon_get_addon_path;
  api->kodi_addon_get_info = &kodi_addon_get_info;
  api->kodi_addon_get_lib_path = &kodi_addon_get_lib_path;
  api->kodi_addon_get_setting_bool = &kodi_addon_get_setting_bool;
  api->kodi_addon_get_setting_float = &kodi_addon_get_setting_float;
  api->kodi_addon_get_setting_int = &kodi_addon_get_setting_int;
  api->kodi_addon_get_setting_string = &kodi_addon_get_setting_string;
  api->kodi_addon_get_temp_path = &kodi_addon_get_temp_path;
  api->kodi_addon_get_user_path = &kodi_addon_get_user_path;
  api->kodi_addon_instance_get_setting_bool = &kodi_addon_instance_get_setting_bool;
  api->kodi_addon_instance_get_setting_float = &kodi_addon_instance_get_setting_float;
  api->kodi_addon_instance_get_setting_int = &kodi_addon_instance_get_setting_int;
  api->kodi_addon_instance_get_setting_string = &kodi_addon_instance_get_setting_string;
  api->kodi_addon_instance_get_user_path = &kodi_addon_instance_get_user_path;
  api->kodi_addon_instance_is_setting_using_default = &kodi_addon_instance_is_setting_using_default;
  api->kodi_addon_instance_set_setting_bool = &kodi_addon_instance_set_setting_bool;
  api->kodi_addon_instance_set_setting_float = &kodi_addon_instance_set_setting_float;
  api->kodi_addon_instance_set_setting_int = &kodi_addon_instance_set_setting_int;
  api->kodi_addon_instance_set_setting_string = &kodi_addon_instance_set_setting_string;
  api->kodi_addon_is_setting_using_default = &kodi_addon_is_setting_using_default;
  api->kodi_addon_localized_string = &kodi_addon_localized_string;
  api->kodi_addon_open_settings_dialog = &kodi_addon_open_settings_dialog;
  api->kodi_addon_set_setting_bool = &kodi_addon_set_setting_bool;
  api->kodi_addon_set_setting_float = &kodi_addon_set_setting_float;
  api->kodi_addon_set_setting_int = &kodi_addon_set_setting_int;
  api->kodi_addon_set_setting_string = &kodi_addon_set_setting_string;
  api->kodi_check_main_shared = &kodi_check_main_shared;
  api->kodi_deinit = &kodi_deinit;
  api->kodi_init = &kodi_init;
  api->kodi_log = &kodi_log;
  api->kodi_process = &kodi_process;

  // audio_engine.h
  api->kodi_audioengine_get_current_sink_format = &kodi_audioengine_get_current_sink_format;
  api->kodi_audioengine_is_planar_format = &kodi_audioengine_is_planar_format;
  api->kodi_audioengine_stream_add_data = &kodi_audioengine_stream_add_data;
  api->kodi_audioengine_stream_drain = &kodi_audioengine_stream_drain;
  api->kodi_audioengine_stream_flush = &kodi_audioengine_stream_flush;
  api->kodi_audioengine_stream_free = &kodi_audioengine_stream_free;
  api->kodi_audioengine_stream_get_amplification = &kodi_audioengine_stream_get_amplification;
  api->kodi_audioengine_stream_get_cache_time = &kodi_audioengine_stream_get_cache_time;
  api->kodi_audioengine_stream_get_cache_total = &kodi_audioengine_stream_get_cache_total;
  api->kodi_audioengine_stream_get_channel_count = &kodi_audioengine_stream_get_channel_count;
  api->kodi_audioengine_stream_get_data_format = &kodi_audioengine_stream_get_data_format;
  api->kodi_audioengine_stream_get_delay = &kodi_audioengine_stream_get_delay;
  api->kodi_audioengine_stream_get_frame_size = &kodi_audioengine_stream_get_frame_size;
  api->kodi_audioengine_stream_get_resample_ratio = &kodi_audioengine_stream_get_resample_ratio;
  api->kodi_audioengine_stream_get_sample_rate = &kodi_audioengine_stream_get_sample_rate;
  api->kodi_audioengine_stream_get_space = &kodi_audioengine_stream_get_space;
  api->kodi_audioengine_stream_get_volume = &kodi_audioengine_stream_get_volume;
  api->kodi_audioengine_stream_is_buffering = &kodi_audioengine_stream_is_buffering;
  api->kodi_audioengine_stream_is_drained = &kodi_audioengine_stream_is_drained;
  api->kodi_audioengine_stream_is_draining = &kodi_audioengine_stream_is_draining;
  api->kodi_audioengine_stream_make = &kodi_audioengine_stream_make;
  api->kodi_audioengine_stream_pause = &kodi_audioengine_stream_pause;
  api->kodi_audioengine_stream_resume = &kodi_audioengine_stream_resume;
  api->kodi_audioengine_stream_set_amplification = &kodi_audioengine_stream_set_amplification;
  api->kodi_audioengine_stream_set_resample_ratio = &kodi_audioengine_stream_set_resample_ratio;
  api->kodi_audioengine_stream_set_volume = &kodi_audioengine_stream_set_volume;

  // filesystem.h
  api->kodi_vfs_can_open_directory = &kodi_vfs_can_open_directory;
  api->kodi_vfs_copy_file = &kodi_vfs_copy_file;
  api->kodi_vfs_create_directory = &kodi_vfs_create_directory;
  api->kodi_vfs_delete_file = &kodi_vfs_delete_file;
  api->kodi_vfs_directory_exists = &kodi_vfs_directory_exists;
  api->kodi_vfs_file_at_end = &kodi_vfs_file_at_end;
  api->kodi_vfs_file_close = &kodi_vfs_file_close;
  api->kodi_vfs_file_curl_add_option = &kodi_vfs_file_curl_add_option;
  api->kodi_vfs_file_curl_create = &kodi_vfs_file_curl_create;
  api->kodi_vfs_file_curl_open = &kodi_vfs_file_curl_open;
  api->kodi_vfs_file_exists = &kodi_vfs_file_exists;
  api->kodi_vfs_file_flush = &kodi_vfs_file_flush;
  api->kodi_vfs_file_get_chunk_size = &kodi_vfs_file_get_chunk_size;
  api->kodi_vfs_file_get_download_speed = &kodi_vfs_file_get_download_speed;
  api->kodi_vfs_file_get_length = &kodi_vfs_file_get_length;
  api->kodi_vfs_file_get_position = &kodi_vfs_file_get_position;
  api->kodi_vfs_file_get_property_value = &kodi_vfs_file_get_property_value;
  api->kodi_vfs_file_get_property_values = &kodi_vfs_file_get_property_values;
  api->kodi_vfs_file_io_ctl_get_cache_status = &kodi_vfs_file_io_ctl_get_cache_status;
  api->kodi_vfs_file_io_ctl_get_seek_possible = &kodi_vfs_file_io_ctl_get_seek_possible;
  api->kodi_vfs_file_io_ctl_set_cache_rate = &kodi_vfs_file_io_ctl_set_cache_rate;
  api->kodi_vfs_file_io_ctl_set_retry = &kodi_vfs_file_io_ctl_set_retry;
  api->kodi_vfs_file_open = &kodi_vfs_file_open;
  api->kodi_vfs_file_open_for_write = &kodi_vfs_file_open_for_write;
  api->kodi_vfs_file_read = &kodi_vfs_file_read;
  api->kodi_vfs_file_read_line = &kodi_vfs_file_read_line;
  api->kodi_vfs_file_seek = &kodi_vfs_file_seek;
  api->kodi_vfs_file_truncate = &kodi_vfs_file_truncate;
  api->kodi_vfs_file_write = &kodi_vfs_file_write;
  api->kodi_vfs_free_directory = &kodi_vfs_free_directory;
  api->kodi_vfs_get_cache_thumb_name = &kodi_vfs_get_cache_thumb_name;
  api->kodi_vfs_get_content_type = &kodi_vfs_get_content_type;
  api->kodi_vfs_get_cookies = &kodi_vfs_get_cookies;
  api->kodi_vfs_get_directory = &kodi_vfs_get_directory;
  api->kodi_vfs_get_disk_space = &kodi_vfs_get_disk_space;
  api->kodi_vfs_get_file_md5 = &kodi_vfs_get_file_md5;
  api->kodi_vfs_get_mime_type = &kodi_vfs_get_mime_type;
  api->kodi_vfs_http_header_close = &kodi_vfs_http_header_close;
  api->kodi_vfs_http_header_get_charset = &kodi_vfs_http_header_get_charset;
  api->kodi_vfs_http_header_get_header = &kodi_vfs_http_header_get_header;
  api->kodi_vfs_http_header_get_mime_type = &kodi_vfs_http_header_get_mime_type;
  api->kodi_vfs_http_header_get_proto_line = &kodi_vfs_http_header_get_proto_line;
  api->kodi_vfs_http_header_get_value = &kodi_vfs_http_header_get_value;
  api->kodi_vfs_http_header_get_values = &kodi_vfs_http_header_get_values;
  api->kodi_vfs_http_header_open = &kodi_vfs_http_header_open;
  api->kodi_vfs_is_internet_stream = &kodi_vfs_is_internet_stream;
  api->kodi_vfs_is_local = &kodi_vfs_is_local;
  api->kodi_vfs_is_on_lan = &kodi_vfs_is_on_lan;
  api->kodi_vfs_is_remote = &kodi_vfs_is_remote;
  api->kodi_vfs_is_url = &kodi_vfs_is_url;
  api->kodi_vfs_make_legal_filename = &kodi_vfs_make_legal_filename;
  api->kodi_vfs_make_legal_path = &kodi_vfs_make_legal_path;
  api->kodi_vfs_remove_directory = &kodi_vfs_remove_directory;
  api->kodi_vfs_remove_directory_recursive = &kodi_vfs_remove_directory_recursive;
  api->kodi_vfs_rename_file = &kodi_vfs_rename_file;
  api->kodi_vfs_stat_file = &kodi_vfs_stat_file;
  api->kodi_vfs_translate_special_protocol = &kodi_vfs_translate_special_protocol;

  // general.h
  api->kodi_change_keyboard_layout = &kodi_change_keyboard_layout;
  api->kodi_get_current_skin_id = &kodi_get_current_skin_id;
  api->kodi_get_digest = &kodi_get_digest;
  api->kodi_get_free_mem = &kodi_get_free_mem;
  api->kodi_get_global_idle_time = &kodi_get_global_idle_time;
  api->kodi_get_keyboard_layout = &kodi_get_keyboard_layout;
  api->kodi_get_language = &kodi_get_language;
  api->kodi_get_localized_string = &kodi_get_localized_string;
  api->kodi_get_region = &kodi_get_region;
  api->kodi_is_addon_avilable = &kodi_is_addon_avilable;
  api->kodi_play_sfx = &kodi_play_sfx;
  api->kodi_queue_notification = &kodi_queue_notification;
  api->kodi_stop_sfx = &kodi_stop_sfx;
  api->kodi_unknown_to_utf8 = &kodi_unknown_to_utf8;
  api->kodi_version = &kodi_version;

  // monitor.h
  api->kodi_monitor_abort_requested = &kodi_monitor_abort_requested;
  api->kodi_monitor_wait_for_abort = &kodi_monitor_wait_for_abort;

  // network.h
  api->kodi_network_dns_lookup = &kodi_network_dns_lookup;
  api->kodi_network_get_hostname = &kodi_network_get_hostname;
  api->kodi_network_get_ip_address = &kodi_network_get_ip_address;
  api->kodi_network_get_user_agent = &kodi_network_get_user_agent;
  api->kodi_network_is_host_on_lan = &kodi_network_is_host_on_lan;
  api->kodi_network_is_local_host = &kodi_network_is_local_host;
  api->kodi_network_url_encode = &kodi_network_url_encode;
  api->kodi_network_wake_on_lan = &kodi_network_wake_on_lan;

  // addon-instance/audioencoder.h
  api->kodi_addon_audioencoder_seek = &kodi_addon_audioencoder_seek;
  api->kodi_addon_audioencoder_write = &kodi_addon_audioencoder_write;

  // addon-instance/game.h
  api->kodi_addon_game_add_stream_data = &kodi_addon_game_add_stream_data;
  api->kodi_addon_game_close = &kodi_addon_game_close;
  api->kodi_addon_game_close_stream = &kodi_addon_game_close_stream;
  api->kodi_addon_game_get_stream_buffer = &kodi_addon_game_get_stream_buffer;
  api->kodi_addon_game_hw_get_proc_address = &kodi_addon_game_hw_get_proc_address;
  api->kodi_addon_game_input_event = &kodi_addon_game_input_event;
  api->kodi_addon_game_open_stream = &kodi_addon_game_open_stream;
  api->kodi_addon_game_release_stream_buffer = &kodi_addon_game_release_stream_buffer;

  // addon-instance/inputstream.h
  api->kodi_addon_inputstream_allocate_demux_packet = &kodi_addon_inputstream_allocate_demux_packet;
  api->kodi_addon_inputstream_allocate_encrypted_demux_packet =
      &kodi_addon_inputstream_allocate_encrypted_demux_packet;
  api->kodi_addon_inputstream_free_demux_packet = &kodi_addon_inputstream_free_demux_packet;

  // addon-instance/peripheral.h
  api->kodi_addon_peripheral_feature_count = &kodi_addon_peripheral_feature_count;
  api->kodi_addon_peripheral_feature_type = &kodi_addon_peripheral_feature_type;
  api->kodi_addon_peripheral_refresh_button_maps = &kodi_addon_peripheral_refresh_button_maps;
  api->kodi_addon_peripheral_trigger_scan = &kodi_addon_peripheral_trigger_scan;

  // addon-instance/pvr.h
  api->kodi_addon_pvr_add_menu_hook = &kodi_addon_pvr_add_menu_hook;
  api->kodi_addon_pvr_connection_state_change = &kodi_addon_pvr_connection_state_change;
  api->kodi_addon_pvr_epg_event_state_change = &kodi_addon_pvr_epg_event_state_change;
  api->kodi_addon_pvr_get_epg_max_future_days = &kodi_addon_pvr_get_epg_max_future_days;
  api->kodi_addon_pvr_get_epg_max_past_days = &kodi_addon_pvr_get_epg_max_past_days;
  api->kodi_addon_pvr_recording_notification = &kodi_addon_pvr_recording_notification;
  api->kodi_addon_pvr_transfer_channel_entry = &kodi_addon_pvr_transfer_channel_entry;
  api->kodi_addon_pvr_transfer_channel_group = &kodi_addon_pvr_transfer_channel_group;
  api->kodi_addon_pvr_transfer_channel_group_member = &kodi_addon_pvr_transfer_channel_group_member;
  api->kodi_addon_pvr_transfer_epg_entry = &kodi_addon_pvr_transfer_epg_entry;
  api->kodi_addon_pvr_transfer_provider_entry = &kodi_addon_pvr_transfer_provider_entry;
  api->kodi_addon_pvr_transfer_recording_entry = &kodi_addon_pvr_transfer_recording_entry;
  api->kodi_addon_pvr_transfer_timer_entry = &kodi_addon_pvr_transfer_timer_entry;
  api->kodi_addon_pvr_trigger_channel_groups_update = &kodi_addon_pvr_trigger_channel_groups_update;
  api->kodi_addon_pvr_trigger_channel_update = &kodi_addon_pvr_trigger_channel_update;
  api->kodi_addon_pvr_trigger_epg_update = &kodi_addon_pvr_trigger_epg_update;
  api->kodi_addon_pvr_trigger_providers_update = &kodi_addon_pvr_trigger_providers_update;
  api->kodi_addon_pvr_trigger_recording_update = &kodi_addon_pvr_trigger_recording_update;
  api->kodi_addon_pvr_trigger_timer_update = &kodi_addon_pvr_trigger_timer_update;

  // addon-instance/screensaver.h
  api->kodi_addon_screensaver_get_properties = &kodi_addon_screensaver_get_properties;

  // addon-instance/vfs.h
  api->kodi_addon_vfs_get_dir_cb__get_keyboard_input =
      &kodi_addon_vfs_get_dir_cb__get_keyboard_input;
  api->kodi_addon_vfs_get_dir_cb__require_authentication =
      &kodi_addon_vfs_get_dir_cb__require_authentication;
  api->kodi_addon_vfs_get_dir_cb__set_error_dialog = &kodi_addon_vfs_get_dir_cb__set_error_dialog;

  // addon-instance/videocodec.h
  api->kodi_addon_videocodec_get_frame_buffer = &kodi_addon_videocodec_get_frame_buffer;
  api->kodi_addon_videocodec_release_frame_buffer = &kodi_addon_videocodec_release_frame_buffer;

  // addon-instance/visualization.h
  api->kodi_addon_visualization_clear_presets = &kodi_addon_visualization_clear_presets;
  api->kodi_addon_visualization_get_properties = &kodi_addon_visualization_get_properties;
  api->kodi_addon_visualization_transfer_preset = &kodi_addon_visualization_transfer_preset;

  // addon-instance/web.h
  api->kodi_addon_web_control_request_open_site_in_new_tab =
      &kodi_addon_web_control_request_open_site_in_new_tab;
  api->kodi_addon_web_control_set_control_ready = &kodi_addon_web_control_set_control_ready;
  api->kodi_addon_web_control_set_fullscreen = &kodi_addon_web_control_set_fullscreen;
  api->kodi_addon_web_control_set_icon_url = &kodi_addon_web_control_set_icon_url;
  api->kodi_addon_web_control_set_loading_state = &kodi_addon_web_control_set_loading_state;
  api->kodi_addon_web_control_set_opened_address = &kodi_addon_web_control_set_opened_address;
  api->kodi_addon_web_control_set_opened_title = &kodi_addon_web_control_set_opened_title;
  api->kodi_addon_web_control_set_status_message = &kodi_addon_web_control_set_status_message;
  api->kodi_addon_web_control_set_tooltip = &kodi_addon_web_control_set_tooltip;
  api->kodi_addon_web_inhibit_screensaver = &kodi_addon_web_inhibit_screensaver;
  api->kodi_addon_web_inhibit_shutdown = &kodi_addon_web_inhibit_shutdown;
  api->kodi_addon_web_is_muted = &kodi_addon_web_is_muted;

  // gui/general.h
  api->kodi_gui_get_current_window_dialog_id = &kodi_gui_get_current_window_dialog_id;
  api->kodi_gui_get_current_window_id = &kodi_gui_get_current_window_id;
  api->kodi_gui_get_hw_context = &kodi_gui_get_hw_context;
  api->kodi_gui_get_screen_height = &kodi_gui_get_screen_height;
  api->kodi_gui_get_screen_width = &kodi_gui_get_screen_width;
  api->kodi_gui_get_video_resolution = &kodi_gui_get_video_resolution;
  api->kodi_gui_lock = &kodi_gui_lock;
  api->kodi_gui_unlock = &kodi_gui_unlock;

  // gui/list_item.h
  api->kodi_gui_listitem_create = &kodi_gui_listitem_create;
  api->kodi_gui_listitem_destroy = &kodi_gui_listitem_destroy;
  api->kodi_gui_listitem_get_art = &kodi_gui_listitem_get_art;
  api->kodi_gui_listitem_get_label = &kodi_gui_listitem_get_label;
  api->kodi_gui_listitem_get_label2 = &kodi_gui_listitem_get_label2;
  api->kodi_gui_listitem_get_path = &kodi_gui_listitem_get_path;
  api->kodi_gui_listitem_get_property = &kodi_gui_listitem_get_property;
  api->kodi_gui_listitem_is_selected = &kodi_gui_listitem_is_selected;
  api->kodi_gui_listitem_select = &kodi_gui_listitem_select;
  api->kodi_gui_listitem_set_art = &kodi_gui_listitem_set_art;
  api->kodi_gui_listitem_set_label = &kodi_gui_listitem_set_label;
  api->kodi_gui_listitem_set_label2 = &kodi_gui_listitem_set_label2;
  api->kodi_gui_listitem_set_path = &kodi_gui_listitem_set_path;
  api->kodi_gui_listitem_set_property = &kodi_gui_listitem_set_property;

  // gui/window.h
  api->kodi_gui_window_add_list_item = &kodi_gui_window_add_list_item;
  api->kodi_gui_window_clear_item_list = &kodi_gui_window_clear_item_list;
  api->kodi_gui_window_clear_properties = &kodi_gui_window_clear_properties;
  api->kodi_gui_window_clear_property = &kodi_gui_window_clear_property;
  api->kodi_gui_window_close = &kodi_gui_window_close;
  api->kodi_gui_window_create = &kodi_gui_window_create;
  api->kodi_gui_window_destroy = &kodi_gui_window_destroy;
  api->kodi_gui_window_do_modal = &kodi_gui_window_do_modal;
  api->kodi_gui_window_get_control_button = &kodi_gui_window_get_control_button;
  api->kodi_gui_window_get_control_edit = &kodi_gui_window_get_control_edit;
  api->kodi_gui_window_get_control_fade_label = &kodi_gui_window_get_control_fade_label;
  api->kodi_gui_window_get_control_image = &kodi_gui_window_get_control_image;
  api->kodi_gui_window_get_control_label = &kodi_gui_window_get_control_label;
  api->kodi_gui_window_get_control_progress = &kodi_gui_window_get_control_progress;
  api->kodi_gui_window_get_control_radio_button = &kodi_gui_window_get_control_radio_button;
  api->kodi_gui_window_get_control_render_addon = &kodi_gui_window_get_control_render_addon;
  api->kodi_gui_window_get_control_settings_slider = &kodi_gui_window_get_control_settings_slider;
  api->kodi_gui_window_get_control_slider = &kodi_gui_window_get_control_slider;
  api->kodi_gui_window_get_control_spin = &kodi_gui_window_get_control_spin;
  api->kodi_gui_window_get_control_text_box = &kodi_gui_window_get_control_text_box;
  api->kodi_gui_window_get_current_container_id = &kodi_gui_window_get_current_container_id;
  api->kodi_gui_window_get_current_list_position = &kodi_gui_window_get_current_list_position;
  api->kodi_gui_window_get_focus_id = &kodi_gui_window_get_focus_id;
  api->kodi_gui_window_get_list_item = &kodi_gui_window_get_list_item;
  api->kodi_gui_window_get_list_size = &kodi_gui_window_get_list_size;
  api->kodi_gui_window_get_property = &kodi_gui_window_get_property;
  api->kodi_gui_window_get_property_bool = &kodi_gui_window_get_property_bool;
  api->kodi_gui_window_get_property_double = &kodi_gui_window_get_property_double;
  api->kodi_gui_window_get_property_int = &kodi_gui_window_get_property_int;
  api->kodi_gui_window_mark_dirty_region = &kodi_gui_window_mark_dirty_region;
  api->kodi_gui_window_remove_list_item = &kodi_gui_window_remove_list_item;
  api->kodi_gui_window_remove_list_item_from_position =
      &kodi_gui_window_remove_list_item_from_position;
  api->kodi_gui_window_set_container_content = &kodi_gui_window_set_container_content;
  api->kodi_gui_window_set_container_property = &kodi_gui_window_set_container_property;
  api->kodi_gui_window_set_control_label = &kodi_gui_window_set_control_label;
  api->kodi_gui_window_set_control_selected = &kodi_gui_window_set_control_selected;
  api->kodi_gui_window_set_control_visible = &kodi_gui_window_set_control_visible;
  api->kodi_gui_window_set_current_list_position = &kodi_gui_window_set_current_list_position;
  api->kodi_gui_window_set_focus_id = &kodi_gui_window_set_focus_id;
  api->kodi_gui_window_set_property = &kodi_gui_window_set_property;
  api->kodi_gui_window_set_property_bool = &kodi_gui_window_set_property_bool;
  api->kodi_gui_window_set_property_double = &kodi_gui_window_set_property_double;
  api->kodi_gui_window_set_property_int = &kodi_gui_window_set_property_int;
  api->kodi_gui_window_show = &kodi_gui_window_show;

  // gui/controls/button.h
  api->kodi_gui_controls_button_get_label = &kodi_gui_controls_button_get_label;
  api->kodi_gui_controls_button_get_label2 = &kodi_gui_controls_button_get_label2;
  api->kodi_gui_controls_button_set_enabled = &kodi_gui_controls_button_set_enabled;
  api->kodi_gui_controls_button_set_label = &kodi_gui_controls_button_set_label;
  api->kodi_gui_controls_button_set_label2 = &kodi_gui_controls_button_set_label2;
  api->kodi_gui_controls_button_set_visible = &kodi_gui_controls_button_set_visible;

  // gui/controls/edit.h
  api->kodi_gui_controls_edit_get_cursor_position = &kodi_gui_controls_edit_get_cursor_position;
  api->kodi_gui_controls_edit_get_label = &kodi_gui_controls_edit_get_label;
  api->kodi_gui_controls_edit_get_text = &kodi_gui_controls_edit_get_text;
  api->kodi_gui_controls_edit_set_cursor_position = &kodi_gui_controls_edit_set_cursor_position;
  api->kodi_gui_controls_edit_set_enabled = &kodi_gui_controls_edit_set_enabled;
  api->kodi_gui_controls_edit_set_input_type = &kodi_gui_controls_edit_set_input_type;
  api->kodi_gui_controls_edit_set_label = &kodi_gui_controls_edit_set_label;
  api->kodi_gui_controls_edit_set_text = &kodi_gui_controls_edit_set_text;
  api->kodi_gui_controls_edit_set_visible = &kodi_gui_controls_edit_set_visible;

  // gui/controls/fade_label.h
  api->kodi_gui_controls_fade_label_add_label = &kodi_gui_controls_fade_label_add_label;
  api->kodi_gui_controls_fade_label_get_label = &kodi_gui_controls_fade_label_get_label;
  api->kodi_gui_controls_fade_label_reset = &kodi_gui_controls_fade_label_reset;
  api->kodi_gui_controls_fade_label_set_scrolling = &kodi_gui_controls_fade_label_set_scrolling;
  api->kodi_gui_controls_fade_label_set_visible = &kodi_gui_controls_fade_label_set_visible;

  // gui/controls/image.h
  api->kodi_gui_controls_image_set_color_diffuse = &kodi_gui_controls_image_set_color_diffuse;
  api->kodi_gui_controls_image_set_filename = &kodi_gui_controls_image_set_filename;
  api->kodi_gui_controls_image_set_visible = &kodi_gui_controls_image_set_visible;

  // gui/controls/label.h
  api->kodi_gui_controls_label_get_label = &kodi_gui_controls_label_get_label;
  api->kodi_gui_controls_label_set_label = &kodi_gui_controls_label_set_label;
  api->kodi_gui_controls_label_set_visible = &kodi_gui_controls_label_set_visible;

  // gui/controls/progress.h
  api->kodi_gui_controls_progress_get_percentage = &kodi_gui_controls_progress_get_percentage;
  api->kodi_gui_controls_progress_set_percentage = &kodi_gui_controls_progress_set_percentage;
  api->kodi_gui_controls_progress_set_visible = &kodi_gui_controls_progress_set_visible;

  // gui/controls/radio_button.h
  api->kodi_gui_controls_radio_button_get_label = &kodi_gui_controls_radio_button_get_label;
  api->kodi_gui_controls_radio_button_is_selected = &kodi_gui_controls_radio_button_is_selected;
  api->kodi_gui_controls_radio_button_set_enabled = &kodi_gui_controls_radio_button_set_enabled;
  api->kodi_gui_controls_radio_button_set_label = &kodi_gui_controls_radio_button_set_label;
  api->kodi_gui_controls_radio_button_set_selected = &kodi_gui_controls_radio_button_set_selected;
  api->kodi_gui_controls_radio_button_set_visible = &kodi_gui_controls_radio_button_set_visible;

  // gui/controls/rendering.h
  api->kodi_gui_controls_radio_rendering_destroy = &kodi_gui_controls_radio_rendering_destroy;

  // gui/controls/settings_slider.h
  api->kodi_gui_controls_settings_slider_get_float_value =
      &kodi_gui_controls_settings_slider_get_float_value;
  api->kodi_gui_controls_settings_slider_get_int_value =
      &kodi_gui_controls_settings_slider_get_int_value;
  api->kodi_gui_controls_settings_slider_get_percentage =
      &kodi_gui_controls_settings_slider_get_percentage;
  api->kodi_gui_controls_settings_slider_reset = &kodi_gui_controls_settings_slider_reset;
  api->kodi_gui_controls_settings_slider_set_enabled =
      &kodi_gui_controls_settings_slider_set_enabled;
  api->kodi_gui_controls_settings_slider_set_float_interval =
      &kodi_gui_controls_settings_slider_set_float_interval;
  api->kodi_gui_controls_settings_slider_set_float_range =
      &kodi_gui_controls_settings_slider_set_float_range;
  api->kodi_gui_controls_settings_slider_set_float_value =
      &kodi_gui_controls_settings_slider_set_float_value;
  api->kodi_gui_controls_settings_slider_set_int_interval =
      &kodi_gui_controls_settings_slider_set_int_interval;
  api->kodi_gui_controls_settings_slider_set_int_range =
      &kodi_gui_controls_settings_slider_set_int_range;
  api->kodi_gui_controls_settings_slider_set_int_value =
      &kodi_gui_controls_settings_slider_set_int_value;
  api->kodi_gui_controls_settings_slider_set_percentage =
      &kodi_gui_controls_settings_slider_set_percentage;
  api->kodi_gui_controls_settings_slider_set_text = &kodi_gui_controls_settings_slider_set_text;
  api->kodi_gui_controls_settings_slider_set_visible =
      &kodi_gui_controls_settings_slider_set_visible;

  // gui/controls/slider.h
  api->kodi_gui_controls_slider_get_description = &kodi_gui_controls_slider_get_description;
  api->kodi_gui_controls_slider_get_float_value = &kodi_gui_controls_slider_get_float_value;
  api->kodi_gui_controls_slider_get_int_value = &kodi_gui_controls_slider_get_int_value;
  api->kodi_gui_controls_slider_get_percentage = &kodi_gui_controls_slider_get_percentage;
  api->kodi_gui_controls_slider_reset = &kodi_gui_controls_slider_reset;
  api->kodi_gui_controls_slider_set_enabled = &kodi_gui_controls_slider_set_enabled;
  api->kodi_gui_controls_slider_set_float_interval = &kodi_gui_controls_slider_set_float_interval;
  api->kodi_gui_controls_slider_set_float_range = &kodi_gui_controls_slider_set_float_range;
  api->kodi_gui_controls_slider_set_float_value = &kodi_gui_controls_slider_set_float_value;
  api->kodi_gui_controls_slider_set_int_interval = &kodi_gui_controls_slider_set_int_interval;
  api->kodi_gui_controls_slider_set_int_range = &kodi_gui_controls_slider_set_int_range;
  api->kodi_gui_controls_slider_set_int_value = &kodi_gui_controls_slider_set_int_value;
  api->kodi_gui_controls_slider_set_percentage = &kodi_gui_controls_slider_set_percentage;
  api->kodi_gui_controls_slider_set_visible = &kodi_gui_controls_slider_set_visible;

  // gui/controls/spin.h
  api->kodi_gui_controls_spin_add_int_label = &kodi_gui_controls_spin_add_int_label;
  api->kodi_gui_controls_spin_add_string_label = &kodi_gui_controls_spin_add_string_label;
  api->kodi_gui_controls_spin_get_float_value = &kodi_gui_controls_spin_get_float_value;
  api->kodi_gui_controls_spin_get_int_value = &kodi_gui_controls_spin_get_int_value;
  api->kodi_gui_controls_spin_get_string_value = &kodi_gui_controls_spin_get_string_value;
  api->kodi_gui_controls_spin_reset = &kodi_gui_controls_spin_reset;
  api->kodi_gui_controls_spin_set_enabled = &kodi_gui_controls_spin_set_enabled;
  api->kodi_gui_controls_spin_set_float_interval = &kodi_gui_controls_spin_set_float_interval;
  api->kodi_gui_controls_spin_set_float_range = &kodi_gui_controls_spin_set_float_range;
  api->kodi_gui_controls_spin_set_float_value = &kodi_gui_controls_spin_set_float_value;
  api->kodi_gui_controls_spin_set_int_range = &kodi_gui_controls_spin_set_int_range;
  api->kodi_gui_controls_spin_set_int_value = &kodi_gui_controls_spin_set_int_value;
  api->kodi_gui_controls_spin_set_string_value = &kodi_gui_controls_spin_set_string_value;
  api->kodi_gui_controls_spin_set_text = &kodi_gui_controls_spin_set_text;
  api->kodi_gui_controls_spin_set_type = &kodi_gui_controls_spin_set_type;
  api->kodi_gui_controls_spin_set_visible = &kodi_gui_controls_spin_set_visible;

  // gui/controls/text_box.h
  api->kodi_gui_controls_text_box_get_text = &kodi_gui_controls_text_box_get_text;
  api->kodi_gui_controls_text_box_reset = &kodi_gui_controls_text_box_reset;
  api->kodi_gui_controls_text_box_scroll = &kodi_gui_controls_text_box_scroll;
  api->kodi_gui_controls_text_box_set_auto_scrolling =
      &kodi_gui_controls_text_box_set_auto_scrolling;
  api->kodi_gui_controls_text_box_set_text = &kodi_gui_controls_text_box_set_text;
  api->kodi_gui_controls_text_box_set_visible = &kodi_gui_controls_text_box_set_visible;

  // gui/dialogs/context_menu.h
  api->kodi_gui_dialogs_context_menu_open = &kodi_gui_dialogs_context_menu_open;

  // gui/dialogs/extended_progress.h
  api->kodi_gui_dialogs_extended_progress_delete_dialog =
      &kodi_gui_dialogs_extended_progress_delete_dialog;
  api->kodi_gui_dialogs_extended_progress_get_percentage =
      &kodi_gui_dialogs_extended_progress_get_percentage;
  api->kodi_gui_dialogs_extended_progress_get_text = &kodi_gui_dialogs_extended_progress_get_text;
  api->kodi_gui_dialogs_extended_progress_get_title = &kodi_gui_dialogs_extended_progress_get_title;
  api->kodi_gui_dialogs_extended_progress_is_finished =
      &kodi_gui_dialogs_extended_progress_is_finished;
  api->kodi_gui_dialogs_extended_progress_mark_finished =
      &kodi_gui_dialogs_extended_progress_mark_finished;
  api->kodi_gui_dialogs_extended_progress_new_dialog =
      &kodi_gui_dialogs_extended_progress_new_dialog;
  api->kodi_gui_dialogs_extended_progress_set_percentage =
      &kodi_gui_dialogs_extended_progress_set_percentage;
  api->kodi_gui_dialogs_extended_progress_set_progress =
      &kodi_gui_dialogs_extended_progress_set_progress;
  api->kodi_gui_dialogs_extended_progress_set_text = &kodi_gui_dialogs_extended_progress_set_text;
  api->kodi_gui_dialogs_extended_progress_set_title = &kodi_gui_dialogs_extended_progress_set_title;

  // gui/dialogs/filebrowser.h
  api->kodi_gui_dialogs_file_browser_clear_file_list =
      &kodi_gui_dialogs_file_browser_clear_file_list;
  api->kodi_gui_dialogs_file_browser_show_and_get_directory =
      &kodi_gui_dialogs_file_browser_show_and_get_directory;
  api->kodi_gui_dialogs_file_browser_show_and_get_file =
      &kodi_gui_dialogs_file_browser_show_and_get_file;
  api->kodi_gui_dialogs_file_browser_show_and_get_file_from_dir =
      &kodi_gui_dialogs_file_browser_show_and_get_file_from_dir;
  api->kodi_gui_dialogs_file_browser_show_and_get_file_list =
      &kodi_gui_dialogs_file_browser_show_and_get_file_list;
  api->kodi_gui_dialogs_file_browser_show_and_get_image =
      &kodi_gui_dialogs_file_browser_show_and_get_image;
  api->kodi_gui_dialogs_file_browser_show_and_get_image_list =
      &kodi_gui_dialogs_file_browser_show_and_get_image_list;
  api->kodi_gui_dialogs_file_browser_show_and_get_source =
      &kodi_gui_dialogs_file_browser_show_and_get_source;

  // gui/dialogs/keyboard.h
  api->kodi_gui_dialogs_keyboard_is_keyboard_activated =
      &kodi_gui_dialogs_keyboard_is_keyboard_activated;
  api->kodi_gui_dialogs_keyboard_send_text_to_active_keyboard =
      &kodi_gui_dialogs_keyboard_send_text_to_active_keyboard;
  api->kodi_gui_dialogs_keyboard_show_and_get_filter =
      &kodi_gui_dialogs_keyboard_show_and_get_filter;
  api->kodi_gui_dialogs_keyboard_show_and_get_input = &kodi_gui_dialogs_keyboard_show_and_get_input;
  api->kodi_gui_dialogs_keyboard_show_and_get_input_with_head =
      &kodi_gui_dialogs_keyboard_show_and_get_input_with_head;
  api->kodi_gui_dialogs_keyboard_show_and_get_new_password =
      &kodi_gui_dialogs_keyboard_show_and_get_new_password;
  api->kodi_gui_dialogs_keyboard_show_and_get_new_password_with_head =
      &kodi_gui_dialogs_keyboard_show_and_get_new_password_with_head;
  api->kodi_gui_dialogs_keyboard_show_and_verify_new_password =
      &kodi_gui_dialogs_keyboard_show_and_verify_new_password;
  api->kodi_gui_dialogs_keyboard_show_and_verify_new_password_with_head =
      &kodi_gui_dialogs_keyboard_show_and_verify_new_password_with_head;
  api->kodi_gui_dialogs_keyboard_show_and_verify_password =
      &kodi_gui_dialogs_keyboard_show_and_verify_password;

  // gui/dialogs/numeric.h
  api->kodi_gui_dialogs_numeric_show_and_get_date = &kodi_gui_dialogs_numeric_show_and_get_date;
  api->kodi_gui_dialogs_numeric_show_and_get_ip_address =
      &kodi_gui_dialogs_numeric_show_and_get_ip_address;
  api->kodi_gui_dialogs_numeric_show_and_get_number = &kodi_gui_dialogs_numeric_show_and_get_number;
  api->kodi_gui_dialogs_numeric_show_and_get_seconds =
      &kodi_gui_dialogs_numeric_show_and_get_seconds;
  api->kodi_gui_dialogs_numeric_show_and_get_time = &kodi_gui_dialogs_numeric_show_and_get_time;
  api->kodi_gui_dialogs_numeric_show_and_verify_input =
      &kodi_gui_dialogs_numeric_show_and_verify_input;
  api->kodi_gui_dialogs_numeric_show_and_verify_new_password =
      &kodi_gui_dialogs_numeric_show_and_verify_new_password;
  api->kodi_gui_dialogs_numeric_show_and_verify_password =
      &kodi_gui_dialogs_numeric_show_and_verify_password;

  // gui/dialogs/ok.h
  api->kodi_gui_dialogs_ok_show_and_get_input_line_text =
      &kodi_gui_dialogs_ok_show_and_get_input_line_text;
  api->kodi_gui_dialogs_ok_show_and_get_input_single_text =
      &kodi_gui_dialogs_ok_show_and_get_input_single_text;

  // gui/dialogs/progress.h
  api->kodi_gui_dialogs_progress_abort = &kodi_gui_dialogs_progress_abort;
  api->kodi_gui_dialogs_progress_delete_dialog = &kodi_gui_dialogs_progress_delete_dialog;
  api->kodi_gui_dialogs_progress_get_percentage = &kodi_gui_dialogs_progress_get_percentage;
  api->kodi_gui_dialogs_progress_is_canceled = &kodi_gui_dialogs_progress_is_canceled;
  api->kodi_gui_dialogs_progress_new_dialog = &kodi_gui_dialogs_progress_new_dialog;
  api->kodi_gui_dialogs_progress_open = &kodi_gui_dialogs_progress_open;
  api->kodi_gui_dialogs_progress_set_can_cancel = &kodi_gui_dialogs_progress_set_can_cancel;
  api->kodi_gui_dialogs_progress_set_heading = &kodi_gui_dialogs_progress_set_heading;
  api->kodi_gui_dialogs_progress_set_line = &kodi_gui_dialogs_progress_set_line;
  api->kodi_gui_dialogs_progress_set_percentage = &kodi_gui_dialogs_progress_set_percentage;
  api->kodi_gui_dialogs_progress_set_progress_advance =
      &kodi_gui_dialogs_progress_set_progress_advance;
  api->kodi_gui_dialogs_progress_set_progress_max = &kodi_gui_dialogs_progress_set_progress_max;
  api->kodi_gui_dialogs_progress_show_progress_bar = &kodi_gui_dialogs_progress_show_progress_bar;

  // gui/dialogs/select.h
  api->kodi_gui_dialogs_select_open = &kodi_gui_dialogs_select_open;
  api->kodi_gui_dialogs_select_open_multi_select = &kodi_gui_dialogs_select_open_multi_select;

  // gui/dialogs/text_viewer.h
  api->kodi_gui_dialogs_text_viewer_show = &kodi_gui_dialogs_text_viewer_show;

  // gui/dialogs/yes_no.h
  api->kodi_gui_dialogs_yesno_show_and_get_input_line_button_text =
      &kodi_gui_dialogs_yesno_show_and_get_input_line_button_text;
  api->kodi_gui_dialogs_yesno_show_and_get_input_line_text =
      &kodi_gui_dialogs_yesno_show_and_get_input_line_text;
  api->kodi_gui_dialogs_yesno_show_and_get_input_single_text =
      &kodi_gui_dialogs_yesno_show_and_get_input_single_text;

  return true;
}
