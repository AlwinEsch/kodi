/*
 *  Copyright (C) 2013 Arne Morten Kvarving
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "EncoderAddon.h"

using namespace ADDON;
using namespace KODI::CDRIP;

CEncoderAddon::CEncoderAddon(const AddonInfoPtr& addonInfo)
  : IInstanceHandler(this, ADDON_INSTANCE_AUDIOENCODER, addonInfo)
{
}

bool CEncoderAddon::Init()
{
  if (CreateInstance() != ADDON_STATUS_OK)
    return false;

  KODI_ADDON_AUDIOENCODER_INFO_TAG tag{};
  tag.channels = m_iInChannels;
  tag.samplerate = m_iInSampleRate;
  tag.bits_per_sample = m_iInBitsPerSample;
  tag.track_length = m_iTrackLength;
  tag.title = m_strTitle.c_str();
  tag.artist = m_strArtist.c_str();
  tag.album_artist = m_strAlbumArtist.c_str();
  tag.album = m_strAlbum.c_str();
  tag.release_date = m_strYear.c_str();
  tag.track = atoi(m_strTrack.c_str());
  tag.genre = m_strGenre.c_str();
  tag.comment = m_strComment.c_str();

  const auto ifc = m_ifc->kodi_addoninstance_audioencoder_h;
  const bool ret = ifc->kodi_addon_audioencoder_start_v1(m_instance, &tag);
  if (!ret)
    DestroyInstance();

  return ret;
}

ssize_t CEncoderAddon::Encode(uint8_t* pbtStream, size_t nNumBytesRead)
{
  const auto ifc = m_ifc->kodi_addoninstance_audioencoder_h;
  return ifc->kodi_addon_audioencoder_encode_v1(m_instance, pbtStream, nNumBytesRead);
}

bool CEncoderAddon::Close()
{
  const auto ifc = m_ifc->kodi_addoninstance_audioencoder_h;
  const bool ret = ifc->kodi_addon_audioencoder_finish_v1(m_instance);

  DestroyInstance();

  return ret;
}

ssize_t CEncoderAddon::Write(const uint8_t* data, size_t len)
{
  return CEncoder::Write(data, len);
}

ssize_t CEncoderAddon::Seek(ssize_t pos, int whence)
{
  return CEncoder::Seek(pos, whence);
}
