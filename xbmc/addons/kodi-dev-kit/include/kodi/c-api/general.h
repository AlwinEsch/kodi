/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_GENERAL_H
#define C_API_GENERAL_H

#include "addon_base.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  //============================================================================
  /// \ingroup cpp_kodi_Defs
  /// @brief For kodi::CurrentKeyboardLayout used defines
  ///
  typedef enum StdKbButtons
  {
    /// The quantity of buttons per row on Kodi's standard keyboard
    STD_KB_BUTTONS_PER_ROW = 20,
    /// The quantity of rows on Kodi's standard keyboard
    STD_KB_BUTTONS_MAX_ROWS = 4,
    /// Keyboard layout type, this for initial standard
    STD_KB_MODIFIER_KEY_NONE = 0x00,
    /// Keyboard layout type, this for shift controlled layout (uppercase)
    STD_KB_MODIFIER_KEY_SHIFT = 0x01,
    /// Keyboard layout type, this to show symbols
    STD_KB_MODIFIER_KEY_SYMBOL = 0x02
  } StdKbButtons;
  //----------------------------------------------------------------------------

  //============================================================================
  /// \ingroup cpp_kodi_Defs
  /// @brief For kodi::QueueNotification() used message types
  ///
  typedef enum QueueMsg
  {
    /// Show info notification message
    QUEUE_INFO,
    /// Show warning notification message
    QUEUE_WARNING,
    /// Show error notification message
    QUEUE_ERROR,
    /// Show with own given image and parts if set on values
    QUEUE_OWN_STYLE
  } QueueMsg;
  //----------------------------------------------------------------------------

  //============================================================================
  /// \ingroup cpp_kodi_Defs
  /// @brief Format codes to get string from them.
  ///
  /// Used on kodi::GetLanguage().
  ///
  typedef enum LangFormats
  {
    /// two letter code as defined in ISO 639-1
    LANG_FMT_ISO_639_1,
    /// three letter code as defined in ISO 639-2/T or ISO 639-2/B
    LANG_FMT_ISO_639_2,
    /// full language name in English
    LANG_FMT_ENGLISH_NAME
  } LangFormats;
  //----------------------------------------------------------------------------

  typedef enum DigestType
  {
    DIGEST_MD5,
    DIGEST_SHA1,
    DIGEST_SHA256,
    DIGEST_SHA512
  } DigestType;

  typedef struct AddonKeyboardKeyTable
  {
    char* keys[STD_KB_BUTTONS_MAX_ROWS][STD_KB_BUTTONS_PER_ROW];
  } AddonKeyboardKeyTable;

  ATTR_DLL_EXPORT char* kodi_get_localized_string(long label_id) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_get_free_mem(long* free, long* total, bool as_bytes)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_unknown_to_utf8(const char* source, bool* ret, bool failOnBadChar)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_get_language(enum LangFormats format, bool region)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_queue_notification(enum QueueMsg type,
                                               const char* header,
                                               const char* message,
                                               const char* imageFile,
                                               unsigned int displayTime,
                                               bool withSound,
                                               unsigned int messageTime) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_get_digest(enum DigestType type, const char* text)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_get_region(const char* id) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT int kodi_get_global_idle_time() __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_is_addon_avilable(const char* id, char** version, bool* enabled)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_version(
      char** compile_name, int* major, int* minor, char** revision, char** tag, char** tagversion)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_get_current_skin_id() __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_get_keyboard_layout(int modifier_key,
                                                char** layout_name,
                                                struct AddonKeyboardKeyTable* layout)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_change_keyboard_layout(char** layout_name) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_play_sfx(const char* filename, bool use_cached) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_stop_sfx() __INTRODUCED_IN_KODI(1);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !C_API_GENERAL_H */
