/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/*---AUTO_GEN_PARSE<OVERRIDE;USE_DIRECT_API_ONLY>---*/

#ifndef C_API_ADDONINSTANCE_GAME_H
#define C_API_ADDONINSTANCE_GAME_H

#include "../addon_base.h"

#include <stddef.h>
#include <stdint.h>

//==============================================================================
/// @ingroup cpp_kodi_addon_game_Defs
/// @brief **Port ID used when topology is unknown**
#define DEFAULT_PORT_ID "1"
//------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  typedef void* KODI_ADDON_GAME_HDL;

  typedef enum GAME_ERROR
  {
    GAME_ERROR_NO_ERROR,
    GAME_ERROR_UNKNOWN,
    GAME_ERROR_NOT_IMPLEMENTED,
    GAME_ERROR_REJECTED,
    GAME_ERROR_INVALID_PARAMETERS,
    GAME_ERROR_FAILED,
    GAME_ERROR_NOT_LOADED,
    GAME_ERROR_RESTRICTED,
  } GAME_ERROR;

  typedef enum GAME_PCM_FORMAT
  {
    GAME_PCM_FORMAT_UNKNOWN,
    GAME_PCM_FORMAT_S16NE,
  } GAME_PCM_FORMAT;

  typedef enum GAME_AUDIO_CHANNEL
  {
    GAME_CH_NULL,
    GAME_CH_FL,
    GAME_CH_FR,
    GAME_CH_FC,
    GAME_CH_LFE,
    GAME_CH_BL,
    GAME_CH_BR,
    GAME_CH_FLOC,
    GAME_CH_FROC,
    GAME_CH_BC,
    GAME_CH_SL,
    GAME_CH_SR,
    GAME_CH_TFL,
    GAME_CH_TFR,
    GAME_CH_TFC,
    GAME_CH_TC,
    GAME_CH_TBL,
    GAME_CH_TBR,
    GAME_CH_TBC,
    GAME_CH_BLOC,
    GAME_CH_BROC,
  } GAME_AUDIO_CHANNEL;

  typedef struct game_stream_audio_properties
  {
    enum GAME_PCM_FORMAT format;
    const enum GAME_AUDIO_CHANNEL* channel_map;
  } game_stream_audio_properties;
  //----------------------------------------------------------------------------

  typedef struct game_stream_audio_packet
  {
    const uint8_t* data;
    size_t size;
  } game_stream_audio_packet;
  //----------------------------------------------------------------------------

  ///@}

  typedef enum GAME_PIXEL_FORMAT
  {
    GAME_PIXEL_FORMAT_UNKNOWN,
    GAME_PIXEL_FORMAT_0RGB8888,
    GAME_PIXEL_FORMAT_RGB565,
    GAME_PIXEL_FORMAT_0RGB1555,
  } GAME_PIXEL_FORMAT;

  typedef enum GAME_VIDEO_ROTATION
  {
    GAME_VIDEO_ROTATION_0,
    GAME_VIDEO_ROTATION_90_CCW,
    GAME_VIDEO_ROTATION_180_CCW,
    GAME_VIDEO_ROTATION_270_CCW,
  } GAME_VIDEO_ROTATION;

  typedef struct game_stream_video_properties
  {
    enum GAME_PIXEL_FORMAT format;
    unsigned int nominal_width;
    unsigned int nominal_height;
    unsigned int max_width;
    unsigned int max_height;
    float aspect_ratio;
  } game_stream_video_properties;
  //----------------------------------------------------------------------------

  typedef struct game_stream_video_packet
  {
    unsigned int width;
    unsigned int height;
    enum GAME_VIDEO_ROTATION rotation;
    const uint8_t* data;
    size_t size;
  } game_stream_video_packet;
  //----------------------------------------------------------------------------

  ///@}

  typedef enum GAME_HW_CONTEXT_TYPE
  {
    GAME_HW_CONTEXT_NONE,
    GAME_HW_CONTEXT_OPENGL,
    GAME_HW_CONTEXT_OPENGLES2,
    GAME_HW_CONTEXT_OPENGL_CORE,
    GAME_HW_CONTEXT_OPENGLES3,
    GAME_HW_CONTEXT_OPENGLES_VERSION,
    GAME_HW_CONTEXT_VULKAN
  } GAME_HW_CONTEXT_TYPE;

  typedef struct game_stream_hw_framebuffer_properties
  {
    enum GAME_HW_CONTEXT_TYPE context_type;
    bool depth;
    bool stencil;
    bool bottom_left_origin;
    unsigned int version_major;
    unsigned int version_minor;
    bool cache_context;
    bool debug_context;
  } game_stream_hw_framebuffer_properties;
  //----------------------------------------------------------------------------

  typedef struct game_stream_hw_framebuffer_buffer
  {
    uintptr_t framebuffer;
  } game_stream_hw_framebuffer_buffer;
  //----------------------------------------------------------------------------

  typedef struct game_stream_hw_framebuffer_packet
  {
    uintptr_t framebuffer;
  } game_stream_hw_framebuffer_packet;
  //----------------------------------------------------------------------------

  typedef void (*game_proc_address_t)(void);

  typedef game_stream_video_properties game_stream_sw_framebuffer_properties;

  typedef struct game_stream_sw_framebuffer_buffer
  {
    enum GAME_PIXEL_FORMAT format;
    uint8_t* data;
    size_t size;
  } game_stream_sw_framebuffer_buffer;
  //----------------------------------------------------------------------------

  typedef game_stream_video_packet game_stream_sw_framebuffer_packet;

  typedef enum GAME_STREAM_TYPE
  {
    GAME_STREAM_UNKNOWN,
    GAME_STREAM_AUDIO,
    GAME_STREAM_VIDEO,
    GAME_STREAM_HW_FRAMEBUFFER,
    GAME_STREAM_SW_FRAMEBUFFER,
  } GAME_STREAM_TYPE;

  typedef struct game_stream_properties
  {
    enum GAME_STREAM_TYPE type;
    union
    {
      game_stream_audio_properties audio;
      game_stream_video_properties video;
      game_stream_hw_framebuffer_properties hw_framebuffer;
      game_stream_sw_framebuffer_properties sw_framebuffer;
    };
  } game_stream_properties;
  //----------------------------------------------------------------------------

  typedef struct game_stream_buffer
  {
    GAME_STREAM_TYPE type;
    union
    {
      game_stream_hw_framebuffer_buffer hw_framebuffer;
      game_stream_sw_framebuffer_buffer sw_framebuffer;
    };
  } game_stream_buffer;
  //----------------------------------------------------------------------------

  typedef struct game_stream_packet
  {
    GAME_STREAM_TYPE type;
    union
    {
      game_stream_audio_packet audio;
      game_stream_video_packet video;
      game_stream_hw_framebuffer_packet hw_framebuffer;
      game_stream_sw_framebuffer_packet sw_framebuffer;
    };
  } game_stream_packet;
  //----------------------------------------------------------------------------

  ///@}

  //--==----==----==----==----==----==----==----==----==----==----==----==----==--
  /// @defgroup cpp_kodi_addon_game_Defs_GameTypes 6. Game types
  /// @ingroup cpp_kodi_addon_game_Defs
  /// @brief **Game types data**
  ///
  ///@{

  typedef enum GAME_REGION
  {
    GAME_REGION_UNKNOWN,
    GAME_REGION_NTSC,
    GAME_REGION_PAL,
  } GAME_REGION;

  typedef enum SPECIAL_GAME_TYPE
  {
    SPECIAL_GAME_TYPE_BSX,
    SPECIAL_GAME_TYPE_BSX_SLOTTED,
    SPECIAL_GAME_TYPE_SUFAMI_TURBO,
    SPECIAL_GAME_TYPE_SUPER_GAME_BOY,
  } SPECIAL_GAME_TYPE;

  typedef enum GAME_MEMORY
  {
    GAME_MEMORY_MASK = 0xff,
    GAME_MEMORY_SAVE_RAM = 0,
    GAME_MEMORY_RTC = 1,
    GAME_MEMORY_SYSTEM_RAM = 2,
    GAME_MEMORY_VIDEO_RAM = 3,
    GAME_MEMORY_SNES_BSX_RAM = ((1 << 8) | GAME_MEMORY_SAVE_RAM),
    GAME_MEMORY_SNES_BSX_PRAM = ((2 << 8) | GAME_MEMORY_SAVE_RAM),
    GAME_MEMORY_SNES_SUFAMI_TURBO_A_RAM = ((3 << 8) | GAME_MEMORY_SAVE_RAM),
    GAME_MEMORY_SNES_SUFAMI_TURBO_B_RAM = ((4 << 8) | GAME_MEMORY_SAVE_RAM),
    GAME_MEMORY_SNES_GAME_BOY_RAM = ((5 << 8) | GAME_MEMORY_SAVE_RAM),
    GAME_MEMORY_SNES_GAME_BOY_RTC = ((6 << 8) | GAME_MEMORY_RTC),
  } GAME_MEMORY;

  typedef enum GAME_SIMD
  {
    GAME_SIMD_SSE = (1 << 0),
    GAME_SIMD_SSE2 = (1 << 1),
    GAME_SIMD_VMX = (1 << 2),
    GAME_SIMD_VMX128 = (1 << 3),
    GAME_SIMD_AVX = (1 << 4),
    GAME_SIMD_NEON = (1 << 5),
    GAME_SIMD_SSE3 = (1 << 6),
    GAME_SIMD_SSSE3 = (1 << 7),
    GAME_SIMD_MMX = (1 << 8),
    GAME_SIMD_MMXEXT = (1 << 9),
    GAME_SIMD_SSE4 = (1 << 10),
    GAME_SIMD_SSE42 = (1 << 11),
    GAME_SIMD_AVX2 = (1 << 12),
    GAME_SIMD_VFPU = (1 << 13),
  } GAME_SIMD;

  typedef enum GAME_INPUT_EVENT_SOURCE
  {
    GAME_INPUT_EVENT_DIGITAL_BUTTON,
    GAME_INPUT_EVENT_ANALOG_BUTTON,
    GAME_INPUT_EVENT_AXIS,
    GAME_INPUT_EVENT_ANALOG_STICK,
    GAME_INPUT_EVENT_ACCELEROMETER,
    GAME_INPUT_EVENT_KEY,
    GAME_INPUT_EVENT_RELATIVE_POINTER,
    GAME_INPUT_EVENT_ABSOLUTE_POINTER,
    GAME_INPUT_EVENT_MOTOR,
  } GAME_INPUT_EVENT_SOURCE;

  typedef enum GAME_KEY_MOD
  {
    GAME_KEY_MOD_NONE = 0x0000,
    GAME_KEY_MOD_SHIFT = 0x0001,
    GAME_KEY_MOD_CTRL = 0x0002,
    GAME_KEY_MOD_ALT = 0x0004,
    GAME_KEY_MOD_META = 0x0008,
    GAME_KEY_MOD_SUPER = 0x0010,
    GAME_KEY_MOD_NUMLOCK = 0x0100,
    GAME_KEY_MOD_CAPSLOCK = 0x0200,
    GAME_KEY_MOD_SCROLLOCK = 0x0400,
  } GAME_KEY_MOD;

  typedef enum GAME_PORT_TYPE
  {
    GAME_PORT_UNKNOWN,
    GAME_PORT_KEYBOARD,
    GAME_PORT_MOUSE,
    GAME_PORT_CONTROLLER,
  } GAME_PORT_TYPE;

  typedef struct game_controller_layout
  {
    char* controller_id;
    bool provides_input; // False for multitaps
    char** digital_buttons;
    unsigned int digital_button_count;
    char** analog_buttons;
    unsigned int analog_button_count;
    char** analog_sticks;
    unsigned int analog_stick_count;
    char** accelerometers;
    unsigned int accelerometer_count;
    char** keys;
    unsigned int key_count;
    char** rel_pointers;
    unsigned int rel_pointer_count;
    char** abs_pointers;
    unsigned int abs_pointer_count;
    char** motors;
    unsigned int motor_count;
  } game_controller_layout;
  /*! @endcond */

  struct game_input_port;

  typedef struct game_input_device
  {
    const char* controller_id;
    const char* port_address;
    struct game_input_port* available_ports;
    unsigned int port_count;
  } game_input_device;
  //----------------------------------------------------------------------------

  typedef struct game_input_port
  {
    enum GAME_PORT_TYPE type;
    const char* port_id;

    /// @brief Flag to prevent a port from being disconnected
    ///
    /// Set to true to prevent a disconnection option from appearing in the
    /// GUI.
    ///
    bool force_connected;

    /// @brief
    struct game_input_device* accepted_devices;

    /// @brief
    unsigned int device_count;
  } game_input_port;
  //----------------------------------------------------------------------------

  typedef struct game_input_topology
  {
    struct game_input_port* ports;
    unsigned int port_count;
    int player_limit;
  } game_input_topology;
  //----------------------------------------------------------------------------

  typedef struct game_digital_button_event
  {
    bool pressed;
  } game_digital_button_event;
  //----------------------------------------------------------------------------

  typedef struct game_analog_button_event
  {
    float magnitude;
  } game_analog_button_event;
  //----------------------------------------------------------------------------

  typedef struct game_axis_event
  {
    /// @brief
    float position;
  } game_axis_event;
  //----------------------------------------------------------------------------

  typedef struct game_analog_stick_event
  {
    float x;
    float y;
  } game_analog_stick_event;
  //----------------------------------------------------------------------------

  typedef struct game_accelerometer_event
  {
    float x;
    float y;
    float z;
  } game_accelerometer_event;
  //----------------------------------------------------------------------------

  typedef struct game_key_event
  {
    bool pressed;
    uint32_t unicode;
    /// @brief
    enum GAME_KEY_MOD modifiers;
  } game_key_event;
  //----------------------------------------------------------------------------

  //============================================================================
  /// @brief
  typedef struct game_rel_pointer_event
  {
    int x;
    int y;
  } game_rel_pointer_event;
  //----------------------------------------------------------------------------

  typedef struct game_abs_pointer_event
  {
    bool pressed;
    float x;
    float y;
  } game_abs_pointer_event;
  //----------------------------------------------------------------------------

  typedef struct game_motor_event
  {
    float magnitude;
  } game_motor_event;
  //----------------------------------------------------------------------------

  typedef struct game_input_event
  {
    enum GAME_INPUT_EVENT_SOURCE type;
    const char* controller_id;
    enum GAME_PORT_TYPE port_type;
    const char* port_address;
    const char* feature_name;
    union
    {
      struct game_digital_button_event digital_button;
      struct game_analog_button_event analog_button;
      struct game_axis_event axis;
      struct game_analog_stick_event analog_stick;
      struct game_accelerometer_event accelerometer;
      struct game_key_event key;
      struct game_rel_pointer_event rel_pointer;
      struct game_abs_pointer_event abs_pointer;
      struct game_motor_event motor;
    };
  } game_input_event;
  //----------------------------------------------------------------------------

  ///@}

  //--==----==----==----==----==----==----==----==----==----==----==----==----==--
  /// @defgroup cpp_kodi_addon_game_Defs_EnvironmentTypes 8. Environment types
  /// @ingroup cpp_kodi_addon_game_Defs
  /// @brief **Environment types**
  ///
  ///@{

  struct game_system_timing
  {
    double fps;
    double sample_rate;
  };

  struct GAME_PROPS
  {
    const char* game_client_dll_path;
    const char** proxy_dll_paths;
    unsigned int proxy_dll_count;
    const char** resource_directories;
    unsigned int resource_directory_count;
    const char* profile_directory;
    bool supports_vfs;
    const char** extensions;
    unsigned int extension_count;
  };

  typedef KODI_ADDON_GAME_HDL(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_CREATE_V1)(
      KODI_ADDON_INSTANCE_BACKEND_HDL kodi_hdl);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_DESTROY_V1)(KODI_ADDON_GAME_HDL hdl);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_LOAD_GAME_V1)(
      const KODI_ADDON_GAME_HDL hdl, const char* url);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_LOAD_GAME_SPECIAL_V1)(
      const KODI_ADDON_GAME_HDL hdl,
      enum SPECIAL_GAME_TYPE type,
      const char** urls,
      size_t urlCount);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_LOAD_STANDALONE_V1)(
      const KODI_ADDON_GAME_HDL hdl);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_UNLOAD_GAME_V1)(
      const KODI_ADDON_GAME_HDL hdl);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_GET_GAME_TIMING_V1)(
      const KODI_ADDON_GAME_HDL hdl, struct game_system_timing* timing_info);
  typedef enum GAME_REGION(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_GET_REGION_V1)(
      const KODI_ADDON_GAME_HDL hdl);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_REQUIRES_GAME_LOOP_V1)(
      const KODI_ADDON_GAME_HDL hdl);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_RUN_FRAME_V1)(
      const KODI_ADDON_GAME_HDL hdl);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_RESET_V1)(
      const KODI_ADDON_GAME_HDL hdl);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_HW_CONTEXT_RESET_V1)(
      const KODI_ADDON_GAME_HDL hdl);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_HW_CONTEXT_DESTROY_V1)(
      const KODI_ADDON_GAME_HDL hdl);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_HAS_FEATURE_V1)(const KODI_ADDON_GAME_HDL hdl,
                                                                  const char* controller_id,
                                                                  const char* feature_name);
  typedef struct game_input_topology*(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_GET_TOPOLOGY_V1)(
      const KODI_ADDON_GAME_HDL hdl);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_FREE_TOPOLOGY_V1)(
      const KODI_ADDON_GAME_HDL hdl, struct game_input_topology* topology);
  typedef void(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_SET_CONTROLLER_LAYOUTS_V1)(
      const KODI_ADDON_GAME_HDL hdl,
      const struct game_controller_layout* controllers,
      size_t controller_count);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_ENABLE_KEYBOARD_V1)(const KODI_ADDON_GAME_HDL hdl,
                                                                      bool enable,
                                                                      const char* controller_id);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_ENABLE_MOUSE_V1)(const KODI_ADDON_GAME_HDL hdl,
                                                                   bool enable,
                                                                   const char* controller_id);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_CONNECT_CONTROLLER_V1)(
      const KODI_ADDON_GAME_HDL hdl,
      bool connect,
      const char* port_address,
      const char* controller_id);
  typedef bool(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_INPUT_EVENT_V1)(
      const KODI_ADDON_GAME_HDL hdl, const struct game_input_event* event);
  typedef size_t(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_SERIALIZE_SIZE_V1)(
      const KODI_ADDON_GAME_HDL hdl);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_SERIALIZE_V1)(
      const KODI_ADDON_GAME_HDL hdl, uint8_t* data, size_t size);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_DESERIALIZE_V1)(
      const KODI_ADDON_GAME_HDL hdl, const uint8_t* data, size_t size);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_CHEAT_RESET_V1)(
      const KODI_ADDON_GAME_HDL hdl);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_GET_MEMORY_V1)(
      const KODI_ADDON_GAME_HDL hdl, enum GAME_MEMORY type, uint8_t** data, size_t* size);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_SET_CHEAT_V1)(
      const KODI_ADDON_GAME_HDL hdl, unsigned int index, bool enabled, const char* code);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_RC_GENERATE_HASH_FROM_FILE_V1)(const KODI_ADDON_GAME_HDL hdl, char** hash, unsigned int console_id, const char* file_path);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_RC_GET_GAME_ID_URL_V1)(const KODI_ADDON_GAME_HDL hdl, char** url, const char* hash);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_RC_GET_PATCH_FILE_URL_V1)(const KODI_ADDON_GAME_HDL hdl, char** url, const char* username, const char* token, unsigned int game_id);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_RC_POST_RICH_PRESENCE_URL_V1)(const KODI_ADDON_GAME_HDL hdl,
     char** url,
     char** post_data,
     const char* username,
     const char* token,
     unsigned int game_id,
     const char* rich_presence);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_RC_ENABLE_RICH_PRESENCE_V1)(const KODI_ADDON_GAME_HDL hdl, const char* script);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_RC_GET_RICH_PRESENCE_EVALUATION_V1)(const KODI_ADDON_GAME_HDL hdl, char** evaluation, unsigned int console_id);
  typedef enum GAME_ERROR(ATTR_APIENTRYP PFN_KODI_ADDON_GAME_RC_RESET_RUNTIME_V1)(const KODI_ADDON_GAME_HDL hdl);

  typedef struct KODI_ADDON_GAME_FUNC
  {
    PFN_KODI_ADDON_GAME_CREATE_V1 create;
    PFN_KODI_ADDON_GAME_DESTROY_V1 destroy;
    PFN_KODI_ADDON_GAME_LOAD_GAME_V1 load_game;
    PFN_KODI_ADDON_GAME_LOAD_GAME_SPECIAL_V1 load_game_special;
    PFN_KODI_ADDON_GAME_LOAD_STANDALONE_V1 load_standalone;
    PFN_KODI_ADDON_GAME_UNLOAD_GAME_V1 unload_game;
    PFN_KODI_ADDON_GAME_GET_GAME_TIMING_V1 get_game_timing;
    PFN_KODI_ADDON_GAME_GET_REGION_V1 get_region;
    PFN_KODI_ADDON_GAME_REQUIRES_GAME_LOOP_V1 requires_game_loop;
    PFN_KODI_ADDON_GAME_RUN_FRAME_V1 run_frame;
    PFN_KODI_ADDON_GAME_RESET_V1 reset;
    PFN_KODI_ADDON_GAME_HW_CONTEXT_RESET_V1 hw_context_reset;
    PFN_KODI_ADDON_GAME_HW_CONTEXT_DESTROY_V1 hw_context_destroy;
    PFN_KODI_ADDON_GAME_HAS_FEATURE_V1 has_feature;
    PFN_KODI_ADDON_GAME_GET_TOPOLOGY_V1 get_topology;
    PFN_KODI_ADDON_GAME_FREE_TOPOLOGY_V1 free_topology;
    PFN_KODI_ADDON_GAME_SET_CONTROLLER_LAYOUTS_V1 set_controller_layouts;
    PFN_KODI_ADDON_GAME_ENABLE_KEYBOARD_V1 enable_keyboard;
    PFN_KODI_ADDON_GAME_ENABLE_MOUSE_V1 enable_mouse;
    PFN_KODI_ADDON_GAME_CONNECT_CONTROLLER_V1 connect_controller;
    PFN_KODI_ADDON_GAME_INPUT_EVENT_V1 input_event;
    PFN_KODI_ADDON_GAME_SERIALIZE_SIZE_V1 serialize_size;
    PFN_KODI_ADDON_GAME_SERIALIZE_V1 serialize;
    PFN_KODI_ADDON_GAME_DESERIALIZE_V1 deserialize;
    PFN_KODI_ADDON_GAME_CHEAT_RESET_V1 cheat_reset;
    PFN_KODI_ADDON_GAME_GET_MEMORY_V1 get_memory;
    PFN_KODI_ADDON_GAME_SET_CHEAT_V1 set_cheat;
    PFN_KODI_ADDON_GAME_RC_GENERATE_HASH_FROM_FILE_V1 rc_generate_hash_from_file;
    PFN_KODI_ADDON_GAME_RC_GET_GAME_ID_URL_V1 rc_get_game_id_url;
    PFN_KODI_ADDON_GAME_RC_GET_PATCH_FILE_URL_V1 rc_get_patch_file_url;
    PFN_KODI_ADDON_GAME_RC_POST_RICH_PRESENCE_URL_V1 rc_post_rich_presence_url;
    PFN_KODI_ADDON_GAME_RC_ENABLE_RICH_PRESENCE_V1 rc_enable_rich_presence;
    PFN_KODI_ADDON_GAME_RC_GET_RICH_PRESENCE_EVALUATION_V1 rc_get_rich_presence_evaluation;
    PFN_KODI_ADDON_GAME_RC_RESET_RUNTIME_V1 rc_reset_runtime;
  } KODI_ADDON_GAME_FUNC;

  typedef void* KODI_GAME_STREAM_HANDLE;

  ATTR_DLL_EXPORT void kodi_addon_game_close(KODI_ADDON_INSTANCE_BACKEND_HDL hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT KODI_GAME_STREAM_HANDLE kodi_addon_game_open_stream(
      KODI_ADDON_INSTANCE_BACKEND_HDL hdl, const struct game_stream_properties* properties)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_addon_game_get_stream_buffer(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                                         KODI_GAME_STREAM_HANDLE game_hdl,
                                                         unsigned int width,
                                                         unsigned int height,
                                                         struct game_stream_buffer* buffer)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_game_add_stream_data(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                                       KODI_GAME_STREAM_HANDLE game_hdl,
                                                       const struct game_stream_packet* packet)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_game_release_stream_buffer(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                                             KODI_GAME_STREAM_HANDLE game_hdl,
                                                             struct game_stream_buffer* buffer)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT void kodi_addon_game_close_stream(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                                    KODI_GAME_STREAM_HANDLE game_hdl)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT game_proc_address_t kodi_addon_game_hw_get_proc_address(
      KODI_ADDON_INSTANCE_BACKEND_HDL hdl, const char* symbol) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_addon_game_input_event(KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                                   const struct game_input_event* event)
      __INTRODUCED_IN_KODI(1);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* !C_API_ADDONINSTANCE_GAME_H */
