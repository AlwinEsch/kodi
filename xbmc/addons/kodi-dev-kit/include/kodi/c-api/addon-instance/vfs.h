/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_ADDONINSTANCE_VFS_H
#define C_API_ADDONINSTANCE_VFS_H

#include "../addon_base.h"
#include "../filesystem.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  typedef KODI_ADDON_INSTANCE_HDL KODI_ADDON_VFS_HDL;
  typedef void* KODI_VFS_FILE_HDL;
  typedef void* KODI_CTX_CB_HDL;

  struct KODI_ADDON_VFS_URL
  {
    const char* url;
    const char* domain;
    const char* hostname;
    const char* filename;
    unsigned int port;
    const char* options;
    const char* username;
    const char* password;
    const char* redacted;
    const char* sharename;
    const char* protocol;
  };

  typedef KODI_VFS_FILE_HDL(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_FILE_OPEN_V1)(
      const KODI_ADDON_VFS_HDL hdl, const struct KODI_ADDON_VFS_URL* url);
  typedef KODI_VFS_FILE_HDL(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_FILE_OPEN_FOR_WRITE_V1)(
      const KODI_ADDON_VFS_HDL hdl, const struct KODI_ADDON_VFS_URL* url, bool overwrite);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_FILE_CLOSE_V1)(const KODI_ADDON_VFS_HDL hdl,
                                                                KODI_VFS_FILE_HDL context);
  typedef ssize_t(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_FILE_READ_V1)(const KODI_ADDON_VFS_HDL hdl,
                                                                  KODI_VFS_FILE_HDL context,
                                                                  uint8_t* buffer,
                                                                  size_t buf_size);
  typedef ssize_t(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_FILE_WRITE_V1)(const KODI_ADDON_VFS_HDL hdl,
                                                                   KODI_VFS_FILE_HDL context,
                                                                   const uint8_t* buffer,
                                                                   size_t buf_size);
  typedef int64_t(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_FILE_SEEK_V1)(const KODI_ADDON_VFS_HDL hdl,
                                                                  KODI_VFS_FILE_HDL context,
                                                                  int64_t position,
                                                                  int whence);
  typedef int(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_FILE_TRUNCATE_V1)(const KODI_ADDON_VFS_HDL hdl,
                                                                  KODI_VFS_FILE_HDL context,
                                                                  int64_t size);
  typedef int64_t(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_FILE_GET_LENGTH_V1)(
      const KODI_ADDON_VFS_HDL hdl, KODI_VFS_FILE_HDL context);
  typedef int64_t(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_FILE_GET_POSITION_V1)(
      const KODI_ADDON_VFS_HDL hdl, KODI_VFS_FILE_HDL context);
  typedef int(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_FILE_GET_CHUNK_SIZE_V1)(
      const KODI_ADDON_VFS_HDL hdl, KODI_VFS_FILE_HDL context);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_FILE_IO_CONTROL_GET_SEEK_POSSIBLE_V1)(
      const KODI_ADDON_VFS_HDL hdl, KODI_VFS_FILE_HDL context);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_FILE_IO_CONTROL_GET_CACHE_STATUS_V1)(
      const KODI_ADDON_VFS_HDL hdl, KODI_VFS_FILE_HDL context, struct VFS_CACHE_STATUS* status);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_FILE_IO_CONTROL_SET_CACHE_RATE_V1)(
      const KODI_ADDON_VFS_HDL hdl, KODI_VFS_FILE_HDL context, uint32_t rate);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_FILE_IO_CONTROL_SET_RETRY_V1)(
      const KODI_ADDON_VFS_HDL hdl, KODI_VFS_FILE_HDL context, bool retry);
  typedef int(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_STAT_V1)(const KODI_ADDON_VFS_HDL hdl,
                                                         const struct KODI_ADDON_VFS_URL* url,
                                                         struct VFS_STAT_STRUCTURE* buffer);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_EXISTS_V1)(const KODI_ADDON_VFS_HDL hdl,
                                                            const struct KODI_ADDON_VFS_URL* url);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_CLEAR_OUT_IDLE_V1)(const KODI_ADDON_VFS_HDL hdl);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_DISCONNECT_ALL_V1)(const KODI_ADDON_VFS_HDL hdl);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_DELETE_IT_V1)(
      const KODI_ADDON_VFS_HDL hdl, const struct KODI_ADDON_VFS_URL* url);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_RENAME_V1)(const KODI_ADDON_VFS_HDL hdl,
                                                            const struct KODI_ADDON_VFS_URL* url,
                                                            const struct KODI_ADDON_VFS_URL* url2);

  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_DIRECTORY_EXISTS_V1)(
      const KODI_ADDON_VFS_HDL hdl, const struct KODI_ADDON_VFS_URL* url);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_DIRECTORY_REMOVE_V1)(
      const KODI_ADDON_VFS_HDL hdl, const struct KODI_ADDON_VFS_URL* url);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_DIRECTORY_CREATE_V1)(
      const KODI_ADDON_VFS_HDL hdl, const struct KODI_ADDON_VFS_URL* url);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_DIRECTORY_GET_V1)(
      const KODI_ADDON_VFS_HDL hdl,
      const struct KODI_ADDON_VFS_URL* url,
      struct VFS_DIR_ENTRY** entries,
      size_t* num_entries,
      KODI_CTX_CB_HDL ctx_cb_hdl);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_VFS_DIRECTORY_CONTAINS_FILES_V1)(
      const KODI_ADDON_VFS_HDL hdl,
      const struct KODI_ADDON_VFS_URL* url,
      struct VFS_DIR_ENTRY** entries,
      size_t* num_entries,
      char** rootpath);

  struct KODI_ADDON_VFS_FUNC
  {
    PFN_KODI_ADDON_INSTANCE_CREATE_V1 create;
    PFN_KODI_ADDON_INSTANCE_DESTROY_V1 destroy;

    PFN_KODI_ADDON_VFS_CLEAR_OUT_IDLE_V1 clear_out_idle;
    PFN_KODI_ADDON_VFS_DISCONNECT_ALL_V1 disconnect_all;

    PFN_KODI_ADDON_VFS_FILE_OPEN_V1 file_open;
    PFN_KODI_ADDON_VFS_FILE_OPEN_FOR_WRITE_V1 file_open_for_write;
    PFN_KODI_ADDON_VFS_FILE_CLOSE_V1 file_close;
    PFN_KODI_ADDON_VFS_FILE_READ_V1 file_read;
    PFN_KODI_ADDON_VFS_FILE_WRITE_V1 file_write;
    PFN_KODI_ADDON_VFS_FILE_SEEK_V1 file_seek;
    PFN_KODI_ADDON_VFS_FILE_TRUNCATE_V1 file_truncate;
    PFN_KODI_ADDON_VFS_FILE_GET_LENGTH_V1 file_get_length;
    PFN_KODI_ADDON_VFS_FILE_GET_POSITION_V1 file_get_position;
    PFN_KODI_ADDON_VFS_FILE_GET_CHUNK_SIZE_V1 file_get_chunk_size;
    PFN_KODI_ADDON_VFS_FILE_IO_CONTROL_GET_SEEK_POSSIBLE_V1 file_io_control_get_seek_possible;
    PFN_KODI_ADDON_VFS_FILE_IO_CONTROL_GET_CACHE_STATUS_V1 file_io_control_get_cache_status;
    PFN_KODI_ADDON_VFS_FILE_IO_CONTROL_SET_CACHE_RATE_V1 file_io_control_set_cache_rate;
    PFN_KODI_ADDON_VFS_FILE_IO_CONTROL_SET_RETRY_V1 file_io_control_set_retry;

    PFN_KODI_ADDON_VFS_STAT_V1 stat;
    PFN_KODI_ADDON_VFS_EXISTS_V1 exists;
    PFN_KODI_ADDON_VFS_DELETE_IT_V1 delete_it;
    PFN_KODI_ADDON_VFS_RENAME_V1 rename;

    PFN_KODI_ADDON_VFS_DIRECTORY_EXISTS_V1 directory_exists;
    PFN_KODI_ADDON_VFS_DIRECTORY_REMOVE_V1 directory_remove;
    PFN_KODI_ADDON_VFS_DIRECTORY_CREATE_V1 directory_create;
    PFN_KODI_ADDON_VFS_DIRECTORY_GET_V1 directory_get;
    PFN_KODI_ADDON_VFS_DIRECTORY_CONTAINS_FILES_V1 directory_contains_files;
  };

  ATTR_DLL_EXPORT bool kodi_addon_vfs_get_dir_cb__get_keyboard_input(
      KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
      KODI_CTX_CB_HDL ctx_cb_hdl,
      const char* heading,
      char** input,
      bool hidden_input) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_vfs_get_dir_cb__set_error_dialog(
      KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
      KODI_CTX_CB_HDL ctx_cb_hdl,
      const char* heading,
      const char* line1,
      const char* line2,
      const char* line3) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_vfs_get_dir_cb__require_authentication(
      KODI_ADDON_INSTANCE_BACKEND_HDL hdl, KODI_CTX_CB_HDL ctx_cb_hdl, const char* url)
      __INTRODUCED_IN_KODI(1);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_ADDONINSTANCE_VFS_H */
