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

void CHdl_kodi_gui_controls_image_h::InitDirect(directFuncToAddon_gui_controls_image_h* ifcToAddon)
{
  ifcToAddon->thisClassHdl = this;
}

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3_END>---*/

/* NOTE: unused (no functions from Kodi to addon here) */

} /* namespace INTERNAL */
} /* namespace KODI_ADDON */

ATTR_DLL_EXPORT void kodi_gui_controls_image_set_visible(KODI_GUI_CONTROL_HANDLE handle,
                                                         bool visible)
{
  // Original API call: ATTR_DLL_EXPORT void kodi_gui_controls_image_set_visible(KODI_GUI_CONTROL_HANDLE handle, bool visible) __INTRODUCED_IN_KODI(1);
  // Tuple in:          typedef std::tuple<PtrValue, bool> msgParent__IN_kodi_gui_controls_image_set_visible_v1; /* Autogenerated */
  // Tuple out:         typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_controls_image_set_visible_v1; /* Autogenerated */

  if (handle == nullptr)
    return;

#ifndef KODI_INHIBIT_SHARED
  if (!AddonIfc::g_ifc->direct_used)
  {
    msgpack::sbuffer in;
    msgpack::pack(in,
                  msgParent__IN_kodi_gui_controls_image_set_visible_v1(PtrValue(handle), visible));
    AddonIfc::g_ifc->control->GetThreadIfc()->SendMessage(
        funcGroup_gui_controls_image_h, funcParent_kodi_gui_controls_image_set_visible_v1, in);
    return;
  }
#endif /* !KODI_INHIBIT_SHARED */

  const auto& auto_gen_group = AddonIfc::g_ifc->direct->to_kodi.gui_controls_image_h;
  auto_gen_group.kodi_gui_controls_image_set_visible_v1(auto_gen_group.thisClassHdl, handle,
                                                        visible);
}

ATTR_DLL_EXPORT void kodi_gui_controls_image_set_filename(KODI_GUI_CONTROL_HANDLE handle,
                                                          const char* filename,
                                                          bool use_cache)
{
  // Original API call: ATTR_DLL_EXPORT void kodi_gui_controls_image_set_filename(KODI_GUI_CONTROL_HANDLE handle, const char* filename, bool use_cache) __INTRODUCED_IN_KODI(1);
  // Tuple in:          typedef std::tuple<PtrValue, std::string, bool> msgParent__IN_kodi_gui_controls_image_set_filename_v1; /* Autogenerated */
  // Tuple out:         typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_controls_image_set_filename_v1; /* Autogenerated */

  if (handle == nullptr || filename == nullptr)
    return;

#ifndef KODI_INHIBIT_SHARED
  if (!AddonIfc::g_ifc->direct_used)
  {
    msgpack::sbuffer in;
    msgpack::pack(in, msgParent__IN_kodi_gui_controls_image_set_filename_v1(PtrValue(handle),
                                                                            filename, use_cache));
    AddonIfc::g_ifc->control->GetThreadIfc()->SendMessage(
        funcGroup_gui_controls_image_h, funcParent_kodi_gui_controls_image_set_filename_v1, in);
    return;
  }
#endif /* !KODI_INHIBIT_SHARED */

  const auto& auto_gen_group = AddonIfc::g_ifc->direct->to_kodi.gui_controls_image_h;
  auto_gen_group.kodi_gui_controls_image_set_filename_v1(auto_gen_group.thisClassHdl, handle,
                                                         filename, use_cache);
}

ATTR_DLL_EXPORT void kodi_gui_controls_image_set_color_diffuse(KODI_GUI_CONTROL_HANDLE handle,
                                                               uint32_t color_diffuse)
{
  // Original API call: ATTR_DLL_EXPORT void kodi_gui_controls_image_set_color_diffuse(KODI_GUI_CONTROL_HANDLE handle, uint32_t color_diffuse) __INTRODUCED_IN_KODI(1);
  // Tuple in:          typedef std::tuple<PtrValue, uint32_t> msgParent__IN_kodi_gui_controls_image_set_color_diffuse_v1; /* Autogenerated */
  // Tuple out:         typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_controls_image_set_color_diffuse_v1; /* Autogenerated */

  if (handle == nullptr)
    return;

#ifndef KODI_INHIBIT_SHARED
  if (!AddonIfc::g_ifc->direct_used)
  {
    msgpack::sbuffer in;
    msgpack::pack(in, msgParent__IN_kodi_gui_controls_image_set_color_diffuse_v1(PtrValue(handle),
                                                                                 color_diffuse));
    AddonIfc::g_ifc->control->GetThreadIfc()->SendMessage(
        funcGroup_gui_controls_image_h, funcParent_kodi_gui_controls_image_set_color_diffuse_v1,
        in);
    return;
  }
#endif /* !KODI_INHIBIT_SHARED */

  const auto& auto_gen_group = AddonIfc::g_ifc->direct->to_kodi.gui_controls_image_h;
  auto_gen_group.kodi_gui_controls_image_set_color_diffuse_v1(auto_gen_group.thisClassHdl, handle,
                                                              color_diffuse);
}

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_4>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_4_END>---*/
