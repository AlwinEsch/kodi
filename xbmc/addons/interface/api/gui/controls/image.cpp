/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/* File autogenerated, see xbmc/addons/kodi-dev-kit/tools/code-generator/code_generator.py */

#include "image.h"

// Shared API
#include "addons/kodi-dev-kit/src/shared/api/gui/controls/image.h"

// Kodi
#include "addons/interface/RunningProcess.h"
#include "utils/log.h"

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1>---*/

#include "guilib/GUIImage.h"

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

void CHdl_kodi_gui_controls_image_h::InitDirect(directFuncToKodi_gui_controls_image_h* ifcToKodi,
                                                directFuncToAddon_gui_controls_image_h* ifcToAddon)
{
  ifcToKodi->thisClassHdl = this;
  m_ifcToAddon = ifcToAddon;
  ifcToKodi->kodi_gui_controls_image_set_visible_v1 = kodi_gui_controls_image_set_visible_v1;
  ifcToKodi->kodi_gui_controls_image_set_filename_v1 = kodi_gui_controls_image_set_filename_v1;
  ifcToKodi->kodi_gui_controls_image_set_color_diffuse_v1 =
      kodi_gui_controls_image_set_color_diffuse_v1;
}

bool CHdl_kodi_gui_controls_image_h::HandleMessage(int funcGroup,
                                                   int func,
                                                   const msgpack::unpacked& in,
                                                   msgpack::sbuffer& out)
{
  if (funcGroup != funcGroup_gui_controls_image_h)
    return false;

  switch (func)
  {
    case funcParent_kodi_gui_controls_image_set_visible_v1:
    {
      // Original API call: ATTR_DLL_EXPORT void kodi_gui_controls_image_set_visible(KODI_GUI_CONTROL_HANDLE handle, bool visible) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<PtrValue, bool> msgParent__IN_kodi_gui_controls_image_set_visible_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_controls_image_set_visible_v1; /* Autogenerated */
      msgParent__IN_kodi_gui_controls_image_set_visible_v1 t = in.get().as<decltype(t)>();
      KODI_GUI_CONTROL_HANDLE handle = reinterpret_cast<KODI_GUI_CONTROL_HANDLE>(std::get<0>(t));
      bool visible = std::get<1>(t);
      kodi_gui_controls_image_set_visible_v1(this, handle, visible);
      return true;
    }
    case funcParent_kodi_gui_controls_image_set_filename_v1:
    {
      // Original API call: ATTR_DLL_EXPORT void kodi_gui_controls_image_set_filename(KODI_GUI_CONTROL_HANDLE handle, const char* filename, bool use_cache) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<PtrValue, std::string, bool> msgParent__IN_kodi_gui_controls_image_set_filename_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_controls_image_set_filename_v1; /* Autogenerated */
      msgParent__IN_kodi_gui_controls_image_set_filename_v1 t = in.get().as<decltype(t)>();
      KODI_GUI_CONTROL_HANDLE handle = reinterpret_cast<KODI_GUI_CONTROL_HANDLE>(std::get<0>(t));
      const std::string& filename = std::get<1>(t);
      bool use_cache = std::get<2>(t);
      kodi_gui_controls_image_set_filename_v1(this, handle, filename.c_str(), use_cache);
      return true;
    }
    case funcParent_kodi_gui_controls_image_set_color_diffuse_v1:
    {
      // Original API call: ATTR_DLL_EXPORT void kodi_gui_controls_image_set_color_diffuse(KODI_GUI_CONTROL_HANDLE handle, uint32_t color_diffuse) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<PtrValue, uint32_t> msgParent__IN_kodi_gui_controls_image_set_color_diffuse_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_controls_image_set_color_diffuse_v1; /* Autogenerated */
      msgParent__IN_kodi_gui_controls_image_set_color_diffuse_v1 t = in.get().as<decltype(t)>();
      KODI_GUI_CONTROL_HANDLE handle = reinterpret_cast<KODI_GUI_CONTROL_HANDLE>(std::get<0>(t));
      uint32_t color_diffuse = std::get<1>(t);
      kodi_gui_controls_image_set_color_diffuse_v1(this, handle, color_diffuse);
      return true;
    }
    default:
      CLog::Log(LOGERROR,
                "CHdl_kodi_gui_controls_image_h::{}: addon called with unknown function id '{}' on "
                "group 'gui_controls_image_h'",
                __func__, func);
  }

  return false;
}

// Function calls from Kodi to addon
/* NOTE: unused (no functions here) */

// Callbacks from addon to Kodi

void CHdl_kodi_gui_controls_image_h::kodi_gui_controls_image_set_visible_v1(
    void* thisClassHdl, KODI_GUI_CONTROL_HANDLE handle, bool visible)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_gui_controls_image_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_CONTROLS_IMAGE_SET_VISIBLE>---*/

  static_cast<CGUIImage*>(handle)->SetVisible(visible);

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_CONTROLS_IMAGE_SET_VISIBLE_END>---*/
}

void CHdl_kodi_gui_controls_image_h::kodi_gui_controls_image_set_filename_v1(
    void* thisClassHdl, KODI_GUI_CONTROL_HANDLE handle, const char* filename, bool use_cache)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_gui_controls_image_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_CONTROLS_IMAGE_SET_FILENAME>---*/

  static_cast<CGUIImage*>(handle)->SetFileName(filename, false, use_cache);

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_CONTROLS_IMAGE_SET_FILENAME_END>---*/
}

void CHdl_kodi_gui_controls_image_h::kodi_gui_controls_image_set_color_diffuse_v1(
    void* thisClassHdl, KODI_GUI_CONTROL_HANDLE handle, uint32_t color_diffuse)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_gui_controls_image_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_CONTROLS_IMAGE_SET_COLOR_DIFFUSE>---*/

  static_cast<CGUIImage*>(handle)->SetColorDiffuse(GUILIB::GUIINFO::CGUIInfoColor(color_diffuse));

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_CONTROLS_IMAGE_SET_COLOR_DIFFUSE_END>---*/
}

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3_END>---*/

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
