/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/* File autogenerated, see xbmc/addons/kodi-dev-kit/tools/code-generator/code_generator.py */

#pragma once

#include "core/IMsgHdl.h"
#include "src/shared/api/gui/controls/fade_label.h"

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1_END>---*/

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

struct directFuncToAddon_gui_controls_fade_label_h;
struct directFuncToKodi_gui_controls_fade_label_h;

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */

namespace KODI_ADDON
{
namespace INTERNAL
{

class ATTR_DLL_LOCAL CHdl_kodi_gui_controls_fade_label_h : public IMsgHdl
{
public:
  CHdl_kodi_gui_controls_fade_label_h(AddonIfc* ifc) : IMsgHdl(ifc) {}

  void InitDirect(KODI::ADDONS::INTERFACE::directFuncToAddon_gui_controls_fade_label_h* ifcToAddon);

  /* Code below becomes on auto generation reused, hand edited parts should included here */
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_2>---*/
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_2_END>---*/
};

} /* namespace INTERNAL */
} /* namespace KODI_ADDON */
