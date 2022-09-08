/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#ifndef C_API_NETWORK_H
#define C_API_NETWORK_H

#include "addon_base.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if __KODI_API__ >= 1

  ATTR_DLL_EXPORT bool kodi_network_wake_on_lan(const char* mac) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_network_get_ip_address() __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_network_dns_lookup(const char* url, bool* ret) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_network_url_encode(const char* url) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_network_get_hostname() __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_network_is_local_host(const char* hostname) __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT bool kodi_network_is_host_on_lan(const char* hostname, bool offLineCheck)
      __INTRODUCED_IN_KODI(1);
  ATTR_DLL_EXPORT char* kodi_network_get_user_agent() __INTRODUCED_IN_KODI(1);

#endif /* __KODI_API__ >= 1 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !C_API_NETWORK_H */
