/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/* File autogenerated, see xbmc/addons/kodi-dev-kit/tools/code-generator/code_generator.py */

#pragma once

#include "addons/interface/IMsgHdl.h"

// API include
#include "addons/kodi-dev-kit/include/kodi/c-api/network.h"

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1_END>---*/

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

struct directFuncToAddon_network_h;
struct directFuncToKodi_network_h;

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_2>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_2_END>---*/

class CHdl_kodi_network_h : public IMsgHdl
{
public:
  CHdl_kodi_network_h(CInterface& interface) : IMsgHdl(interface) {}

  void InitDirect(directFuncToKodi_network_h* ifcToKodi, directFuncToAddon_network_h* ifcToAddon);

  // To handle callbacks from addon by shared sandbox API (static functions below in private becomes used)
  bool HandleMessage(int funcGroup,
                     int func,
                     const msgpack::unpacked& in,
                     msgpack::sbuffer& out) override;

  /* Code below becomes on auto generation reused, hand edited parts should included here */
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3>---*/
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3_END>---*/

  // Function calls from Kodi to addon

private:
  /* Code below becomes on auto generation reused, hand edited parts should included here */
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_4>---*/
  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_4_END>---*/

  // Callbacks from addon to Kodi
  static bool kodi_network_wake_on_lan_v1(void* thisClassHdl, const char* mac); // Added with API 1

  static char* kodi_network_get_ip_address_v1(void* thisClassHdl); // Added with API 1

  static char* kodi_network_dns_lookup_v1(void* thisClassHdl,
                                          const char* url,
                                          bool* ret); // Added with API 1

  static char* kodi_network_url_encode_v1(void* thisClassHdl, const char* url); // Added with API 1

  static char* kodi_network_get_hostname_v1(void* thisClassHdl); // Added with API 1

  static bool kodi_network_is_local_host_v1(void* thisClassHdl,
                                            const char* hostname); // Added with API 1

  static bool kodi_network_is_host_on_lan_v1(void* thisClassHdl,
                                             const char* hostname,
                                             bool offLineCheck); // Added with API 1

  static char* kodi_network_get_user_agent_v1(void* thisClassHdl); // Added with API 1

  directFuncToAddon_network_h* m_ifcToAddon{nullptr};
};

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
