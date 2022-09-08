/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_FILESYSTEM_H
#define C_API_FILESYSTEM_H

#include "addon_base.h"

#include <time.h>

#ifdef _WIN32 // windows
#ifndef _SSIZE_T_DEFINED
typedef intptr_t ssize_t;
#define _SSIZE_T_DEFINED
#endif // !_SSIZE_T_DEFINED

// Prevent conflicts with Windows macros where have this names.
#ifdef CreateDirectory
#undef CreateDirectory
#endif // CreateDirectory
#ifdef DeleteFile
#undef DeleteFile
#endif // DeleteFile
#ifdef RemoveDirectory
#undef RemoveDirectory
#endif // RemoveDirectory
#endif // _WIN32

#ifdef TARGET_POSIX // Linux, Mac, FreeBSD
#include <sys/types.h>
#endif // TARGET_POSIX

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  typedef void* KODI_FILE_HDL;
  typedef void* KODI_HTTP_HEADER_HDL;

  //¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
  // "C" Definitions, structures and enumerators of filesystem
  //{{{

  //============================================================================
  /// @defgroup cpp_kodi_vfs_Defs_OpenFileFlags enum OpenFileFlags
  /// @ingroup cpp_kodi_vfs_Defs
  /// @brief **Flags to define way how file becomes opened**\n
  /// The values can be used together, e.g. <b>`file.Open("myfile", ADDON_READ_TRUNCATED | ADDON_READ_CHUNKED);`</b>
  ///
  /// Used on @ref kodi::vfs::CFile::OpenFile().
  ///
  ///@{
  typedef enum OpenFileFlags
  {
    /// @brief **0000 0000 0001** :\n
    /// Indicate that caller can handle truncated reads, where function
    /// returns before entire buffer has been filled.
    ADDON_READ_TRUNCATED = 0x01,

    /// @brief **0000 0000 0010** :\n
    /// Indicate that that caller support read in the minimum defined
    /// chunk size, this disables internal cache then.
    ADDON_READ_CHUNKED = 0x02,

    /// @brief **0000 0000 0100** :\n
    /// Use cache to access this file.
    ADDON_READ_CACHED = 0x04,

    /// @brief **0000 0000 1000** :\n
    /// Open without caching. regardless to file type.
    ADDON_READ_NO_CACHE = 0x08,

    /// @brief **0000 0001 0000** :\n
    /// Calculate bitrate for file while reading.
    ADDON_READ_BITRATE = 0x10,

    /// @brief **0000 0010 0000** :\n
    /// Indicate to the caller we will seek between multiple streams in
    /// the file frequently.
    ADDON_READ_MULTI_STREAM = 0x20,

    /// @brief **0000 0100 0000** :\n
    /// indicate to the caller file is audio and/or video (and e.g. may
    /// grow).
    ADDON_READ_AUDIO_VIDEO = 0x40,

    /// @brief **0000 1000 0000** :\n
    /// Indicate that caller will do write operations before reading.
    ADDON_READ_AFTER_WRITE = 0x80,

    /// @brief **0001 0000 0000** :\n
    /// Indicate that caller want to reopen a file if its already open.
    ADDON_READ_REOPEN = 0x100
  } OpenFileFlags;
  ///@}
  //----------------------------------------------------------------------------

  //============================================================================
  /// @defgroup cpp_kodi_vfs_Defs_CURLOptiontype enum CURLOptiontype
  /// @ingroup cpp_kodi_vfs_Defs
  /// @brief **CURL message types**\n
  /// Used on kodi::vfs::CFile::CURLAddOption().
  ///
  ///@{
  typedef enum CURLOptiontype
  {
    /// @brief Set a general option.
    ADDON_CURL_OPTION_OPTION,

    /// @brief Set a protocol option.\n
    ///\n
    /// The following names for *ADDON_CURL_OPTION_PROTOCOL* are possible:
    /// | Option name                         | Description
    /// |------------------------------------:|:--------------------------------
    /// | <b>`accept-charset`</b>             | Set the "accept-charset" header
    /// | <b>`acceptencoding or encoding`</b> | Set the "accept-encoding" header
    /// | <b>`active-remote`</b>              | Set the "active-remote" header
    /// | <b>`auth`</b>                       | Set the authentication method. Possible values: any, anysafe, digest, ntlm
    /// | <b>`connection-timeout`</b>         | Set the connection timeout in seconds
    /// | <b>`cookie`</b>                     | Set the "cookie" header
    /// | <b>`customrequest`</b>              | Set a custom HTTP request like DELETE
    /// | <b>`noshout`</b>                    | Set to true if kodi detects a stream as shoutcast by mistake.
    /// | <b>`postdata`</b>                   | Set the post body (value needs to be base64 encoded). (Implicitly sets the request to POST)
    /// | <b>`referer`</b>                    | Set the "referer" header
    /// | <b>`user-agent`</b>                 | Set the "user-agent" header
    /// | <b>`seekable`</b>                   | Set the stream seekable. 1: enable, 0: disable
    /// | <b>`sslcipherlist`</b>              | Set list of accepted SSL ciphers.
    ///
    ADDON_CURL_OPTION_PROTOCOL,

    /// @brief Set User and password
    ADDON_CURL_OPTION_CREDENTIALS,

    /// @brief Add a Header
    ADDON_CURL_OPTION_HEADER
  } CURLOptiontype;
  ///@}
  //----------------------------------------------------------------------------

  //============================================================================
  /// @defgroup cpp_kodi_vfs_Defs_FilePropertyTypes enum FilePropertyTypes
  /// @ingroup cpp_kodi_vfs_Defs
  /// @brief **File property types**\n
  /// Mostly to read internet sources.
  ///
  /// Used on kodi::vfs::CFile::GetPropertyValue() and kodi::vfs::CFile::GetPropertyValues().
  ///
  ///@{
  typedef enum FilePropertyTypes
  {
    /// @brief Get protocol response line.
    ADDON_FILE_PROPERTY_RESPONSE_PROTOCOL,
    /// @brief Get a response header.
    ADDON_FILE_PROPERTY_RESPONSE_HEADER,
    /// @brief Get file content type.
    ADDON_FILE_PROPERTY_CONTENT_TYPE,
    /// @brief Get file content charset.
    ADDON_FILE_PROPERTY_CONTENT_CHARSET,
    /// @brief Get file mime type.
    ADDON_FILE_PROPERTY_MIME_TYPE,
    /// @brief Get file effective URL (last one if redirected).
    ADDON_FILE_PROPERTY_EFFECTIVE_URL
  } FilePropertyTypes;
  ///@}
  //----------------------------------------------------------------------------

  //}}}

  struct VFS_STAT_STRUCTURE
  {
    uint32_t deviceId;
    uint64_t size;
    time_t accessTime;
    time_t modificationTime;
    time_t statusTime;
    bool isDirectory;
    bool isSymLink;
    bool isBlock;
    bool isCharacter;
    bool isFifo;
    bool isRegular;
    bool isSocket;
    uint64_t fileSerialNumber;
  };

  struct VFS_CACHE_STATUS
  {
    uint64_t forward;
    uint32_t maxrate;
    uint32_t currate;
    uint32_t lowrate;
  };

  struct VFS_PROPERTY
  {
    char* name;
    char* val;
  };

  struct VFS_DIR_ENTRY
  {
    char* label;
    char* title;
    char* path;
    size_t num_props;
    struct VFS_PROPERTY* properties;
    time_t date_time;
    bool folder;
    uint64_t size;
  };

  /*!@{*/
  ATTR_DLL_EXPORT bool kodi_vfs_can_open_directory(const char* url) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_create_directory(const char* path) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_remove_directory(const char* path) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_remove_directory_recursive(const char* path)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_directory_exists(const char* path) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_get_directory(const char* path,
                                              const char* mask,
                                              struct VFS_DIR_ENTRY** items,
                                              size_t* num_items) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_vfs_free_directory(struct VFS_DIR_ENTRY* items, size_t num_items)
      __INTRODUCED_IN_KODI(1);
  /*---AUTO_GEN_PARSE<OVERRIDE;USE_INTERNAL=kodi_vfs_free_directory>---*/
  /*!@}*/

  /*!@{*/
  ATTR_DLL_EXPORT bool kodi_vfs_file_exists(const char* filename, bool useCache)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_stat_file(const char* filename, struct VFS_STAT_STRUCTURE* buffer)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_delete_file(const char* filename) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_rename_file(const char* filename, const char* newFileName)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_copy_file(const char* filename, const char* dest)
      __INTRODUCED_IN_KODI(1);
  /*!@}*/

  /*!@{*/
  ATTR_DLL_EXPORT char* kodi_vfs_get_file_md5(const char* filename) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_vfs_get_cache_thumb_name(const char* filename) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_vfs_make_legal_filename(const char* filename) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_vfs_make_legal_path(const char* path) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_vfs_translate_special_protocol(const char* strSource)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_is_internet_stream(const char* path, bool strictCheck)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_is_on_lan(const char* path) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_is_remote(const char* path) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_is_local(const char* path) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_is_url(const char* path) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_get_mime_type(const char* url,
                                              char** content,
                                              const char* useragent) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_get_content_type(const char* url,
                                                 char** content,
                                                 const char* useragent) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_get_cookies(const char* url, char** cookies)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_get_disk_space(const char* path,
                                               uint64_t* capacity,
                                               uint64_t* free,
                                               uint64_t* available) __INTRODUCED_IN_KODI(1);
  /*!@}*/

  /*!@{*/
  ATTR_DLL_EXPORT KODI_HTTP_HEADER_HDL kodi_vfs_http_header_open(const char* url)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_vfs_http_header_close(KODI_HTTP_HEADER_HDL hdl) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_vfs_http_header_get_value(KODI_HTTP_HEADER_HDL hdl, const char* param)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char** kodi_vfs_http_header_get_values(KODI_HTTP_HEADER_HDL hdl,
                                                         const char* param,
                                                         size_t* length) __INTRODUCED_IN_KODI(1);

  ATTR_DLL_EXPORT char* kodi_vfs_http_header_get_header(KODI_HTTP_HEADER_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_vfs_http_header_get_mime_type(KODI_HTTP_HEADER_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_vfs_http_header_get_charset(KODI_HTTP_HEADER_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_vfs_http_header_get_proto_line(KODI_HTTP_HEADER_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  /*!@}*/

  /*!@{*/
  ATTR_DLL_EXPORT KODI_FILE_HDL kodi_vfs_file_open(const char* filename, unsigned int flags)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT KODI_FILE_HDL kodi_vfs_file_open_for_write(const char* filename, bool overwrite)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT KODI_FILE_HDL kodi_vfs_file_curl_create(const char* url) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_file_curl_add_option(KODI_FILE_HDL hdl,
                                                     enum CURLOptiontype type,
                                                     const char* name,
                                                     const char* value) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_file_curl_open(KODI_FILE_HDL hdl, unsigned int flags)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_vfs_file_close(KODI_FILE_HDL hdl) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT ssize_t kodi_vfs_file_read(KODI_FILE_HDL hdl, uint8_t* ptr, size_t size)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_file_read_line(KODI_FILE_HDL hdl, char* szLine, size_t lineLength)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT ssize_t kodi_vfs_file_write(KODI_FILE_HDL hdl, const uint8_t* ptr, size_t size)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_vfs_file_flush(KODI_FILE_HDL hdl) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT int64_t kodi_vfs_file_seek(KODI_FILE_HDL hdl, int64_t position, int whence)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT int kodi_vfs_file_truncate(KODI_FILE_HDL hdl, int64_t size)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT int64_t kodi_vfs_file_get_position(KODI_FILE_HDL hdl) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT int64_t kodi_vfs_file_get_length(KODI_FILE_HDL hdl) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_file_at_end(KODI_FILE_HDL hdl) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT double kodi_vfs_file_get_download_speed(KODI_FILE_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT int kodi_vfs_file_get_chunk_size(KODI_FILE_HDL hdl) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_file_io_ctl_get_seek_possible(KODI_FILE_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_file_io_ctl_get_cache_status(KODI_FILE_HDL hdl,
                                                             struct VFS_CACHE_STATUS* status)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_file_io_ctl_set_cache_rate(KODI_FILE_HDL hdl, uint32_t rate)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_vfs_file_io_ctl_set_retry(KODI_FILE_HDL hdl, bool retry)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_vfs_file_get_property_value(KODI_FILE_HDL hdl,
                                                         enum FilePropertyTypes type,
                                                         const char* name) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char** kodi_vfs_file_get_property_values(KODI_FILE_HDL hdl,
                                                           enum FilePropertyTypes type,
                                                           const char* name,
                                                           size_t* length) __INTRODUCED_IN_KODI(1);
  /*!@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !C_API_FILESYSTEM_H */
