/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/*---AUTO_GEN_PARSE<OVERRIDE;USE_DIRECT_API_ONLY>---*/

#ifndef C_API_ADDONINSTANCE_INPUTSTREAM_H
#define C_API_ADDONINSTANCE_INPUTSTREAM_H

#include "../addon_base.h"
#include "inputstream/demux_packet.h"
#include "inputstream/stream_codec.h"
#include "inputstream/stream_constants.h"
#include "inputstream/stream_crypto.h"
#include "inputstream/timing_constants.h"

#include <time.h>

// Increment this level always if you add features which can lead to compile failures in the addon
#define INPUTSTREAM_VERSION_LEVEL 4

#define INPUTSTREAM_MAX_INFO_COUNT 8
#define INPUTSTREAM_MAX_STREAM_COUNT 256
#define INPUTSTREAM_MAX_STRING_NAME_SIZE 256
#define INPUTSTREAM_MAX_STRING_CODEC_SIZE 32
#define INPUTSTREAM_MAX_STRING_LANGUAGE_SIZE 64

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  typedef void* KODI_ADDON_INPUTSTREAM_HDL;

  //==============================================================================
  /// @ingroup cpp_kodi_addon_inputstream_Defs_Interface_InputstreamCapabilities
  /// @brief **Capability types of inputstream addon.**\n
  /// This values are needed to tell Kodi which options are supported on the addon.
  ///
  /// If one of this is defined, then the corresponding methods from
  /// @ref cpp_kodi_addon_inputstream "kodi::addon::CInstanceInputStream" need
  /// to be implemented.
  ///
  /// Used on @ref kodi::addon::CInstanceInputStream::GetCapabilities().
  ///
  ///@{
  enum INPUTSTREAM_MASKTYPE
  {
    /// @brief **0000 0000 0000 0001 :** Supports interface demuxing.
    ///
    /// If set must be @ref cpp_kodi_addon_inputstream_Demux "Demux support" included.
    INPUTSTREAM_SUPPORTS_IDEMUX = (1 << 0),

    /// @brief **0000 0000 0000 0010 :** Supports interface position time.
    ///
    /// This means that the start time and the current stream time are used.
    ///
    /// If set must be @ref cpp_kodi_addon_inputstream_Time "Time support" included.
    INPUTSTREAM_SUPPORTS_IPOSTIME = (1 << 1),

    /// @brief **0000 0000 0000 0100 :** Supports interface for display time.
    ///
    /// This will call up the complete stream time information. The start time
    /// and the individual PTS times are then given using @ref cpp_kodi_addon_inputstream_Defs_Times.
    ///
    /// If set must be @ref cpp_kodi_addon_inputstream_Times "Times support" included.
    INPUTSTREAM_SUPPORTS_IDISPLAYTIME = (1 << 2),

    /// @brief **0000 0000 0000 1000 :** Supports seek
    INPUTSTREAM_SUPPORTS_SEEK = (1 << 3),

    /// @brief **0000 0000 0001 0000 :** Supports pause
    INPUTSTREAM_SUPPORTS_PAUSE = (1 << 4),

    /// @brief **0000 0000 0010 0000 :** Supports interface to give position time.
    ///
    /// This will only ask for the current time of the stream, not for length or
    /// start.
    ///
    /// If set must be @ref cpp_kodi_addon_inputstream_PosTime "Position time support" included.
    INPUTSTREAM_SUPPORTS_ITIME = (1 << 5),

    /// @brief **0000 0000 0100 0000 :** Supports interface for chapter selection.
    ///
    /// If set must be @ref cpp_kodi_addon_inputstream_Chapter "Chapter support" included.
    INPUTSTREAM_SUPPORTS_ICHAPTER = (1 << 6),
  };
  ///@}
  //----------------------------------------------------------------------------

  /*!
   * @brief InputStream add-on capabilities. All capabilities are set to "false" as default.
   */
  struct INPUTSTREAM_CAPABILITIES
  {
    /// set of supported capabilities
    uint32_t m_mask;
  };

  /*!
   * @brief structure of key/value pairs passed to addon on Open()
   */
  struct INPUTSTREAM_PROPERTY
  {
    const char* m_strURL;
    const char* m_mimeType;

    unsigned int m_nCountInfoValues;
    struct LISTITEMPROPERTY
    {
      const char* m_strKey;
      const char* m_strValue;
    } m_ListItemProperties[STREAM_MAX_PROPERTY_COUNT];

    const char* m_libFolder;
    const char* m_profileFolder;
  };

  /*!
   * @brief Array of stream IDs
   */
  struct INPUTSTREAM_IDS
  {
    unsigned int m_streamCount;
    unsigned int m_streamIds[INPUTSTREAM_MAX_STREAM_COUNT];
  };

  /*!
   * @brief MASTERING Metadata
   */
  struct INPUTSTREAM_MASTERING_METADATA
  {
    double primary_r_chromaticity_x;
    double primary_r_chromaticity_y;
    double primary_g_chromaticity_x;
    double primary_g_chromaticity_y;
    double primary_b_chromaticity_x;
    double primary_b_chromaticity_y;
    double white_point_chromaticity_x;
    double white_point_chromaticity_y;
    double luminance_max;
    double luminance_min;
  };

  /*!
   * @brief CONTENTLIGHT Metadata
   */
  struct INPUTSTREAM_CONTENTLIGHT_METADATA
  {
    uint64_t max_cll;
    uint64_t max_fall;
  };

  //==============================================================================
  /// @defgroup cpp_kodi_addon_inputstream_Defs_Interface_INPUTSTREAM_TYPE enum INPUTSTREAM_TYPE
  /// @ingroup cpp_kodi_addon_inputstream_Defs_Interface
  /// @brief **Inputstream types**\n
  /// To identify type on stream.
  ///
  /// Used on @ref kodi::addon::InputstreamInfo::SetStreamType and @ref kodi::addon::InputstreamInfo::GetStreamType.
  ///
  ///@{
  enum INPUTSTREAM_TYPE
  {
    /// @brief **0 :** To set nothing defined
    INPUTSTREAM_TYPE_NONE = 0,

    /// @brief **1 :** To identify @ref cpp_kodi_addon_inputstream_Defs_Info as Video
    INPUTSTREAM_TYPE_VIDEO,

    /// @brief **2 :** To identify @ref cpp_kodi_addon_inputstream_Defs_Info as Audio
    INPUTSTREAM_TYPE_AUDIO,

    /// @brief **3 :** To identify @ref cpp_kodi_addon_inputstream_Defs_Info as Subtitle
    INPUTSTREAM_TYPE_SUBTITLE,

    /// @brief **4 :** To identify @ref cpp_kodi_addon_inputstream_Defs_Info as Teletext
    INPUTSTREAM_TYPE_TELETEXT,

    /// @brief **5 :** To identify @ref cpp_kodi_addon_inputstream_Defs_Info as Radio RDS
    INPUTSTREAM_TYPE_RDS,
  };
  ///@}
  //------------------------------------------------------------------------------

  //==============================================================================
  /// @defgroup cpp_kodi_addon_inputstream_Defs_Interface_INPUTSTREAM_CODEC_FEATURES enum INPUTSTREAM_CODEC_FEATURES
  /// @ingroup cpp_kodi_addon_inputstream_Defs_Interface
  /// @brief **Inputstream codec features**\n
  /// To identify special extra features used for optional codec on inputstream.
  ///
  /// Used on @ref kodi::addon::InputstreamInfo::SetFeatures and @ref kodi::addon::InputstreamInfo::GetFeatures.
  ///
  /// @note These variables are bit flags which are created using "|" can be used together.
  ///
  ///@{
  enum INPUTSTREAM_CODEC_FEATURES
  {
    /// @brief **0000 0000 0000 0000 :** Empty to set if nothing is used
    INPUTSTREAM_FEATURE_NONE = 0,

    /// @brief **0000 0000 0000 0001 :** To set addon decode should used with @ref cpp_kodi_addon_videocodec.
    INPUTSTREAM_FEATURE_DECODE = (1 << 0)
  };
  ///@}
  //----------------------------------------------------------------------------

  //============================================================================
  /// @defgroup cpp_kodi_addon_inputstream_Defs_Interface_INPUTSTREAM_FLAGS enum INPUTSTREAM_FLAGS
  /// @ingroup cpp_kodi_addon_inputstream_Defs_Interface
  /// @brief **Inputstream flags**\n
  /// To identify extra stream flags used on inputstream.
  ///
  /// Used on @ref kodi::addon::InputstreamInfo::SetFlags and @ref kodi::addon::InputstreamInfo::GetFlags.
  ///
  /// @note These variables are bit flags which are created using "|" can be used together.
  ///
  ///@{
  enum INPUTSTREAM_FLAGS
  {
    /// @brief **0000 0000 0000 0000 :** Empty to set if nothing is used
    INPUTSTREAM_FLAG_NONE = 0,

    /// @brief **0000 0000 0000 0001 :** Default
    INPUTSTREAM_FLAG_DEFAULT = (1 << 0),

    /// @brief **0000 0000 0000 0010 :** Dub
    INPUTSTREAM_FLAG_DUB = (1 << 1),

    /// @brief **0000 0000 0000 0100 :** Original
    INPUTSTREAM_FLAG_ORIGINAL = (1 << 2),

    /// @brief **0000 0000 0000 1000 :** Comment
    INPUTSTREAM_FLAG_COMMENT = (1 << 3),

    /// @brief **0000 0000 0001 0000 :** Lyrics
    INPUTSTREAM_FLAG_LYRICS = (1 << 4),

    /// @brief **0000 0000 0010 0000 :** Karaoke
    INPUTSTREAM_FLAG_KARAOKE = (1 << 5),

    /// @brief **0000 0000 0100 0000 :** Forced
    INPUTSTREAM_FLAG_FORCED = (1 << 6),

    /// @brief **0000 0000 1000 0000 :** Hearing impaired
    INPUTSTREAM_FLAG_HEARING_IMPAIRED = (1 << 7),

    /// @brief **0000 0001 0000 0000 :** Visual impaired
    INPUTSTREAM_FLAG_VISUAL_IMPAIRED = (1 << 8),
  };
  ///@}
  //----------------------------------------------------------------------------

  // Keep in sync with AVColorSpace
  //============================================================================
  /// @defgroup cpp_kodi_addon_inputstream_Defs_Interface_INPUTSTREAM_COLORSPACE enum INPUTSTREAM_COLORSPACE
  /// @ingroup cpp_kodi_addon_inputstream_Defs_Interface
  /// @brief **Inputstream color space flags**\n
  /// YUV colorspace type. These values match the ones defined by ISO/IEC 23001-8_2013 § 7.3.
  ///
  /// Used on @ref kodi::addon::InputstreamInfo::SetColorSpace and @ref kodi::addon::InputstreamInfo::GetColorSpace.
  ///
  ///@{
  enum INPUTSTREAM_COLORSPACE
  {
    /// @brief **0 :** Order of coefficients is actually GBR, also IEC 61966-2-1 (sRGB)
    INPUTSTREAM_COLORSPACE_RGB = 0,

    /// @brief **1 :** Also ITU-R BT1361 / IEC 61966-2-4 xvYCC709 / SMPTE RP177 Annex B
    INPUTSTREAM_COLORSPACE_BT709 = 1,

    /// @brief **2 :** To set stream is unspecified
    INPUTSTREAM_COLORSPACE_UNSPECIFIED = 2,

    /// @brief **2 :** To set stream is unknown
    /// @note Same as @ref INPUTSTREAM_COLORSPACE_UNSPECIFIED
    INPUTSTREAM_COLORSPACE_UNKNOWN = INPUTSTREAM_COLORSPACE_UNSPECIFIED, // compatibility

    /// @brief **3 :** To set colorspace reserved
    INPUTSTREAM_COLORSPACE_RESERVED = 3,

    /// @brief **4 :** FCC Title 47 Code of Federal Regulations 73.682 (a)(20)
    INPUTSTREAM_COLORSPACE_FCC = 4,

    /// @brief **5 :** Also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM / IEC 61966-2-4 xvYCC601
    INPUTSTREAM_COLORSPACE_BT470BG = 5,

    /// @brief **6 :** Also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC
    INPUTSTREAM_COLORSPACE_SMPTE170M = 6,

    /// @brief **7 :** Functionally identical to above @ref INPUTSTREAM_COLORSPACE_SMPTE170M
    INPUTSTREAM_COLORSPACE_SMPTE240M = 7,

    /// @brief **8 :** Used by Dirac / VC-2 and H.264 FRext, see ITU-T SG16
    INPUTSTREAM_COLORSPACE_YCGCO = 8,

    /// @brief **8 :** To set colorspace as YCOCG
    /// @note Same as @ref INPUTSTREAM_COLORSPACE_YCGCO
    INPUTSTREAM_COLORSPACE_YCOCG = INPUTSTREAM_COLORSPACE_YCGCO,

    /// @brief **9 :** ITU-R BT2020 non-constant luminance system
    INPUTSTREAM_COLORSPACE_BT2020_NCL = 9,

    /// @brief **10 :** ITU-R BT2020 constant luminance system
    INPUTSTREAM_COLORSPACE_BT2020_CL = 10,

    /// @brief **11 :** SMPTE 2085, Y'D'zD'x
    INPUTSTREAM_COLORSPACE_SMPTE2085 = 11,

    /// @brief **12 :** Chromaticity-derived non-constant luminance system
    INPUTSTREAM_COLORSPACE_CHROMA_DERIVED_NCL = 12,

    /// @brief **13 :** Chromaticity-derived constant luminance system
    INPUTSTREAM_COLORSPACE_CHROMA_DERIVED_CL = 13,

    /// @brief **14 :** ITU-R BT.2100-0, ICtCp
    INPUTSTREAM_COLORSPACE_ICTCP = 14,

    /// @brief The maximum value to use in a list.
    INPUTSTREAM_COLORSPACE_MAX
  };
  ///@}
  //------------------------------------------------------------------------------

  // Keep in sync with AVColorPrimaries
  //==============================================================================
  /// @defgroup cpp_kodi_addon_inputstream_Defs_Interface_INPUTSTREAM_COLORPRIMARIES enum INPUTSTREAM_COLORPRIMARIES
  /// @ingroup cpp_kodi_addon_inputstream_Defs_Interface
  /// @brief **Inputstream color primaries flags**\n
  /// Chromaticity coordinates of the source primaries. These values match the ones defined by ISO/IEC 23001-8_2013 § 7.1.
  ///
  /// Used on @ref kodi::addon::InputstreamInfo::SetColorPrimaries and @ref kodi::addon::InputstreamInfo::GetColorPrimaries.
  ///
  ///@{
  enum INPUTSTREAM_COLORPRIMARIES
  {
    /// @brief **0 :** Reserved
    INPUTSTREAM_COLORPRIMARY_RESERVED0 = 0,

    /// @brief **1 :** Also ITU-R BT1361 / IEC 61966-2-4 / SMPTE RP177 Annex B
    INPUTSTREAM_COLORPRIMARY_BT709 = 1,

    /// @brief **2 :** Unspecified
    INPUTSTREAM_COLORPRIMARY_UNSPECIFIED = 2,

    /// @brief **3 :** Reserved
    INPUTSTREAM_COLORPRIMARY_RESERVED = 3,

    /// @brief **4 :** Also FCC Title 47 Code of Federal Regulations 73.682 (a)(20)
    INPUTSTREAM_COLORPRIMARY_BT470M = 4,

    /// @brief **5 :** Also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM
    INPUTSTREAM_COLORPRIMARY_BT470BG = 5,

    /// @brief **6 :** Also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC
    INPUTSTREAM_COLORPRIMARY_SMPTE170M = 6,

    /// @brief **7 :** Functionally identical to above
    INPUTSTREAM_COLORPRIMARY_SMPTE240M = 7,

    /// @brief **8 :** Colour filters using Illuminant C
    INPUTSTREAM_COLORPRIMARY_FILM = 8,

    /// @brief **9 :** ITU-R BT2020
    INPUTSTREAM_COLORPRIMARY_BT2020 = 9,

    /// @brief **10 :** SMPTE ST 428-1 (CIE 1931 XYZ)
    INPUTSTREAM_COLORPRIMARY_SMPTE428 = 10,

    /// @brief **10 :**
    /// @note Same as @ref INPUTSTREAM_COLORPRIMARY_SMPTE428
    INPUTSTREAM_COLORPRIMARY_SMPTEST428_1 = INPUTSTREAM_COLORPRIMARY_SMPTE428,

    /// @brief **11 :** SMPTE ST 431-2 (2011) / DCI P3
    INPUTSTREAM_COLORPRIMARY_SMPTE431 = 11,

    /// @brief **12 :** SMPTE ST 432-1 (2010) / P3 D65 / Display P3
    INPUTSTREAM_COLORPRIMARY_SMPTE432 = 12,

    /// @brief **22 :** JEDEC P22 phosphors
    INPUTSTREAM_COLORPRIMARY_JEDEC_P22 = 22,

    /// @brief The maximum value to use in a list.
    INPUTSTREAM_COLORPRIMARY_MAX
  };
  ///@}
  //------------------------------------------------------------------------------

  // Keep in sync with AVColorRange
  //==============================================================================
  /// @defgroup cpp_kodi_addon_inputstream_Defs_Interface_INPUTSTREAM_COLORRANGE enum INPUTSTREAM_COLORRANGE
  /// @ingroup cpp_kodi_addon_inputstream_Defs_Interface
  /// @brief **Inputstream color range flags**\n
  /// MPEG vs JPEG YUV range.
  ///
  /// Used on @ref kodi::addon::InputstreamInfo::SetColorRange and @ref kodi::addon::InputstreamInfo::GetColorRange.
  ///
  ///@{
  enum INPUTSTREAM_COLORRANGE
  {
    /// @brief **0 :** To define as unknown
    INPUTSTREAM_COLORRANGE_UNKNOWN = 0,

    /// @brief **1 :** The normal 219*2^(n-8) "MPEG" YUV ranges
    INPUTSTREAM_COLORRANGE_LIMITED,

    /// @brief **2 :** The normal 2^n-1 "JPEG" YUV ranges
    INPUTSTREAM_COLORRANGE_FULLRANGE,

    /// @brief The maximum value to use in a list.
    INPUTSTREAM_COLORRANGE_MAX
  };
  ///@}
  //------------------------------------------------------------------------------

  // keep in sync with AVColorTransferCharacteristic
  //==============================================================================
  /// @defgroup cpp_kodi_addon_inputstream_Defs_Interface_INPUTSTREAM_COLORTRC enum INPUTSTREAM_COLORTRC
  /// @ingroup cpp_kodi_addon_inputstream_Defs_Interface
  /// @brief **Inputstream color TRC flags**\n
  /// Color Transfer Characteristic. These values match the ones defined by ISO/IEC 23001-8_2013 § 7.2.
  ///
  /// Used on @ref kodi::addon::InputstreamInfo::SetColorTransferCharacteristic and @ref kodi::addon::InputstreamInfo::GetColorTransferCharacteristic.
  ///
  ///@{
  enum INPUTSTREAM_COLORTRC
  {
    /// @brief **0 :** Reserved
    INPUTSTREAM_COLORTRC_RESERVED0 = 0,

    /// @brief **1 :** Also ITU-R BT1361
    INPUTSTREAM_COLORTRC_BT709 = 1,

    /// @brief **2 :** Unspecified
    INPUTSTREAM_COLORTRC_UNSPECIFIED = 2,

    /// @brief **3 :** Reserved
    INPUTSTREAM_COLORTRC_RESERVED = 3,

    /// @brief **4 :** Also ITU-R BT470M / ITU-R BT1700 625 PAL & SECAM
    INPUTSTREAM_COLORTRC_GAMMA22 = 4,

    /// @brief **5 :** Also ITU-R BT470BG
    INPUTSTREAM_COLORTRC_GAMMA28 = 5,

    /// @brief **6 :** Also ITU-R BT601-6 525 or 625 / ITU-R BT1358 525 or 625 / ITU-R BT1700 NTSC
    INPUTSTREAM_COLORTRC_SMPTE170M = 6,

    /// @brief **7 :** Functionally identical to above @ref INPUTSTREAM_COLORTRC_SMPTE170M
    INPUTSTREAM_COLORTRC_SMPTE240M = 7,

    /// @brief **8 :** Linear transfer characteristics
    INPUTSTREAM_COLORTRC_LINEAR = 8,

    /// @brief **9 :** Logarithmic transfer characteristic (100:1 range)
    INPUTSTREAM_COLORTRC_LOG = 9,

    /// @brief **10 :** Logarithmic transfer characteristic (100 * Sqrt(10) : 1 range)
    INPUTSTREAM_COLORTRC_LOG_SQRT = 10,

    /// @brief **11 :** IEC 61966-2-4
    INPUTSTREAM_COLORTRC_IEC61966_2_4 = 11,

    /// @brief **12 :** ITU-R BT1361 Extended Colour Gamut
    INPUTSTREAM_COLORTRC_BT1361_ECG = 12,

    /// @brief **13 :** IEC 61966-2-1 (sRGB or sYCC)
    INPUTSTREAM_COLORTRC_IEC61966_2_1 = 13,

    /// @brief **14 :**  ITU-R BT2020 for 10-bit system
    INPUTSTREAM_COLORTRC_BT2020_10 = 14,

    /// @brief **15 :**  ITU-R BT2020 for 12-bit system
    INPUTSTREAM_COLORTRC_BT2020_12 = 15,

    /// @brief **16 :**  SMPTE ST 2084 for 10-, 12-, 14- and 16-bit systems
    INPUTSTREAM_COLORTRC_SMPTE2084 = 16,

    /// @brief **16 :** Same as @ref INPUTSTREAM_COLORTRC_SMPTE2084
    INPUTSTREAM_COLORTRC_SMPTEST2084 = INPUTSTREAM_COLORTRC_SMPTE2084,

    /// @brief **17 :**  SMPTE ST 428-1
    INPUTSTREAM_COLORTRC_SMPTE428 = 17,

    /// @brief **17 :**  Same as @ref INPUTSTREAM_COLORTRC_SMPTE428
    INPUTSTREAM_COLORTRC_SMPTEST428_1 = INPUTSTREAM_COLORTRC_SMPTE428,

    /// @brief **18 :**  ARIB STD-B67, known as "Hybrid log-gamma"
    INPUTSTREAM_COLORTRC_ARIB_STD_B67 = 18,

    /// @brief The maximum value to use in a list.
    INPUTSTREAM_COLORTRC_MAX
  };
  ///@}
  //------------------------------------------------------------------------------

  /*!
   * @brief stream properties
   */
  struct INPUTSTREAM_INFO
  {
    enum INPUTSTREAM_TYPE m_streamType;
    uint32_t m_features;
    uint32_t m_flags;

    //! @brief (optional) name of the stream, \0 for default handling
    char m_name[INPUTSTREAM_MAX_STRING_NAME_SIZE];

    //! @brief (required) name of codec according to ffmpeg
    char m_codecName[INPUTSTREAM_MAX_STRING_CODEC_SIZE];

    //! @brief (optional) internal name of codec (selectionstream info)
    char m_codecInternalName[INPUTSTREAM_MAX_STRING_CODEC_SIZE];

    //! @brief (optional) the profile of the codec
    enum STREAMCODEC_PROFILE m_codecProfile;

    //! @brief (required) physical index
    unsigned int m_pID;

    const uint8_t* m_ExtraData;
    unsigned int m_ExtraSize;

    //! @brief RFC 5646 language code (empty string if undefined)
    char m_language[INPUTSTREAM_MAX_STRING_LANGUAGE_SIZE];

    //! Video stream related data
    //@{

    //! @brief Scale of 1000 and a rate of 29970 will result in 29.97 fps
    unsigned int m_FpsScale;

    unsigned int m_FpsRate;

    //! @brief height of the stream reported by the demuxer
    unsigned int m_Height;

    //! @brief width of the stream reported by the demuxer
    unsigned int m_Width;

    //! @brief display aspect of stream
    float m_Aspect;

    //@}

    //! Audio stream related data
    //@{

    //! @brief (required) amount of channels
    unsigned int m_Channels;

    //! @brief (required) sample rate
    unsigned int m_SampleRate;

    //! @brief (required) bit rate
    unsigned int m_BitRate;

    //! @brief (required) bits per sample
    unsigned int m_BitsPerSample;

    unsigned int m_BlockAlign;

    //@}

    struct STREAM_CRYPTO_SESSION m_cryptoSession;

    // new in API version 2.0.8
    //@{
    //! @brief Codec If available, the fourcc code codec
    unsigned int m_codecFourCC;

    //! @brief definition of colorspace
    enum INPUTSTREAM_COLORSPACE m_colorSpace;

    //! @brief color range if available
    enum INPUTSTREAM_COLORRANGE m_colorRange;
    //@}

    //new in API 2.0.9 / INPUTSTREAM_VERSION_LEVEL 1
    //@{
    enum INPUTSTREAM_COLORPRIMARIES m_colorPrimaries;
    enum INPUTSTREAM_COLORTRC m_colorTransferCharacteristic;
    //@}

    //! @brief mastering static Metadata
    struct INPUTSTREAM_MASTERING_METADATA* m_masteringMetadata;

    //! @brief content light static Metadata
    struct INPUTSTREAM_CONTENTLIGHT_METADATA* m_contentLightMetadata;
  };

  struct INPUTSTREAM_TIMES
  {
    time_t startTime;
    double ptsStart;
    double ptsBegin;
    double ptsEnd;
  };

  typedef KODI_ADDON_INSTANCE_BACKEND_HDL (*PFN_CB_KODI_ADDON_INPUTSTREAM_TRANSFER_STREAM)(
      KODI_ADDON_INSTANCE_BACKEND_HDL handle, int streamId, struct INPUTSTREAM_INFO* stream);

  typedef KODI_ADDON_INPUTSTREAM_HDL(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_CREATE_V1)(
      KODI_ADDON_INSTANCE_BACKEND_HDL kodi_hdl);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_DESTROY_V1)(
      KODI_ADDON_INPUTSTREAM_HDL hdl);

  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_OPEN_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl, struct INPUTSTREAM_PROPERTY* props);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_CLOSE_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl);
  typedef const char*(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_GET_PATH_LIST_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_GET_CAPABILITIES_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl, struct INPUTSTREAM_CAPABILITIES* capabilities);

  // IDemux
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_GET_STREAM_IDS_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl, struct INPUTSTREAM_IDS* ids);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_GET_STREAM_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl,
      int streamid,
      struct INPUTSTREAM_INFO* info,
      KODI_OWN_HDL* demuxStream,
      PFN_CB_KODI_ADDON_INPUTSTREAM_TRANSFER_STREAM transfer_stream);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_ENABLE_STREAM_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl, int streamid, bool enable);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_OPEN_STREAM_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl, int streamid);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_DEMUX_RESET_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_DEMUX_ABORT_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_DEMUX_FLUSH_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl);
  typedef struct DEMUX_PACKET*(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_DEMUX_READ_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_DEMUX_SEEK_TIME_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl, double time, bool backwards, double* startpts);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_DEMUX_SET_SPEED_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl, int speed);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_SET_VIDEO_RESOLUTION_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl, int width, int height);

  // IDisplayTime
  typedef int(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_GET_TOTAL_TIME_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl);
  typedef int(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_GET_TIME_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl);

  // ITime
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_GET_TIMES_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl, struct INPUTSTREAM_TIMES* times);

  // IPosTime
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_POS_TIME_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl, int ms);

  typedef int(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_READ_STREAM_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl, uint8_t* buffer, unsigned int bufferSize);
  typedef int64_t(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_SEEK_STREAM_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl, int64_t position, int whence);
  typedef int64_t(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_POSITION_STREAM_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl);
  typedef int64_t(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_LENGTH_STREAM_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_IS_REAL_TIME_STREAM_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl);

  // IChapter
  typedef int(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_GET_CHAPTER_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl);
  typedef int(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_GET_CHAPTER_COUNT_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl);
  typedef const char*(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_GET_CHAPTER_NAME_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl, int ch);
  typedef int64_t(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_GET_CHAPTER_POS_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl, int ch);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_SEEK_CHAPTER_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl, int ch);

  typedef int(ATTR_APIENTRYP PFN_KODI_ADDON_INPUTSTREAM_BLOCK_SIZE_STREAM_V1)(
      const KODI_ADDON_INPUTSTREAM_HDL hdl);

  typedef struct KODI_ADDON_INPUTSTREAM_FUNC
  {
    PFN_KODI_ADDON_INPUTSTREAM_CREATE_V1 create;
    PFN_KODI_ADDON_INPUTSTREAM_DESTROY_V1 destroy;

    PFN_KODI_ADDON_INPUTSTREAM_OPEN_V1 open;
    PFN_KODI_ADDON_INPUTSTREAM_CLOSE_V1 close;
    PFN_KODI_ADDON_INPUTSTREAM_GET_PATH_LIST_V1 get_path_list;
    PFN_KODI_ADDON_INPUTSTREAM_GET_CAPABILITIES_V1 get_capabilities;

    // IDemux
    PFN_KODI_ADDON_INPUTSTREAM_GET_STREAM_IDS_V1 get_stream_ids;
    PFN_KODI_ADDON_INPUTSTREAM_GET_STREAM_V1 get_stream;
    PFN_KODI_ADDON_INPUTSTREAM_ENABLE_STREAM_V1 enable_stream;
    PFN_KODI_ADDON_INPUTSTREAM_OPEN_STREAM_V1 open_stream;
    PFN_KODI_ADDON_INPUTSTREAM_DEMUX_RESET_V1 demux_reset;
    PFN_KODI_ADDON_INPUTSTREAM_DEMUX_ABORT_V1 demux_abort;
    PFN_KODI_ADDON_INPUTSTREAM_DEMUX_FLUSH_V1 demux_flush;
    PFN_KODI_ADDON_INPUTSTREAM_DEMUX_READ_V1 demux_read;
    PFN_KODI_ADDON_INPUTSTREAM_DEMUX_SEEK_TIME_V1 demux_seek_time;
    PFN_KODI_ADDON_INPUTSTREAM_DEMUX_SET_SPEED_V1 demux_set_speed;
    PFN_KODI_ADDON_INPUTSTREAM_SET_VIDEO_RESOLUTION_V1 set_video_resolution;

    // IDisplayTime
    PFN_KODI_ADDON_INPUTSTREAM_GET_TOTAL_TIME_V1 get_total_time;
    PFN_KODI_ADDON_INPUTSTREAM_GET_TIME_V1 get_time;

    // ITime
    PFN_KODI_ADDON_INPUTSTREAM_GET_TIMES_V1 get_times;

    // IPosTime
    PFN_KODI_ADDON_INPUTSTREAM_POS_TIME_V1 pos_time;
    PFN_KODI_ADDON_INPUTSTREAM_READ_STREAM_V1 read_stream;
    PFN_KODI_ADDON_INPUTSTREAM_SEEK_STREAM_V1 seek_stream;
    PFN_KODI_ADDON_INPUTSTREAM_POSITION_STREAM_V1 position_stream;
    PFN_KODI_ADDON_INPUTSTREAM_LENGTH_STREAM_V1 length_stream;
    PFN_KODI_ADDON_INPUTSTREAM_IS_REAL_TIME_STREAM_V1 is_real_time_stream;

    // IChapter
    PFN_KODI_ADDON_INPUTSTREAM_GET_CHAPTER_V1 get_chapter;
    PFN_KODI_ADDON_INPUTSTREAM_GET_CHAPTER_COUNT_V1 get_chapter_count;
    PFN_KODI_ADDON_INPUTSTREAM_GET_CHAPTER_NAME_V1 get_chapter_name;
    PFN_KODI_ADDON_INPUTSTREAM_GET_CHAPTER_POS_V1 get_chapter_pos;
    PFN_KODI_ADDON_INPUTSTREAM_SEEK_CHAPTER_V1 seek_chapter;
    PFN_KODI_ADDON_INPUTSTREAM_BLOCK_SIZE_STREAM_V1 block_size_stream;
  } KODI_ADDON_INPUTSTREAM_FUNC;

  ATTR_DLL_EXPORT struct DEMUX_PACKET* kodi_addon_inputstream_allocate_demux_packet(
      KODI_ADDON_INSTANCE_BACKEND_HDL hdl, int data_size) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT struct DEMUX_PACKET* kodi_addon_inputstream_allocate_encrypted_demux_packet(
      KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
      unsigned int data_size,
      unsigned int encrypted_subsample_count) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_inputstream_free_demux_packet(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                                                struct DEMUX_PACKET* packet)
      __INTRODUCED_IN_KODI(1);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_ADDONINSTANCE_INPUTSTREAM_H */
