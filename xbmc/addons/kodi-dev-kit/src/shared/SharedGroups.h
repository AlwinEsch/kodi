/*
 *  Copyright (C) 2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "../../include/kodi/c-api/addon_base.h"
#include "SharedData.h"

#ifndef ATTR_INT_APIENTRYP
#define ATTR_INT_APIENTRYP ATTR_APIENTRYP
#endif

#ifndef ATTR_INT_DLL_EXPORT
#define ATTR_INT_DLL_EXPORT ATTR_DLL_EXPORT
#endif

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

struct IFC_tm
{
  IFC_tm() = default;
  IFC_tm(const tm* c_data)
  {
    if (c_data == nullptr)
      return;

    tm_sec = c_data->tm_sec;
    tm_min = c_data->tm_min;
    tm_hour = c_data->tm_hour;
    tm_mday = c_data->tm_mday;
    tm_mon = c_data->tm_mon;
    tm_year = c_data->tm_year;
    tm_wday = c_data->tm_wday;
    tm_yday = c_data->tm_yday;
    tm_isdst = c_data->tm_isdst;

#ifdef __USE_MISC
    tm_gmtoff = c_data->tm_gmtoff;
    tm_zone = c_data->tm_zone;
#else
    tm_gmtoff = c_data->__tm_gmtoff; /* Seconds east of UTC.  */
    tm_zone = c_data->__tm_zone; /* Timezone abbreviation.  */
#endif
  }

  void CleanCStructure(tm* c_data)
  {
    if (c_data == nullptr)
      return;
  }

  void SetCStructure(tm* c_data)
  {
    if (c_data == nullptr)
      return;

    c_data->tm_sec = tm_sec;
    c_data->tm_min = tm_min;
    c_data->tm_hour = tm_hour;
    c_data->tm_mday = tm_mday;
    c_data->tm_mon = tm_mon;
    c_data->tm_year = tm_year;
    c_data->tm_wday = tm_wday;
    c_data->tm_yday = tm_yday;
    c_data->tm_isdst = tm_isdst;

#ifdef __USE_MISC
    c_data->tm_gmtoff = tm_gmtoff;
    c_data->tm_zone = tm_zone.c_str();
#else
    c_data->tm_gmtoff = __tm_gmtoff; /* Seconds east of UTC.  */
    c_data->tm_zone = __tm_zone; /* Timezone abbreviation.  */
#endif
  }

  int tm_sec; /* Seconds.	[0-60] (1 leap second) */
  int tm_min; /* Minutes.	[0-59] */
  int tm_hour; /* Hours.	[0-23] */
  int tm_mday; /* Day.		[1-31] */
  int tm_mon; /* Month.	[0-11] */
  int tm_year; /* Year	- 1900.  */
  int tm_wday; /* Day of week.	[0-6] */
  int tm_yday; /* Days in year.[0-365]	*/
  int tm_isdst; /* DST.		[-1/0/1]*/

  long int tm_gmtoff; /* Seconds east of UTC.  */
  std::string tm_zone; /* Timezone abbreviation.  */

  MSGPACK_DEFINE(tm_sec,
                 tm_min,
                 tm_hour,
                 tm_mday,
                 tm_mon,
                 tm_year,
                 tm_wday,
                 tm_yday,
                 tm_isdst,
                 tm_gmtoff,
                 tm_zone);
};

typedef int DummyValue;
constexpr DummyValue DummyValueNull = 0;
typedef uint64_t PtrValue;
typedef uint64_t PtrKodiOffscreen;
typedef uint64_t PtrAddonOffscreen;
typedef int NEED_TODO_VALUES;

typedef enum funcGroup : int
{
  funcGroup_Main = 0,
  /*---AUTO_GEN_PARSE<FUNC_GROUP_ENUM>---*/
  /* Code below autogenerated, see xbmc/addons/kodi-dev-kit/tools/code-generator/code_generator.py */
  funcGroup_addon_base_h,
  funcGroup_audio_engine_h,
  funcGroup_filesystem_h,
  funcGroup_general_h,
  funcGroup_monitor_h,
  funcGroup_network_h,
  funcGroup_addoninstance_audiodecoder_h,
  funcGroup_addoninstance_audioencoder_h,
  funcGroup_addoninstance_game_h,
  funcGroup_addoninstance_imagedecoder_h,
  funcGroup_addoninstance_inputstream_h,
  funcGroup_addoninstance_peripheral_h,
  funcGroup_addoninstance_pvr_h,
  funcGroup_addoninstance_screensaver_h,
  funcGroup_addoninstance_vfs_h,
  funcGroup_addoninstance_videocodec_h,
  funcGroup_addoninstance_visualization_h,
  funcGroup_addoninstance_web_h,
  funcGroup_gui_general_h,
  funcGroup_gui_list_item_h,
  funcGroup_gui_window_h,
  funcGroup_gui_controls_button_h,
  funcGroup_gui_controls_edit_h,
  funcGroup_gui_controls_fade_label_h,
  funcGroup_gui_controls_image_h,
  funcGroup_gui_controls_label_h,
  funcGroup_gui_controls_progress_h,
  funcGroup_gui_controls_radio_button_h,
  funcGroup_gui_controls_rendering_h,
  funcGroup_gui_controls_settings_slider_h,
  funcGroup_gui_controls_slider_h,
  funcGroup_gui_controls_spin_h,
  funcGroup_gui_controls_text_box_h,
  funcGroup_gui_dialogs_context_menu_h,
  funcGroup_gui_dialogs_extended_progress_h,
  funcGroup_gui_dialogs_filebrowser_h,
  funcGroup_gui_dialogs_keyboard_h,
  funcGroup_gui_dialogs_numeric_h,
  funcGroup_gui_dialogs_ok_h,
  funcGroup_gui_dialogs_progress_h,
  funcGroup_gui_dialogs_select_h,
  funcGroup_gui_dialogs_text_viewer_h,
  funcGroup_gui_dialogs_yes_no_h,
  /*---AUTO_GEN_PARSE<FUNC_GROUP_ENUM_END>---*/
  funcGroup_Max
} funcGroup;

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
