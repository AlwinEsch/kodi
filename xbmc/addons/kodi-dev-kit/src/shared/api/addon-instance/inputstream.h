/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/* File autogenerated, see xbmc/addons/kodi-dev-kit/tools/code-generator/code_generator.py */

#pragma once

#include "../../../../include/kodi/c-api/addon-instance/inputstream.h"
#include "src/shared/SharedGroups.h"

#include <string.h>
#include <string>
#include <tuple>
#include <vector>

/* API struct from 1 to 1 */
typedef struct KODI_ADDON_INPUTSTREAM_FUNC_V1_TO_V1
{
  PFN_KODI_ADDON_INPUTSTREAM_CREATE_V1 create; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_DESTROY_V1 destroy; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_OPEN_V1 open; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_CLOSE_V1 close; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_GET_PATH_LIST_V1 get_path_list; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_GET_CAPABILITIES_V1 get_capabilities; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_GET_STREAM_IDS_V1 get_stream_ids; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_GET_STREAM_V1 get_stream; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_ENABLE_STREAM_V1 enable_stream; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_OPEN_STREAM_V1 open_stream; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_DEMUX_RESET_V1 demux_reset; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_DEMUX_ABORT_V1 demux_abort; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_DEMUX_FLUSH_V1 demux_flush; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_DEMUX_READ_V1 demux_read; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_DEMUX_SEEK_TIME_V1 demux_seek_time; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_DEMUX_SET_SPEED_V1 demux_set_speed; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_SET_VIDEO_RESOLUTION_V1
      set_video_resolution; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_GET_TOTAL_TIME_V1 get_total_time; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_GET_TIME_V1 get_time; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_GET_TIMES_V1 get_times; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_POS_TIME_V1 pos_time; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_READ_STREAM_V1 read_stream; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_SEEK_STREAM_V1 seek_stream; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_POSITION_STREAM_V1 position_stream; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_LENGTH_STREAM_V1 length_stream; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_IS_REAL_TIME_STREAM_V1
      is_real_time_stream; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_GET_CHAPTER_V1 get_chapter; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_GET_CHAPTER_COUNT_V1
      get_chapter_count; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_GET_CHAPTER_NAME_V1 get_chapter_name; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_GET_CHAPTER_POS_V1 get_chapter_pos; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_SEEK_CHAPTER_V1 seek_chapter; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
  PFN_KODI_ADDON_INPUTSTREAM_BLOCK_SIZE_STREAM_V1
      block_size_stream; /*---AUTO<EQUAL_FROM_TO:1_1>---*/
} KODI_ADDON_INPUTSTREAM_FUNC_V1_TO_V1;

union KODI_ADDON_INPUTSTREAM_FUNC_UNION
{
  struct KODI_ADDON_INPUTSTREAM_FUNC_V1_TO_V1 v1;
};

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1_END>---*/

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_2>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_2_END>---*/

// Code here relates to direct API calls without use of shared mem from addon to Kodi.
//{{{

typedef struct DEMUX_PACKET*(
    ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_ALLOCATE_DEMUX_PACKET_V1)(
    void*, KODI_ADDON_INSTANCE_BACKEND_HDL, int);
typedef struct DEMUX_PACKET*(
    ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_ALLOCATE_ENCRYPTED_DEMUX_PACKET_V1)(
    void*, KODI_ADDON_INSTANCE_BACKEND_HDL, unsigned int, unsigned int);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_FREE_DEMUX_PACKET_V1)(
    void*, KODI_ADDON_INSTANCE_BACKEND_HDL, struct DEMUX_PACKET*);

struct directFuncToKodi_addoninstance_inputstream_h
{
  void* thisClassHdl;
  PFN_INT_KODI_ADDON_INPUTSTREAM_ALLOCATE_DEMUX_PACKET_V1
      kodi_addon_inputstream_allocate_demux_packet_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_ALLOCATE_ENCRYPTED_DEMUX_PACKET_V1
      kodi_addon_inputstream_allocate_encrypted_demux_packet_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_FREE_DEMUX_PACKET_V1 kodi_addon_inputstream_free_demux_packet_v1;
};

//}}}

// Code here relates to direct API calls without use of shared mem from Kodi to addon.
//{{{

typedef KODI_ADDON_INPUTSTREAM_HDL(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_CREATE_V1)(
    void*, KODI_ADDON_INSTANCE_BACKEND_HDL);
typedef bool(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_DEMUX_SEEK_TIME_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL, double, bool, double*);
typedef bool(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_GET_STREAM_IDS_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL, struct INPUTSTREAM_IDS*);
typedef bool(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_GET_STREAM_V1)(
    void*,
    const KODI_ADDON_INPUTSTREAM_HDL,
    int,
    struct INPUTSTREAM_INFO*,
    KODI_OWN_HDL*,
    PFN_CB_KODI_ADDON_INPUTSTREAM_TRANSFER_STREAM);
typedef bool(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_GET_TIMES_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL, struct INPUTSTREAM_TIMES*);
typedef bool(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_IS_REAL_TIME_STREAM_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL);
typedef bool(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_OPEN_STREAM_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL, int);
typedef bool(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_OPEN_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL, struct INPUTSTREAM_PROPERTY*);
typedef bool(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_POS_TIME_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL, int);
typedef bool(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_SEEK_CHAPTER_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL, int);
typedef const char*(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_GET_CHAPTER_NAME_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL, int);
typedef const char*(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_GET_PATH_LIST_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL);
typedef int(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_BLOCK_SIZE_STREAM_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL);
typedef int(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_GET_CHAPTER_COUNT_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL);
typedef int(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_GET_CHAPTER_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL);
typedef int(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_GET_TIME_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL);
typedef int(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_GET_TOTAL_TIME_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL);
typedef int(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_READ_STREAM_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL, uint8_t*, unsigned int);
typedef int64_t(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_GET_CHAPTER_POS_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL, int);
typedef int64_t(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_LENGTH_STREAM_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL);
typedef int64_t(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_POSITION_STREAM_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL);
typedef int64_t(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_SEEK_STREAM_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL, int64_t, int);
typedef struct DEMUX_PACKET*(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_DEMUX_READ_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_CLOSE_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_DEMUX_ABORT_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_DEMUX_FLUSH_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_DEMUX_RESET_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_DEMUX_SET_SPEED_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL, int);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_DESTROY_V1)(
    void*, KODI_ADDON_INPUTSTREAM_HDL);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_ENABLE_STREAM_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL, int, bool);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_GET_CAPABILITIES_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL, struct INPUTSTREAM_CAPABILITIES*);
typedef void(ATTR_INT_APIENTRYP PFN_INT_KODI_ADDON_INPUTSTREAM_SET_VIDEO_RESOLUTION_V1)(
    void*, const KODI_ADDON_INPUTSTREAM_HDL, int, int);

struct directFuncToAddon_addoninstance_inputstream_h
{
  void* thisClassHdl;
  PFN_INT_KODI_ADDON_INPUTSTREAM_BLOCK_SIZE_STREAM_V1 kodi_addon_inputstream_block_size_stream_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_CLOSE_V1 kodi_addon_inputstream_close_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_CREATE_V1 kodi_addon_inputstream_create_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_DEMUX_ABORT_V1 kodi_addon_inputstream_demux_abort_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_DEMUX_FLUSH_V1 kodi_addon_inputstream_demux_flush_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_DEMUX_READ_V1 kodi_addon_inputstream_demux_read_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_DEMUX_RESET_V1 kodi_addon_inputstream_demux_reset_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_DEMUX_SEEK_TIME_V1 kodi_addon_inputstream_demux_seek_time_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_DEMUX_SET_SPEED_V1 kodi_addon_inputstream_demux_set_speed_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_DESTROY_V1 kodi_addon_inputstream_destroy_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_ENABLE_STREAM_V1 kodi_addon_inputstream_enable_stream_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_GET_CAPABILITIES_V1 kodi_addon_inputstream_get_capabilities_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_GET_CHAPTER_COUNT_V1 kodi_addon_inputstream_get_chapter_count_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_GET_CHAPTER_NAME_V1 kodi_addon_inputstream_get_chapter_name_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_GET_CHAPTER_POS_V1 kodi_addon_inputstream_get_chapter_pos_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_GET_CHAPTER_V1 kodi_addon_inputstream_get_chapter_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_GET_PATH_LIST_V1 kodi_addon_inputstream_get_path_list_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_GET_STREAM_IDS_V1 kodi_addon_inputstream_get_stream_ids_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_GET_STREAM_V1 kodi_addon_inputstream_get_stream_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_GET_TIMES_V1 kodi_addon_inputstream_get_times_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_GET_TIME_V1 kodi_addon_inputstream_get_time_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_GET_TOTAL_TIME_V1 kodi_addon_inputstream_get_total_time_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_IS_REAL_TIME_STREAM_V1
      kodi_addon_inputstream_is_real_time_stream_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_LENGTH_STREAM_V1 kodi_addon_inputstream_length_stream_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_OPEN_STREAM_V1 kodi_addon_inputstream_open_stream_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_OPEN_V1 kodi_addon_inputstream_open_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_POSITION_STREAM_V1 kodi_addon_inputstream_position_stream_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_POS_TIME_V1 kodi_addon_inputstream_pos_time_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_READ_STREAM_V1 kodi_addon_inputstream_read_stream_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_SEEK_CHAPTER_V1 kodi_addon_inputstream_seek_chapter_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_SEEK_STREAM_V1 kodi_addon_inputstream_seek_stream_v1;
  PFN_INT_KODI_ADDON_INPUTSTREAM_SET_VIDEO_RESOLUTION_V1
      kodi_addon_inputstream_set_video_resolution_v1;
};

//}}}

typedef enum funcParent_addoninstance_inputstream_h : int
{
  funcParent_addoninstance_inputstream_h_dummy
} funcParent_addoninstance_inputstream_h;

// clang-format off
/*---AUTO_GEN_PARSE<FUNC_PARENT_TUPLES>---*/
/* NOTE: Ignored as direct api only is set! */
/*---AUTO_GEN_PARSE<FUNC_PARENT_TUPLES_END>---*/
// clang-format on

//==============================================================================

typedef enum funcChild_addoninstance_inputstream_h : int
{
  funcChild_addoninstance_inputstream_h_dummy
} funcChild_addoninstance_inputstream_h;

// clang-format off
/*---AUTO_GEN_PARSE<FUNC_CHILD_TUPLES>---*/
/* NOTE: Ignored as direct api only is set! */
/*---AUTO_GEN_PARSE<FUNC_CHILD_TUPLES_END>---*/
// clang-format on

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
