/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_ADDONINSTANCE_AUDIOENCODER_H
#define C_API_ADDONINSTANCE_AUDIOENCODER_H

#include "../addon_base.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if __KODI_API__ >= 1

  typedef KODI_ADDON_INSTANCE_HDL KODI_ADDON_AUDIOENCODER_HDL;

  struct KODI_ADDON_AUDIOENCODER_INFO_TAG
  {
    const char* title;
    const char* artist;
    const char* album;
    const char* album_artist;
    const char* media_type;
    const char* genre;
    int duration;
    int track;
    int disc;
    const char* disc_subtitle;
    int disc_total;
    const char* release_date;
    const char* lyrics;
    int samplerate;
    int channels;
    int bits_per_sample;
    int track_length;
    const char* comment;
  };

  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_AUDIOENCODER_START_V1)(
      KODI_ADDON_AUDIOENCODER_HDL hdl, const struct KODI_ADDON_AUDIOENCODER_INFO_TAG* tag);
  typedef ssize_t(ATTR_APIENTRYP PFN_KODI_ADDON_AUDIOENCODER_ENCODE_V1)(
      KODI_ADDON_AUDIOENCODER_HDL hdl, const uint8_t* pbt_stream, size_t num_bytes_read);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_AUDIOENCODER_FINISH_V1)(
      KODI_ADDON_AUDIOENCODER_HDL hdl);

  typedef struct KODI_ADDON_AUDIOENCODER_FUNC
  {
    PFN_KODI_ADDON_INSTANCE_CREATE_V1 create;
    PFN_KODI_ADDON_INSTANCE_DESTROY_V1 destroy;
    PFN_KODI_ADDON_AUDIOENCODER_START_V1 start;
    PFN_KODI_ADDON_AUDIOENCODER_ENCODE_V1 encode;
    PFN_KODI_ADDON_AUDIOENCODER_FINISH_V1 finish;
  } KODI_ADDON_AUDIOENCODER_FUNC;

  ATTR_DLL_EXPORT ssize_t kodi_addon_audioencoder_write(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                                        const uint8_t* data,
                                                        size_t len) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT ssize_t kodi_addon_audioencoder_seek(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                                       ssize_t pos,
                                                       int whence) __INTRODUCED_IN_KODI(1);

#endif /* __KODI_API__ >= 1 */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_ADDONINSTANCE_AUDIOENCODER_H */
