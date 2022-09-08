/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "AddonBase.h"
#include "General.h"
#include "c-api/audio_engine.h"

#ifdef __cplusplus

#include <vector>

namespace kodi
{
namespace audioengine
{

//¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// Main page text for audio engine group by Doxygen.
//{{{

//==============================================================================
///
/// @defgroup cpp_kodi_audioengine  Interface - kodi::audioengine
/// @ingroup cpp
/// @brief **Audio engine functions**\n
/// This interface contains auxiliary functions and classes which allow an addon
/// to play their own individual audio stream in Kodi.
///
/// Using @ref cpp_kodi_audioengine_CAEStream "kodi::audioengine::CAEStream",
/// a class can be created in this regard, about which the necessary stream data and
/// information are given to Kodi.
///
/// Via @ref kodi::audioengine::GetCurrentSinkFormat(), the audio formats currently
/// processed in Kodi can be called up beforehand in order to adapt your own stream
/// to them.
///
/// However, the created stream can also differ from this because Kodi changes
/// it to suit it.
///
///
/// ------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
///
/// #include <kodi/AudioEngine.h>
///
/// ...
///
/// kodi::audioengine::AudioEngineFormat format;
/// if (!kodi::audioengine::GetCurrentSinkFormat(format))
///   return false;
///
/// format.SetDataFormat(AUDIOENGINE_FMT_FLOATP);
/// format.SetChannelLayout(std::vector<AudioEngineChannel>(AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FR));
///
/// unsigned int myUsedSampleRate = format.GetSampleRate();
///
/// ...
///
/// kodi::audioengine::CAEStream* stream = new kodi::audioengine::CAEStream(format, AUDIO_STREAM_AUTOSTART);
///
/// ~~~~~~~~~~~~~
///
/// ------------------------------------------------------------------------
///
/// It has the header @ref AudioEngine.h "#include <kodi/AudioEngine.h>" be included
/// to enjoy it.
///
//------------------------------------------------------------------------------

//==============================================================================
///
/// @defgroup cpp_kodi_audioengine_Defs Definitions, structures and enumerators
/// @ingroup cpp_kodi_audioengine
/// @brief **Library definition values**\n
/// All audio engine functions associated data structures.
///
//------------------------------------------------------------------------------

//}}}

//¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// "C++" related audio engine definitions
//{{{

//==============================================================================
/// @defgroup cpp_kodi_audioengine_Defs_AudioEngineFormat class AudioEngineFormat
/// @ingroup cpp_kodi_audioengine_Defs
/// @brief **Audio format structure**\n
/// The audio format structure that fully defines a stream's audio
/// information.
///
/// With the help of this format information, Kodi adjusts its processing
/// accordingly.
///
///@{
class ATTR_DLL_LOCAL AudioEngineFormat
  : public addon::CStructHdl<AudioEngineFormat, AUDIO_ENGINE_FMT>
{
public:
  AudioEngineFormat()
  {
    m_cStructure->data_format = AUDIOENGINE_FMT_INVALID;
    m_cStructure->sample_rate = 0;
    m_cStructure->frames = 0;
    m_cStructure->frame_size = 0;
    m_cStructure->channel_count = 0;

    for (size_t ch = 0; ch < AUDIOENGINE_CH_MAX; ++ch)
      m_cStructure->channels[ch] = AUDIOENGINE_CH_NULL;
  }
  AudioEngineFormat(const AudioEngineFormat& channel) : CStructHdl(channel) {}
  AudioEngineFormat(const AUDIO_ENGINE_FMT* channel) : CStructHdl(channel) {}
  AudioEngineFormat(AUDIO_ENGINE_FMT* channel) : CStructHdl(channel) {}

  void SetDataFormat(AudioEngineDataFormat format) { m_cStructure->data_format = format; }
  AudioEngineDataFormat GetDataFormat() const { return m_cStructure->data_format; }

  void SetSampleRate(unsigned int rate) { m_cStructure->sample_rate = rate; }
  unsigned int GetSampleRate() const { return m_cStructure->sample_rate; }

  void SetChannelLayout(const std::vector<AudioEngineChannel>& layout)
  {
    // Reset first all to empty values to AUDIOENGINE_CH_NULL, in case given list is empty
    m_cStructure->channel_count = 0;
    for (size_t ch = 0; ch < AUDIOENGINE_CH_MAX; ++ch)
      m_cStructure->channels[ch] = AUDIOENGINE_CH_NULL;

    for (size_t ch = 0; ch < layout.size() && ch < AUDIOENGINE_CH_MAX; ++ch)
    {
      m_cStructure->channels[ch] = layout[ch];
      m_cStructure->channel_count++;
    }
  }

  std::vector<AudioEngineChannel> GetChannelLayout() const
  {
    std::vector<AudioEngineChannel> channels;
    for (size_t ch = 0; ch < AUDIOENGINE_CH_MAX; ++ch)
    {
      if (m_cStructure->channels[ch] == AUDIOENGINE_CH_NULL)
        break;

      channels.push_back(m_cStructure->channels[ch]);
    }
    return channels;
  }

  unsigned int GetChannelCount() { return m_cStructure->channel_count; }

  void SetFramesAmount(unsigned int frames) { m_cStructure->frames = frames; }
  unsigned int GetFramesAmount() const { return m_cStructure->frames; }

  void SetFrameSize(unsigned int frameSize) { m_cStructure->frame_size = frameSize; }
  unsigned int GetFrameSize() const { return m_cStructure->frame_size; }

  bool CompareFormat(const AudioEngineFormat* fmt)
  {
    if (!fmt)
    {
      return false;
    }

    if (m_cStructure->data_format != fmt->m_cStructure->data_format ||
        m_cStructure->sample_rate != fmt->m_cStructure->sample_rate ||
        m_cStructure->frames != fmt->m_cStructure->frames ||
        m_cStructure->frame_size != fmt->m_cStructure->frame_size ||
        m_cStructure->channel_count != fmt->m_cStructure->channel_count)
    {
      return false;
    }

    for (unsigned int ch = 0; ch < AUDIOENGINE_CH_MAX; ++ch)
    {
      if (fmt->m_cStructure->channels[ch] != m_cStructure->channels[ch])
      {
        return false;
      }
    }

    return true;
  }
};
///@}
//----------------------------------------------------------------------------

//}}}

//¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// "C++" AudioEngine addon interface
//{{{

//============================================================================
///
/// @defgroup cpp_kodi_audioengine_CAEStream class CAEStream
/// @ingroup cpp_kodi_audioengine
/// @brief **Audio Engine Stream Class**\n
/// Class that can be created by the addon in order to be able to transfer
/// audiostream data processed on the addon to Kodi and output it audibly.
///
/// This can create individually several times and performed in different
/// processes simultaneously.
///
/// It has the header @ref AudioEngine.h "#include <kodi/AudioEngine.h>" be
/// included to enjoy it.
///
//----------------------------------------------------------------------------
class ATTR_DLL_LOCAL CAEStream
{
public:
  //==========================================================================
  /// @ingroup cpp_kodi_audioengine_CAEStream
  /// @brief Constructs new class to a Kodi IAEStream in the format specified.
  ///
  /// @param[in] format       The data format the incoming audio will be in
  ///                         (e.g. @ref AUDIOENGINE_FMT_S16LE)
  /// @param[in] options      [opt] A bit field of stream options (see: enum @ref AudioEngineStreamOptions)
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// @copydetails cpp_kodi_audioengine_Defs_AudioEngineFormat_Help
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Bit options to pass (on Kodi by <c>IAE::MakeStream</c>)**
  ///
  /// | enum AEStreamOptions        | Value: | Description:
  /// |----------------------------:|:------:|:-----------------------------------
  /// | AUDIO_STREAM_FORCE_RESAMPLE | 1 << 0 | Force resample even if rates match
  /// | AUDIO_STREAM_PAUSED         | 1 << 1 | Create the stream paused
  /// | AUDIO_STREAM_AUTOSTART      | 1 << 2 | Autostart the stream when enough data is buffered
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  ///
  /// #include <kodi/AudioEngine.h>
  ///
  /// ...
  ///
  /// kodi::audioengine::AudioEngineFormat format;
  ///
  /// format.SetDataFormat(AUDIOENGINE_FMT_FLOATP); /* The stream's data format (eg, AUDIOENGINE_FMT_S16LE) */
  /// format.SetChannelLayout(std::vector<AudioEngineChannel>(AUDIOENGINE_CH_FL, AUDIOENGINE_CH_FR)); /* The stream's channel layout */
  /// format.SetSampleRate(48000); /* The stream's sample rate (eg, 48000) */
  /// format.SetFrameSize(sizeof(float)*2); /* The size of one frame in bytes */
  /// format.SetFramesAmount(882); /* The number of samples in one frame */
  ///
  /// kodi::audioengine::CAEStream* stream = new kodi::audioengine::CAEStream(format, AUDIO_STREAM_AUTOSTART);
  ///
  /// ~~~~~~~~~~~~~
  ///
  CAEStream(AudioEngineFormat& format, unsigned int options = 0) : m_format(format)
  {
    m_streamHandle = kodi::dl::api.kodi_audioengine_stream_make(m_format, options);
    if (m_streamHandle == nullptr)
    {
      kodi::Log(ADDON_LOG_FATAL, "CAEStream: make_stream failed!");
      return;
    }

    m_planes = kodi::dl::api.kodi_audioengine_is_planar_format(format.GetDataFormat())
                   ? format.GetChannelCount()
                   : 1;
  }
  //------------------------------------------------------------------------------

  ~CAEStream()
  {
    if (m_streamHandle)
    {
      kodi::dl::api.kodi_audioengine_stream_free(m_streamHandle);
      m_streamHandle = nullptr;
    }
  }

  unsigned int GetSpace()
  {
    return kodi::dl::api.kodi_audioengine_stream_get_space(m_streamHandle);
  }

  unsigned int AddData(uint8_t* const* data,
                       unsigned int offset,
                       unsigned int frames,
                       double pts = 0,
                       bool hasDownmix = false,
                       double centerMixLevel = 1.0)
  {
    return kodi::dl::api.kodi_audioengine_stream_add_data(
        m_streamHandle, data, m_planes, frames * m_format.GetFrameSize(), offset, frames,
        m_format.GetFrameSize(), pts, hasDownmix, centerMixLevel);
  }

  double GetDelay() { return kodi::dl::api.kodi_audioengine_stream_get_delay(m_streamHandle); }

  bool IsBuffering() { return kodi::dl::api.kodi_audioengine_stream_is_buffering(m_streamHandle); }

  double GetCacheTime()
  {
    return kodi::dl::api.kodi_audioengine_stream_get_cache_time(m_streamHandle);
  }

  double GetCacheTotal()
  {
    return kodi::dl::api.kodi_audioengine_stream_get_cache_total(m_streamHandle);
  }

  void Pause() { return kodi::dl::api.kodi_audioengine_stream_pause(m_streamHandle); }

  void Resume() { return kodi::dl::api.kodi_audioengine_stream_resume(m_streamHandle); }

  void Drain(bool wait = true)
  {
    return kodi::dl::api.kodi_audioengine_stream_drain(m_streamHandle, wait);
  }

  bool IsDraining() { return kodi::dl::api.kodi_audioengine_stream_is_draining(m_streamHandle); }

  bool IsDrained() { return kodi::dl::api.kodi_audioengine_stream_is_drained(m_streamHandle); }

  void Flush() { return kodi::dl::api.kodi_audioengine_stream_flush(m_streamHandle); }

  float GetVolume() { return kodi::dl::api.kodi_audioengine_stream_get_volume(m_streamHandle); }

  void SetVolume(float volume)
  {
    return kodi::dl::api.kodi_audioengine_stream_set_volume(m_streamHandle, volume);
  }

  float GetAmplification()
  {
    return kodi::dl::api.kodi_audioengine_stream_get_amplification(m_streamHandle);
  }

  void SetAmplification(float amplify)
  {
    return kodi::dl::api.kodi_audioengine_stream_set_amplification(m_streamHandle, amplify);
  }

  unsigned int GetFrameSize() const
  {
    return kodi::dl::api.kodi_audioengine_stream_get_frame_size(m_streamHandle);
  }

  unsigned int GetChannelCount() const
  {
    return kodi::dl::api.kodi_audioengine_stream_get_channel_count(m_streamHandle);
  }

  unsigned int GetSampleRate() const
  {
    return kodi::dl::api.kodi_audioengine_stream_get_sample_rate(m_streamHandle);
  }

  AudioEngineDataFormat GetDataFormat() const
  {
    return kodi::dl::api.kodi_audioengine_stream_get_data_format(m_streamHandle);
  }

  double GetResampleRatio()
  {
    return kodi::dl::api.kodi_audioengine_stream_get_resample_ratio(m_streamHandle);
  }

  void SetResampleRatio(double ratio)
  {
    kodi::dl::api.kodi_audioengine_stream_set_resample_ratio(m_streamHandle, ratio);
  }

private:
  const AudioEngineFormat m_format;
  int m_planes = 1;
  KODI_AE_HDL m_streamHandle;
};
//----------------------------------------------------------------------------

//============================================================================
/// @ingroup cpp_kodi_audioengine
/// @brief Get the current sink data format.
///
/// @param[in] format Current sink data format. For more details see AudioEngineFormat.
/// @return Returns true on success, else false.
///
///
/// ------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
///
/// #include <kodi/AudioEngine.h>
///
/// ...
///
/// kodi::audioengine::AudioEngineFormat format;
/// if (!kodi::audioengine::GetCurrentSinkFormat(format))
///   return false;
///
/// std::vector<AudioEngineChannel> layout = format.GetChannelLayout();
///
/// ...
/// return true;
///
/// ~~~~~~~~~~~~~
///
inline bool ATTR_DLL_LOCAL GetCurrentSinkFormat(AudioEngineFormat& format)
{
  return kodi::dl::api.kodi_audioengine_get_current_sink_format(format);
}

inline bool IsPlanarFormat(AudioEngineDataFormat format)
{
  return kodi::dl::api.kodi_audioengine_is_planar_format(format);
}

} // namespace audioengine
} // namespace kodi

#endif /* __cplusplus */
