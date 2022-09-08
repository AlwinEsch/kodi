/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "Visualization.h"

#include "addons/interface/api/addon-instance/visualization.h"
#include "filesystem/SpecialProtocol.h"
#include "guilib/GUIWindowManager.h"
#include "utils/log.h"

using namespace ADDON;
using namespace KODI::ADDONS;
using namespace KODI::ADDONS::INTERFACE;

CVisualization::CVisualization(const AddonInfoPtr& addonInfo, float x, float y, float w, float h)
  : IInstanceHandler(this, ADDON_INSTANCE_VISUALIZATION, addonInfo)
{
  /* Open the class "kodi::addon::CInstanceVisualization" on add-on side */
  if (CreateInstance() != ADDON_STATUS_OK)
  {
    CLog::Log(LOGFATAL, "Visualization: failed to create instance for '{}' and not usable!", ID());
    return;
  }

  /* presets becomes send with "transfer_preset" during call of function below */
  const auto ifc = m_ifc->kodi_addoninstance_visualization_h;
  ifc->kodi_addon_visualization_get_presets_v1(m_instance);
}

CVisualization::~CVisualization()
{
  /* Destroy the class "kodi::addon::CInstanceVisualization" on add-on side */
  DestroyInstance();
}

bool CVisualization::Start(int channels,
                           int samplesPerSec,
                           int bitsPerSample,
                           const std::string& songName)
{
  const auto ifc = m_ifc->kodi_addoninstance_visualization_h;
  return ifc->kodi_addon_visualization_start_v1(this, m_instance, channels, samplesPerSec,
                                                bitsPerSample, songName.c_str());
}

void CVisualization::Stop()
{
  const auto ifc = m_ifc->kodi_addoninstance_visualization_h;
  ifc->kodi_addon_visualization_stop_v1(this, m_instance);
}

void CVisualization::AudioData(const float* audioData, int audioDataLength)
{
  const auto ifc = m_ifc->kodi_addoninstance_visualization_h;
  ifc->kodi_addon_visualization_audio_data_v1(m_instance, audioData, audioDataLength);
}

bool CVisualization::IsDirty()
{
  const auto ifc = m_ifc->kodi_addoninstance_visualization_h;
  return ifc->kodi_addon_visualization_is_dirty_v1(m_instance);
}

void CVisualization::Render()
{
  const auto ifc = m_ifc->kodi_addoninstance_visualization_h;
  ifc->kodi_addon_visualization_render_v1(this, m_instance);
}

int CVisualization::GetSyncDelay()
{
  const auto ifc = m_ifc->kodi_addoninstance_visualization_h;
  return ifc->kodi_addon_visualization_get_sync_delay_v1(m_instance);
}

bool CVisualization::NextPreset()
{
  const auto ifc = m_ifc->kodi_addoninstance_visualization_h;
  return ifc->kodi_addon_visualization_next_preset_v1(m_instance);
}

bool CVisualization::PrevPreset()
{
  const auto ifc = m_ifc->kodi_addoninstance_visualization_h;
  return ifc->kodi_addon_visualization_prev_preset_v1(m_instance);
}

bool CVisualization::LoadPreset(int select)
{
  const auto ifc = m_ifc->kodi_addoninstance_visualization_h;
  return ifc->kodi_addon_visualization_load_preset_v1(m_instance, select);
}

bool CVisualization::RandomPreset()
{
  const auto ifc = m_ifc->kodi_addoninstance_visualization_h;
  return ifc->kodi_addon_visualization_random_preset_v1(m_instance);
}

bool CVisualization::LockPreset()
{
  const auto ifc = m_ifc->kodi_addoninstance_visualization_h;
  return ifc->kodi_addon_visualization_lock_preset_v1(m_instance);
}

bool CVisualization::RatePreset(bool plus_minus)
{
  const auto ifc = m_ifc->kodi_addoninstance_visualization_h;
  return ifc->kodi_addon_visualization_rate_preset_v1(m_instance, plus_minus);
}

bool CVisualization::UpdateAlbumart(const char* albumart)
{
  const auto ifc = m_ifc->kodi_addoninstance_visualization_h;
  return ifc->kodi_addon_visualization_update_albumart_v1(m_instance, albumart);
}

bool CVisualization::UpdateTrack(const KODI_ADDON_VISUALIZATION_TRACK* track)
{
  const auto ifc = m_ifc->kodi_addoninstance_visualization_h;
  return ifc->kodi_addon_visualization_update_track_v1(m_instance, track);
}

bool CVisualization::HasPresets()
{
  return !m_presets.empty();
}

bool CVisualization::GetPresetList(std::vector<std::string>& vecpresets)
{
  vecpresets = m_presets;
  return !m_presets.empty();
}

int CVisualization::GetActivePreset()
{
  const auto ifc = m_ifc->kodi_addoninstance_visualization_h;
  return ifc->kodi_addon_visualization_get_active_preset_v1(m_instance);
}

std::string CVisualization::GetActivePresetName()
{
  if (!m_presets.empty())
    return m_presets[GetActivePreset()];
  return "";
}

bool CVisualization::IsLocked()
{
  const auto ifc = m_ifc->kodi_addoninstance_visualization_h;
  return ifc->kodi_addon_visualization_is_locked_v1(m_instance);
}

bool CVisualization::GetOffscreenRenderInfos(
    int& x, int& y, int& width, int& height, ADDON_HARDWARE_CONTEXT& context)
{
  const auto winSystem = CServiceBroker::GetWinSystem();
  if (!winSystem)
    return false;

  x = 0;
  y = 0;
  context = winSystem->GetHWContext();
  width = winSystem->GetGfxContext().GetWidth();
  height = winSystem->GetGfxContext().GetHeight();

  return true;
}

void CVisualization::get_properties(struct KODI_ADDON_VISUALIZATION_PROPS* props)
{
  if (!props)
    return;

  const auto winSystem = CServiceBroker::GetWinSystem();
  if (!winSystem)
    return;

  props->x = 0;
  props->y = 0;
  props->device = winSystem->GetHWContext();
  props->width = winSystem->GetGfxContext().GetWidth();
  props->height = winSystem->GetGfxContext().GetHeight();
  props->pixelRatio = winSystem->GetGfxContext().GetResInfo().fPixelRatio;
}

void CVisualization::transfer_preset(const char* preset)
{
  m_presets.emplace_back(preset);
}

void CVisualization::clear_presets()
{
  m_presets.clear();
}
