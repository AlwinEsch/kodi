/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/* File autogenerated, see xbmc/addons/kodi-dev-kit/tools/code-generator/code_generator.py */

#pragma once

#include "../../../../../include/kodi/c-api/gui/controls/spin.h"
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

typedef char*(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_CONTROLS_SPIN_GET_STRING_VALUE_V1)(
    void*, KODI_GUI_CONTROL_HANDLE);
typedef float(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_CONTROLS_SPIN_GET_FLOAT_VALUE_V1)(
    void*, KODI_GUI_CONTROL_HANDLE);
typedef int(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_CONTROLS_SPIN_GET_INT_VALUE_V1)(
    void*, KODI_GUI_CONTROL_HANDLE);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_CONTROLS_SPIN_ADD_INT_LABEL_V1)(
    void*, KODI_GUI_CONTROL_HANDLE, const char*, int);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_CONTROLS_SPIN_ADD_STRING_LABEL_V1)(
    void*, KODI_GUI_CONTROL_HANDLE, const char*, const char*);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_CONTROLS_SPIN_RESET_V1)(void*,
                                                                         KODI_GUI_CONTROL_HANDLE);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_CONTROLS_SPIN_SET_ENABLED_V1)(
    void*, KODI_GUI_CONTROL_HANDLE, bool);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_CONTROLS_SPIN_SET_FLOAT_INTERVAL_V1)(
    void*, KODI_GUI_CONTROL_HANDLE, float);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_CONTROLS_SPIN_SET_FLOAT_RANGE_V1)(
    void*, KODI_GUI_CONTROL_HANDLE, float, float);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_CONTROLS_SPIN_SET_FLOAT_VALUE_V1)(
    void*, KODI_GUI_CONTROL_HANDLE, float);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_CONTROLS_SPIN_SET_INT_RANGE_V1)(
    void*, KODI_GUI_CONTROL_HANDLE, int, int);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_CONTROLS_SPIN_SET_INT_VALUE_V1)(
    void*, KODI_GUI_CONTROL_HANDLE, int);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_CONTROLS_SPIN_SET_STRING_VALUE_V1)(
    void*, KODI_GUI_CONTROL_HANDLE, const char*);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_CONTROLS_SPIN_SET_TEXT_V1)(void*,
                                                                            KODI_GUI_CONTROL_HANDLE,
                                                                            const char*);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_CONTROLS_SPIN_SET_TYPE_V1)(void*,
                                                                            KODI_GUI_CONTROL_HANDLE,
                                                                            int);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_GUI_CONTROLS_SPIN_SET_VISIBLE_V1)(
    void*, KODI_GUI_CONTROL_HANDLE, bool);

struct directFuncToKodi_gui_controls_spin_h
{
  void* thisClassHdl;
  PFN_INT_KODI_GUI_CONTROLS_SPIN_ADD_INT_LABEL_V1 kodi_gui_controls_spin_add_int_label_v1;
  PFN_INT_KODI_GUI_CONTROLS_SPIN_ADD_STRING_LABEL_V1 kodi_gui_controls_spin_add_string_label_v1;
  PFN_INT_KODI_GUI_CONTROLS_SPIN_GET_FLOAT_VALUE_V1 kodi_gui_controls_spin_get_float_value_v1;
  PFN_INT_KODI_GUI_CONTROLS_SPIN_GET_INT_VALUE_V1 kodi_gui_controls_spin_get_int_value_v1;
  PFN_INT_KODI_GUI_CONTROLS_SPIN_GET_STRING_VALUE_V1 kodi_gui_controls_spin_get_string_value_v1;
  PFN_INT_KODI_GUI_CONTROLS_SPIN_RESET_V1 kodi_gui_controls_spin_reset_v1;
  PFN_INT_KODI_GUI_CONTROLS_SPIN_SET_ENABLED_V1 kodi_gui_controls_spin_set_enabled_v1;
  PFN_INT_KODI_GUI_CONTROLS_SPIN_SET_FLOAT_INTERVAL_V1 kodi_gui_controls_spin_set_float_interval_v1;
  PFN_INT_KODI_GUI_CONTROLS_SPIN_SET_FLOAT_RANGE_V1 kodi_gui_controls_spin_set_float_range_v1;
  PFN_INT_KODI_GUI_CONTROLS_SPIN_SET_FLOAT_VALUE_V1 kodi_gui_controls_spin_set_float_value_v1;
  PFN_INT_KODI_GUI_CONTROLS_SPIN_SET_INT_RANGE_V1 kodi_gui_controls_spin_set_int_range_v1;
  PFN_INT_KODI_GUI_CONTROLS_SPIN_SET_INT_VALUE_V1 kodi_gui_controls_spin_set_int_value_v1;
  PFN_INT_KODI_GUI_CONTROLS_SPIN_SET_STRING_VALUE_V1 kodi_gui_controls_spin_set_string_value_v1;
  PFN_INT_KODI_GUI_CONTROLS_SPIN_SET_TEXT_V1 kodi_gui_controls_spin_set_text_v1;
  PFN_INT_KODI_GUI_CONTROLS_SPIN_SET_TYPE_V1 kodi_gui_controls_spin_set_type_v1;
  PFN_INT_KODI_GUI_CONTROLS_SPIN_SET_VISIBLE_V1 kodi_gui_controls_spin_set_visible_v1;
};

//}}}

// Code here relates to direct API calls without use of shared mem from Kodi to addon.
//{{{

struct directFuncToAddon_gui_controls_spin_h
{
  void* thisClassHdl;
};

//}}}

typedef enum funcParent_gui_controls_spin_h : int
{
  funcParent_kodi_gui_controls_spin_set_visible_v1,
  funcParent_kodi_gui_controls_spin_set_enabled_v1,
  funcParent_kodi_gui_controls_spin_set_text_v1,
  funcParent_kodi_gui_controls_spin_reset_v1,
  funcParent_kodi_gui_controls_spin_set_type_v1,
  funcParent_kodi_gui_controls_spin_add_string_label_v1,
  funcParent_kodi_gui_controls_spin_set_string_value_v1,
  funcParent_kodi_gui_controls_spin_get_string_value_v1,
  funcParent_kodi_gui_controls_spin_add_int_label_v1,
  funcParent_kodi_gui_controls_spin_set_int_range_v1,
  funcParent_kodi_gui_controls_spin_set_int_value_v1,
  funcParent_kodi_gui_controls_spin_get_int_value_v1,
  funcParent_kodi_gui_controls_spin_set_float_range_v1,
  funcParent_kodi_gui_controls_spin_set_float_value_v1,
  funcParent_kodi_gui_controls_spin_get_float_value_v1,
  funcParent_kodi_gui_controls_spin_set_float_interval_v1,
} funcParent_gui_controls_spin_h;

// clang-format off
/*---AUTO_GEN_PARSE<FUNC_PARENT_TUPLES>---*/
// Original API call: ATTR_DLL_EXPORT void kodi_gui_controls_spin_set_visible(KODI_GUI_CONTROL_HANDLE handle, bool visible) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue, bool> msgParent__IN_kodi_gui_controls_spin_set_visible_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_controls_spin_set_visible_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_controls_spin_set_enabled(KODI_GUI_CONTROL_HANDLE handle, bool enabled) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue, bool> msgParent__IN_kodi_gui_controls_spin_set_enabled_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_controls_spin_set_enabled_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_controls_spin_set_text(KODI_GUI_CONTROL_HANDLE handle, const char* text) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue, std::string> msgParent__IN_kodi_gui_controls_spin_set_text_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_controls_spin_set_text_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_controls_spin_reset(KODI_GUI_CONTROL_HANDLE handle) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue> msgParent__IN_kodi_gui_controls_spin_reset_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_controls_spin_reset_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_controls_spin_set_type(KODI_GUI_CONTROL_HANDLE handle, int type) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue, int> msgParent__IN_kodi_gui_controls_spin_set_type_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_controls_spin_set_type_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_controls_spin_add_string_label(KODI_GUI_CONTROL_HANDLE handle, const char* label, const char* value) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue, std::string, std::string> msgParent__IN_kodi_gui_controls_spin_add_string_label_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_controls_spin_add_string_label_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_controls_spin_set_string_value(KODI_GUI_CONTROL_HANDLE handle, const char* value) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue, std::string> msgParent__IN_kodi_gui_controls_spin_set_string_value_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_controls_spin_set_string_value_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT char* kodi_gui_controls_spin_get_string_value(KODI_GUI_CONTROL_HANDLE handle) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue> msgParent__IN_kodi_gui_controls_spin_get_string_value_v1; /* Autogenerated */
typedef std::tuple<std::string> msgParent_OUT_kodi_gui_controls_spin_get_string_value_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_controls_spin_add_int_label(KODI_GUI_CONTROL_HANDLE handle, const char* label, int value) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue, std::string, int> msgParent__IN_kodi_gui_controls_spin_add_int_label_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_controls_spin_add_int_label_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_controls_spin_set_int_range(KODI_GUI_CONTROL_HANDLE handle, int start, int end) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue, int, int> msgParent__IN_kodi_gui_controls_spin_set_int_range_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_controls_spin_set_int_range_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_controls_spin_set_int_value(KODI_GUI_CONTROL_HANDLE handle, int value) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue, int> msgParent__IN_kodi_gui_controls_spin_set_int_value_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_controls_spin_set_int_value_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT int kodi_gui_controls_spin_get_int_value(KODI_GUI_CONTROL_HANDLE handle) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue> msgParent__IN_kodi_gui_controls_spin_get_int_value_v1; /* Autogenerated */
typedef std::tuple<int> msgParent_OUT_kodi_gui_controls_spin_get_int_value_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_controls_spin_set_float_range(KODI_GUI_CONTROL_HANDLE handle, float start, float end) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue, float, float> msgParent__IN_kodi_gui_controls_spin_set_float_range_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_controls_spin_set_float_range_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_controls_spin_set_float_value(KODI_GUI_CONTROL_HANDLE handle, float value) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue, float> msgParent__IN_kodi_gui_controls_spin_set_float_value_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_controls_spin_set_float_value_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT float kodi_gui_controls_spin_get_float_value(KODI_GUI_CONTROL_HANDLE handle) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue> msgParent__IN_kodi_gui_controls_spin_get_float_value_v1; /* Autogenerated */
typedef std::tuple<float> msgParent_OUT_kodi_gui_controls_spin_get_float_value_v1; /* Autogenerated */
// Original API call: ATTR_DLL_EXPORT void kodi_gui_controls_spin_set_float_interval(KODI_GUI_CONTROL_HANDLE handle, float interval) __INTRODUCED_IN_KODI(1);
typedef std::tuple<PtrValue, float> msgParent__IN_kodi_gui_controls_spin_set_float_interval_v1; /* Autogenerated */
typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_controls_spin_set_float_interval_v1; /* Autogenerated */
/*---AUTO_GEN_PARSE<FUNC_PARENT_TUPLES_END>---*/
// clang-format on

//==============================================================================

typedef enum funcChild_gui_controls_spin_h : int
{
  funcChild_gui_controls_spin_h_dummy
} funcChild_gui_controls_spin_h;

// clang-format off
/*---AUTO_GEN_PARSE<FUNC_CHILD_TUPLES>---*/
/*---AUTO_GEN_PARSE<FUNC_CHILD_TUPLES_END>---*/
// clang-format on

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
