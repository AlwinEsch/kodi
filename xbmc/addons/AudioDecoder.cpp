/*
 *  Copyright (C) 2013 Arne Morten Kvarving
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "AudioDecoder.h"

// Devkit API interface
#include "interface/api/audio_engine.h"

// Kodi
#include "FileItem.h"
#include "ServiceBroker.h"
#include "addons/AddonManager.h"
#include "cores/AudioEngine/Utils/AEUtil.h"
#include "filesystem/File.h"
#include "music/tags/MusicInfoTag.h"
#include "music/tags/TagLoaderTagLib.h"
#include "utils/Mime.h"
#include "utils/URIUtils.h"
#include "utils/log.h"

using namespace ADDON;
using namespace KODI::ADDONS;
using namespace KODI::ADDONS::INTERFACE;

CAudioDecoder::CAudioDecoder(const AddonInfoPtr& addonInfo)
  : IInstanceHandler(this, ADDON_INSTANCE_AUDIODECODER, addonInfo)
{
  m_CodecName = addonInfo->Type(ADDON_AUDIODECODER)->GetValue("@name").asString();
  m_strExt = m_CodecName + KODI_ADDON_AUDIODECODER_TRACK_EXT;
  m_hasTags = addonInfo->Type(ADDON_AUDIODECODER)->GetValue("@tags").asBoolean();
}

CAudioDecoder::~CAudioDecoder()
{
  DestroyInstance();
}

bool CAudioDecoder::CreateDecoder()
{
  return CreateInstance() == ADDON_STATUS_OK;
}

bool CAudioDecoder::SupportsFile(const std::string& filename)
{
  // Create in case not available, possible as this done by IAddonSupportCheck
  if ((!m_instance && !CreateDecoder()))
    return false;

  const auto ifc = m_ifc->kodi_addoninstance_audiodecoder_h;
  return ifc->kodi_addon_audiodecoder_supports_file_v1(m_instance, filename.c_str());
}

bool CAudioDecoder::Init(const CFileItem& file, unsigned int filecache)
{
  /// for replaygain
  /// @todo About audio decoder in most cases Kodi's one not work, add fallback
  /// to use addon if this fails. Need API change about addons music info tag!
  CTagLoaderTagLib tag;
  tag.Load(file.GetDynPath(), XFILE::CMusicFileDirectory::m_tag, nullptr);

  int channels = -1;
  int sampleRate = -1;
  AudioEngineDataFormat addonFormat = AUDIOENGINE_FMT_INVALID;
  AudioEngineChannel channelList[AUDIOENGINE_CH_MAX] = {AUDIOENGINE_CH_NULL};

  const auto ifc = m_ifc->kodi_addoninstance_audiodecoder_h;
  bool ret = ifc->kodi_addon_audiodecoder_init_v1(
      m_instance, file.GetDynPath().c_str(), filecache, &channels, &sampleRate, &m_bitsPerSample,
      &m_TotalTime, &m_bitRate, &addonFormat, channelList);
  if (ret)
  {
    if (channels <= 0 || sampleRate <= 0 || addonFormat == AUDIOENGINE_FMT_INVALID)
    {
      CLog::Log(LOGERROR,
                "CAudioDecoder::{} - Addon '{}' returned true without set of needed values",
                __func__, ID());
      return false;
    }

    m_format.m_dataFormat = TranslateAEFormatToKodi(addonFormat);
    m_format.m_sampleRate = sampleRate;
    if (channelList[0] != AUDIOENGINE_CH_NULL)
    {
      CAEChannelInfo layout;
      for (const auto& channel : channelList)
      {
        if (channel == AUDIOENGINE_CH_NULL)
          break;
        layout += TranslateAEChannelToKodi(channel);
      }

      m_format.m_channelLayout = layout;
    }
    else
      m_format.m_channelLayout = CAEUtil::GuessChLayout(channels);
  }

  return ret;
}

int CAudioDecoder::ReadPCM(uint8_t* buffer, size_t size, size_t* actualsize)
{
  const auto ifc = m_ifc->kodi_addoninstance_audiodecoder_h;
  return ifc->kodi_addon_audiodecoder_read_pcm_v1(m_instance, buffer, size, actualsize);
}

bool CAudioDecoder::Seek(int64_t time)
{
  const auto ifc = m_ifc->kodi_addoninstance_audiodecoder_h;
  ifc->kodi_addon_audiodecoder_seek_v1(m_instance, time);
  return true;
}

bool CAudioDecoder::Load(const std::string& fileName,
                         MUSIC_INFO::CMusicInfoTag& tag,
                         EmbeddedArt* art)
{
  KODI_ADDON_AUDIODECODER_INFO_TAG ifcTag = {};
  const auto ifc = m_ifc->kodi_addoninstance_audiodecoder_h;
  bool ret = ifc->kodi_addon_audiodecoder_read_tag_v1(m_instance, fileName.c_str(), &ifcTag);
  if (ret)
  {
    if (ifcTag.title)
    {
      tag.SetTitle(ifcTag.title);
      free(ifcTag.title);
    }
    if (ifcTag.artist)
    {
      tag.SetArtist(ifcTag.artist);
      free(ifcTag.artist);
    }
    if (ifcTag.album)
    {
      tag.SetAlbum(ifcTag.album);
      free(ifcTag.album);
    }
    if (ifcTag.album_artist)
    {
      tag.SetAlbumArtist(ifcTag.album_artist);
      free(ifcTag.album_artist);
    }
    if (ifcTag.media_type)
    {
      tag.SetType(ifcTag.media_type);
      free(ifcTag.media_type);
    }
    if (ifcTag.genre)
    {
      tag.SetGenre(ifcTag.genre);
      free(ifcTag.genre);
    }
    tag.SetDuration(ifcTag.duration);
    tag.SetTrackNumber(ifcTag.track);
    tag.SetDiscNumber(ifcTag.disc);
    if (ifcTag.disc_subtitle)
    {
      tag.SetDiscSubtitle(ifcTag.disc_subtitle);
      free(ifcTag.disc_subtitle);
    }
    tag.SetTotalDiscs(ifcTag.disc_total);
    if (ifcTag.release_date)
    {
      tag.SetReleaseDate(ifcTag.release_date);
      free(ifcTag.release_date);
    }
    if (ifcTag.lyrics)
    {
      tag.SetLyrics(ifcTag.lyrics);
      free(ifcTag.lyrics);
    }
    tag.SetSampleRate(ifcTag.samplerate);
    tag.SetNoOfChannels(ifcTag.channels);
    tag.SetBitRate(ifcTag.bitrate);
    if (ifcTag.comment)
    {
      tag.SetComment(ifcTag.comment);
      free(ifcTag.comment);
    }

    if (ifcTag.cover_art_path)
    {
      const std::string mimetype =
          CMime::GetMimeType(URIUtils::GetExtension(ifcTag.cover_art_path));
      if (StringUtils::StartsWith(mimetype, "image/"))
      {
        XFILE::CFile file;
        std::vector<uint8_t> buf;

        if (file.LoadFile(ifcTag.cover_art_path, buf) > 0)
        {
          tag.SetCoverArtInfo(buf.size(), mimetype);
          if (art)
            art->Set(reinterpret_cast<const uint8_t*>(buf.data()), buf.size(), mimetype);
        }
      }
      free(ifcTag.cover_art_path);
    }
    else if (ifcTag.cover_art_mem_mimetype && ifcTag.cover_art_mem && ifcTag.cover_art_mem_size > 0)
    {
      tag.SetCoverArtInfo(ifcTag.cover_art_mem_size, ifcTag.cover_art_mem_mimetype);
      if (art)
        art->Set(ifcTag.cover_art_mem, ifcTag.cover_art_mem_size, ifcTag.cover_art_mem_mimetype);
    }

    if (ifcTag.cover_art_mem_mimetype)
      free(ifcTag.cover_art_mem_mimetype);
    if (ifcTag.cover_art_mem)
      free(ifcTag.cover_art_mem);

    tag.SetLoaded(true);
  }

  return ret;
}

int CAudioDecoder::GetTrackCount(const std::string& strPath)
{
  const auto ifc = m_ifc->kodi_addoninstance_audiodecoder_h;
  int result = ifc->kodi_addon_audiodecoder_track_count_v1(m_instance, strPath.c_str());
  if (result > 1)
  {
    if (m_hasTags)
    {
      if (!Load(strPath, XFILE::CMusicFileDirectory::m_tag, nullptr))
        return 0;
    }
    else
      XFILE::CMusicFileDirectory::m_tag.SetTitle(CURL(strPath).GetFileNameWithoutPath());
    XFILE::CMusicFileDirectory::m_tag.SetLoaded(true);
  }

  return result;
}
