/*
 *  Copyright (C) 2005-2013 Team XBMC
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <string>
#include <vector>

#include "settings/lib/ISettingCallback.h"

#include "PlatformDefs.h"

enum EncMode { ENC_NONE = 0, ENC_WEP = 1, ENC_WPA = 2, ENC_WPA2 = 3 };
enum NetworkAssignment { NETWORK_DASH = 0, NETWORK_DHCP = 1, NETWORK_STATIC = 2, NETWORK_DISABLED = 3 };

class NetworkAccessPoint
{
public:
    NetworkAccessPoint(const std::string &essId, const std::string &macAddress, int signalStrength, EncMode encryption, int channel = 0):
      m_essId(essId),
      m_macAddress(macAddress)
    {
      m_dBm            = signalStrength;
      m_encryptionMode = encryption;
      m_channel        = channel;
   }

   const std::string &getEssId() const { return m_essId; }
   const std::string &getMacAddress() const { return m_macAddress; }
   int getSignalStrength() const { return m_dBm; }
   EncMode getEncryptionMode() const { return m_encryptionMode; }
   int getChannel() const { return m_channel; }

   /*!
    \brief  Returns the quality, normalized as a percentage, of the network access point
    \return The quality as an integer between 0 and 100
    */
   int getQuality() const;

   /*!
    \brief  Translates a 802.11a+g frequency into the corresponding channel
    \param  frequency  The frequency of the channel in units of Hz
    \return The channel as an integer between 1 and 14 (802.11b+g) or
            between 36 and 165 (802.11a), or 0 if unknown.
    */
   static int FreqToChannel(float frequency);

private:
   std::string  m_essId;
   std::string  m_macAddress;
   int         m_dBm;
   EncMode     m_encryptionMode;
   int         m_channel;
};

class CNetworkInterface
{
public:
   virtual ~CNetworkInterface() = default;

   virtual std::string& GetName(void) = 0;

   virtual bool IsEnabled(void) = 0;
   virtual bool IsConnected(void) = 0;
   virtual bool IsWireless(void) = 0;

   virtual std::string GetMacAddress(void) = 0;
   virtual void GetMacAddressRaw(char rawMac[6]) = 0;

   virtual bool GetHostMacAddress(unsigned long host, std::string& mac) = 0;

   virtual std::string GetCurrentIPAddress() = 0;
   virtual std::string GetCurrentNetmask() = 0;
   virtual std::string GetCurrentDefaultGateway(void) = 0;
   virtual std::string GetCurrentWirelessEssId(void) = 0;

   // Returns the list of access points in the area
   virtual std::vector<NetworkAccessPoint> GetAccessPoints(void) = 0;

   virtual void GetSettings(NetworkAssignment& assignment, std::string& ipAddress, std::string& networkMask, std::string& defaultGateway, std::string& essId, std::string& key, EncMode& encryptionMode) = 0;
   virtual void SetSettings(NetworkAssignment& assignment, std::string& ipAddress, std::string& networkMask, std::string& defaultGateway, std::string& essId, std::string& key, EncMode& encryptionMode) = 0;
};

class CSettings;
class CNetworkServices;

class CNetworkBase
{
public:
  enum EMESSAGE
  {
    SERVICES_UP,
    SERVICES_DOWN
  };

   CNetworkBase(CSettings &settings);
   virtual ~CNetworkBase();

   // Get network services
   CNetworkServices& GetServices() { return *m_services; }

   // Return our hostname
   virtual bool GetHostName(std::string& hostname);

   // Return the list of interfaces
   virtual std::vector<CNetworkInterface*>& GetInterfaceList(void) = 0;
   CNetworkInterface* GetInterfaceByName(const std::string& name);

   // Return the first interface which is active
   virtual CNetworkInterface* GetFirstConnectedInterface(void);

   // Return true if there is a interface for the same network as address
   bool HasInterfaceForIP(unsigned long address);

   // Return true if there's at least one defined network interface
   bool IsAvailable(void);

   // Return true if there's at least one interface which is connected
   bool IsConnected(void);

   // Return true if the magic packet was send
   bool WakeOnLan(const char *mac);

   // Return true if host replies to ping
   bool PingHost(unsigned long host, unsigned short port, unsigned int timeout_ms = 2000, bool readability_check = false);
   virtual bool PingHost(unsigned long host, unsigned int timeout_ms = 2000) = 0;

   // Get/set the nameserver(s)
   virtual std::vector<std::string> GetNameServers(void) = 0;
   virtual void SetNameServers(const std::vector<std::string>& nameServers) = 0;

   // callback from application controlled thread to handle any setup
   void NetworkMessage(EMESSAGE message, int param);

   void StartServices();
   void StopServices(bool bWait);

   static int ParseHex(char *str, unsigned char *addr);

   // Return true if given name or ip address corresponds to localhost
   bool IsLocalHost(const std::string& hostname);

   // Waits for the first network interface to become available
   void WaitForNet();
  std::unique_ptr<CNetworkServices> m_services;
};

#if defined(TARGET_ANDROID)
#include "platform/android/network/NetworkAndroid.h"
#elif defined(HAS_LINUX_NETWORK)
#include "platform/linux/network/NetworkLinux.h"
#elif defined(HAS_WIN32_NETWORK)
#include "platform/win32/network/NetworkWin32.h"
#elif defined(HAS_WIN10_NETWORK)
#include "platform/win10/network/NetworkWin10.h"
#else
using CNetwork = CNetworkBase;
#endif

//creates, binds and listens tcp sockets on the desired port. Set bindLocal to
//true to bind to localhost only.
std::vector<SOCKET> CreateTCPServerSocket(const int port, const bool bindLocal, const int backlog, const char *callerName);

