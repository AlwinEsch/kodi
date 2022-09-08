/*
 *  Copyright (C) 2013 Arne Morten Kvarving
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "Encoder.h"
#include "addons/interface/InstanceHandler.h"
#include "addons/interface/api/addon-instance/audioencoder.h"

namespace KODI
{
namespace CDRIP
{

class CEncoderAddon : public CEncoder, public KODI::ADDONS::INTERFACE::IInstanceHandler
{
public:
  explicit CEncoderAddon(const ADDON::AddonInfoPtr& addonInfo);
  ~CEncoderAddon() override = default;

  // Child functions related to IEncoder within CEncoder
  bool Init() override;
  ssize_t Encode(uint8_t* pbtStream, size_t nNumBytesRead) override;
  bool Close() override;

  // Addon callback functions
  ssize_t Write(const uint8_t* data, size_t len) override;
  /*---AUTO_GEN_PARSE<CB:kodi_addon_audioencoder_write>---*/
  ssize_t Seek(ssize_t pos, int whence) override;
  /*---AUTO_GEN_PARSE<CB:kodi_addon_audioencoder_seek>---*/
};

} /* namespace CDRIP */
} /* namespace KODI */
