/*
 *  Copyright (C) 2017-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "AddonVideoCodec.h"

#include "addons/interface/api/addon-instance/videocodec.h"
#include "cores/VideoPlayer/Buffers/VideoBuffer.h"
#include "cores/VideoPlayer/DVDCodecs/DVDCodecs.h"
#include "cores/VideoPlayer/DVDStreamInfo.h"
#include "cores/VideoPlayer/Interface/DemuxCrypto.h"
#include "cores/VideoPlayer/Interface/TimingConstants.h"
#include "utils/log.h"

CAddonVideoCodec::CAddonVideoCodec(CProcessInfo& processInfo,
                                   ADDON::AddonInfoPtr& addonInfo,
                                   KODI_HANDLE parentInstance)
  : CDVDVideoCodec(processInfo),
    IInstanceHandler(this, ADDON_INSTANCE_VIDEOCODEC, addonInfo, ADDON::ADDON_INSTANCE_ID_UNUSED, parentInstance)
{
  if (CreateInstance() != ADDON_STATUS_OK)
  {
    CLog::Log(LOGERROR, "CInputStreamAddon: Failed to create add-on instance for '{}'",
              addonInfo->ID());
    return;
  }
}

CAddonVideoCodec::~CAddonVideoCodec()
{
  //free remaining buffers
  Reset();

  DestroyInstance();
}

bool CAddonVideoCodec::CopyToInitData(VIDEOCODEC_INITDATA &initData, CDVDStreamInfo &hints)
{
  initData.codec_profile = STREAMCODEC_PROFILE::CodecProfileNotNeeded;
  switch (hints.codec)
  {
  case AV_CODEC_ID_H264:
    initData.codec = VIDEOCODEC_H264;
    switch (hints.profile)
    {
    case 0:
    case FF_PROFILE_UNKNOWN:
      initData.codec_profile = STREAMCODEC_PROFILE::CodecProfileUnknown;
      break;
    case FF_PROFILE_H264_BASELINE:
      initData.codec_profile = STREAMCODEC_PROFILE::H264CodecProfileBaseline;
      break;
    case FF_PROFILE_H264_MAIN:
      initData.codec_profile = STREAMCODEC_PROFILE::H264CodecProfileMain;
      break;
    case FF_PROFILE_H264_EXTENDED:
      initData.codec_profile = STREAMCODEC_PROFILE::H264CodecProfileExtended;
      break;
    case FF_PROFILE_H264_HIGH:
      initData.codec_profile = STREAMCODEC_PROFILE::H264CodecProfileHigh;
      break;
    case FF_PROFILE_H264_HIGH_10:
      initData.codec_profile = STREAMCODEC_PROFILE::H264CodecProfileHigh10;
      break;
    case FF_PROFILE_H264_HIGH_422:
      initData.codec_profile = STREAMCODEC_PROFILE::H264CodecProfileHigh422;
      break;
    case FF_PROFILE_H264_HIGH_444_PREDICTIVE:
      initData.codec_profile = STREAMCODEC_PROFILE::H264CodecProfileHigh444Predictive;
      break;
    default:
      return false;
    }
    break;
  case AV_CODEC_ID_VP8:
    initData.codec = VIDEOCODEC_VP8;
    break;
  case AV_CODEC_ID_VP9:
    initData.codec = VIDEOCODEC_VP9;
    switch (hints.profile)
    {
    case FF_PROFILE_UNKNOWN:
      initData.codec_profile = STREAMCODEC_PROFILE::CodecProfileUnknown;
      break;
    case FF_PROFILE_VP9_0:
      initData.codec_profile = STREAMCODEC_PROFILE::VP9CodecProfile0;
      break;
    case FF_PROFILE_VP9_1:
      initData.codec_profile = STREAMCODEC_PROFILE::VP9CodecProfile1;
      break;
    case FF_PROFILE_VP9_2:
      initData.codec_profile = STREAMCODEC_PROFILE::VP9CodecProfile2;
      break;
    case FF_PROFILE_VP9_3:
      initData.codec_profile = STREAMCODEC_PROFILE::VP9CodecProfile3;
      break;
    default:
      return false;
    }
    break;
  default:
    return false;
  }
  if (hints.cryptoSession)
  {
    switch (hints.cryptoSession->keySystem)
    {
    case CRYPTO_SESSION_SYSTEM_NONE:
      initData.crypto_session.keySystem = STREAM_CRYPTO_KEY_SYSTEM_NONE;
      break;
    case CRYPTO_SESSION_SYSTEM_WIDEVINE:
      initData.crypto_session.keySystem = STREAM_CRYPTO_KEY_SYSTEM_WIDEVINE;
      break;
    case CRYPTO_SESSION_SYSTEM_PLAYREADY:
      initData.crypto_session.keySystem = STREAM_CRYPTO_KEY_SYSTEM_PLAYREADY;
      break;
    case CRYPTO_SESSION_SYSTEM_WISEPLAY:
      initData.crypto_session.keySystem = STREAM_CRYPTO_KEY_SYSTEM_WISEPLAY;
      break;
    default:
      return false;
    }

    strncpy(initData.crypto_session.sessionId, hints.cryptoSession->sessionId.c_str(),
            sizeof(initData.crypto_session.sessionId) - 1);
  }

  initData.extra_data = reinterpret_cast<const uint8_t*>(hints.extradata);
  initData.extra_data_size = hints.extrasize;
  initData.width = hints.width;
  initData.height = hints.height;
  initData.video_formats = m_formats;

  m_displayAspect = (hints.aspect > 0.0 && !hints.forced_aspect) ? static_cast<float>(hints.aspect) : 0.0f;
  m_width = hints.width;
  m_height = hints.height;

  m_processInfo.SetVideoDimensions(hints.width, hints.height);
  m_processInfo.SetVideoDAR(m_displayAspect);
  if (hints.fpsscale)
    m_processInfo.SetVideoFps(static_cast<float>(hints.fpsrate) / hints.fpsscale);

  return true;
}

bool CAddonVideoCodec::Open(CDVDStreamInfo &hints, CDVDCodecOptions &options)
{
  unsigned int nformats(0);
  m_formats[nformats++] = VIDEOCODEC_FORMAT_YV12;
  m_formats[nformats] = VIDEOCODEC_FORMAT_UNKNOWN;

  VIDEOCODEC_INITDATA initData;
  if (!CopyToInitData(initData, hints))
    return false;

  bool ret =
      m_ifc->kodi_addoninstance_videocodec_h->kodi_addon_videocodec_open_v1(m_instance, &initData);
  m_processInfo.SetVideoDecoderName(GetName(), false);

  return ret;
}

bool CAddonVideoCodec::Reconfigure(CDVDStreamInfo &hints)
{
  VIDEOCODEC_INITDATA initData;
  if (!CopyToInitData(initData, hints))
    return false;

  return m_ifc->kodi_addoninstance_videocodec_h->kodi_addon_videocodec_reconfigure_v1(m_instance,
                                                                                      &initData);
}

bool CAddonVideoCodec::AddData(const DemuxPacket &packet)
{
  return m_ifc->kodi_addoninstance_videocodec_h->kodi_addon_videocodec_add_data_v1(m_instance,
                                                                                   &packet);
}

CDVDVideoCodec::VCReturn CAddonVideoCodec::GetPicture(VideoPicture* pVideoPicture)
{
  VIDEOCODEC_PICTURE picture;
  picture.flags = (m_codecFlags & DVD_CODEC_CTRL_DRAIN) ? VIDEOCODEC_PICTURE_FLAG_DRAIN
                                                        : VIDEOCODEC_PICTURE_FLAG_DROP;

  switch (m_ifc->kodi_addoninstance_videocodec_h->kodi_addon_videocodec_get_picture_v1(m_instance,
                                                                                       &picture))
  {
  case VIDEOCODEC_RETVAL::VC_NONE:
    return CDVDVideoCodec::VC_NONE;
  case VIDEOCODEC_RETVAL::VC_ERROR:
    return CDVDVideoCodec::VC_ERROR;
  case VIDEOCODEC_RETVAL::VC_BUFFER:
    return CDVDVideoCodec::VC_BUFFER;
  case VIDEOCODEC_RETVAL::VC_PICTURE:
    pVideoPicture->iWidth = picture.width;
    pVideoPicture->iHeight = picture.height;
    pVideoPicture->pts = static_cast<double>(picture.pts);
    pVideoPicture->dts = DVD_NOPTS_VALUE;
    pVideoPicture->iFlags = 0;
    pVideoPicture->chroma_position = 0;
    pVideoPicture->colorBits = 8;
    pVideoPicture->color_primaries = AVColorPrimaries::AVCOL_PRI_UNSPECIFIED;
    pVideoPicture->color_range = 0;
    pVideoPicture->color_space = AVCOL_SPC_UNSPECIFIED;
    pVideoPicture->color_transfer = 0;
    pVideoPicture->hasDisplayMetadata = false;
    pVideoPicture->hasLightMetadata = false;
    pVideoPicture->iDuration = 0;
    pVideoPicture->iFrameType = 0;
    pVideoPicture->iRepeatPicture = 0;
    pVideoPicture->pict_type = 0;
    pVideoPicture->qp_table = nullptr;
    pVideoPicture->qscale_type = 0;
    pVideoPicture->qstride = 0;
    pVideoPicture->stereoMode.clear();

    if (m_codecFlags & DVD_CODEC_CTRL_DROP)
      pVideoPicture->iFlags |= DVP_FLAG_DROPPED;

    if (pVideoPicture->videoBuffer)
      pVideoPicture->videoBuffer->Release();

    pVideoPicture->videoBuffer = static_cast<CVideoBuffer*>(picture.video_buffer_handle);

    int strides[YuvImage::MAX_PLANES], plane_offsets[YuvImage::MAX_PLANES];
    for (int i = 0; i<YuvImage::MAX_PLANES; ++i)
      strides[i] = picture.stride[i];
    for (int i = 0; i<YuvImage::MAX_PLANES; ++i)
      plane_offsets[i] = picture.plane_offsets[i];

    pVideoPicture->videoBuffer->SetDimensions(picture.width, picture.height, strides,
                                              plane_offsets);

    pVideoPicture->iDisplayWidth = pVideoPicture->iWidth;
    pVideoPicture->iDisplayHeight = pVideoPicture->iHeight;
    if (m_displayAspect > 0.0f)
    {
      pVideoPicture->iDisplayWidth = ((int)lrint(pVideoPicture->iHeight * m_displayAspect)) & ~3;
      if (pVideoPicture->iDisplayWidth > pVideoPicture->iWidth)
      {
        pVideoPicture->iDisplayWidth = pVideoPicture->iWidth;
        pVideoPicture->iDisplayHeight = ((int)lrint(pVideoPicture->iWidth / m_displayAspect)) & ~3;
      }
    }

    CLog::Log(LOGDEBUG, LOGVIDEO,
              "CAddonVideoCodec: GetPicture::VC_PICTURE with pts {} {}x{} ({}x{}) {} {}:{} "
              "offset:{},{},{}, stride:{},{},{}",
              picture.pts, pVideoPicture->iWidth, pVideoPicture->iHeight,
              pVideoPicture->iDisplayWidth, pVideoPicture->iDisplayHeight, m_displayAspect,
              fmt::ptr(picture.decoded_data), picture.decoded_data_size, picture.plane_offsets[0],
              picture.plane_offsets[1], picture.plane_offsets[2], picture.stride[0],
              picture.stride[1], picture.stride[2]);

    if (picture.width != m_width || picture.height != m_height)
    {
      m_width = picture.width;
      m_height = picture.height;
      m_processInfo.SetVideoDimensions(m_width, m_height);
    }

    return CDVDVideoCodec::VC_PICTURE;
  case VIDEOCODEC_RETVAL::VC_EOF:
    CLog::Log(LOGINFO, "CAddonVideoCodec: GetPicture: EOF");
    return CDVDVideoCodec::VC_EOF;
  default:
    return CDVDVideoCodec::VC_ERROR;
  }
}

const char* CAddonVideoCodec::GetName()
{
  return m_ifc->kodi_addoninstance_videocodec_h->kodi_addon_videocodec_get_name_v1(m_instance);
}

void CAddonVideoCodec::Reset()
{
  CVideoBuffer *videoBuffer;

  CLog::Log(LOGDEBUG, "CAddonVideoCodec: Reset");

  // Get the remaining pictures out of the external decoder
  VIDEOCODEC_PICTURE picture;
  picture.flags = VIDEOCODEC_PICTURE_FLAG_DRAIN;

  VIDEOCODEC_RETVAL ret;
  while ((ret = m_ifc->kodi_addoninstance_videocodec_h->kodi_addon_videocodec_get_picture_v1(
              m_instance, &picture)) != VIDEOCODEC_RETVAL::VC_EOF)
  {
    if (ret == VIDEOCODEC_RETVAL::VC_PICTURE)
    {
      videoBuffer = static_cast<CVideoBuffer*>(picture.video_buffer_handle);
      if (videoBuffer)
        videoBuffer->Release();
    }
  }
  m_ifc->kodi_addoninstance_videocodec_h->kodi_addon_videocodec_reset_v1(m_instance);
}

/*********************     ADDON-TO-KODI    **********************/

bool CAddonVideoCodec::GetFrameBuffer(VIDEOCODEC_PICTURE* picture)
{
  CVideoBuffer* videoBuffer = m_processInfo.GetVideoBufferManager().Get(
      AV_PIX_FMT_YUV420P, picture->decoded_data_size, nullptr);
  if (!videoBuffer)
  {
    CLog::Log(LOGERROR,"CAddonVideoCodec::GetFrameBuffer Failed to allocate buffer");
    return false;
  }
  picture->decoded_data = videoBuffer->GetMemPtr();
  picture->video_buffer_handle = videoBuffer;

  return true;
}

void CAddonVideoCodec::ReleaseFrameBuffer(KODI_HANDLE video_buffer_handle)
{
  if (video_buffer_handle)
    static_cast<CVideoBuffer*>(video_buffer_handle)->Release();
}
