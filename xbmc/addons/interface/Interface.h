/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "../kodi-dev-kit/src/shared/SharedGroups.h"
#include "IMsgHdl.h"

#include <array>

namespace ADDON
{
class CAddonInfo;
} /* namespace ADDON */

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

/*---AUTO_GEN_PARSE<INTERFACE_CPP_H_SET1>---*/
class CHdl_kodi_addon_base_h;
class CHdl_kodi_audio_engine_h;
class CHdl_kodi_filesystem_h;
class CHdl_kodi_general_h;
class CHdl_kodi_monitor_h;
class CHdl_kodi_network_h;
class CHdl_kodi_addoninstance_audiodecoder_h;
class CHdl_kodi_addoninstance_audioencoder_h;
class CHdl_kodi_addoninstance_game_h;
class CHdl_kodi_addoninstance_imagedecoder_h;
class CHdl_kodi_addoninstance_inputstream_h;
class CHdl_kodi_addoninstance_peripheral_h;
class CHdl_kodi_addoninstance_pvr_h;
class CHdl_kodi_addoninstance_screensaver_h;
class CHdl_kodi_addoninstance_vfs_h;
class CHdl_kodi_addoninstance_videocodec_h;
class CHdl_kodi_addoninstance_visualization_h;
class CHdl_kodi_addoninstance_web_h;
class CHdl_kodi_gui_general_h;
class CHdl_kodi_gui_list_item_h;
class CHdl_kodi_gui_window_h;
class CHdl_kodi_gui_controls_button_h;
class CHdl_kodi_gui_controls_edit_h;
class CHdl_kodi_gui_controls_fade_label_h;
class CHdl_kodi_gui_controls_image_h;
class CHdl_kodi_gui_controls_label_h;
class CHdl_kodi_gui_controls_progress_h;
class CHdl_kodi_gui_controls_radio_button_h;
class CHdl_kodi_gui_controls_rendering_h;
class CHdl_kodi_gui_controls_settings_slider_h;
class CHdl_kodi_gui_controls_slider_h;
class CHdl_kodi_gui_controls_spin_h;
class CHdl_kodi_gui_controls_text_box_h;
class CHdl_kodi_gui_dialogs_context_menu_h;
class CHdl_kodi_gui_dialogs_extended_progress_h;
class CHdl_kodi_gui_dialogs_filebrowser_h;
class CHdl_kodi_gui_dialogs_keyboard_h;
class CHdl_kodi_gui_dialogs_numeric_h;
class CHdl_kodi_gui_dialogs_ok_h;
class CHdl_kodi_gui_dialogs_progress_h;
class CHdl_kodi_gui_dialogs_select_h;
class CHdl_kodi_gui_dialogs_text_viewer_h;
class CHdl_kodi_gui_dialogs_yes_no_h;
/*---AUTO_GEN_PARSE<INTERFACE_CPP_H_SET1_END>---*/

struct DirectAddonIfc;

enum class InterfaceKodiClass
{
  CFile,
  CHttpHeader,
  IAEStream,
};

class IInterfaceClassHdl
{
public:
  IInterfaceClassHdl(InterfaceKodiClass ifcClass) : m_ifcClass(ifcClass) {}
  virtual ~IInterfaceClassHdl() = default;

  const InterfaceKodiClass m_ifcClass;
};

class CInterface
{
public:
  CInterface(CRunningProcess* process, bool directDll);
  ~CInterface();

  bool FuncCall(int group, int func, const msgpack::unpacked& in, msgpack::sbuffer& out);

  /*---AUTO_GEN_PARSE<INTERFACE_CPP_H_SET2>---*/
  CHdl_kodi_addon_base_h* kodi_addon_base_h;
  CHdl_kodi_audio_engine_h* kodi_audio_engine_h;
  CHdl_kodi_filesystem_h* kodi_filesystem_h;
  CHdl_kodi_general_h* kodi_general_h;
  CHdl_kodi_monitor_h* kodi_monitor_h;
  CHdl_kodi_network_h* kodi_network_h;
  CHdl_kodi_addoninstance_audiodecoder_h* kodi_addoninstance_audiodecoder_h;
  CHdl_kodi_addoninstance_audioencoder_h* kodi_addoninstance_audioencoder_h;
  CHdl_kodi_addoninstance_game_h* kodi_addoninstance_game_h;
  CHdl_kodi_addoninstance_imagedecoder_h* kodi_addoninstance_imagedecoder_h;
  CHdl_kodi_addoninstance_inputstream_h* kodi_addoninstance_inputstream_h;
  CHdl_kodi_addoninstance_peripheral_h* kodi_addoninstance_peripheral_h;
  CHdl_kodi_addoninstance_pvr_h* kodi_addoninstance_pvr_h;
  CHdl_kodi_addoninstance_screensaver_h* kodi_addoninstance_screensaver_h;
  CHdl_kodi_addoninstance_vfs_h* kodi_addoninstance_vfs_h;
  CHdl_kodi_addoninstance_videocodec_h* kodi_addoninstance_videocodec_h;
  CHdl_kodi_addoninstance_visualization_h* kodi_addoninstance_visualization_h;
  CHdl_kodi_addoninstance_web_h* kodi_addoninstance_web_h;
  CHdl_kodi_gui_general_h* kodi_gui_general_h;
  CHdl_kodi_gui_list_item_h* kodi_gui_list_item_h;
  CHdl_kodi_gui_window_h* kodi_gui_window_h;
  CHdl_kodi_gui_controls_button_h* kodi_gui_controls_button_h;
  CHdl_kodi_gui_controls_edit_h* kodi_gui_controls_edit_h;
  CHdl_kodi_gui_controls_fade_label_h* kodi_gui_controls_fade_label_h;
  CHdl_kodi_gui_controls_image_h* kodi_gui_controls_image_h;
  CHdl_kodi_gui_controls_label_h* kodi_gui_controls_label_h;
  CHdl_kodi_gui_controls_progress_h* kodi_gui_controls_progress_h;
  CHdl_kodi_gui_controls_radio_button_h* kodi_gui_controls_radio_button_h;
  CHdl_kodi_gui_controls_rendering_h* kodi_gui_controls_rendering_h;
  CHdl_kodi_gui_controls_settings_slider_h* kodi_gui_controls_settings_slider_h;
  CHdl_kodi_gui_controls_slider_h* kodi_gui_controls_slider_h;
  CHdl_kodi_gui_controls_spin_h* kodi_gui_controls_spin_h;
  CHdl_kodi_gui_controls_text_box_h* kodi_gui_controls_text_box_h;
  CHdl_kodi_gui_dialogs_context_menu_h* kodi_gui_dialogs_context_menu_h;
  CHdl_kodi_gui_dialogs_extended_progress_h* kodi_gui_dialogs_extended_progress_h;
  CHdl_kodi_gui_dialogs_filebrowser_h* kodi_gui_dialogs_filebrowser_h;
  CHdl_kodi_gui_dialogs_keyboard_h* kodi_gui_dialogs_keyboard_h;
  CHdl_kodi_gui_dialogs_numeric_h* kodi_gui_dialogs_numeric_h;
  CHdl_kodi_gui_dialogs_ok_h* kodi_gui_dialogs_ok_h;
  CHdl_kodi_gui_dialogs_progress_h* kodi_gui_dialogs_progress_h;
  CHdl_kodi_gui_dialogs_select_h* kodi_gui_dialogs_select_h;
  CHdl_kodi_gui_dialogs_text_viewer_h* kodi_gui_dialogs_text_viewer_h;
  CHdl_kodi_gui_dialogs_yes_no_h* kodi_gui_dialogs_yes_no_h;
  /*---AUTO_GEN_PARSE<INTERFACE_CPP_H_SET2_END>---*/

  CRunningProcess* const m_process;
  std::unique_ptr<DirectAddonIfc> const m_directIfc;
  const bool m_directDll;

private:
  std::array<IMsgHdl*, funcGroup_Max> m_handlers{nullptr};
};

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
