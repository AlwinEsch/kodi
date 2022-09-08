/*
 *  Copyright (C) 2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

/*---AUTO_GEN_PARSE<USED_INCLUDES>---*/
#include "src/shared/api/addon-instance/audiodecoder.h"
#include "src/shared/api/addon-instance/audioencoder.h"
#include "src/shared/api/addon-instance/game.h"
#include "src/shared/api/addon-instance/imagedecoder.h"
#include "src/shared/api/addon-instance/inputstream.h"
#include "src/shared/api/addon-instance/peripheral.h"
#include "src/shared/api/addon-instance/pvr.h"
#include "src/shared/api/addon-instance/screensaver.h"
#include "src/shared/api/addon-instance/vfs.h"
#include "src/shared/api/addon-instance/videocodec.h"
#include "src/shared/api/addon-instance/visualization.h"
#include "src/shared/api/addon-instance/web.h"
#include "src/shared/api/addon_base.h"
/*---AUTO_GEN_PARSE<USED_INCLUDES_END>---*/

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

struct UnionAddonFunctions
{
  /* related addon instancy type */
  KODI_ADDON_INSTANCE_TYPE type;
  /* the from addon self created and used pointer for instance */
  KODI_IFC_HDL hdl;
  /* instance related pointer on Kodi side, normally the related class there */
  KODI_OWN_HDL kodi;
  /* special pointer to related instance function structure of addon, this becomes given from addon
     and again given from Kodi by destroy instance call, to allow addon a delete if it was
     created before. If addon use a static structure can be this ignored. */
  KODI_IFC_HDL addon_struct_ptr;
  /* Below the unions where store related instance function pointers, for here the whole structure
     is used, other as on addon sided "KODI_INSTANCE_HDL" where a pointer to is set.
     This with pointer is related to allow addon a static set of this and give his pointer on
     instance creation.
     Only as we can not know what addon does becomes it copied to here. */
  union
  {
    /*---AUTO_GEN_PARSE<USED_UNIONS>---*/
    uint32_t dummy;
    union KODI_ADDON_FUNC_UNION kodi_addon;
    union KODI_ADDON_INSTANCE_FUNC_UNION kodi_addon_instance;
    union KODI_ADDON_AUDIODECODER_FUNC_UNION kodi_addon_audiodecoder;
    union KODI_ADDON_AUDIOENCODER_FUNC_UNION kodi_addon_audioencoder;
    union KODI_ADDON_GAME_FUNC_UNION kodi_addon_game;
    union KODI_ADDON_IMAGEDECODER_FUNC_UNION kodi_addon_imagedecoder;
    union KODI_ADDON_INPUTSTREAM_FUNC_UNION kodi_addon_inputstream;
    union KODI_ADDON_PERIPHERAL_FUNC_UNION kodi_addon_peripheral;
    union KODI_ADDON_PVR_FUNC_UNION kodi_addon_pvr;
    union KODI_ADDON_SCREENSAVER_FUNC_UNION kodi_addon_screensaver;
    union KODI_ADDON_VFS_FUNC_UNION kodi_addon_vfs;
    union KODI_ADDON_VIDEOCODEC_FUNC_UNION kodi_addon_videocodec;
    union KODI_ADDON_VISUALIZATION_FUNC_UNION kodi_addon_visualization;
    union KODI_ADDON_WEB_FUNC_UNION kodi_addon_web;
    /*---AUTO_GEN_PARSE<USED_UNIONS_END>---*/
  } types;
};

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
