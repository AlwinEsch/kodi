/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/* File autogenerated, see xbmc/addons/kodi-dev-kit/tools/code-generator/code_generator.py */

#pragma once

#include "../../../../../include/kodi/c-api/gui/dialogs/progress.h"
#include "src/shared/SharedGroups.h"

#include <string.h>
#include <string>
#include <tuple>
#include <vector>

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

// Code here relates to direct API calls without use of shared mem from addon to Kodi.
//{{{

typedef KODI_GUI_HANDLE(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_DIALOGS_PROGRESS_NEW_DIALOG_V1)(void*);
typedef bool(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_DIALOGS_PROGRESS_ABORT_V1)(void*, KODI_GUI_HANDLE);
typedef bool(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_DIALOGS_PROGRESS_IS_CANCELED_V1)(void*,
                                                                                  KODI_GUI_HANDLE);
typedef int(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_DIALOGS_PROGRESS_GET_PERCENTAGE_V1)(
    void*, KODI_GUI_HANDLE);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_DIALOGS_PROGRESS_DELETE_DIALOG_V1)(
    void*, KODI_GUI_HANDLE);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_DIALOGS_PROGRESS_OPEN_V1)(void*, KODI_GUI_HANDLE);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_DIALOGS_PROGRESS_SET_CAN_CANCEL_V1)(
    void*, KODI_GUI_HANDLE, bool);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_DIALOGS_PROGRESS_SET_HEADING_V1)(void*,
                                                                                  KODI_GUI_HANDLE,
                                                                                  const char*);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_DIALOGS_PROGRESS_SET_LINE_V1)(void*,
                                                                               KODI_GUI_HANDLE,
                                                                               unsigned int,
                                                                               const char*);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_DIALOGS_PROGRESS_SET_PERCENTAGE_V1)(
    void*, KODI_GUI_HANDLE, int);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_DIALOGS_PROGRESS_SET_PROGRESS_ADVANCE_V1)(
    void*, KODI_GUI_HANDLE, int);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_DIALOGS_PROGRESS_SET_PROGRESS_MAX_V1)(
    void*, KODI_GUI_HANDLE, int);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_DIALOGS_PROGRESS_SHOW_PROGRESS_BAR_V1)(
    void*, KODI_GUI_HANDLE, bool);

struct directFuncToKodi_gui_dialogs_progress_h
{
  void* thisClassHdl;
  PFN_INT_KODI_GUI_DIALOGS_PROGRESS_ABORT_V1 kodi_gui_dialogs_progress_abort_v1;
  PFN_INT_KODI_GUI_DIALOGS_PROGRESS_DELETE_DIALOG_V1 kodi_gui_dialogs_progress_delete_dialog_v1;
  PFN_INT_KODI_GUI_DIALOGS_PROGRESS_GET_PERCENTAGE_V1 kodi_gui_dialogs_progress_get_percentage_v1;
  PFN_INT_KODI_GUI_DIALOGS_PROGRESS_IS_CANCELED_V1 kodi_gui_dialogs_progress_is_canceled_v1;
  PFN_INT_KODI_GUI_DIALOGS_PROGRESS_NEW_DIALOG_V1 kodi_gui_dialogs_progress_new_dialog_v1;
  PFN_INT_KODI_GUI_DIALOGS_PROGRESS_OPEN_V1 kodi_gui_dialogs_progress_open_v1;
  PFN_INT_KODI_GUI_DIALOGS_PROGRESS_SET_CAN_CANCEL_V1 kodi_gui_dialogs_progress_set_can_cancel_v1;
  PFN_INT_KODI_GUI_DIALOGS_PROGRESS_SET_HEADING_V1 kodi_gui_dialogs_progress_set_heading_v1;
  PFN_INT_KODI_GUI_DIALOGS_PROGRESS_SET_LINE_V1 kodi_gui_dialogs_progress_set_line_v1;
  PFN_INT_KODI_GUI_DIALOGS_PROGRESS_SET_PERCENTAGE_V1 kodi_gui_dialogs_progress_set_percentage_v1;
  PFN_INT_KODI_GUI_DIALOGS_PROGRESS_SET_PROGRESS_ADVANCE_V1
      kodi_gui_dialogs_progress_set_progress_advance_v1;
  PFN_INT_KODI_GUI_DIALOGS_PROGRESS_SET_PROGRESS_MAX_V1
      kodi_gui_dialogs_progress_set_progress_max_v1;
  PFN_INT_KODI_GUI_DIALOGS_PROGRESS_SHOW_PROGRESS_BAR_V1
      kodi_gui_dialogs_progress_show_progress_bar_v1;
};

//}}}

// Code here relates to direct API calls without use of shared mem from Kodi to addon.
//{{{

struct directFuncToAddon_gui_dialogs_progress_h
{
  void* thisClassHdl;
};

//}}}

typedef enum funcParent_gui_dialogs_progress_h : int
{
  funcParent_kodi_gui_dialogs_progress_new_dialog_v1,
  funcParent_kodi_gui_dialogs_progress_delete_dialog_v1,
  funcParent_kodi_gui_dialogs_progress_open_v1,
  funcParent_kodi_gui_dialogs_progress_set_heading_v1,
  funcParent_kodi_gui_dialogs_progress_set_line_v1,
  funcParent_kodi_gui_dialogs_progress_set_can_cancel_v1,
  funcParent_kodi_gui_dialogs_progress_is_canceled_v1,
  funcParent_kodi_gui_dialogs_progress_set_percentage_v1,
  funcParent_kodi_gui_dialogs_progress_get_percentage_v1,
  funcParent_kodi_gui_dialogs_progress_show_progress_bar_v1,
  funcParent_kodi_gui_dialogs_progress_set_progress_max_v1,
  funcParent_kodi_gui_dialogs_progress_set_progress_advance_v1,
  funcParent_kodi_gui_dialogs_progress_abort_v1,
} funcParent_gui_dialogs_progress_h;

// clang-format off
/*---AUTO_GEN_PARSE<FUNC_PARENT_TUPLES>---*/
// Original API call: ATTR_DLL_EXPORT KODI_GUI_HANDLE kodi_gui_dialogs_progress_new_dialog() __INTRODUCED_IN_KODI(1);
typedef std::tuple<DummyValue> msgParent__IN_kodi_gui_dialogs_progress_new_dialog_v1; /* Autogenerated */
typedef std::tuple<PtrValue> msgParent_OUT_kodi_gui_dialogs_progress_new_dialog_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_dialogs_progress_delete_dialog(KODI_GUI_HANDLE handle) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue> msgParent__IN_kodi_gui_dialogs_progress_delete_dialog_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_dialogs_progress_delete_dialog_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_dialogs_progress_open(KODI_GUI_HANDLE handle) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue> msgParent__IN_kodi_gui_dialogs_progress_open_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_dialogs_progress_open_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_dialogs_progress_set_heading(KODI_GUI_HANDLE handle, const char* heading) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue, std::string> msgParent__IN_kodi_gui_dialogs_progress_set_heading_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_dialogs_progress_set_heading_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_dialogs_progress_set_line(KODI_GUI_HANDLE handle, unsigned int line_no, const char* line) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue, unsigned int, std::string> msgParent__IN_kodi_gui_dialogs_progress_set_line_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_dialogs_progress_set_line_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_dialogs_progress_set_can_cancel(KODI_GUI_HANDLE handle, bool can_cancel) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue, bool> msgParent__IN_kodi_gui_dialogs_progress_set_can_cancel_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_dialogs_progress_set_can_cancel_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT bool kodi_gui_dialogs_progress_is_canceled(KODI_GUI_HANDLE handle) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue> msgParent__IN_kodi_gui_dialogs_progress_is_canceled_v1; /* Autogenerated */
typedef std::tuple<bool> msgParent_OUT_kodi_gui_dialogs_progress_is_canceled_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_dialogs_progress_set_percentage(KODI_GUI_HANDLE handle, int percentage) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue, int> msgParent__IN_kodi_gui_dialogs_progress_set_percentage_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_dialogs_progress_set_percentage_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT int kodi_gui_dialogs_progress_get_percentage(KODI_GUI_HANDLE handle) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue> msgParent__IN_kodi_gui_dialogs_progress_get_percentage_v1; /* Autogenerated */
typedef std::tuple<int> msgParent_OUT_kodi_gui_dialogs_progress_get_percentage_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_dialogs_progress_show_progress_bar(KODI_GUI_HANDLE handle, bool on_off) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue, bool> msgParent__IN_kodi_gui_dialogs_progress_show_progress_bar_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_dialogs_progress_show_progress_bar_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_dialogs_progress_set_progress_max(KODI_GUI_HANDLE handle, int max) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue, int> msgParent__IN_kodi_gui_dialogs_progress_set_progress_max_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_dialogs_progress_set_progress_max_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_dialogs_progress_set_progress_advance(KODI_GUI_HANDLE handle, int n_steps) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue, int> msgParent__IN_kodi_gui_dialogs_progress_set_progress_advance_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_dialogs_progress_set_progress_advance_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT bool kodi_gui_dialogs_progress_abort(KODI_GUI_HANDLE handle) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue> msgParent__IN_kodi_gui_dialogs_progress_abort_v1; /* Autogenerated */
typedef std::tuple<bool> msgParent_OUT_kodi_gui_dialogs_progress_abort_v1; /* Autogenerated */
/*---AUTO_GEN_PARSE<FUNC_PARENT_TUPLES_END>---*/
// clang-format on

//==============================================================================

typedef enum funcChild_gui_dialogs_progress_h : int
{
  funcChild_gui_dialogs_progress_h_dummy
} funcChild_gui_dialogs_progress_h;

// clang-format off
/*---AUTO_GEN_PARSE<FUNC_CHILD_TUPLES>---*/
/*---AUTO_GEN_PARSE<FUNC_CHILD_TUPLES_END>---*/
// clang-format on

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
