/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/* File autogenerated, see xbmc/addons/kodi-dev-kit/tools/code-generator/code_generator.py */

#include "keyboard.h"

// Shared API
#include "src/shared/Instances.h"

// Lib code
#include "core/addon_control.h"

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1_END>---*/

using namespace KODI::ADDONS::INTERFACE;
using namespace KODI_ADDON::INTERNAL;

namespace KODI_ADDON
{
namespace INTERNAL
{

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_2>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_2_END>---*/

void CHdl_kodi_gui_dialogs_keyboard_h::InitDirect(
    directFuncToAddon_gui_dialogs_keyboard_h* ifcToAddon)
{
  ifcToAddon->thisClassHdl = this;
}

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3_END>---*/

/* NOTE: unused (no functions from Kodi to addon here) */

} /* namespace INTERNAL */
} /* namespace KODI_ADDON */

ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_get_input_with_head(
    const char* text_in,
    char** text_out,
    const char* heading,
    bool allow_empty_result,
    bool hidden_input,
    unsigned int auto_close_ms)
{
  // Original API call: ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_get_input_with_head( const char* text_in, char** text_out, const char* heading, bool allow_empty_result, bool hidden_input, unsigned int auto_close_ms) __INTRODUCED_IN_KODI(1);
  // Tuple in:          typedef std::tuple<std::string, std::string, bool, bool, unsigned int> msgParent__IN_kodi_gui_dialogs_keyboard_show_and_get_input_with_head_v1; /* Autogenerated */
  // Tuple out:         typedef std::tuple<bool, std::string> msgParent_OUT_kodi_gui_dialogs_keyboard_show_and_get_input_with_head_v1; /* Autogenerated */

  if (text_in == nullptr || text_out == nullptr || heading == nullptr)
    return false;

#ifndef KODI_INHIBIT_SHARED
  if (!AddonIfc::g_ifc->direct_used)
  {
    msgpack::sbuffer in;
    msgpack::sbuffer out;
    msgpack::pack(in, msgParent__IN_kodi_gui_dialogs_keyboard_show_and_get_input_with_head_v1(
                          text_in, heading, allow_empty_result, hidden_input, auto_close_ms));
    if (!AddonIfc::g_ifc->control->GetThreadIfc()->SendMessage(
            funcGroup_gui_dialogs_keyboard_h,
            funcParent_kodi_gui_dialogs_keyboard_show_and_get_input_with_head_v1, in, out))
      return false;
    msgpack::unpacked ident = msgpack::unpack(out.data(), out.size());
    msgParent_OUT_kodi_gui_dialogs_keyboard_show_and_get_input_with_head_v1 t =
        ident.get().as<decltype(t)>();

    *text_out = strdup(std::get<1>(t).c_str());
    return std::get<0>(t);
  }
#endif /* !KODI_INHIBIT_SHARED */

  const auto& auto_gen_group = AddonIfc::g_ifc->direct->to_kodi.gui_dialogs_keyboard_h;
  return auto_gen_group.kodi_gui_dialogs_keyboard_show_and_get_input_with_head_v1(
      auto_gen_group.thisClassHdl, text_in, text_out, heading, allow_empty_result, hidden_input,
      auto_close_ms);
}

ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_get_input(const char* text_in,
                                                                  char** text_out,
                                                                  bool allow_empty_result,
                                                                  unsigned int auto_close_ms)
{
  // Original API call: ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_get_input(const char* text_in, char** text_out, bool allow_empty_result, unsigned int auto_close_ms) __INTRODUCED_IN_KODI(1);
  // Tuple in:          typedef std::tuple<std::string, bool, unsigned int> msgParent__IN_kodi_gui_dialogs_keyboard_show_and_get_input_v1; /* Autogenerated */
  // Tuple out:         typedef std::tuple<bool, std::string> msgParent_OUT_kodi_gui_dialogs_keyboard_show_and_get_input_v1; /* Autogenerated */

  if (text_in == nullptr || text_out == nullptr)
    return false;

#ifndef KODI_INHIBIT_SHARED
  if (!AddonIfc::g_ifc->direct_used)
  {
    msgpack::sbuffer in;
    msgpack::sbuffer out;
    msgpack::pack(in, msgParent__IN_kodi_gui_dialogs_keyboard_show_and_get_input_v1(
                          text_in, allow_empty_result, auto_close_ms));
    if (!AddonIfc::g_ifc->control->GetThreadIfc()->SendMessage(
            funcGroup_gui_dialogs_keyboard_h,
            funcParent_kodi_gui_dialogs_keyboard_show_and_get_input_v1, in, out))
      return false;
    msgpack::unpacked ident = msgpack::unpack(out.data(), out.size());
    msgParent_OUT_kodi_gui_dialogs_keyboard_show_and_get_input_v1 t = ident.get().as<decltype(t)>();

    *text_out = strdup(std::get<1>(t).c_str());
    return std::get<0>(t);
  }
#endif /* !KODI_INHIBIT_SHARED */

  const auto& auto_gen_group = AddonIfc::g_ifc->direct->to_kodi.gui_dialogs_keyboard_h;
  return auto_gen_group.kodi_gui_dialogs_keyboard_show_and_get_input_v1(
      auto_gen_group.thisClassHdl, text_in, text_out, allow_empty_result, auto_close_ms);
}

ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_get_new_password_with_head(
    const char* password_in,
    char** password_out,
    const char* heading,
    bool allow_empty_result,
    unsigned int auto_close_ms)
{
  // Original API call: ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_get_new_password_with_head( const char* password_in, char** password_out, const char* heading, bool allow_empty_result, unsigned int auto_close_ms) __INTRODUCED_IN_KODI(1);
  // Tuple in:          typedef std::tuple<std::string, std::string, bool, unsigned int> msgParent__IN_kodi_gui_dialogs_keyboard_show_and_get_new_password_with_head_v1; /* Autogenerated */
  // Tuple out:         typedef std::tuple<bool, std::string> msgParent_OUT_kodi_gui_dialogs_keyboard_show_and_get_new_password_with_head_v1; /* Autogenerated */

  if (password_in == nullptr || password_out == nullptr || heading == nullptr)
    return false;

#ifndef KODI_INHIBIT_SHARED
  if (!AddonIfc::g_ifc->direct_used)
  {
    msgpack::sbuffer in;
    msgpack::sbuffer out;
    msgpack::pack(in,
                  msgParent__IN_kodi_gui_dialogs_keyboard_show_and_get_new_password_with_head_v1(
                      password_in, heading, allow_empty_result, auto_close_ms));
    if (!AddonIfc::g_ifc->control->GetThreadIfc()->SendMessage(
            funcGroup_gui_dialogs_keyboard_h,
            funcParent_kodi_gui_dialogs_keyboard_show_and_get_new_password_with_head_v1, in, out))
      return false;
    msgpack::unpacked ident = msgpack::unpack(out.data(), out.size());
    msgParent_OUT_kodi_gui_dialogs_keyboard_show_and_get_new_password_with_head_v1 t =
        ident.get().as<decltype(t)>();

    *password_out = strdup(std::get<1>(t).c_str());
    return std::get<0>(t);
  }
#endif /* !KODI_INHIBIT_SHARED */

  const auto& auto_gen_group = AddonIfc::g_ifc->direct->to_kodi.gui_dialogs_keyboard_h;
  return auto_gen_group.kodi_gui_dialogs_keyboard_show_and_get_new_password_with_head_v1(
      auto_gen_group.thisClassHdl, password_in, password_out, heading, allow_empty_result,
      auto_close_ms);
}

ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_get_new_password(const char* password_in,
                                                                         char** password_out,
                                                                         unsigned int auto_close_ms)
{
  // Original API call: ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_get_new_password( const char* password_in, char** password_out, unsigned int auto_close_ms) __INTRODUCED_IN_KODI(1);
  // Tuple in:          typedef std::tuple<std::string, unsigned int> msgParent__IN_kodi_gui_dialogs_keyboard_show_and_get_new_password_v1; /* Autogenerated */
  // Tuple out:         typedef std::tuple<bool, std::string> msgParent_OUT_kodi_gui_dialogs_keyboard_show_and_get_new_password_v1; /* Autogenerated */

  if (password_in == nullptr || password_out == nullptr)
    return false;

#ifndef KODI_INHIBIT_SHARED
  if (!AddonIfc::g_ifc->direct_used)
  {
    msgpack::sbuffer in;
    msgpack::sbuffer out;
    msgpack::pack(in, msgParent__IN_kodi_gui_dialogs_keyboard_show_and_get_new_password_v1(
                          password_in, auto_close_ms));
    if (!AddonIfc::g_ifc->control->GetThreadIfc()->SendMessage(
            funcGroup_gui_dialogs_keyboard_h,
            funcParent_kodi_gui_dialogs_keyboard_show_and_get_new_password_v1, in, out))
      return false;
    msgpack::unpacked ident = msgpack::unpack(out.data(), out.size());
    msgParent_OUT_kodi_gui_dialogs_keyboard_show_and_get_new_password_v1 t =
        ident.get().as<decltype(t)>();

    *password_out = strdup(std::get<1>(t).c_str());
    return std::get<0>(t);
  }
#endif /* !KODI_INHIBIT_SHARED */

  const auto& auto_gen_group = AddonIfc::g_ifc->direct->to_kodi.gui_dialogs_keyboard_h;
  return auto_gen_group.kodi_gui_dialogs_keyboard_show_and_get_new_password_v1(
      auto_gen_group.thisClassHdl, password_in, password_out, auto_close_ms);
}

ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_verify_new_password_with_head(
    char** password_out, const char* heading, bool allow_empty_result, unsigned int auto_close_ms)
{
  // Original API call: ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_verify_new_password_with_head( char** password_out, const char* heading, bool allow_empty_result, unsigned int auto_close_ms) __INTRODUCED_IN_KODI(1);
  // Tuple in:          typedef std::tuple<std::string, bool, unsigned int> msgParent__IN_kodi_gui_dialogs_keyboard_show_and_verify_new_password_with_head_v1; /* Autogenerated */
  // Tuple out:         typedef std::tuple<bool, std::string> msgParent_OUT_kodi_gui_dialogs_keyboard_show_and_verify_new_password_with_head_v1; /* Autogenerated */

  if (password_out == nullptr || heading == nullptr)
    return false;

#ifndef KODI_INHIBIT_SHARED
  if (!AddonIfc::g_ifc->direct_used)
  {
    msgpack::sbuffer in;
    msgpack::sbuffer out;
    msgpack::pack(in,
                  msgParent__IN_kodi_gui_dialogs_keyboard_show_and_verify_new_password_with_head_v1(
                      heading, allow_empty_result, auto_close_ms));
    if (!AddonIfc::g_ifc->control->GetThreadIfc()->SendMessage(
            funcGroup_gui_dialogs_keyboard_h,
            funcParent_kodi_gui_dialogs_keyboard_show_and_verify_new_password_with_head_v1, in,
            out))
      return false;
    msgpack::unpacked ident = msgpack::unpack(out.data(), out.size());
    msgParent_OUT_kodi_gui_dialogs_keyboard_show_and_verify_new_password_with_head_v1 t =
        ident.get().as<decltype(t)>();

    *password_out = strdup(std::get<1>(t).c_str());
    return std::get<0>(t);
  }
#endif /* !KODI_INHIBIT_SHARED */

  const auto& auto_gen_group = AddonIfc::g_ifc->direct->to_kodi.gui_dialogs_keyboard_h;
  return auto_gen_group.kodi_gui_dialogs_keyboard_show_and_verify_new_password_with_head_v1(
      auto_gen_group.thisClassHdl, password_out, heading, allow_empty_result, auto_close_ms);
}

ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_verify_new_password(
    char** password_out, unsigned int auto_close_ms)
{
  // Original API call: ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_verify_new_password( char** password_out, unsigned int auto_close_ms) __INTRODUCED_IN_KODI(1);
  // Tuple in:          typedef std::tuple<unsigned int> msgParent__IN_kodi_gui_dialogs_keyboard_show_and_verify_new_password_v1; /* Autogenerated */
  // Tuple out:         typedef std::tuple<bool, std::string> msgParent_OUT_kodi_gui_dialogs_keyboard_show_and_verify_new_password_v1; /* Autogenerated */

  if (password_out == nullptr)
    return false;

#ifndef KODI_INHIBIT_SHARED
  if (!AddonIfc::g_ifc->direct_used)
  {
    msgpack::sbuffer in;
    msgpack::sbuffer out;
    msgpack::pack(
        in, msgParent__IN_kodi_gui_dialogs_keyboard_show_and_verify_new_password_v1(auto_close_ms));
    if (!AddonIfc::g_ifc->control->GetThreadIfc()->SendMessage(
            funcGroup_gui_dialogs_keyboard_h,
            funcParent_kodi_gui_dialogs_keyboard_show_and_verify_new_password_v1, in, out))
      return false;
    msgpack::unpacked ident = msgpack::unpack(out.data(), out.size());
    msgParent_OUT_kodi_gui_dialogs_keyboard_show_and_verify_new_password_v1 t =
        ident.get().as<decltype(t)>();

    *password_out = strdup(std::get<1>(t).c_str());
    return std::get<0>(t);
  }
#endif /* !KODI_INHIBIT_SHARED */

  const auto& auto_gen_group = AddonIfc::g_ifc->direct->to_kodi.gui_dialogs_keyboard_h;
  return auto_gen_group.kodi_gui_dialogs_keyboard_show_and_verify_new_password_v1(
      auto_gen_group.thisClassHdl, password_out, auto_close_ms);
}

ATTR_DLL_EXPORT int kodi_gui_dialogs_keyboard_show_and_verify_password(const char* password_in,
                                                                       char** password_out,
                                                                       const char* heading,
                                                                       int retries,
                                                                       unsigned int auto_close_ms)
{
  // Original API call: ATTR_DLL_EXPORT int kodi_gui_dialogs_keyboard_show_and_verify_password(const char* password_in, char** password_out, const char* heading, int retries, unsigned int auto_close_ms) __INTRODUCED_IN_KODI(1);
  // Tuple in:          typedef std::tuple<std::string, std::string, int, unsigned int> msgParent__IN_kodi_gui_dialogs_keyboard_show_and_verify_password_v1; /* Autogenerated */
  // Tuple out:         typedef std::tuple<int, std::string> msgParent_OUT_kodi_gui_dialogs_keyboard_show_and_verify_password_v1; /* Autogenerated */

  if (password_in == nullptr || password_out == nullptr || heading == nullptr)
    return -1;

#ifndef KODI_INHIBIT_SHARED
  if (!AddonIfc::g_ifc->direct_used)
  {
    msgpack::sbuffer in;
    msgpack::sbuffer out;
    msgpack::pack(in, msgParent__IN_kodi_gui_dialogs_keyboard_show_and_verify_password_v1(
                          password_in, heading, retries, auto_close_ms));
    if (!AddonIfc::g_ifc->control->GetThreadIfc()->SendMessage(
            funcGroup_gui_dialogs_keyboard_h,
            funcParent_kodi_gui_dialogs_keyboard_show_and_verify_password_v1, in, out))
      return -1;
    msgpack::unpacked ident = msgpack::unpack(out.data(), out.size());
    msgParent_OUT_kodi_gui_dialogs_keyboard_show_and_verify_password_v1 t =
        ident.get().as<decltype(t)>();

    *password_out = strdup(std::get<1>(t).c_str());
    return std::get<0>(t);
  }
#endif /* !KODI_INHIBIT_SHARED */

  const auto& auto_gen_group = AddonIfc::g_ifc->direct->to_kodi.gui_dialogs_keyboard_h;
  return auto_gen_group.kodi_gui_dialogs_keyboard_show_and_verify_password_v1(
      auto_gen_group.thisClassHdl, password_in, password_out, heading, retries, auto_close_ms);
}

ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_get_filter(const char* text_in,
                                                                   char** text_out,
                                                                   bool searching,
                                                                   unsigned int auto_close_ms)
{
  // Original API call: ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_show_and_get_filter(const char* text_in, char** text_out, bool searching, unsigned int auto_close_ms) __INTRODUCED_IN_KODI(1);
  // Tuple in:          typedef std::tuple<std::string, bool, unsigned int> msgParent__IN_kodi_gui_dialogs_keyboard_show_and_get_filter_v1; /* Autogenerated */
  // Tuple out:         typedef std::tuple<bool, std::string> msgParent_OUT_kodi_gui_dialogs_keyboard_show_and_get_filter_v1; /* Autogenerated */

  if (text_in == nullptr || text_out == nullptr)
    return false;

#ifndef KODI_INHIBIT_SHARED
  if (!AddonIfc::g_ifc->direct_used)
  {
    msgpack::sbuffer in;
    msgpack::sbuffer out;
    msgpack::pack(in, msgParent__IN_kodi_gui_dialogs_keyboard_show_and_get_filter_v1(
                          text_in, searching, auto_close_ms));
    if (!AddonIfc::g_ifc->control->GetThreadIfc()->SendMessage(
            funcGroup_gui_dialogs_keyboard_h,
            funcParent_kodi_gui_dialogs_keyboard_show_and_get_filter_v1, in, out))
      return false;
    msgpack::unpacked ident = msgpack::unpack(out.data(), out.size());
    msgParent_OUT_kodi_gui_dialogs_keyboard_show_and_get_filter_v1 t =
        ident.get().as<decltype(t)>();

    *text_out = strdup(std::get<1>(t).c_str());
    return std::get<0>(t);
  }
#endif /* !KODI_INHIBIT_SHARED */

  const auto& auto_gen_group = AddonIfc::g_ifc->direct->to_kodi.gui_dialogs_keyboard_h;
  return auto_gen_group.kodi_gui_dialogs_keyboard_show_and_get_filter_v1(
      auto_gen_group.thisClassHdl, text_in, text_out, searching, auto_close_ms);
}

ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_send_text_to_active_keyboard(const char* text,
                                                                            bool close_keyboard)
{
  // Original API call: ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_send_text_to_active_keyboard(const char* text, bool close_keyboard) __INTRODUCED_IN_KODI(1);
  // Tuple in:          typedef std::tuple<std::string, bool> msgParent__IN_kodi_gui_dialogs_keyboard_send_text_to_active_keyboard_v1; /* Autogenerated */
  // Tuple out:         typedef std::tuple<bool> msgParent_OUT_kodi_gui_dialogs_keyboard_send_text_to_active_keyboard_v1; /* Autogenerated */

  if (text == nullptr)
    return false;

#ifndef KODI_INHIBIT_SHARED
  if (!AddonIfc::g_ifc->direct_used)
  {
    msgpack::sbuffer in;
    msgpack::sbuffer out;
    msgpack::pack(in, msgParent__IN_kodi_gui_dialogs_keyboard_send_text_to_active_keyboard_v1(
                          text, close_keyboard));
    if (!AddonIfc::g_ifc->control->GetThreadIfc()->SendMessage(
            funcGroup_gui_dialogs_keyboard_h,
            funcParent_kodi_gui_dialogs_keyboard_send_text_to_active_keyboard_v1, in, out))
      return false;
    msgpack::unpacked ident = msgpack::unpack(out.data(), out.size());
    msgParent_OUT_kodi_gui_dialogs_keyboard_send_text_to_active_keyboard_v1 t =
        ident.get().as<decltype(t)>();

    return std::get<0>(t);
  }
#endif /* !KODI_INHIBIT_SHARED */

  const auto& auto_gen_group = AddonIfc::g_ifc->direct->to_kodi.gui_dialogs_keyboard_h;
  return auto_gen_group.kodi_gui_dialogs_keyboard_send_text_to_active_keyboard_v1(
      auto_gen_group.thisClassHdl, text, close_keyboard);
}

ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_is_keyboard_activated()
{
  // Original API call: ATTR_DLL_EXPORT bool kodi_gui_dialogs_keyboard_is_keyboard_activated() __INTRODUCED_IN_KODI(1);
  // Tuple in:          typedef std::tuple<DummyValue> msgParent__IN_kodi_gui_dialogs_keyboard_is_keyboard_activated_v1; /* Autogenerated */
  // Tuple out:         typedef std::tuple<bool> msgParent_OUT_kodi_gui_dialogs_keyboard_is_keyboard_activated_v1; /* Autogenerated */

#ifndef KODI_INHIBIT_SHARED
  if (!AddonIfc::g_ifc->direct_used)
  {
    msgpack::sbuffer out;
    if (!AddonIfc::g_ifc->control->GetThreadIfc()->SendMessageOnlyGetReturn(
            funcGroup_gui_dialogs_keyboard_h,
            funcParent_kodi_gui_dialogs_keyboard_is_keyboard_activated_v1, out))
      return false;
    msgpack::unpacked ident = msgpack::unpack(out.data(), out.size());
    msgParent_OUT_kodi_gui_dialogs_keyboard_is_keyboard_activated_v1 t =
        ident.get().as<decltype(t)>();

    return std::get<0>(t);
  }
#endif /* !KODI_INHIBIT_SHARED */

  const auto& auto_gen_group = AddonIfc::g_ifc->direct->to_kodi.gui_dialogs_keyboard_h;
  return auto_gen_group.kodi_gui_dialogs_keyboard_is_keyboard_activated_v1(
      auto_gen_group.thisClassHdl);
}

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_4>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_4_END>---*/
