/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/* File autogenerated, see xbmc/addons/kodi-dev-kit/tools/code-generator/code_generator.py */

#pragma once

#include "addons/interface/IMsgHdl.h"

// API include
#include "addons/kodi-dev-kit/include/kodi/c-api/filesystem.h"

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1_END>---*/

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

struct directFuncToAddon_filesystem_h;
struct directFuncToKodi_filesystem_h;

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_2>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_2_END>---*/

class CHdl_kodi_filesystem_h : public IMsgHdl
{
public:
  CHdl_kodi_filesystem_h(CInterface& interface) : IMsgHdl(interface) {}

  void InitDirect(directFuncToKodi_filesystem_h* ifcToKodi,
                  directFuncToAddon_filesystem_h* ifcToAddon);

  // To handle callbacks from addon by shared sandbox API (static functions below in private becomes used)
  bool HandleMessage(int funcGroup,
                     int func,
                     const msgpack::unpacked& in,
                     msgpack::sbuffer& out) override;

  /* Code below becomes on auto generation reused, hand edited parts should included here */
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3>---*/
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3_END>---*/

  // Function calls from Kodi to addon

private:
  /* Code below becomes on auto generation reused, hand edited parts should included here */
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_4>---*/
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_4_END>---*/

  // Callbacks from addon to Kodi
  static bool kodi_vfs_can_open_directory_v1(void* thisClassHdl,
                                             const char* url); // Added with API 1

  static bool kodi_vfs_create_directory_v1(void* thisClassHdl,
                                           const char* path); // Added with API 1

  static bool kodi_vfs_remove_directory_v1(void* thisClassHdl,
                                           const char* path); // Added with API 1

  static bool kodi_vfs_remove_directory_recursive_v1(void* thisClassHdl,
                                                     const char* path); // Added with API 1

  static bool kodi_vfs_directory_exists_v1(void* thisClassHdl,
                                           const char* path); // Added with API 1

  static bool kodi_vfs_get_directory_v1(void* thisClassHdl,
                                        const char* path,
                                        const char* mask,
                                        struct VFS_DIR_ENTRY** items,
                                        size_t* num_items); // Added with API 1

  static bool kodi_vfs_file_exists_v1(void* thisClassHdl,
                                      const char* filename,
                                      bool useCache); // Added with API 1

  static bool kodi_vfs_stat_file_v1(void* thisClassHdl,
                                    const char* filename,
                                    struct VFS_STAT_STRUCTURE* buffer); // Added with API 1

  static bool kodi_vfs_delete_file_v1(void* thisClassHdl, const char* filename); // Added with API 1

  static bool kodi_vfs_rename_file_v1(void* thisClassHdl,
                                      const char* filename,
                                      const char* newFileName); // Added with API 1

  static bool kodi_vfs_copy_file_v1(void* thisClassHdl,
                                    const char* filename,
                                    const char* dest); // Added with API 1

  static char* kodi_vfs_get_file_md5_v1(void* thisClassHdl,
                                        const char* filename); // Added with API 1

  static char* kodi_vfs_get_cache_thumb_name_v1(void* thisClassHdl,
                                                const char* filename); // Added with API 1

  static char* kodi_vfs_make_legal_filename_v1(void* thisClassHdl,
                                               const char* filename); // Added with API 1

  static char* kodi_vfs_make_legal_path_v1(void* thisClassHdl,
                                           const char* path); // Added with API 1

  static char* kodi_vfs_translate_special_protocol_v1(void* thisClassHdl,
                                                      const char* strSource); // Added with API 1

  static bool kodi_vfs_is_internet_stream_v1(void* thisClassHdl,
                                             const char* path,
                                             bool strictCheck); // Added with API 1

  static bool kodi_vfs_is_on_lan_v1(void* thisClassHdl, const char* path); // Added with API 1

  static bool kodi_vfs_is_remote_v1(void* thisClassHdl, const char* path); // Added with API 1

  static bool kodi_vfs_is_local_v1(void* thisClassHdl, const char* path); // Added with API 1

  static bool kodi_vfs_is_url_v1(void* thisClassHdl, const char* path); // Added with API 1

  static bool kodi_vfs_get_mime_type_v1(void* thisClassHdl,
                                        const char* url,
                                        char** content,
                                        const char* useragent); // Added with API 1

  static bool kodi_vfs_get_content_type_v1(void* thisClassHdl,
                                           const char* url,
                                           char** content,
                                           const char* useragent); // Added with API 1

  static bool kodi_vfs_get_cookies_v1(void* thisClassHdl,
                                      const char* url,
                                      char** cookies); // Added with API 1

  static bool kodi_vfs_get_disk_space_v1(void* thisClassHdl,
                                         const char* path,
                                         uint64_t* capacity,
                                         uint64_t* free,
                                         uint64_t* available); // Added with API 1

  static KODI_HTTP_HEADER_HDL kodi_vfs_http_header_open_v1(void* thisClassHdl,
                                                           const char* url); // Added with API 1

  static void kodi_vfs_http_header_close_v1(void* thisClassHdl,
                                            KODI_HTTP_HEADER_HDL hdl); // Added with API 1

  static char* kodi_vfs_http_header_get_value_v1(void* thisClassHdl,
                                                 KODI_HTTP_HEADER_HDL hdl,
                                                 const char* param); // Added with API 1

  static char** kodi_vfs_http_header_get_values_v1(void* thisClassHdl,
                                                   KODI_HTTP_HEADER_HDL hdl,
                                                   const char* param,
                                                   size_t* length); // Added with API 1

  static char* kodi_vfs_http_header_get_header_v1(void* thisClassHdl,
                                                  KODI_HTTP_HEADER_HDL hdl); // Added with API 1

  static char* kodi_vfs_http_header_get_mime_type_v1(void* thisClassHdl,
                                                     KODI_HTTP_HEADER_HDL hdl); // Added with API 1

  static char* kodi_vfs_http_header_get_charset_v1(void* thisClassHdl,
                                                   KODI_HTTP_HEADER_HDL hdl); // Added with API 1

  static char* kodi_vfs_http_header_get_proto_line_v1(void* thisClassHdl,
                                                      KODI_HTTP_HEADER_HDL hdl); // Added with API 1

  static KODI_FILE_HDL kodi_vfs_file_open_v1(void* thisClassHdl,
                                             const char* filename,
                                             unsigned int flags); // Added with API 1

  static KODI_FILE_HDL kodi_vfs_file_open_for_write_v1(void* thisClassHdl,
                                                       const char* filename,
                                                       bool overwrite); // Added with API 1

  static KODI_FILE_HDL kodi_vfs_file_curl_create_v1(void* thisClassHdl,
                                                    const char* url); // Added with API 1

  static bool kodi_vfs_file_curl_add_option_v1(void* thisClassHdl,
                                               KODI_FILE_HDL hdl,
                                               enum CURLOptiontype type,
                                               const char* name,
                                               const char* value); // Added with API 1

  static bool kodi_vfs_file_curl_open_v1(void* thisClassHdl,
                                         KODI_FILE_HDL hdl,
                                         unsigned int flags); // Added with API 1

  static void kodi_vfs_file_close_v1(void* thisClassHdl, KODI_FILE_HDL hdl); // Added with API 1

  static ssize_t kodi_vfs_file_read_v1(void* thisClassHdl,
                                       KODI_FILE_HDL hdl,
                                       uint8_t* ptr,
                                       size_t size); // Added with API 1

  static bool kodi_vfs_file_read_line_v1(void* thisClassHdl,
                                         KODI_FILE_HDL hdl,
                                         char* szLine,
                                         size_t lineLength); // Added with API 1

  static ssize_t kodi_vfs_file_write_v1(void* thisClassHdl,
                                        KODI_FILE_HDL hdl,
                                        const uint8_t* ptr,
                                        size_t size); // Added with API 1

  static void kodi_vfs_file_flush_v1(void* thisClassHdl, KODI_FILE_HDL hdl); // Added with API 1

  static int64_t kodi_vfs_file_seek_v1(void* thisClassHdl,
                                       KODI_FILE_HDL hdl,
                                       int64_t position,
                                       int whence); // Added with API 1

  static int kodi_vfs_file_truncate_v1(void* thisClassHdl,
                                       KODI_FILE_HDL hdl,
                                       int64_t size); // Added with API 1

  static int64_t kodi_vfs_file_get_position_v1(void* thisClassHdl,
                                               KODI_FILE_HDL hdl); // Added with API 1

  static int64_t kodi_vfs_file_get_length_v1(void* thisClassHdl,
                                             KODI_FILE_HDL hdl); // Added with API 1

  static bool kodi_vfs_file_at_end_v1(void* thisClassHdl, KODI_FILE_HDL hdl); // Added with API 1

  static double kodi_vfs_file_get_download_speed_v1(void* thisClassHdl,
                                                    KODI_FILE_HDL hdl); // Added with API 1

  static int kodi_vfs_file_get_chunk_size_v1(void* thisClassHdl,
                                             KODI_FILE_HDL hdl); // Added with API 1

  static bool kodi_vfs_file_io_ctl_get_seek_possible_v1(void* thisClassHdl,
                                                        KODI_FILE_HDL hdl); // Added with API 1

  static bool kodi_vfs_file_io_ctl_get_cache_status_v1(
      void* thisClassHdl, KODI_FILE_HDL hdl, struct VFS_CACHE_STATUS* status); // Added with API 1

  static bool kodi_vfs_file_io_ctl_set_cache_rate_v1(void* thisClassHdl,
                                                     KODI_FILE_HDL hdl,
                                                     uint32_t rate); // Added with API 1

  static bool kodi_vfs_file_io_ctl_set_retry_v1(void* thisClassHdl,
                                                KODI_FILE_HDL hdl,
                                                bool retry); // Added with API 1

  static char* kodi_vfs_file_get_property_value_v1(void* thisClassHdl,
                                                   KODI_FILE_HDL hdl,
                                                   enum FilePropertyTypes type,
                                                   const char* name); // Added with API 1

  static char** kodi_vfs_file_get_property_values_v1(void* thisClassHdl,
                                                     KODI_FILE_HDL hdl,
                                                     enum FilePropertyTypes type,
                                                     const char* name,
                                                     size_t* length); // Added with API 1

  directFuncToAddon_filesystem_h* m_ifcToAddon{nullptr};
};

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
