/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/* File autogenerated, see xbmc/addons/kodi-dev-kit/tools/code-generator/code_generator.py */

#pragma once

#include "addons/interface/IMsgHdl.h"

// API include
#include "addons/kodi-dev-kit/include/kodi/c-api/addon-instance/imagedecoder.h"

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1_END>---*/

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

struct directFuncToAddon_addoninstance_imagedecoder_h;
struct directFuncToKodi_addoninstance_imagedecoder_h;

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_2>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_2_END>---*/

class CHdl_kodi_addoninstance_imagedecoder_h : public IMsgHdl
{
public:
  CHdl_kodi_addoninstance_imagedecoder_h(CInterface& interface) : IMsgHdl(interface) {}

  void InitDirect(directFuncToKodi_addoninstance_imagedecoder_h* ifcToKodi,
                  directFuncToAddon_addoninstance_imagedecoder_h* ifcToAddon);

  // To handle callbacks from addon by shared sandbox API (static functions below in private becomes used)
  bool HandleMessage(int funcGroup,
                     int func,
                     const msgpack::unpacked& in,
                     msgpack::sbuffer& out) override;

  /* Code below becomes on auto generation reused, hand edited parts should included here */
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3>---*/
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3_END>---*/

  // Function calls from Kodi to addon

  bool kodi_addon_imagedecoder_supports_file_v1(const KODI_ADDON_IMAGEDECODER_HDL hdl,
                                                const char* file); // Added with API 1

  bool kodi_addon_imagedecoder_read_tag_v1(
      const KODI_ADDON_IMAGEDECODER_HDL hdl,
      const char* file,
      struct KODI_ADDON_IMAGEDECODER_INFO_TAG* info); // Added with API 1

  bool kodi_addon_imagedecoder_load_image_from_memory_v1(const KODI_ADDON_IMAGEDECODER_HDL hdl,
                                                         const char* mimetype,
                                                         const uint8_t* buffer,
                                                         size_t buf_size,
                                                         unsigned int* width,
                                                         unsigned int* height); // Added with API 1

  bool kodi_addon_imagedecoder_decode_v1(const KODI_ADDON_IMAGEDECODER_HDL hdl,
                                         uint8_t* pixels,
                                         size_t pixels_size,
                                         unsigned int width,
                                         unsigned int height,
                                         unsigned int pitch,
                                         enum ADDON_IMG_FMT format); // Added with API 1

private:
  /* Code below becomes on auto generation reused, hand edited parts should included here */
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_4>---*/
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_4_END>---*/

  // Callbacks from addon to Kodi

  directFuncToAddon_addoninstance_imagedecoder_h* m_ifcToAddon{nullptr};
};

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
