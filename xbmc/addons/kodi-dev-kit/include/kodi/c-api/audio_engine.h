/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_AUDIO_ENGINE_H
#define C_API_AUDIO_ENGINE_H

#include "addon_base.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  typedef void* KODI_AE_HDL;

  typedef enum AudioEngineStreamOptions
  {
    AUDIO_STREAM_FORCE_RESAMPLE = 1 << 0,
    AUDIO_STREAM_PAUSED = 1 << 1,
    AUDIO_STREAM_AUTOSTART = 1 << 2,
  } AudioEngineStreamOptions;

  enum AudioEngineDataFormat
  {
    AUDIOENGINE_FMT_INVALID = -1,
    AUDIOENGINE_FMT_U8,
    AUDIOENGINE_FMT_S16BE,
    AUDIOENGINE_FMT_S16LE,
    AUDIOENGINE_FMT_S16NE,
    AUDIOENGINE_FMT_S32BE,
    AUDIOENGINE_FMT_S32LE,
    AUDIOENGINE_FMT_S32NE,
    AUDIOENGINE_FMT_S24BE4,
    AUDIOENGINE_FMT_S24LE4,
    AUDIOENGINE_FMT_S24NE4,
    AUDIOENGINE_FMT_S24NE4MSB,
    AUDIOENGINE_FMT_S24BE3,
    AUDIOENGINE_FMT_S24LE3,
    AUDIOENGINE_FMT_S24NE3,
    AUDIOENGINE_FMT_DOUBLE,
    AUDIOENGINE_FMT_FLOAT,
    AUDIOENGINE_FMT_RAW,
    AUDIOENGINE_FMT_U8P,
    AUDIOENGINE_FMT_S16NEP,
    AUDIOENGINE_FMT_S32NEP,
    AUDIOENGINE_FMT_S24NE4P,
    AUDIOENGINE_FMT_S24NE4MSBP,
    AUDIOENGINE_FMT_S24NE3P,
    AUDIOENGINE_FMT_DOUBLEP,
    AUDIOENGINE_FMT_FLOATP,
    AUDIOENGINE_FMT_MAX
  };

  enum AudioEngineChannel
  {
    AUDIOENGINE_CH_NULL = -1,
    AUDIOENGINE_CH_RAW,
    AUDIOENGINE_CH_FL,
    AUDIOENGINE_CH_FR,
    AUDIOENGINE_CH_FC,
    AUDIOENGINE_CH_LFE,
    AUDIOENGINE_CH_BL,
    AUDIOENGINE_CH_BR,
    AUDIOENGINE_CH_FLOC,
    AUDIOENGINE_CH_FROC,
    AUDIOENGINE_CH_BC,
    AUDIOENGINE_CH_SL,
    AUDIOENGINE_CH_SR,
    AUDIOENGINE_CH_TFL,
    AUDIOENGINE_CH_TFR,
    AUDIOENGINE_CH_TFC,
    AUDIOENGINE_CH_TC,
    AUDIOENGINE_CH_TBL,
    AUDIOENGINE_CH_TBR,
    AUDIOENGINE_CH_TBC,
    AUDIOENGINE_CH_BLOC,
    AUDIOENGINE_CH_BROC,
    AUDIOENGINE_CH_MAX
  };

  struct AUDIO_ENGINE_FMT
  {
    enum AudioEngineDataFormat data_format;
    unsigned int sample_rate;
    unsigned int channel_count;
    enum AudioEngineChannel channels[AUDIOENGINE_CH_MAX];
    unsigned int frames;
    unsigned int frame_size;
  };

  ATTR_DLL_EXPORT bool kodi_audioengine_get_current_sink_format(struct AUDIO_ENGINE_FMT* fmt)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_audioengine_is_planar_format(enum AudioEngineDataFormat format)
      __INTRODUCED_IN_KODI(1);

  ATTR_DLL_EXPORT KODI_AE_HDL kodi_audioengine_stream_make(const struct AUDIO_ENGINE_FMT* fmt,
                                                           unsigned int options)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_audioengine_stream_free(KODI_AE_HDL hdl) __INTRODUCED_IN_KODI(1);

  ATTR_DLL_EXPORT unsigned int kodi_audioengine_stream_get_space(KODI_AE_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT unsigned int kodi_audioengine_stream_add_data(KODI_AE_HDL hdl,
                                                                uint8_t* const* data,
                                                                size_t planes,
                                                                size_t data_size,
                                                                unsigned int offset,
                                                                unsigned int frames,
                                                                unsigned int frame_size,
                                                                double pts,
                                                                bool hasDownmix,
                                                                double centerMixLevel)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT double kodi_audioengine_stream_get_delay(KODI_AE_HDL hdl) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_audioengine_stream_is_buffering(KODI_AE_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT double kodi_audioengine_stream_get_cache_time(KODI_AE_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT double kodi_audioengine_stream_get_cache_total(KODI_AE_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_audioengine_stream_pause(KODI_AE_HDL hdl) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_audioengine_stream_resume(KODI_AE_HDL hdl) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_audioengine_stream_drain(KODI_AE_HDL hdl, bool wait)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_audioengine_stream_is_draining(KODI_AE_HDL hdl) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_audioengine_stream_is_drained(KODI_AE_HDL hdl) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_audioengine_stream_flush(KODI_AE_HDL hdl) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT float kodi_audioengine_stream_get_volume(KODI_AE_HDL hdl) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_audioengine_stream_set_volume(KODI_AE_HDL hdl, float volume)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT float kodi_audioengine_stream_get_amplification(KODI_AE_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_audioengine_stream_set_amplification(KODI_AE_HDL hdl, float amplify)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT unsigned int kodi_audioengine_stream_get_frame_size(KODI_AE_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT unsigned int kodi_audioengine_stream_get_channel_count(KODI_AE_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT unsigned int kodi_audioengine_stream_get_sample_rate(KODI_AE_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT enum AudioEngineDataFormat kodi_audioengine_stream_get_data_format(
      KODI_AE_HDL hdl) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT double kodi_audioengine_stream_get_resample_ratio(KODI_AE_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_audioengine_stream_set_resample_ratio(KODI_AE_HDL hdl, double ratio)
      __INTRODUCED_IN_KODI(1);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !C_API_AUDIO_ENGINE_H */
