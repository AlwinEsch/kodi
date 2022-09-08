/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "Interface.h"

#include "addons/kodi-dev-kit/src/shared/DirectData.h"

/*---AUTO_GEN_PARSE<INTERFACE_CPP_SET_INCLUDE>---*/
#include "api/addon-instance/audiodecoder.h"
#include "api/addon-instance/audioencoder.h"
#include "api/addon-instance/game.h"
#include "api/addon-instance/imagedecoder.h"
#include "api/addon-instance/inputstream.h"
#include "api/addon-instance/peripheral.h"
#include "api/addon-instance/pvr.h"
#include "api/addon-instance/screensaver.h"
#include "api/addon-instance/vfs.h"
#include "api/addon-instance/videocodec.h"
#include "api/addon-instance/visualization.h"
#include "api/addon-instance/web.h"
#include "api/addon_base.h"
#include "api/audio_engine.h"
#include "api/filesystem.h"
#include "api/general.h"
#include "api/gui/controls/button.h"
#include "api/gui/controls/edit.h"
#include "api/gui/controls/fade_label.h"
#include "api/gui/controls/image.h"
#include "api/gui/controls/label.h"
#include "api/gui/controls/progress.h"
#include "api/gui/controls/radio_button.h"
#include "api/gui/controls/rendering.h"
#include "api/gui/controls/settings_slider.h"
#include "api/gui/controls/slider.h"
#include "api/gui/controls/spin.h"
#include "api/gui/controls/text_box.h"
#include "api/gui/dialogs/context_menu.h"
#include "api/gui/dialogs/extended_progress.h"
#include "api/gui/dialogs/filebrowser.h"
#include "api/gui/dialogs/keyboard.h"
#include "api/gui/dialogs/numeric.h"
#include "api/gui/dialogs/ok.h"
#include "api/gui/dialogs/progress.h"
#include "api/gui/dialogs/select.h"
#include "api/gui/dialogs/text_viewer.h"
#include "api/gui/dialogs/yes_no.h"
#include "api/gui/general.h"
#include "api/gui/list_item.h"
#include "api/gui/window.h"
#include "api/monitor.h"
#include "api/network.h"
/*---AUTO_GEN_PARSE<INTERFACE_CPP_SET_INCLUDE_END>---*/

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

CInterface::CInterface(CRunningProcess* process, bool directDll)
  : m_process(process),
    m_directIfc(directDll ? std::make_unique<DirectAddonIfc>() : nullptr),
    m_directDll(directDll)
{
  /*---AUTO_GEN_PARSE<INTERFACE_CPP_H_SET>---*/
  kodi_addon_base_h = new CHdl_kodi_addon_base_h(*this);
  m_handlers[funcGroup_addon_base_h] = kodi_addon_base_h;
  kodi_audio_engine_h = new CHdl_kodi_audio_engine_h(*this);
  m_handlers[funcGroup_audio_engine_h] = kodi_audio_engine_h;
  kodi_filesystem_h = new CHdl_kodi_filesystem_h(*this);
  m_handlers[funcGroup_filesystem_h] = kodi_filesystem_h;
  kodi_general_h = new CHdl_kodi_general_h(*this);
  m_handlers[funcGroup_general_h] = kodi_general_h;
  kodi_monitor_h = new CHdl_kodi_monitor_h(*this);
  m_handlers[funcGroup_monitor_h] = kodi_monitor_h;
  kodi_network_h = new CHdl_kodi_network_h(*this);
  m_handlers[funcGroup_network_h] = kodi_network_h;
  kodi_addoninstance_audiodecoder_h = new CHdl_kodi_addoninstance_audiodecoder_h(*this);
  m_handlers[funcGroup_addoninstance_audiodecoder_h] = kodi_addoninstance_audiodecoder_h;
  kodi_addoninstance_audioencoder_h = new CHdl_kodi_addoninstance_audioencoder_h(*this);
  m_handlers[funcGroup_addoninstance_audioencoder_h] = kodi_addoninstance_audioencoder_h;
  kodi_addoninstance_game_h = new CHdl_kodi_addoninstance_game_h(*this);
  m_handlers[funcGroup_addoninstance_game_h] = kodi_addoninstance_game_h;
  kodi_addoninstance_imagedecoder_h = new CHdl_kodi_addoninstance_imagedecoder_h(*this);
  m_handlers[funcGroup_addoninstance_imagedecoder_h] = kodi_addoninstance_imagedecoder_h;
  kodi_addoninstance_inputstream_h = new CHdl_kodi_addoninstance_inputstream_h(*this);
  m_handlers[funcGroup_addoninstance_inputstream_h] = kodi_addoninstance_inputstream_h;
  kodi_addoninstance_peripheral_h = new CHdl_kodi_addoninstance_peripheral_h(*this);
  m_handlers[funcGroup_addoninstance_peripheral_h] = kodi_addoninstance_peripheral_h;
  kodi_addoninstance_pvr_h = new CHdl_kodi_addoninstance_pvr_h(*this);
  m_handlers[funcGroup_addoninstance_pvr_h] = kodi_addoninstance_pvr_h;
  kodi_addoninstance_screensaver_h = new CHdl_kodi_addoninstance_screensaver_h(*this);
  m_handlers[funcGroup_addoninstance_screensaver_h] = kodi_addoninstance_screensaver_h;
  kodi_addoninstance_vfs_h = new CHdl_kodi_addoninstance_vfs_h(*this);
  m_handlers[funcGroup_addoninstance_vfs_h] = kodi_addoninstance_vfs_h;
  kodi_addoninstance_videocodec_h = new CHdl_kodi_addoninstance_videocodec_h(*this);
  m_handlers[funcGroup_addoninstance_videocodec_h] = kodi_addoninstance_videocodec_h;
  kodi_addoninstance_visualization_h = new CHdl_kodi_addoninstance_visualization_h(*this);
  m_handlers[funcGroup_addoninstance_visualization_h] = kodi_addoninstance_visualization_h;
  kodi_addoninstance_web_h = new CHdl_kodi_addoninstance_web_h(*this);
  m_handlers[funcGroup_addoninstance_web_h] = kodi_addoninstance_web_h;
  kodi_gui_general_h = new CHdl_kodi_gui_general_h(*this);
  m_handlers[funcGroup_gui_general_h] = kodi_gui_general_h;
  kodi_gui_list_item_h = new CHdl_kodi_gui_list_item_h(*this);
  m_handlers[funcGroup_gui_list_item_h] = kodi_gui_list_item_h;
  kodi_gui_window_h = new CHdl_kodi_gui_window_h(*this);
  m_handlers[funcGroup_gui_window_h] = kodi_gui_window_h;
  kodi_gui_controls_button_h = new CHdl_kodi_gui_controls_button_h(*this);
  m_handlers[funcGroup_gui_controls_button_h] = kodi_gui_controls_button_h;
  kodi_gui_controls_edit_h = new CHdl_kodi_gui_controls_edit_h(*this);
  m_handlers[funcGroup_gui_controls_edit_h] = kodi_gui_controls_edit_h;
  kodi_gui_controls_fade_label_h = new CHdl_kodi_gui_controls_fade_label_h(*this);
  m_handlers[funcGroup_gui_controls_fade_label_h] = kodi_gui_controls_fade_label_h;
  kodi_gui_controls_image_h = new CHdl_kodi_gui_controls_image_h(*this);
  m_handlers[funcGroup_gui_controls_image_h] = kodi_gui_controls_image_h;
  kodi_gui_controls_label_h = new CHdl_kodi_gui_controls_label_h(*this);
  m_handlers[funcGroup_gui_controls_label_h] = kodi_gui_controls_label_h;
  kodi_gui_controls_progress_h = new CHdl_kodi_gui_controls_progress_h(*this);
  m_handlers[funcGroup_gui_controls_progress_h] = kodi_gui_controls_progress_h;
  kodi_gui_controls_radio_button_h = new CHdl_kodi_gui_controls_radio_button_h(*this);
  m_handlers[funcGroup_gui_controls_radio_button_h] = kodi_gui_controls_radio_button_h;
  kodi_gui_controls_rendering_h = new CHdl_kodi_gui_controls_rendering_h(*this);
  m_handlers[funcGroup_gui_controls_rendering_h] = kodi_gui_controls_rendering_h;
  kodi_gui_controls_settings_slider_h = new CHdl_kodi_gui_controls_settings_slider_h(*this);
  m_handlers[funcGroup_gui_controls_settings_slider_h] = kodi_gui_controls_settings_slider_h;
  kodi_gui_controls_slider_h = new CHdl_kodi_gui_controls_slider_h(*this);
  m_handlers[funcGroup_gui_controls_slider_h] = kodi_gui_controls_slider_h;
  kodi_gui_controls_spin_h = new CHdl_kodi_gui_controls_spin_h(*this);
  m_handlers[funcGroup_gui_controls_spin_h] = kodi_gui_controls_spin_h;
  kodi_gui_controls_text_box_h = new CHdl_kodi_gui_controls_text_box_h(*this);
  m_handlers[funcGroup_gui_controls_text_box_h] = kodi_gui_controls_text_box_h;
  kodi_gui_dialogs_context_menu_h = new CHdl_kodi_gui_dialogs_context_menu_h(*this);
  m_handlers[funcGroup_gui_dialogs_context_menu_h] = kodi_gui_dialogs_context_menu_h;
  kodi_gui_dialogs_extended_progress_h = new CHdl_kodi_gui_dialogs_extended_progress_h(*this);
  m_handlers[funcGroup_gui_dialogs_extended_progress_h] = kodi_gui_dialogs_extended_progress_h;
  kodi_gui_dialogs_filebrowser_h = new CHdl_kodi_gui_dialogs_filebrowser_h(*this);
  m_handlers[funcGroup_gui_dialogs_filebrowser_h] = kodi_gui_dialogs_filebrowser_h;
  kodi_gui_dialogs_keyboard_h = new CHdl_kodi_gui_dialogs_keyboard_h(*this);
  m_handlers[funcGroup_gui_dialogs_keyboard_h] = kodi_gui_dialogs_keyboard_h;
  kodi_gui_dialogs_numeric_h = new CHdl_kodi_gui_dialogs_numeric_h(*this);
  m_handlers[funcGroup_gui_dialogs_numeric_h] = kodi_gui_dialogs_numeric_h;
  kodi_gui_dialogs_ok_h = new CHdl_kodi_gui_dialogs_ok_h(*this);
  m_handlers[funcGroup_gui_dialogs_ok_h] = kodi_gui_dialogs_ok_h;
  kodi_gui_dialogs_progress_h = new CHdl_kodi_gui_dialogs_progress_h(*this);
  m_handlers[funcGroup_gui_dialogs_progress_h] = kodi_gui_dialogs_progress_h;
  kodi_gui_dialogs_select_h = new CHdl_kodi_gui_dialogs_select_h(*this);
  m_handlers[funcGroup_gui_dialogs_select_h] = kodi_gui_dialogs_select_h;
  kodi_gui_dialogs_text_viewer_h = new CHdl_kodi_gui_dialogs_text_viewer_h(*this);
  m_handlers[funcGroup_gui_dialogs_text_viewer_h] = kodi_gui_dialogs_text_viewer_h;
  kodi_gui_dialogs_yes_no_h = new CHdl_kodi_gui_dialogs_yes_no_h(*this);
  m_handlers[funcGroup_gui_dialogs_yes_no_h] = kodi_gui_dialogs_yes_no_h;

  if (m_directIfc != nullptr)
  {
    kodi_addon_base_h->InitDirect(&m_directIfc->to_kodi.addon_base_h,
                                  &m_directIfc->to_addon.addon_base_h);
    kodi_audio_engine_h->InitDirect(&m_directIfc->to_kodi.audio_engine_h,
                                    &m_directIfc->to_addon.audio_engine_h);
    kodi_filesystem_h->InitDirect(&m_directIfc->to_kodi.filesystem_h,
                                  &m_directIfc->to_addon.filesystem_h);
    kodi_general_h->InitDirect(&m_directIfc->to_kodi.general_h, &m_directIfc->to_addon.general_h);
    kodi_monitor_h->InitDirect(&m_directIfc->to_kodi.monitor_h, &m_directIfc->to_addon.monitor_h);
    kodi_network_h->InitDirect(&m_directIfc->to_kodi.network_h, &m_directIfc->to_addon.network_h);
    kodi_addoninstance_audiodecoder_h->InitDirect(
        &m_directIfc->to_kodi.addoninstance_audiodecoder_h,
        &m_directIfc->to_addon.addoninstance_audiodecoder_h);
    kodi_addoninstance_audioencoder_h->InitDirect(
        &m_directIfc->to_kodi.addoninstance_audioencoder_h,
        &m_directIfc->to_addon.addoninstance_audioencoder_h);
    kodi_addoninstance_game_h->InitDirect(&m_directIfc->to_kodi.addoninstance_game_h,
                                          &m_directIfc->to_addon.addoninstance_game_h);
    kodi_addoninstance_imagedecoder_h->InitDirect(
        &m_directIfc->to_kodi.addoninstance_imagedecoder_h,
        &m_directIfc->to_addon.addoninstance_imagedecoder_h);
    kodi_addoninstance_inputstream_h->InitDirect(
        &m_directIfc->to_kodi.addoninstance_inputstream_h,
        &m_directIfc->to_addon.addoninstance_inputstream_h);
    kodi_addoninstance_peripheral_h->InitDirect(&m_directIfc->to_kodi.addoninstance_peripheral_h,
                                                &m_directIfc->to_addon.addoninstance_peripheral_h);
    kodi_addoninstance_pvr_h->InitDirect(&m_directIfc->to_kodi.addoninstance_pvr_h,
                                         &m_directIfc->to_addon.addoninstance_pvr_h);
    kodi_addoninstance_screensaver_h->InitDirect(
        &m_directIfc->to_kodi.addoninstance_screensaver_h,
        &m_directIfc->to_addon.addoninstance_screensaver_h);
    kodi_addoninstance_vfs_h->InitDirect(&m_directIfc->to_kodi.addoninstance_vfs_h,
                                         &m_directIfc->to_addon.addoninstance_vfs_h);
    kodi_addoninstance_videocodec_h->InitDirect(&m_directIfc->to_kodi.addoninstance_videocodec_h,
                                                &m_directIfc->to_addon.addoninstance_videocodec_h);
    kodi_addoninstance_visualization_h->InitDirect(
        &m_directIfc->to_kodi.addoninstance_visualization_h,
        &m_directIfc->to_addon.addoninstance_visualization_h);
    kodi_addoninstance_web_h->InitDirect(&m_directIfc->to_kodi.addoninstance_web_h,
                                         &m_directIfc->to_addon.addoninstance_web_h);
    kodi_gui_general_h->InitDirect(&m_directIfc->to_kodi.gui_general_h,
                                   &m_directIfc->to_addon.gui_general_h);
    kodi_gui_list_item_h->InitDirect(&m_directIfc->to_kodi.gui_list_item_h,
                                     &m_directIfc->to_addon.gui_list_item_h);
    kodi_gui_window_h->InitDirect(&m_directIfc->to_kodi.gui_window_h,
                                  &m_directIfc->to_addon.gui_window_h);
    kodi_gui_controls_button_h->InitDirect(&m_directIfc->to_kodi.gui_controls_button_h,
                                           &m_directIfc->to_addon.gui_controls_button_h);
    kodi_gui_controls_edit_h->InitDirect(&m_directIfc->to_kodi.gui_controls_edit_h,
                                         &m_directIfc->to_addon.gui_controls_edit_h);
    kodi_gui_controls_fade_label_h->InitDirect(&m_directIfc->to_kodi.gui_controls_fade_label_h,
                                               &m_directIfc->to_addon.gui_controls_fade_label_h);
    kodi_gui_controls_image_h->InitDirect(&m_directIfc->to_kodi.gui_controls_image_h,
                                          &m_directIfc->to_addon.gui_controls_image_h);
    kodi_gui_controls_label_h->InitDirect(&m_directIfc->to_kodi.gui_controls_label_h,
                                          &m_directIfc->to_addon.gui_controls_label_h);
    kodi_gui_controls_progress_h->InitDirect(&m_directIfc->to_kodi.gui_controls_progress_h,
                                             &m_directIfc->to_addon.gui_controls_progress_h);
    kodi_gui_controls_radio_button_h->InitDirect(
        &m_directIfc->to_kodi.gui_controls_radio_button_h,
        &m_directIfc->to_addon.gui_controls_radio_button_h);
    kodi_gui_controls_rendering_h->InitDirect(&m_directIfc->to_kodi.gui_controls_rendering_h,
                                              &m_directIfc->to_addon.gui_controls_rendering_h);
    kodi_gui_controls_settings_slider_h->InitDirect(
        &m_directIfc->to_kodi.gui_controls_settings_slider_h,
        &m_directIfc->to_addon.gui_controls_settings_slider_h);
    kodi_gui_controls_slider_h->InitDirect(&m_directIfc->to_kodi.gui_controls_slider_h,
                                           &m_directIfc->to_addon.gui_controls_slider_h);
    kodi_gui_controls_spin_h->InitDirect(&m_directIfc->to_kodi.gui_controls_spin_h,
                                         &m_directIfc->to_addon.gui_controls_spin_h);
    kodi_gui_controls_text_box_h->InitDirect(&m_directIfc->to_kodi.gui_controls_text_box_h,
                                             &m_directIfc->to_addon.gui_controls_text_box_h);
    kodi_gui_dialogs_context_menu_h->InitDirect(&m_directIfc->to_kodi.gui_dialogs_context_menu_h,
                                                &m_directIfc->to_addon.gui_dialogs_context_menu_h);
    kodi_gui_dialogs_extended_progress_h->InitDirect(
        &m_directIfc->to_kodi.gui_dialogs_extended_progress_h,
        &m_directIfc->to_addon.gui_dialogs_extended_progress_h);
    kodi_gui_dialogs_filebrowser_h->InitDirect(&m_directIfc->to_kodi.gui_dialogs_filebrowser_h,
                                               &m_directIfc->to_addon.gui_dialogs_filebrowser_h);
    kodi_gui_dialogs_keyboard_h->InitDirect(&m_directIfc->to_kodi.gui_dialogs_keyboard_h,
                                            &m_directIfc->to_addon.gui_dialogs_keyboard_h);
    kodi_gui_dialogs_numeric_h->InitDirect(&m_directIfc->to_kodi.gui_dialogs_numeric_h,
                                           &m_directIfc->to_addon.gui_dialogs_numeric_h);
    kodi_gui_dialogs_ok_h->InitDirect(&m_directIfc->to_kodi.gui_dialogs_ok_h,
                                      &m_directIfc->to_addon.gui_dialogs_ok_h);
    kodi_gui_dialogs_progress_h->InitDirect(&m_directIfc->to_kodi.gui_dialogs_progress_h,
                                            &m_directIfc->to_addon.gui_dialogs_progress_h);
    kodi_gui_dialogs_select_h->InitDirect(&m_directIfc->to_kodi.gui_dialogs_select_h,
                                          &m_directIfc->to_addon.gui_dialogs_select_h);
    kodi_gui_dialogs_text_viewer_h->InitDirect(&m_directIfc->to_kodi.gui_dialogs_text_viewer_h,
                                               &m_directIfc->to_addon.gui_dialogs_text_viewer_h);
    kodi_gui_dialogs_yes_no_h->InitDirect(&m_directIfc->to_kodi.gui_dialogs_yes_no_h,
                                          &m_directIfc->to_addon.gui_dialogs_yes_no_h);
  }
  /*---AUTO_GEN_PARSE<INTERFACE_CPP_H_SET_END>---*/
}

CInterface::~CInterface()
{
  for (auto& handler : m_handlers)
  {
    delete handler;
  }
}

bool CInterface::FuncCall(int group, int func, const msgpack::unpacked& in, msgpack::sbuffer& out)
{
  if (m_handlers[group])
    return m_handlers[group]->HandleMessage(group, func, in, out);
  return false;
}

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
