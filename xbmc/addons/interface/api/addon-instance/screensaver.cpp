/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/* File autogenerated, see xbmc/addons/kodi-dev-kit/tools/code-generator/code_generator.py */

#include "screensaver.h"

// Shared API
#include "addons/kodi-dev-kit/src/shared/api/addon-instance/screensaver.h"

// Kodi
#include "addons/ScreenSaver.h"
#include "addons/interface/IOffscreenRenderProcess.h"
#include "addons/interface/RunningProcess.h"
#include "utils/log.h"

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

void CHdl_kodi_addoninstance_screensaver_h::InitDirect(
    directFuncToKodi_addoninstance_screensaver_h* ifcToKodi,
    directFuncToAddon_addoninstance_screensaver_h* ifcToAddon)
{
  ifcToKodi->thisClassHdl = this;
  m_ifcToAddon = ifcToAddon;
  ifcToKodi->kodi_addon_screensaver_get_properties_v1 = kodi_addon_screensaver_get_properties_v1;
}

bool CHdl_kodi_addoninstance_screensaver_h::HandleMessage(int funcGroup,
                                                          int func,
                                                          const msgpack::unpacked& in,
                                                          msgpack::sbuffer& out)
{
  if (funcGroup != funcGroup_addoninstance_screensaver_h)
    return false;

  switch (func)
  {
    case funcParent_kodi_addon_screensaver_get_properties_v1:
    {
      // Original API call: ATTR_DLL_EXPORT void kodi_addon_screensaver_get_properties( const KODI_ADDON_INSTANCE_BACKEND_HDL hdl, struct KODI_ADDON_SCREENSAVER_PROPS* props) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<PtrValue> msgParent__IN_kodi_addon_screensaver_get_properties_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<IFC_KODI_ADDON_SCREENSAVER_PROPS> msgParent_OUT_kodi_addon_screensaver_get_properties_v1; /* Autogenerated */
      msgParent__IN_kodi_addon_screensaver_get_properties_v1 t = in.get().as<decltype(t)>();
      KODI_ADDON_INSTANCE_BACKEND_HDL hdl =
          reinterpret_cast<KODI_ADDON_INSTANCE_BACKEND_HDL>(std::get<0>(t));
      struct KODI_ADDON_SCREENSAVER_PROPS props = {};
      kodi_addon_screensaver_get_properties_v1(this, hdl, &props);
      msgpack::pack(out, msgParent_OUT_kodi_addon_screensaver_get_properties_v1(&props));
      return true;
    }
    default:
      CLog::Log(LOGERROR,
                "CHdl_kodi_addoninstance_screensaver_h::{}: addon called with unknown function id "
                "'{}' on group 'addoninstance_screensaver_h'",
                __func__, func);
  }

  return false;
}

// Function calls from Kodi to addon

bool CHdl_kodi_addoninstance_screensaver_h::kodi_addon_screensaver_start_v1(
    IOffscreenRenderProcess* proc, const KODI_ADDON_SCREENSAVER_HDL hdl)
{
  // Original API call: typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_SCREENSAVER_START_V1)(const KODI_ADDON_SCREENSAVER_HDL hdl);
  // Tuple in:          typedef std::tuple<PtrKodiOffscreen, uint64_t, int, int, int, int, PtrValue /* < Offscreen render */, PtrValue> msgChild__IN_kodi_addon_screensaver_start_v1; /* Autogenerated */
  // Tuple out:         typedef std::tuple<bool, PtrAddonOffscreen, bool /* < Offscreen render */> msgChild_OUT_kodi_addon_screensaver_start_v1; /* Autogenerated */

#ifndef KODI_INHIBIT_SHARED
  if (!m_interface.m_directDll)
  {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    ADDON_HARDWARE_CONTEXT context = nullptr;
    if (!proc || !proc->GetOffscreenRenderInfos(x, y, width, height, context) ||
        !proc->CreateRenderHelper(m_process))
      return false;

    msgpack::sbuffer in;
    msgpack::sbuffer out;
    msgpack::pack(in, msgChild__IN_kodi_addon_screensaver_start_v1(
                          PtrKodiOffscreen(proc), proc->GetUsedId(), x, y, width, height,
                          PtrValue(context), PtrValue(hdl)));
    if (!m_process->SendMessage(funcGroup_addoninstance_screensaver_h,
                                funcChild_kodi_addon_screensaver_start_v1, in, out))
      return false;
    msgpack::unpacked ident = msgpack::unpack(out.data(), out.size());
    msgChild_OUT_kodi_addon_screensaver_start_v1 t = ident.get().as<decltype(t)>();
    if (std::get<1>(t) == 0 || std::get<2>(t) == false)
      return false;
    proc->SetAddonOffscreenHdl(std::get<1>(t));
    proc->GetRenderHelper()->Init();
    return std::get<0>(t);
  }
#endif /* !KODI_INHIBIT_SHARED */

  return m_ifcToAddon->kodi_addon_screensaver_start_v1(m_ifcToAddon->thisClassHdl, hdl);
}

void CHdl_kodi_addoninstance_screensaver_h::kodi_addon_screensaver_stop_v1(
    IOffscreenRenderProcess* proc, const KODI_ADDON_SCREENSAVER_HDL hdl)
{
  // Original API call: typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_SCREENSAVER_STOP_V1)(const KODI_ADDON_SCREENSAVER_HDL hdl);
  // Tuple in:          typedef std::tuple<PtrAddonOffscreen /* < Offscreen render */, PtrValue> msgChild__IN_kodi_addon_screensaver_stop_v1; /* Autogenerated */
  // Tuple out:         typedef std::tuple<DummyValue> msgChild_OUT_kodi_addon_screensaver_stop_v1; /* Autogenerated */

#ifndef KODI_INHIBIT_SHARED
  if (!m_interface.m_directDll)
  {
    if (!proc)
      return;

    msgpack::sbuffer in;
    msgpack::pack(in, msgChild__IN_kodi_addon_screensaver_stop_v1(proc->GetAddonOffscreenHdl(),
                                                                  PtrValue(hdl)));
    m_process->SendMessage(funcGroup_addoninstance_screensaver_h,
                           funcChild_kodi_addon_screensaver_stop_v1, in);

    proc->GetRenderHelper()->Deinit();
    return;
  }
#endif /* !KODI_INHIBIT_SHARED */

  m_ifcToAddon->kodi_addon_screensaver_stop_v1(m_ifcToAddon->thisClassHdl, hdl);
}

void CHdl_kodi_addoninstance_screensaver_h::kodi_addon_screensaver_render_v1(
    IOffscreenRenderProcess* proc, const KODI_ADDON_SCREENSAVER_HDL hdl)
{
  // Original API call: typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_SCREENSAVER_RENDER_V1)(const KODI_ADDON_SCREENSAVER_HDL hdl);
  // Tuple in:          typedef std::tuple<PtrAddonOffscreen /* < Offscreen render */, PtrValue> msgChild__IN_kodi_addon_screensaver_render_v1; /* Autogenerated */
  // Tuple out:         typedef std::tuple<DummyValue> msgChild_OUT_kodi_addon_screensaver_render_v1; /* Autogenerated */

#ifndef KODI_INHIBIT_SHARED
  if (!m_interface.m_directDll)
  {
    if (!proc)
      return;

    proc->GetRenderHelper()->Begin();

    msgpack::sbuffer in;
    msgpack::pack(in, msgChild__IN_kodi_addon_screensaver_render_v1(proc->GetAddonOffscreenHdl(),
                                                                    PtrValue(hdl)));
    m_process->SendMessage(funcGroup_addoninstance_screensaver_h,
                           funcChild_kodi_addon_screensaver_render_v1, in);

    proc->GetRenderHelper()->End();
    return;
  }
#endif /* !KODI_INHIBIT_SHARED */

  m_ifcToAddon->kodi_addon_screensaver_render_v1(m_ifcToAddon->thisClassHdl, hdl);
}

// Callbacks from addon to Kodi

void CHdl_kodi_addoninstance_screensaver_h::kodi_addon_screensaver_get_properties_v1(
    void* thisClassHdl,
    const KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
    struct KODI_ADDON_SCREENSAVER_PROPS* props)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_addoninstance_screensaver_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_ADDON_SCREENSAVER_GET_PROPERTIES>---*/
  // AUTOGENERATED - Used by class "KODI::ADDONS::CScreenSaver" with "void GetProperties(struct KODI_ADDON_SCREENSAVER_PROPS* props)" on "addons/ScreenSaver.h".

  KODI::ADDONS::CScreenSaver* cb = static_cast<KODI::ADDONS::CScreenSaver*>(hdl);
  cb->GetProperties(props);
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_ADDON_SCREENSAVER_GET_PROPERTIES_END>---*/
}

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3_END>---*/

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
