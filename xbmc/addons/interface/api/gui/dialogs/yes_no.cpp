/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/* File autogenerated, see xbmc/addons/kodi-dev-kit/tools/code-generator/code_generator.py */

#include "yes_no.h"

// Shared API
#include "addons/kodi-dev-kit/src/shared/api/gui/dialogs/yes_no.h"

// Kodi
#include "addons/interface/RunningProcess.h"
#include "utils/log.h"

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1>---*/

#include "dialogs/GUIDialogYesNo.h"
#include "messaging/helpers/DialogHelper.h"

using namespace KODI::MESSAGING;
using KODI::MESSAGING::HELPERS::DialogResponse;

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

void CHdl_kodi_gui_dialogs_yes_no_h::InitDirect(directFuncToKodi_gui_dialogs_yes_no_h* ifcToKodi,
                                                directFuncToAddon_gui_dialogs_yes_no_h* ifcToAddon)
{
  ifcToKodi->thisClassHdl = this;
  m_ifcToAddon = ifcToAddon;
  ifcToKodi->kodi_gui_dialogs_yesno_show_and_get_input_single_text_v1 =
      kodi_gui_dialogs_yesno_show_and_get_input_single_text_v1;
  ifcToKodi->kodi_gui_dialogs_yesno_show_and_get_input_line_text_v1 =
      kodi_gui_dialogs_yesno_show_and_get_input_line_text_v1;
  ifcToKodi->kodi_gui_dialogs_yesno_show_and_get_input_line_button_text_v1 =
      kodi_gui_dialogs_yesno_show_and_get_input_line_button_text_v1;
}

bool CHdl_kodi_gui_dialogs_yes_no_h::HandleMessage(int funcGroup,
                                                   int func,
                                                   const msgpack::unpacked& in,
                                                   msgpack::sbuffer& out)
{
  if (funcGroup != funcGroup_gui_dialogs_yes_no_h)
    return false;

  switch (func)
  {
    case funcParent_kodi_gui_dialogs_yesno_show_and_get_input_single_text_v1:
    {
      // Original API call: ATTR_DLL_EXPORT bool kodi_gui_dialogs_yesno_show_and_get_input_single_text(const char* heading, const char* text, bool* canceled, const char* noLabel, const char* yesLabel) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<std::string, std::string, bool, std::string, std::string> msgParent__IN_kodi_gui_dialogs_yesno_show_and_get_input_single_text_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<bool, bool> msgParent_OUT_kodi_gui_dialogs_yesno_show_and_get_input_single_text_v1; /* Autogenerated */
      msgParent__IN_kodi_gui_dialogs_yesno_show_and_get_input_single_text_v1 t =
          in.get().as<decltype(t)>();
      const std::string& heading = std::get<0>(t);
      const std::string& text = std::get<1>(t);
      bool canceled = std::get<2>(t);
      const std::string& noLabel = std::get<3>(t);
      const std::string& yesLabel = std::get<4>(t);
      bool auto_gen_ret = kodi_gui_dialogs_yesno_show_and_get_input_single_text_v1(
          this, heading.c_str(), text.c_str(), &canceled, noLabel.c_str(), yesLabel.c_str());
      msgpack::pack(out, msgParent_OUT_kodi_gui_dialogs_yesno_show_and_get_input_single_text_v1(
                             auto_gen_ret, canceled));
      return true;
    }
    case funcParent_kodi_gui_dialogs_yesno_show_and_get_input_line_text_v1:
    {
      // Original API call: ATTR_DLL_EXPORT bool kodi_gui_dialogs_yesno_show_and_get_input_line_text(const char* heading, const char* line0, const char* line1, const char* line2, const char* noLabel, const char* yesLabel) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<std::string, std::string, std::string, std::string, std::string, std::string> msgParent__IN_kodi_gui_dialogs_yesno_show_and_get_input_line_text_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<bool> msgParent_OUT_kodi_gui_dialogs_yesno_show_and_get_input_line_text_v1; /* Autogenerated */
      msgParent__IN_kodi_gui_dialogs_yesno_show_and_get_input_line_text_v1 t =
          in.get().as<decltype(t)>();
      const std::string& heading = std::get<0>(t);
      const std::string& line0 = std::get<1>(t);
      const std::string& line1 = std::get<2>(t);
      const std::string& line2 = std::get<3>(t);
      const std::string& noLabel = std::get<4>(t);
      const std::string& yesLabel = std::get<5>(t);
      bool auto_gen_ret = kodi_gui_dialogs_yesno_show_and_get_input_line_text_v1(
          this, heading.c_str(), line0.c_str(), line1.c_str(), line2.c_str(), noLabel.c_str(),
          yesLabel.c_str());
      msgpack::pack(
          out, msgParent_OUT_kodi_gui_dialogs_yesno_show_and_get_input_line_text_v1(auto_gen_ret));
      return true;
    }
    case funcParent_kodi_gui_dialogs_yesno_show_and_get_input_line_button_text_v1:
    {
      // Original API call: ATTR_DLL_EXPORT bool kodi_gui_dialogs_yesno_show_and_get_input_line_button_text( const char* heading, const char* line0, const char* line1, const char* line2, bool* canceled, const char* noLabel, const char* yesLabel) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<std::string, std::string, std::string, std::string, bool, std::string, std::string> msgParent__IN_kodi_gui_dialogs_yesno_show_and_get_input_line_button_text_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<bool, bool> msgParent_OUT_kodi_gui_dialogs_yesno_show_and_get_input_line_button_text_v1; /* Autogenerated */
      msgParent__IN_kodi_gui_dialogs_yesno_show_and_get_input_line_button_text_v1 t =
          in.get().as<decltype(t)>();
      const std::string& heading = std::get<0>(t);
      const std::string& line0 = std::get<1>(t);
      const std::string& line1 = std::get<2>(t);
      const std::string& line2 = std::get<3>(t);
      bool canceled = std::get<4>(t);
      const std::string& noLabel = std::get<5>(t);
      const std::string& yesLabel = std::get<6>(t);
      bool auto_gen_ret = kodi_gui_dialogs_yesno_show_and_get_input_line_button_text_v1(
          this, heading.c_str(), line0.c_str(), line1.c_str(), line2.c_str(), &canceled,
          noLabel.c_str(), yesLabel.c_str());
      msgpack::pack(out,
                    msgParent_OUT_kodi_gui_dialogs_yesno_show_and_get_input_line_button_text_v1(
                        auto_gen_ret, canceled));
      return true;
    }
    default:
      CLog::Log(LOGERROR,
                "CHdl_kodi_gui_dialogs_yes_no_h::{}: addon called with unknown function id '{}' on "
                "group 'gui_dialogs_yes_no_h'",
                __func__, func);
  }

  return false;
}

// Function calls from Kodi to addon
/* NOTE: unused (no functions here) */

// Callbacks from addon to Kodi

bool CHdl_kodi_gui_dialogs_yes_no_h::kodi_gui_dialogs_yesno_show_and_get_input_single_text_v1(
    void* thisClassHdl,
    const char* heading,
    const char* text,
    bool* canceled,
    const char* noLabel,
    const char* yesLabel)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_gui_dialogs_yes_no_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return false;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_DIALOGS_YESNO_SHOW_AND_GET_INPUT_SINGLE_TEXT>---*/

  if (!heading || !text || !canceled || !noLabel || !yesLabel)
  {
    CLog::Log(LOGERROR,
              "CHdl_kodi_gui_dialogs_yes_no_h::{} - invalid handler data (heading='{}', text='{}', "
              "canceled='{}', noLabel='{}', yesLabel='{}') on addon '{}'",
              __func__, static_cast<const void*>(heading), static_cast<const void*>(text),
              static_cast<void*>(canceled), static_cast<const void*>(noLabel),
              static_cast<const void*>(yesLabel), thisClass->m_process->GetAddonID());
    return false;
  }

  DialogResponse result = HELPERS::ShowYesNoDialogText(heading, text, noLabel, yesLabel);
  *canceled = (result == DialogResponse::CHOICE_CANCELLED);
  return (result == DialogResponse::CHOICE_YES);

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_DIALOGS_YESNO_SHOW_AND_GET_INPUT_SINGLE_TEXT_END>---*/
}

bool CHdl_kodi_gui_dialogs_yes_no_h::kodi_gui_dialogs_yesno_show_and_get_input_line_text_v1(
    void* thisClassHdl,
    const char* heading,
    const char* line0,
    const char* line1,
    const char* line2,
    const char* noLabel,
    const char* yesLabel)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_gui_dialogs_yes_no_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return false;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_DIALOGS_YESNO_SHOW_AND_GET_INPUT_LINE_TEXT>---*/

  if (!heading || !line0 || !line1 || !line2 || !noLabel || !yesLabel)
  {
    CLog::Log(
        LOGERROR,
        "CHdl_kodi_gui_dialogs_yes_no_h::{} - invalid handler data (heading='{}', line0='{}', "
        "line1='{}', line2='{}', "
        "noLabel='{}', yesLabel='{}') on addon '{}'",
        __func__, static_cast<const void*>(heading), static_cast<const void*>(line0),
        static_cast<const void*>(line1), static_cast<const void*>(line2),
        static_cast<const void*>(noLabel), static_cast<const void*>(yesLabel),
        thisClass->m_process->GetAddonID());
    return false;
  }

  return HELPERS::ShowYesNoDialogLines(heading, line0, line1, line2, noLabel, yesLabel) ==
         DialogResponse::CHOICE_YES;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_DIALOGS_YESNO_SHOW_AND_GET_INPUT_LINE_TEXT_END>---*/
}

bool CHdl_kodi_gui_dialogs_yes_no_h::kodi_gui_dialogs_yesno_show_and_get_input_line_button_text_v1(
    void* thisClassHdl,
    const char* heading,
    const char* line0,
    const char* line1,
    const char* line2,
    bool* canceled,
    const char* noLabel,
    const char* yesLabel)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_gui_dialogs_yes_no_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return false;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_DIALOGS_YESNO_SHOW_AND_GET_INPUT_LINE_BUTTON_TEXT>---*/

  if (!heading || !line0 || !line1 || !line2 || !canceled || !noLabel || !yesLabel)
  {
    CLog::Log(
        LOGERROR,
        "CHdl_kodi_gui_dialogs_yes_no_h::{} - invalid handler data (heading='{}', line0='{}', "
        "line1='{}', line2='{}', "
        "canceled='{}', noLabel='{}', yesLabel='{}') on addon '{}'",
        __func__, static_cast<const void*>(heading), static_cast<const void*>(line0),
        static_cast<const void*>(line1), static_cast<const void*>(line2),
        static_cast<const void*>(canceled), static_cast<const void*>(noLabel),
        static_cast<const void*>(yesLabel), thisClass->m_process->GetAddonID());
    return false;
  }

  DialogResponse result =
      HELPERS::ShowYesNoDialogLines(heading, line0, line1, line2, noLabel, yesLabel);
  *canceled = (result == DialogResponse::CHOICE_CANCELLED);
  return (result == DialogResponse::CHOICE_YES);

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_DIALOGS_YESNO_SHOW_AND_GET_INPUT_LINE_BUTTON_TEXT_END>---*/
}

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3_END>---*/

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
