/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/* File autogenerated, see xbmc/addons/kodi-dev-kit/tools/code-generator/code_generator.py */

#include "network.h"

// Shared API
#include "addons/kodi-dev-kit/src/shared/api/network.h"

// Kodi
#include "addons/interface/RunningProcess.h"
#include "utils/log.h"

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1>---*/
#include "ServiceBroker.h"
#include "URL.h"
#include "network/DNSNameCache.h"
#include "network/Network.h"
#include "utils/SystemInfo.h"
#include "utils/URIUtils.h"
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

void CHdl_kodi_network_h::InitDirect(directFuncToKodi_network_h* ifcToKodi,
                                     directFuncToAddon_network_h* ifcToAddon)
{
  ifcToKodi->thisClassHdl = this;
  m_ifcToAddon = ifcToAddon;
  ifcToKodi->kodi_network_wake_on_lan_v1 = kodi_network_wake_on_lan_v1;
  ifcToKodi->kodi_network_get_ip_address_v1 = kodi_network_get_ip_address_v1;
  ifcToKodi->kodi_network_dns_lookup_v1 = kodi_network_dns_lookup_v1;
  ifcToKodi->kodi_network_url_encode_v1 = kodi_network_url_encode_v1;
  ifcToKodi->kodi_network_get_hostname_v1 = kodi_network_get_hostname_v1;
  ifcToKodi->kodi_network_is_local_host_v1 = kodi_network_is_local_host_v1;
  ifcToKodi->kodi_network_is_host_on_lan_v1 = kodi_network_is_host_on_lan_v1;
  ifcToKodi->kodi_network_get_user_agent_v1 = kodi_network_get_user_agent_v1;
}

bool CHdl_kodi_network_h::HandleMessage(int funcGroup,
                                        int func,
                                        const msgpack::unpacked& in,
                                        msgpack::sbuffer& out)
{
  if (funcGroup != funcGroup_network_h)
    return false;

  switch (func)
  {
    case funcParent_kodi_network_wake_on_lan_v1:
    {
      // Original API call: ATTR_DLL_EXPORT bool kodi_network_wake_on_lan(const char* mac) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<std::string> msgParent__IN_kodi_network_wake_on_lan_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<bool> msgParent_OUT_kodi_network_wake_on_lan_v1; /* Autogenerated */
      msgParent__IN_kodi_network_wake_on_lan_v1 t = in.get().as<decltype(t)>();
      const std::string& mac = std::get<0>(t);
      bool auto_gen_ret = kodi_network_wake_on_lan_v1(this, mac.c_str());
      msgpack::pack(out, msgParent_OUT_kodi_network_wake_on_lan_v1(auto_gen_ret));
      return true;
    }
    case funcParent_kodi_network_get_ip_address_v1:
    {
      // Original API call: ATTR_DLL_EXPORT char* kodi_network_get_ip_address() __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<DummyValue> msgParent__IN_kodi_network_get_ip_address_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<std::string> msgParent_OUT_kodi_network_get_ip_address_v1; /* Autogenerated */
      char* auto_gen_ret = kodi_network_get_ip_address_v1(this);
      msgpack::pack(out,
                    msgParent_OUT_kodi_network_get_ip_address_v1(auto_gen_ret ? auto_gen_ret : ""));
      if (auto_gen_ret)
        free(auto_gen_ret);
      return true;
    }
    case funcParent_kodi_network_dns_lookup_v1:
    {
      // Original API call: ATTR_DLL_EXPORT char* kodi_network_dns_lookup(const char* url, bool* ret) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<std::string, bool> msgParent__IN_kodi_network_dns_lookup_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<std::string, bool> msgParent_OUT_kodi_network_dns_lookup_v1; /* Autogenerated */
      msgParent__IN_kodi_network_dns_lookup_v1 t = in.get().as<decltype(t)>();
      const std::string& url = std::get<0>(t);
      bool ret = std::get<1>(t);
      char* auto_gen_ret = kodi_network_dns_lookup_v1(this, url.c_str(), &ret);
      msgpack::pack(
          out, msgParent_OUT_kodi_network_dns_lookup_v1(auto_gen_ret ? auto_gen_ret : "", ret));
      if (auto_gen_ret)
        free(auto_gen_ret);
      return true;
    }
    case funcParent_kodi_network_url_encode_v1:
    {
      // Original API call: ATTR_DLL_EXPORT char* kodi_network_url_encode(const char* url) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<std::string> msgParent__IN_kodi_network_url_encode_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<std::string> msgParent_OUT_kodi_network_url_encode_v1; /* Autogenerated */
      msgParent__IN_kodi_network_url_encode_v1 t = in.get().as<decltype(t)>();
      const std::string& url = std::get<0>(t);
      char* auto_gen_ret = kodi_network_url_encode_v1(this, url.c_str());
      msgpack::pack(out,
                    msgParent_OUT_kodi_network_url_encode_v1(auto_gen_ret ? auto_gen_ret : ""));
      if (auto_gen_ret)
        free(auto_gen_ret);
      return true;
    }
    case funcParent_kodi_network_get_hostname_v1:
    {
      // Original API call: ATTR_DLL_EXPORT char* kodi_network_get_hostname() __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<DummyValue> msgParent__IN_kodi_network_get_hostname_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<std::string> msgParent_OUT_kodi_network_get_hostname_v1; /* Autogenerated */
      char* auto_gen_ret = kodi_network_get_hostname_v1(this);
      msgpack::pack(out,
                    msgParent_OUT_kodi_network_get_hostname_v1(auto_gen_ret ? auto_gen_ret : ""));
      if (auto_gen_ret)
        free(auto_gen_ret);
      return true;
    }
    case funcParent_kodi_network_is_local_host_v1:
    {
      // Original API call: ATTR_DLL_EXPORT bool kodi_network_is_local_host(const char* hostname) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<std::string> msgParent__IN_kodi_network_is_local_host_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<bool> msgParent_OUT_kodi_network_is_local_host_v1; /* Autogenerated */
      msgParent__IN_kodi_network_is_local_host_v1 t = in.get().as<decltype(t)>();
      const std::string& hostname = std::get<0>(t);
      bool auto_gen_ret = kodi_network_is_local_host_v1(this, hostname.c_str());
      msgpack::pack(out, msgParent_OUT_kodi_network_is_local_host_v1(auto_gen_ret));
      return true;
    }
    case funcParent_kodi_network_is_host_on_lan_v1:
    {
      // Original API call: ATTR_DLL_EXPORT bool kodi_network_is_host_on_lan(const char* hostname, bool offLineCheck) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<std::string, bool> msgParent__IN_kodi_network_is_host_on_lan_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<bool> msgParent_OUT_kodi_network_is_host_on_lan_v1; /* Autogenerated */
      msgParent__IN_kodi_network_is_host_on_lan_v1 t = in.get().as<decltype(t)>();
      const std::string& hostname = std::get<0>(t);
      bool offLineCheck = std::get<1>(t);
      bool auto_gen_ret = kodi_network_is_host_on_lan_v1(this, hostname.c_str(), offLineCheck);
      msgpack::pack(out, msgParent_OUT_kodi_network_is_host_on_lan_v1(auto_gen_ret));
      return true;
    }
    case funcParent_kodi_network_get_user_agent_v1:
    {
      // Original API call: ATTR_DLL_EXPORT char* kodi_network_get_user_agent() __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<DummyValue> msgParent__IN_kodi_network_get_user_agent_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<std::string> msgParent_OUT_kodi_network_get_user_agent_v1; /* Autogenerated */
      char* auto_gen_ret = kodi_network_get_user_agent_v1(this);
      msgpack::pack(out,
                    msgParent_OUT_kodi_network_get_user_agent_v1(auto_gen_ret ? auto_gen_ret : ""));
      if (auto_gen_ret)
        free(auto_gen_ret);
      return true;
    }
    default:
      CLog::Log(LOGERROR,
                "CHdl_kodi_network_h::{}: addon called with unknown function id '{}' on group "
                "'network_h'",
                __func__, func);
  }

  return false;
}

// Function calls from Kodi to addon
/* NOTE: unused (no functions here) */

// Callbacks from addon to Kodi

bool CHdl_kodi_network_h::kodi_network_wake_on_lan_v1(void* thisClassHdl, const char* mac)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_network_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return false;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_NETWORK_WAKE_ON_LAN>---*/

  return CServiceBroker::GetNetwork().WakeOnLan(mac);

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_NETWORK_WAKE_ON_LAN_END>---*/
}

char* CHdl_kodi_network_h::kodi_network_get_ip_address_v1(void* thisClassHdl)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_network_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return nullptr;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_NETWORK_GET_IP_ADDRESS>---*/

  std::string titleIP;
  CNetworkInterface* iface = CServiceBroker::GetNetwork().GetFirstConnectedInterface();
  if (iface)
    titleIP = iface->GetCurrentIPAddress();
  else
    titleIP = "127.0.0.1";

  char* buffer = nullptr;
  if (!titleIP.empty())
    buffer = strdup(titleIP.c_str());
  return buffer;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_NETWORK_GET_IP_ADDRESS_END>---*/
}

char* CHdl_kodi_network_h::kodi_network_dns_lookup_v1(void* thisClassHdl,
                                                      const char* url,
                                                      bool* ret)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_network_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return nullptr;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_NETWORK_DNS_LOOKUP>---*/

  std::string string;
  *ret = CDNSNameCache::Lookup(url, string);
  char* buffer = nullptr;
  if (!string.empty())
    buffer = strdup(string.c_str());
  return buffer;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_NETWORK_DNS_LOOKUP_END>---*/
}

char* CHdl_kodi_network_h::kodi_network_url_encode_v1(void* thisClassHdl, const char* url)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_network_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return nullptr;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_NETWORK_URL_ENCODE>---*/

  std::string string = CURL::Encode(url);
  char* buffer = nullptr;
  if (!string.empty())
    buffer = strdup(string.c_str());
  return buffer;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_NETWORK_URL_ENCODE_END>---*/
}

char* CHdl_kodi_network_h::kodi_network_get_hostname_v1(void* thisClassHdl)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_network_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return nullptr;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_NETWORK_GET_HOSTNAME>---*/

  std::string hostname;
  if (!CServiceBroker::GetNetwork().GetHostName(hostname))
    return nullptr;

  char* buffer = nullptr;
  if (!hostname.empty())
    buffer = strdup(hostname.c_str());
  return buffer;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_NETWORK_GET_HOSTNAME_END>---*/
}

bool CHdl_kodi_network_h::kodi_network_is_local_host_v1(void* thisClassHdl, const char* hostname)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_network_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return false;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_NETWORK_IS_LOCAL_HOST>---*/

  return CServiceBroker::GetNetwork().IsLocalHost(hostname);

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_NETWORK_IS_LOCAL_HOST_END>---*/
}

bool CHdl_kodi_network_h::kodi_network_is_host_on_lan_v1(void* thisClassHdl,
                                                         const char* hostname,
                                                         bool offLineCheck)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_network_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return false;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_NETWORK_IS_HOST_ON_LAN>---*/

  return URIUtils::IsHostOnLAN(hostname, offLineCheck);

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_NETWORK_IS_HOST_ON_LAN_END>---*/
}

char* CHdl_kodi_network_h::kodi_network_get_user_agent_v1(void* thisClassHdl)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_network_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return nullptr;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_NETWORK_GET_USER_AGENT>---*/

  std::string string = CSysInfo::GetUserAgent();
  char* buffer = nullptr;
  if (!string.empty())
    buffer = strdup(string.c_str());
  return buffer;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_NETWORK_GET_USER_AGENT_END>---*/
}

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3_END>---*/

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
