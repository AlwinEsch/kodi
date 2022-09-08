/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_ADDON_H
#define C_API_ADDON_H

#if !defined(NOMINMAX)
#define NOMINMAX
#endif

#include "version.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef TARGET_WINDOWS
#ifndef __cdecl
#define __cdecl
#endif
#ifndef __declspec
#define __declspec(X)
#endif
#endif

// Generic helper definitions for smallest possible alignment
//@{
#undef ATTR_PACKED
#undef PRAGMA_PACK_BEGIN
#undef PRAGMA_PACK_END

#if defined(__GNUC__)
#define ATTR_PACKED __attribute__((packed))
#define PRAGMA_PACK 0
#endif

#if !defined(ATTR_PACKED)
#define ATTR_PACKED
#define PRAGMA_PACK 1
#endif
//@}

// Generic helper definitions for inline function support
//@{
#ifdef _MSC_VER
#define ATTR_FORCEINLINE __forceinline
#elif defined(__GNUC__)
#define ATTR_FORCEINLINE inline __attribute__((__always_inline__))
#elif defined(__CLANG__)
#if __has_attribute(__always_inline__)
#define ATTR_FORCEINLINE inline __attribute__((__always_inline__))
#else
#define ATTR_FORCEINLINE inline
#endif
#else
#define ATTR_FORCEINLINE inline
#endif
//@}

// Generic helper definitions for shared library support
//@{
#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
#define ATTR_DLL_IMPORT __declspec(dllimport)
#define ATTR_DLL_EXPORT __declspec(dllexport)
#define ATTR_DLL_LOCAL
#ifdef _WIN64
#define ATTR_APIENTRY __stdcall
#else
#define ATTR_APIENTRY __cdecl
#endif
#else
#if __GNUC__ >= 4
#define ATTR_DLL_IMPORT __attribute__((visibility("default")))
#define ATTR_DLL_EXPORT __attribute__((visibility("default")))
#ifndef SWIG
#define ATTR_DLL_LOCAL __attribute__((visibility("hidden")))
#else
#define ATTR_DLL_LOCAL
#endif
#else
#define ATTR_DLL_IMPORT
#define ATTR_DLL_EXPORT
#define ATTR_DLL_LOCAL
#endif
#define ATTR_APIENTRY
#endif

#ifndef ATTR_APIENTRYP
#define ATTR_APIENTRYP ATTR_APIENTRY*
#endif
//@}

#ifdef _WIN32 // windows
#if !defined(_SSIZE_T_DEFINED) && !defined(HAVE_SSIZE_T)
typedef intptr_t ssize_t;
#define _SSIZE_T_DEFINED
#endif // !_SSIZE_T_DEFINED
#ifndef SSIZE_MAX
#define SSIZE_MAX INTPTR_MAX
#endif // !SSIZE_MAX
#else // Linux, Mac, FreeBSD
#include <sys/types.h>
#endif // TARGET_POSIX

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if __KODI_API__ >= 1

  typedef void* KODI_HANDLE;
  typedef void* KODI_DLL_HDL;
  typedef void* KODI_OWN_HDL;
  typedef void* KODI_IFC_HDL;
  typedef void* KODI_ADDON_HDL;
  typedef void* KODI_ADDON_BACKEND_HDL;
  typedef void* KODI_ADDON_INSTANCE_HDL;
  typedef void* KODI_ADDON_INSTANCE_BACKEND_HDL;

  typedef void* ADDON_HARDWARE_CONTEXT;

  //============================================================================
  /// @ingroup cpp_kodi_addon_addonbase_Defs
  /// @defgroup cpp_kodi_addon_addonbase_Defs_ADDON_STATUS enum ADDON_STATUS
  /// @brief <b>Return value of functions in @ref cpp_kodi_addon_addonbase "kodi::addon::CAddonBase"
  /// and associated classes</b>\n
  /// With this Kodi can do any follow-up work or add-on e.g. declare it as defective.
  ///
  ///@{
  typedef enum ADDON_STATUS
  {
    ADDON_STATUS_OK,
    ADDON_STATUS_LOST_CONNECTION,
    ADDON_STATUS_NEED_RESTART,
    ADDON_STATUS_NEED_SETTINGS,
    ADDON_STATUS_UNKNOWN,
    ADDON_STATUS_PERMANENT_FAILURE,
    ADDON_STATUS_NOT_IMPLEMENTED
  } ADDON_STATUS;

  //============================================================================
  /// @defgroup cpp_kodi_Defs_ADDON_LOG enum ADDON_LOG
  /// @ingroup cpp_kodi_Defs
  /// @brief **Log file type definitions**\n
  /// These define the types of log entries given with @ref kodi::Log() to Kodi.
  ///
  /// -------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  ///
  /// kodi::Log(ADDON_LOG_ERROR, "%s: There is an error occurred!", __func__);
  ///
  /// ~~~~~~~~~~~~~
  ///
  ///@{
  typedef enum ADDON_LOG
  {
    ADDON_LOG_DEBUG = 0,
    ADDON_LOG_INFO = 1,
    ADDON_LOG_WARNING = 2,
    ADDON_LOG_ERROR = 3,
    ADDON_LOG_FATAL = 4
  } ADDON_LOG;
  ///@}
  //----------------------------------------------------------------------------

  typedef enum KODI_ADDON_INSTANCE_TYPE
  {
    ADDON_INSTANCE_UNKNOWN = 0,
    ADDON_INSTANCE_AUDIODECODER,
    ADDON_INSTANCE_AUDIOENCODER,
    ADDON_INSTANCE_GAME,
    ADDON_INSTANCE_INPUTSTREAM,
    ADDON_INSTANCE_PERIPHERAL,
    ADDON_INSTANCE_PVR,
    ADDON_INSTANCE_SCREENSAVER,
    ADDON_INSTANCE_VISUALIZATION,
    ADDON_INSTANCE_VFS,
    ADDON_INSTANCE_IMAGEDECODER,
    ADDON_INSTANCE_VIDEOCODEC,
    ADDON_INSTANCE_WEB,
  } KODI_ADDON_INSTANCE_TYPE;

  struct KODI_ADDON_INSTANCE_INFO
  {
    enum KODI_ADDON_INSTANCE_TYPE type;
    uint32_t instance_id;
    const char* unique_work_id;
    KODI_ADDON_INSTANCE_BACKEND_HDL kodi;
    KODI_ADDON_INSTANCE_HDL parent;
  };
  /*---AUTO_GEN_PARSE<OVERRIDE;USE_AS_SHARED_STRUCT=KODI_ADDON_INSTANCE_INFO>---*/

  struct KODI_ADDON_INSTANCE_STRUCT
  {
    KODI_ADDON_INSTANCE_HDL hdl;
    union
    {
      KODI_HANDLE dummy;
      struct KODI_ADDON_INSTANCE_FUNC* instance;
      struct KODI_ADDON_AUDIODECODER_FUNC* audiodecoder;
      struct KODI_ADDON_AUDIOENCODER_FUNC* audioencoder;
      struct KODI_ADDON_IMAGEDECODER_FUNC* imagedecoder;
      struct KODI_ADDON_GAME_FUNC* game;
      struct KODI_ADDON_INPUTSTREAM_FUNC* inputstream;
      struct KODI_ADDON_PERIPHERAL_FUNC* peripheral;
      struct KODI_ADDON_SCREENSAVER_FUNC* screensaver;
      struct KODI_ADDON_VFS_FUNC* vfs;
      struct KODI_ADDON_VIDEOCODEC_FUNC* videocodec;
      struct KODI_ADDON_VISUALIZATION_FUNC* visualization;
      struct KODI_ADDON_WEB_FUNC* web;
    };
  };

  typedef enum ADDON_STATUS(ATTR_APIENTRYP PFN_KODI_ADDON_CREATE_V1)(const struct KODI_ADDON_INSTANCE_INFO* first_instance, KODI_ADDON_HDL* hdl);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_DESTROY_V1)(KODI_ADDON_HDL hdl);

  typedef enum ADDON_STATUS(ATTR_APIENTRYP PFN_KODI_ADDON_SETTING_CHANGE_STRING_V1)(
      const KODI_ADDON_HDL hdl, const char* name, const char* value);
  typedef enum ADDON_STATUS(ATTR_APIENTRYP PFN_KODI_ADDON_SETTING_CHANGE_BOOLEAN_V1)(
      const KODI_ADDON_HDL hdl, const char* name, bool value);
  typedef enum ADDON_STATUS(ATTR_APIENTRYP PFN_KODI_ADDON_SETTING_CHANGE_INTEGER_V1)(
      const KODI_ADDON_HDL hdl, const char* name, int value);
  typedef enum ADDON_STATUS(ATTR_APIENTRYP PFN_KODI_ADDON_SETTING_CHANGE_FLOAT_V1)(
      const KODI_ADDON_HDL hdl, const char* name, double value);

  typedef enum ADDON_STATUS(ATTR_APIENTRYP PFN_KODI_ADDON_CREATE_INSTANCE_V1)(
      const KODI_ADDON_HDL hdl,
      const struct KODI_ADDON_INSTANCE_INFO* info,
      struct KODI_ADDON_INSTANCE_STRUCT* instance);
  /*---AUTO_GEN_PARSE<OVERRIDE;USE_HAND_MAKE=kodi_addon_create_instance_v1>---*/
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_DESTROY_INSTANCE_V1)(
      const KODI_ADDON_HDL hdl,
      const struct KODI_ADDON_INSTANCE_INFO* info,
      const KODI_ADDON_INSTANCE_HDL instance_hdl);
  /*---AUTO_GEN_PARSE<OVERRIDE;USE_HAND_MAKE=kodi_addon_destroy_instance_v1>---*/

  typedef enum ADDON_STATUS(ATTR_APIENTRYP PFN_KODI_ADDON_INSTANCE_SETTING_CHANGE_STRING_V1)(
      const KODI_ADDON_HDL hdl,
      const KODI_ADDON_INSTANCE_HDL instance_hdl,
      const char* name,
      const char* value);
  typedef enum ADDON_STATUS(ATTR_APIENTRYP PFN_KODI_ADDON_INSTANCE_SETTING_CHANGE_BOOLEAN_V1)(
      const KODI_ADDON_HDL hdl,
      const KODI_ADDON_INSTANCE_HDL instance_hdl,
      const char* name,
      bool value);
  typedef enum ADDON_STATUS(ATTR_APIENTRYP PFN_KODI_ADDON_INSTANCE_SETTING_CHANGE_INTEGER_V1)(
      const KODI_ADDON_HDL hdl,
      const KODI_ADDON_INSTANCE_HDL instance_hdl,
      const char* name,
      int value);
  typedef enum ADDON_STATUS(ATTR_APIENTRYP PFN_KODI_ADDON_INSTANCE_SETTING_CHANGE_FLOAT_V1)(
      const KODI_ADDON_HDL hdl,
      const KODI_ADDON_INSTANCE_HDL instance_hdl,
      const char* name,
      double value);

  struct KODI_ADDON_FUNC
  {
    PFN_KODI_ADDON_CREATE_V1 create;
    PFN_KODI_ADDON_DESTROY_V1 destroy;

    PFN_KODI_ADDON_SETTING_CHANGE_STRING_V1 setting_change_string;
    PFN_KODI_ADDON_SETTING_CHANGE_BOOLEAN_V1 setting_change_boolean;
    PFN_KODI_ADDON_SETTING_CHANGE_INTEGER_V1 setting_change_integer;
    PFN_KODI_ADDON_SETTING_CHANGE_FLOAT_V1 setting_change_float;

    PFN_KODI_ADDON_CREATE_INSTANCE_V1 create_instance;
    PFN_KODI_ADDON_DESTROY_INSTANCE_V1 destroy_instance;

    PFN_KODI_ADDON_INSTANCE_SETTING_CHANGE_STRING_V1 instance_setting_change_string;
    PFN_KODI_ADDON_INSTANCE_SETTING_CHANGE_BOOLEAN_V1 instance_setting_change_boolean;
    PFN_KODI_ADDON_INSTANCE_SETTING_CHANGE_INTEGER_V1 instance_setting_change_integer;
    PFN_KODI_ADDON_INSTANCE_SETTING_CHANGE_FLOAT_V1 instance_setting_change_float;
  };

  typedef KODI_ADDON_INSTANCE_HDL(ATTR_APIENTRYP PFN_KODI_ADDON_INSTANCE_CREATE_V1)(
      KODI_ADDON_INSTANCE_BACKEND_HDL kodi_hdl);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_INSTANCE_DESTROY_V1)(KODI_ADDON_INSTANCE_HDL hdl);

  struct KODI_ADDON_INSTANCE_FUNC
  {
    PFN_KODI_ADDON_INSTANCE_CREATE_V1 create;
    PFN_KODI_ADDON_INSTANCE_DESTROY_V1 destroy;
  };

  ATTR_DLL_EXPORT const char* kodi_check_main_shared(int argc, char* argv[])
      __INTRODUCED_IN_KODI(1);
  /*---AUTO_GEN_PARSE<OVERRIDE;USE_INTERNAL=kodi_check_main_shared>---*/
  ATTR_DLL_EXPORT KODI_IFC_HDL kodi_init(unsigned int api,
                                         int argc,
                                         char* argv[],
                                         struct KODI_ADDON_FUNC* func,
                                         bool via_main,
                                         bool no_receive) __INTRODUCED_IN_KODI(1);
  /*---AUTO_GEN_PARSE<OVERRIDE;USE_INTERNAL=kodi_init>---*/
  ATTR_DLL_EXPORT void kodi_deinit(KODI_IFC_HDL hdl) __INTRODUCED_IN_KODI(1);
  /*---AUTO_GEN_PARSE<OVERRIDE;USE_INTERNAL=kodi_deinit>---*/
  ATTR_DLL_EXPORT bool kodi_process(KODI_IFC_HDL hdl) __INTRODUCED_IN_KODI(1);
  /*---AUTO_GEN_PARSE<OVERRIDE;USE_INTERNAL=kodi_process>---*/

  ATTR_DLL_EXPORT void kodi_log(enum ADDON_LOG loglevel, const char* format, ...)
      __INTRODUCED_IN_KODI(1);

  ATTR_DLL_EXPORT char* kodi_addon_get_addon_path() __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_addon_get_lib_path() __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_addon_get_user_path() __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_addon_get_temp_path() __INTRODUCED_IN_KODI(1);

  ATTR_DLL_EXPORT char* kodi_addon_localized_string(long label_id) __INTRODUCED_IN_KODI(1);

  ATTR_DLL_EXPORT char* kodi_addon_get_info(const char* id) __INTRODUCED_IN_KODI(1);

  ATTR_DLL_EXPORT bool kodi_addon_open_settings_dialog() __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_addon_is_setting_using_default(const char* id) __INTRODUCED_IN_KODI(1);

  ATTR_DLL_EXPORT bool kodi_addon_get_setting_bool(const char* id, bool* value)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_addon_get_setting_int(const char* id, int* value)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_addon_get_setting_float(const char* id, double* value)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_addon_get_setting_string(const char* id, char** value)
      __INTRODUCED_IN_KODI(1);

  ATTR_DLL_EXPORT bool kodi_addon_set_setting_bool(const char* id, bool value)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_addon_set_setting_int(const char* id, int value)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_addon_set_setting_float(const char* id, double value)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_addon_set_setting_string(const char* id, const char* value)
      __INTRODUCED_IN_KODI(1);

  ATTR_DLL_EXPORT char* kodi_addon_instance_get_user_path(const KODI_ADDON_INSTANCE_BACKEND_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_addon_instance_is_setting_using_default(
      const KODI_ADDON_INSTANCE_BACKEND_HDL hdl, const char* id) __INTRODUCED_IN_KODI(1);

  ATTR_DLL_EXPORT bool kodi_addon_instance_get_setting_bool(
      const KODI_ADDON_INSTANCE_BACKEND_HDL hdl, const char* id, bool* value)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_addon_instance_get_setting_int(
      const KODI_ADDON_INSTANCE_BACKEND_HDL hdl, const char* id, int* value)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_addon_instance_get_setting_float(
      const KODI_ADDON_INSTANCE_BACKEND_HDL hdl, const char* id, double* value)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_addon_instance_get_setting_string(
      const KODI_ADDON_INSTANCE_BACKEND_HDL hdl, const char* id, char** value)
      __INTRODUCED_IN_KODI(1);

  ATTR_DLL_EXPORT bool kodi_addon_instance_set_setting_bool(
      const KODI_ADDON_INSTANCE_BACKEND_HDL hdl, const char* id, bool value)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_addon_instance_set_setting_int(
      const KODI_ADDON_INSTANCE_BACKEND_HDL hdl, const char* id, int value) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_addon_instance_set_setting_float(
      const KODI_ADDON_INSTANCE_BACKEND_HDL hdl, const char* id, double value)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_addon_instance_set_setting_string(
      const KODI_ADDON_INSTANCE_BACKEND_HDL hdl, const char* id, const char* value)
      __INTRODUCED_IN_KODI(1);

#endif /* __KODI_API__ >= 1 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !C_API_ADDON_H */
