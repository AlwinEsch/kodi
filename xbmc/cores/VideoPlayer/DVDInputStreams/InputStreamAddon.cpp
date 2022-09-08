/*
 *  Copyright (C) 2015-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "InputStreamAddon.h"

#include "addons/interface/api/addon-instance/inputstream.h"
#include "addons/kodi-dev-kit/include/kodi/c-api/addon-instance/videocodec.h"
#include "cores/VideoPlayer/DVDDemuxers/DVDDemux.h"
#include "cores/VideoPlayer/DVDDemuxers/DVDDemuxUtils.h"
#include "cores/VideoPlayer/Interface/DemuxCrypto.h"
#include "cores/VideoPlayer/Interface/InputStreamConstants.h"
#include "cores/VideoPlayer/Interface/TimingConstants.h"
#include "filesystem/SpecialProtocol.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "utils/log.h"

CInputStreamProvider::CInputStreamProvider(const ADDON::AddonInfoPtr& addonInfo,
                                           KODI_HANDLE parentInstance)
  : m_addonInfo(addonInfo), m_parentInstance(parentInstance)
{
}

void CInputStreamProvider::GetAddonInstance(INSTANCE_TYPE instance_type,
                                            ADDON::AddonInfoPtr& addonInfo,
                                            KODI_HANDLE& parentInstance)
{
  if (instance_type == ADDON::IAddonProvider::INSTANCE_VIDEOCODEC)
  {
    addonInfo = m_addonInfo;
    parentInstance = m_parentInstance;
  }
}

/*****************************************************************************************************************/

using namespace ADDON;

CInputStreamAddon::CInputStreamAddon(const AddonInfoPtr& addonInfo,
                                     IVideoPlayer* player,
                                     const CFileItem& fileitem,
                                     const std::string& instanceId)
  : IInstanceHandler(
        this, ADDON_INSTANCE_INPUTSTREAM, addonInfo, ADDON_INSTANCE_ID_UNUSED, nullptr, instanceId),
    CDVDInputStream(DVDSTREAM_TYPE_ADDON, fileitem),
    m_player(player)
{
  std::string listitemprops =
      addonInfo->Type(ADDON_INPUTSTREAM)->GetValue("@listitemprops").asString();
  std::string name(addonInfo->ID());

  m_fileItemProps = StringUtils::Tokenize(listitemprops, "|");
  for (auto &key : m_fileItemProps)
  {
    StringUtils::Trim(key);
    key = name + "." + key;
  }
  m_caps = {};
}

CInputStreamAddon::~CInputStreamAddon()
{
  Close();
}

bool CInputStreamAddon::Supports(const AddonInfoPtr& addonInfo, const CFileItem& fileitem)
{
  /// @todo Error for users to show deprecation, can be removed in Kodi 20
  CVariant oldAddonProp = fileitem.GetProperty("inputstreamaddon");
  if (!oldAddonProp.isNull())
  {
    CLog::Log(LOGERROR,
              "CInputStreamAddon::{} - 'inputstreamaddon' has been deprecated, "
              "please use `#KODIPROP:inputstream={}` instead",
              __func__, oldAddonProp.asString());
  }

  // check if a specific inputstream addon is requested
  CVariant addon = fileitem.GetProperty(STREAM_PROPERTY_INPUTSTREAM);
  if (!addon.isNull())
    return (addon.asString() == addonInfo->ID());

  // check protocols
  std::string protocol = CURL(fileitem.GetDynPath()).GetProtocol();
  if (!protocol.empty())
  {
    std::string protocols = addonInfo->Type(ADDON_INPUTSTREAM)->GetValue("@protocols").asString();
    if (!protocols.empty())
    {
      std::vector<std::string> protocolsList = StringUtils::Tokenize(protocols, "|");
      for (auto& value : protocolsList)
      {
        StringUtils::Trim(value);
        if (value == protocol)
          return true;
      }
    }
  }

  std::string filetype = fileitem.GetURL().GetFileType();
  if (!filetype.empty())
  {
    std::string extensions = addonInfo->Type(ADDON_INPUTSTREAM)->GetValue("@extension").asString();
    if (!extensions.empty())
    {
      std::vector<std::string> extensionsList = StringUtils::Tokenize(extensions, "|");
      for (auto& value : extensionsList)
      {
        StringUtils::Trim(value);
        if (value == filetype)
          return true;
      }
    }
  }

  return false;
}

bool CInputStreamAddon::Open()
{
  if (CreateInstance() != ADDON_STATUS_OK)
    return false;

  INPUTSTREAM_PROPERTY props = {};
  std::map<std::string, std::string> propsMap;
  for (auto &key : m_fileItemProps)
  {
    if (m_item.GetProperty(key).isNull())
      continue;
    propsMap[key] = m_item.GetProperty(key).asString();
  }

  props.m_nCountInfoValues = 0;
  for (auto &pair : propsMap)
  {
    props.m_ListItemProperties[props.m_nCountInfoValues].m_strKey = pair.first.c_str();
    props.m_ListItemProperties[props.m_nCountInfoValues].m_strValue = pair.second.c_str();
    props.m_nCountInfoValues++;

    if (props.m_nCountInfoValues >= STREAM_MAX_PROPERTY_COUNT)
    {
      CLog::Log(LOGERROR,
                "CInputStreamAddon::{} - Hit max count of stream properties, "
                "have {}, actual count: {}",
                __func__, STREAM_MAX_PROPERTY_COUNT, propsMap.size());
      break;
    }
  }

  props.m_strURL = m_item.GetDynPath().c_str();
  props.m_mimeType = m_item.GetMimeType().c_str();

  std::string libFolder = URIUtils::GetDirectory(Addon()->Path());
  std::string profileFolder = CSpecialProtocol::TranslatePath(Addon()->Profile());
  props.m_libFolder = libFolder.c_str();
  props.m_profileFolder = profileFolder.c_str();

  unsigned int videoWidth = 1280;
  unsigned int videoHeight = 720;
  if (m_player)
    m_player->GetVideoResolution(videoWidth, videoHeight);
  SetVideoResolution(videoWidth, videoHeight);

  bool ret =
      m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_open_v1(m_instance, &props);
  if (ret)
  {
    m_caps = {};
    m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_get_capabilities_v1(m_instance,
                                                                                        &m_caps);

    m_subAddonProvider =
        std::shared_ptr<CInputStreamProvider>(new CInputStreamProvider(GetAddonInfo(), m_instance));
  }
  return ret;
}

void CInputStreamAddon::Close()
{
  m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_close_v1(m_instance);
  DestroyInstance();
}

bool CInputStreamAddon::IsEOF()
{
  return false;
}

int CInputStreamAddon::Read(uint8_t* buf, int buf_size)
{
  return m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_read_stream_v1(
      m_instance, buf, buf_size);
}

int64_t CInputStreamAddon::Seek(int64_t offset, int whence)
{
  return m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_seek_stream_v1(
      m_instance, offset, whence);
}

int64_t CInputStreamAddon::GetLength()
{
  return m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_length_stream_v1(
      m_instance);
}

int CInputStreamAddon::GetBlockSize()
{
  return m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_block_size_stream_v1(
      m_instance);
}

bool CInputStreamAddon::CanSeek()
{
  return (m_caps.m_mask & INPUTSTREAM_SUPPORTS_SEEK) != 0;
}

bool CInputStreamAddon::CanPause()
{
  return (m_caps.m_mask & INPUTSTREAM_SUPPORTS_PAUSE) != 0;
}

// IDisplayTime
CDVDInputStream::IDisplayTime* CInputStreamAddon::GetIDisplayTime()
{
  if ((m_caps.m_mask & INPUTSTREAM_SUPPORTS_IDISPLAYTIME) == 0)
    return nullptr;

  return this;
}

int CInputStreamAddon::GetTotalTime()
{
  return m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_get_total_time_v1(
      m_instance);
}

int CInputStreamAddon::GetTime()
{
  return m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_get_time_v1(m_instance);
}

// ITime
CDVDInputStream::ITimes* CInputStreamAddon::GetITimes()
{
  if ((m_caps.m_mask & INPUTSTREAM_SUPPORTS_ITIME) == 0)
    return nullptr;

  return this;
}

bool CInputStreamAddon::GetTimes(Times &times)
{
  INPUTSTREAM_TIMES i_times;

  if (m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_get_times_v1(m_instance,
                                                                                   &i_times))
  {
    times.ptsBegin = i_times.ptsBegin;
    times.ptsEnd = i_times.ptsEnd;
    times.ptsStart = i_times.ptsStart;
    times.startTime = i_times.startTime;
    return true;
  }
  return false;
}

// IPosTime
CDVDInputStream::IPosTime* CInputStreamAddon::GetIPosTime()
{
  if ((m_caps.m_mask & INPUTSTREAM_SUPPORTS_IPOSTIME) == 0)
    return nullptr;

  return this;
}

bool CInputStreamAddon::PosTime(int ms)
{
  return m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_pos_time_v1(m_instance,
                                                                                     ms);
}

// IDemux
CDVDInputStream::IDemux* CInputStreamAddon::GetIDemux()
{
  if ((m_caps.m_mask & INPUTSTREAM_SUPPORTS_IDEMUX) == 0)
    return nullptr;

  return this;
}

bool CInputStreamAddon::OpenDemux()
{
  if ((m_caps.m_mask & INPUTSTREAM_SUPPORTS_IDEMUX) != 0)
    return true;
  else
    return false;
}

DemuxPacket* CInputStreamAddon::ReadDemux()
{
  return static_cast<DemuxPacket*>(
      m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_demux_read_v1(m_instance));
}

std::vector<CDemuxStream*> CInputStreamAddon::GetStreams() const
{
  std::vector<CDemuxStream*> streams;

  INPUTSTREAM_IDS streamIDs = {};
  bool ret = m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_get_stream_ids_v1(
      m_instance, &streamIDs);
  if (!ret || streamIDs.m_streamCount > INPUTSTREAM_MAX_STREAM_COUNT)
    return streams;

  for (unsigned int i = 0; i < streamIDs.m_streamCount; ++i)
    if (CDemuxStream* stream = GetStream(streamIDs.m_streamIds[i]))
      streams.push_back(stream);

  return streams;
}

CDemuxStream* CInputStreamAddon::GetStream(int streamId) const
{
  INPUTSTREAM_INFO stream{};
  KODI_HANDLE demuxStream = nullptr;
  bool ret = m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_get_stream_v1(
      m_instance, streamId, &stream, &demuxStream, cb_get_stream_transfer);
  if (!ret || stream.m_streamType == INPUTSTREAM_TYPE_NONE)
    return nullptr;

  return static_cast<CDemuxStream*>(demuxStream);
}

KODI_HANDLE CInputStreamAddon::cb_get_stream_transfer(KODI_HANDLE handle,
                                                      int streamId,
                                                      INPUTSTREAM_INFO* stream)
{
  CInputStreamAddon* thisClass = static_cast<CInputStreamAddon*>(handle);
  if (!thisClass || !stream)
    return nullptr;

  std::string codecName(stream->m_codecName);
  AVCodec* codec = nullptr;

  if (stream->m_streamType != INPUTSTREAM_TYPE_TELETEXT &&
      stream->m_streamType != INPUTSTREAM_TYPE_RDS)
  {
    StringUtils::ToLower(codecName);
    codec = avcodec_find_decoder_by_name(codecName.c_str());
    if (!codec)
      return nullptr;
  }

  CDemuxStream* demuxStream;

  if (stream->m_streamType == INPUTSTREAM_TYPE_AUDIO)
  {
    CDemuxStreamAudio *audioStream = new CDemuxStreamAudio();

    audioStream->iChannels = stream->m_Channels;
    audioStream->iSampleRate = stream->m_SampleRate;
    audioStream->iBlockAlign = stream->m_BlockAlign;
    audioStream->iBitRate = stream->m_BitRate;
    audioStream->iBitsPerSample = stream->m_BitsPerSample;
    demuxStream = audioStream;
  }
  else if (stream->m_streamType == INPUTSTREAM_TYPE_VIDEO)
  {
    CDemuxStreamVideo *videoStream = new CDemuxStreamVideo();

    videoStream->iFpsScale = stream->m_FpsScale;
    videoStream->iFpsRate = stream->m_FpsRate;
    videoStream->iWidth = stream->m_Width;
    videoStream->iHeight = stream->m_Height;
    videoStream->fAspect = static_cast<double>(stream->m_Aspect);
    videoStream->iBitRate = stream->m_BitRate;
    videoStream->profile = ConvertVideoCodecProfile(stream->m_codecProfile);

    /*! Added on API version 2.0.8 */
    //@{
    videoStream->colorSpace = static_cast<AVColorSpace>(stream->m_colorSpace);
    videoStream->colorRange = static_cast<AVColorRange>(stream->m_colorRange);
    //@}

    /*! Added on API version 2.0.9 */
    //@{
    videoStream->colorPrimaries = static_cast<AVColorPrimaries>(stream->m_colorPrimaries);
    videoStream->colorTransferCharacteristic =
        static_cast<AVColorTransferCharacteristic>(stream->m_colorTransferCharacteristic);

    if (stream->m_masteringMetadata)
    {
      videoStream->masteringMetaData =
          std::shared_ptr<AVMasteringDisplayMetadata>(new AVMasteringDisplayMetadata);
      videoStream->masteringMetaData->display_primaries[0][0] =
          av_d2q(stream->m_masteringMetadata->primary_r_chromaticity_x, INT_MAX);
      videoStream->masteringMetaData->display_primaries[0][1] =
          av_d2q(stream->m_masteringMetadata->primary_r_chromaticity_y, INT_MAX);
      videoStream->masteringMetaData->display_primaries[1][0] =
          av_d2q(stream->m_masteringMetadata->primary_g_chromaticity_x, INT_MAX);
      videoStream->masteringMetaData->display_primaries[1][1] =
          av_d2q(stream->m_masteringMetadata->primary_g_chromaticity_y, INT_MAX);
      videoStream->masteringMetaData->display_primaries[2][0] =
          av_d2q(stream->m_masteringMetadata->primary_b_chromaticity_x, INT_MAX);
      videoStream->masteringMetaData->display_primaries[2][1] =
          av_d2q(stream->m_masteringMetadata->primary_b_chromaticity_y, INT_MAX);
      videoStream->masteringMetaData->white_point[0] =
          av_d2q(stream->m_masteringMetadata->white_point_chromaticity_x, INT_MAX);
      videoStream->masteringMetaData->white_point[1] =
          av_d2q(stream->m_masteringMetadata->white_point_chromaticity_y, INT_MAX);
      videoStream->masteringMetaData->min_luminance =
          av_d2q(stream->m_masteringMetadata->luminance_min, INT_MAX);
      videoStream->masteringMetaData->max_luminance =
          av_d2q(stream->m_masteringMetadata->luminance_max, INT_MAX);
      videoStream->masteringMetaData->has_luminance =
          videoStream->masteringMetaData->has_primaries = 1;
    }

    if (stream->m_contentLightMetadata)
    {
      videoStream->contentLightMetaData =
          std::shared_ptr<AVContentLightMetadata>(new AVContentLightMetadata);
      videoStream->contentLightMetaData->MaxCLL =
          static_cast<unsigned>(stream->m_contentLightMetadata->max_cll);
      videoStream->contentLightMetaData->MaxFALL =
          static_cast<unsigned>(stream->m_contentLightMetadata->max_fall);
    }
    //@}

    /*
    // Way to include part on new API version
    if (Addon()->GetTypeVersionDll(ADDON_TYPE::ADDON_INSTANCE_INPUTSTREAM) >= AddonVersion("3.0.0")) // Set the version to your new
    {

    }
    */

    demuxStream = videoStream;
  }
  else if (stream->m_streamType == INPUTSTREAM_TYPE_SUBTITLE)
  {
    CDemuxStreamSubtitle *subtitleStream = new CDemuxStreamSubtitle();
    demuxStream = subtitleStream;
  }
  else if (stream->m_streamType == INPUTSTREAM_TYPE_TELETEXT)
  {
    CDemuxStreamTeletext* teletextStream = new CDemuxStreamTeletext();
    demuxStream = teletextStream;
  }
  else if (stream->m_streamType == INPUTSTREAM_TYPE_RDS)
  {
    CDemuxStreamRadioRDS* rdsStream = new CDemuxStreamRadioRDS();
    demuxStream = rdsStream;
  }
  else
    return nullptr;

  demuxStream->name = stream->m_name;
  if (codec)
    demuxStream->codec = codec->id;
  else
    demuxStream->codec = AV_CODEC_ID_DVB_TELETEXT;
  demuxStream->codecName = stream->m_codecInternalName;
  demuxStream->uniqueId = streamId;
  demuxStream->flags = static_cast<StreamFlags>(stream->m_flags);
  demuxStream->language = stream->m_language;
  demuxStream->codec_fourcc = stream->m_codecFourCC;

  if (stream->m_ExtraData && stream->m_ExtraSize)
  {
    demuxStream->ExtraData = new uint8_t[stream->m_ExtraSize];
    demuxStream->ExtraSize = stream->m_ExtraSize;
    for (unsigned int j = 0; j < stream->m_ExtraSize; ++j)
      demuxStream->ExtraData[j] = stream->m_ExtraData[j];
  }

  if (stream->m_cryptoSession.keySystem != STREAM_CRYPTO_KEY_SYSTEM_NONE &&
      stream->m_cryptoSession.keySystem < STREAM_CRYPTO_KEY_SYSTEM_COUNT)
  {
    static const CryptoSessionSystem map[] = {
        CRYPTO_SESSION_SYSTEM_NONE,
        CRYPTO_SESSION_SYSTEM_WIDEVINE,
        CRYPTO_SESSION_SYSTEM_PLAYREADY,
        CRYPTO_SESSION_SYSTEM_WISEPLAY,
    };
    demuxStream->cryptoSession = std::shared_ptr<DemuxCryptoSession>(
        new DemuxCryptoSession(map[stream->m_cryptoSession.keySystem],
                               stream->m_cryptoSession.sessionId, stream->m_cryptoSession.flags));

    if ((stream->m_features & INPUTSTREAM_FEATURE_DECODE) != 0)
      demuxStream->externalInterfaces = thisClass->m_subAddonProvider;
  }
  return demuxStream;
}

void CInputStreamAddon::EnableStream(int streamId, bool enable)
{
  m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_enable_stream_v1(
      m_instance, streamId, enable);
}

bool CInputStreamAddon::OpenStream(int streamId)
{
  return m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_open_stream_v1(m_instance,
                                                                                        streamId);
}

int CInputStreamAddon::GetNrOfStreams() const
{
  return m_streamCount;
}

void CInputStreamAddon::SetSpeed(int speed)
{
  m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_demux_set_speed_v1(m_instance,
                                                                                     speed);
}

bool CInputStreamAddon::SeekTime(double time, bool backward, double* startpts)
{
  if ((m_caps.m_mask & INPUTSTREAM_SUPPORTS_IPOSTIME) != 0)
  {
    if (!PosTime(static_cast<int>(time)))
      return false;

    FlushDemux();

    if(startpts)
      *startpts = DVD_NOPTS_VALUE;
    return true;
  }

  return m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_demux_seek_time_v1(
      m_instance, time, backward, startpts);
}

void CInputStreamAddon::AbortDemux()
{
  m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_demux_abort_v1(m_instance);
}

void CInputStreamAddon::FlushDemux()
{
  m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_demux_flush_v1(m_instance);
}

void CInputStreamAddon::SetVideoResolution(int width, int height)
{
  m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_set_video_resolution_v1(
      m_instance, width, height);
}

bool CInputStreamAddon::IsRealtime()
{
  return m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_is_real_time_stream_v1(
      m_instance);
}


// IChapter
CDVDInputStream::IChapter* CInputStreamAddon::GetIChapter()
{
  if ((m_caps.m_mask & INPUTSTREAM_SUPPORTS_ICHAPTER) == 0)
    return nullptr;

  return this;
}

int CInputStreamAddon::GetChapter()
{
  return m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_get_chapter_v1(m_instance);
}

int CInputStreamAddon::GetChapterCount()
{
  return m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_get_chapter_count_v1(
      m_instance);
}

void CInputStreamAddon::GetChapterName(std::string& name, int ch)
{
  name.clear();
  const char* res =
      m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_get_chapter_name_v1(
          m_instance, ch);
  if (res)
    name = res;
}

int64_t CInputStreamAddon::GetChapterPos(int ch)
{
  return m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_get_chapter_pos_v1(
      m_instance, ch);
}

bool CInputStreamAddon::SeekChapter(int ch)
{
  return m_ifc->kodi_addoninstance_inputstream_h->kodi_addon_inputstream_seek_chapter_v1(m_instance,
                                                                                         ch);
}

int CInputStreamAddon::ConvertVideoCodecProfile(STREAMCODEC_PROFILE profile)
{
  switch (profile)
  {
  case H264CodecProfileBaseline:
    return FF_PROFILE_H264_BASELINE;
  case H264CodecProfileMain:
    return FF_PROFILE_H264_MAIN;
  case H264CodecProfileExtended:
    return FF_PROFILE_H264_EXTENDED;
  case H264CodecProfileHigh:
    return FF_PROFILE_H264_HIGH;
  case H264CodecProfileHigh10:
    return FF_PROFILE_H264_HIGH_10;
  case H264CodecProfileHigh422:
    return FF_PROFILE_H264_HIGH_422;
  case H264CodecProfileHigh444Predictive:
    return FF_PROFILE_H264_HIGH_444_PREDICTIVE;
  case VP9CodecProfile0:
    return FF_PROFILE_VP9_0;
  case VP9CodecProfile1:
    return FF_PROFILE_VP9_1;
  case VP9CodecProfile2:
    return FF_PROFILE_VP9_2;
  case VP9CodecProfile3:
    return FF_PROFILE_VP9_3;
  default:
    return FF_PROFILE_UNKNOWN;
  }
}

/*!
 * Callbacks from add-on to kodi
 */
//@{
DEMUX_PACKET* CInputStreamAddon::cb_allocate_demux_packet(int data_size)
{
  return CDVDDemuxUtils::AllocateDemuxPacket(data_size);
}

DEMUX_PACKET* CInputStreamAddon::cb_allocate_encrypted_demux_packet(
    unsigned int dataSize, unsigned int encryptedSubsampleCount)
{
  return CDVDDemuxUtils::AllocateDemuxPacket(dataSize, encryptedSubsampleCount);
}

void CInputStreamAddon::cb_free_demux_packet(DEMUX_PACKET* packet)
{
  CDVDDemuxUtils::FreeDemuxPacket(static_cast<DemuxPacket*>(packet));
}

//@}
