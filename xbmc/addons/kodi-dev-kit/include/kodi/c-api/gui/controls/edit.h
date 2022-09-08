/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_GUI_CONTROLS_EDIT_H
#define C_API_GUI_CONTROLS_EDIT_H

#include "../../addon_base.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  typedef void* KODI_GUI_CONTROL_HANDLE;

  //============================================================================
  /// @ingroup cpp_kodi_gui_windows_controls_CEdit_Defs
  /// @{
  /// @anchor AddonGUIInputType
  /// @brief Text input types used on kodi::gui::controls::CEdit
  enum AddonGUIInputType
  {
    /// Text inside edit control only readable
    ADDON_INPUT_TYPE_READONLY = -1,
    /// Normal text entries
    ADDON_INPUT_TYPE_TEXT = 0,
    /// To use on edit control only numeric numbers
    ADDON_INPUT_TYPE_NUMBER,
    /// To insert seconds
    ADDON_INPUT_TYPE_SECONDS,
    /// To insert time
    ADDON_INPUT_TYPE_TIME,
    /// To insert a date
    ADDON_INPUT_TYPE_DATE,
    /// Used for write in IP addresses
    ADDON_INPUT_TYPE_IPADDRESS,
    /// Text field used as password entry field with not visible text
    ADDON_INPUT_TYPE_PASSWORD,
    /// Text field used as password entry field with not visible text but
    /// returned as MD5 value
    ADDON_INPUT_TYPE_PASSWORD_MD5,
    /// Use text field for search purpose
    ADDON_INPUT_TYPE_SEARCH,
    /// Text field as filter
    ADDON_INPUT_TYPE_FILTER,
    ///
    ADDON_INPUT_TYPE_PASSWORD_NUMBER_VERIFY_NEW
  };
  /// @}
  //----------------------------------------------------------------------------

  ATTR_DLL_EXPORT void kodi_gui_controls_edit_set_visible(KODI_GUI_CONTROL_HANDLE handle,
                                                          bool visible) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_edit_set_enabled(KODI_GUI_CONTROL_HANDLE handle,
                                                          bool enabled) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_edit_set_label(KODI_GUI_CONTROL_HANDLE handle,
                                                        const char* label) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_gui_controls_edit_get_label(KODI_GUI_CONTROL_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_edit_set_text(KODI_GUI_CONTROL_HANDLE handle,
                                                       const char* text) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_gui_controls_edit_get_text(KODI_GUI_CONTROL_HANDLE handle)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_edit_set_cursor_position(KODI_GUI_CONTROL_HANDLE handle,
                                                                  unsigned int position)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT unsigned int kodi_gui_controls_edit_get_cursor_position(
      KODI_GUI_CONTROL_HANDLE handle) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_gui_controls_edit_set_input_type(KODI_GUI_CONTROL_HANDLE handle,
                                                             int type,
                                                             const char* heading)
      __INTRODUCED_IN_KODI(1);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_GUI_CONTROLS_EDIT_H */
