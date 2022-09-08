/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "addons/kodi-dev-kit/include/kodi/c-api/addon-instance/visualization.h"
#include "interface/IOffscreenRenderProcess.h"
#include "interface/InstanceHandler.h"

namespace KODI
{
namespace ADDONS
{

class CVisualization : public KODI::ADDONS::INTERFACE::IInstanceHandler,
                       public KODI::ADDONS::INTERFACE::IOffscreenRenderProcess
{
public:
  CVisualization(const ADDON::AddonInfoPtr& addonInfo, float x, float y, float w, float h);
  ~CVisualization() override;

  bool Start(int channels, int samplesPerSec, int bitsPerSample, const std::string& songName);
  void Stop();
  void AudioData(const float* audioData, int audioDataLength);
  bool IsDirty();
  void Render();
  int GetSyncDelay();
  bool NextPreset();
  bool PrevPreset();
  bool LoadPreset(int select);
  bool RandomPreset();
  bool LockPreset();
  bool RatePreset(bool plus_minus);
  bool UpdateAlbumart(const char* albumart);
  bool UpdateTrack(const KODI_ADDON_VISUALIZATION_TRACK* track);
  bool HasPresets();
  bool GetPresetList(std::vector<std::string>& vecpresets);
  int GetActivePreset();
  std::string GetActivePresetName();
  bool IsLocked();

  void get_properties(KODI_ADDON_VISUALIZATION_PROPS* props);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_visualization_get_properties>---*/
  void transfer_preset(const char* preset);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_visualization_transfer_preset>---*/
  void clear_presets();
  /*---AUTO_GEN_PARSE<CB:kodi_addon_visualization_clear_presets>---*/

protected:
  bool GetOffscreenRenderInfos(
      int& x, int& y, int& width, int& height, ADDON_HARDWARE_CONTEXT& context) override;

private:
  std::vector<std::string> m_presets; /*!< cached preset list */
};

} /* namespace ADDONS */
} /* namespace KODI */
