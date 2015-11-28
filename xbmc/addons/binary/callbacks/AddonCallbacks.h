#pragma once
/*
 *      Copyright (C) 2012-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "addons/Addon.h"
#include "addons/binary/callbacks/IAddonCallback.h"

#include <stdint.h>
#include <unordered_map>

typedef void* (*KODIAddOnLib_RegisterMe)(void *addonData);
typedef void* (*KODIAddOnLib_RegisterLevel)(void *addonData, int level);
typedef void (*KODIAddOnLib_UnRegisterMe)(void *addonData, void *cbTable);

typedef void* (*KODIAudioEngineLib_RegisterMe)(void *addonData);
typedef void* (*KODIAudioEngineLib_RegisterLevel)(void *addonData, int level);
typedef void (*KODIAudioEngineLib_UnRegisterMe)(void *addonData, void *cbTable);

typedef void* (*KODIGUILib_RegisterMe)(void *addonData);
typedef void* (*KODIGUILib_RegisterLevel)(void *addonData, int level);
typedef void (*KODIGUILib_UnRegisterMe)(void *addonData, void *cbTable);

typedef void* (*KODIPVRLib_RegisterMe)(void *addonData);
typedef void* (*KODIPVRLib_RegisterLevel)(void *addonData, int level);
typedef void (*KODIPVRLib_UnRegisterMe)(void *addonData, void *cbTable);

/*!
 * Deprecated old libraries
 */
typedef void* (*KODIADSPLib_RegisterMe)(void *addonData);
typedef void (*KODIADSPLib_UnRegisterMe)(void *addonData, void *cbTable);

typedef void* (*KODICodecLib_RegisterMe)(void *addonData);
typedef void (*KODICodecLib_UnRegisterMe)(void *addonData, void *cbTable);

typedef struct AddonCB
{
  const char* libBasePath;  ///< Never, never change this!!!
  void*       addonData;

  KODIAddOnLib_RegisterMe           AddOnLib_RegisterMe;
  KODIAddOnLib_UnRegisterMe         AddOnLib_UnRegisterMe;

  KODIAudioEngineLib_RegisterMe     AudioEngineLib_RegisterMe;
  KODIAudioEngineLib_UnRegisterMe   AudioEngineLib_UnRegisterMe;

  KODICodecLib_RegisterMe           CodecLib_RegisterMe;
  KODICodecLib_UnRegisterMe         CodecLib_UnRegisterMe;

  KODIGUILib_RegisterMe             GUILib_RegisterMe;
  KODIGUILib_UnRegisterMe           GUILib_UnRegisterMe;

  KODIPVRLib_RegisterMe             PVRLib_RegisterMe;
  KODIPVRLib_UnRegisterMe           PVRLib_UnRegisterMe;

  KODIADSPLib_RegisterMe            ADSPLib_RegisterMe;
  KODIADSPLib_UnRegisterMe          ADSPLib_UnRegisterMe;

  KODIAddOnLib_RegisterLevel        AddOnLib_RegisterLevel;
  KODIAudioEngineLib_RegisterLevel  AudioEngineLib_RegisterLevel;
  KODIGUILib_RegisterLevel          GUILib_RegisterLevel;
  KODIPVRLib_RegisterLevel          PVRLib_RegisterLevel;
} AddonCB;


namespace ADDON
{

class CAddon;

class CAddonCallbacks
{
public:
  CAddonCallbacks(CAddon* addon);
  ~CAddonCallbacks();

  AddonCB* GetCallbacks() { return m_callbacks; }
  /*\__________________________________________________________________________________________
  \*/
  static void*        AddOnLib_RegisterMe                (void* addonData);
  static void*        AddOnLib_RegisterLevel             (void* addonData, int level);
  static void         AddOnLib_UnRegisterMe              (void* addonData, void* cbTable);
  static int          AddOnLib_APILevel();
  static int          AddOnLib_MinAPILevel();
  static std::string  AddOnLib_Version();
  static std::string  AddOnLib_MinVersion();
  void*               AddOnLib_GetHelper()              { return m_helperAddOn;       }
  /*\__________________________________________________________________________________________
  \*/
  static void*        AudioEngineLib_RegisterMe          (void* addonData);
  static void*        AudioEngineLib_RegisterLevel       (void* addonData, int level);
  static void         AudioEngineLib_UnRegisterMe        (void* addonData, void* cbTable);
  static int          AudioEngineLib_APILevel();
  static int          AudioEngineLib_MinAPILevel();
  static std::string  AudioEngineLib_Version();
  static std::string  AudioEngineLib_MinVersion();
  void*               AudioEngineLib_GetHelper()        { return m_helperAudioEngine; }
  /*\__________________________________________________________________________________________
  \*/
  static void*        GUILib_RegisterMe                  (void* addonData);
  static void*        GUILib_RegisterLevel               (void* addonData, int level);
  static void         GUILib_UnRegisterMe                (void* addonData, void* cbTable);
  static int          GUILib_APILevel();
  static int          GUILib_MinAPILevel();
  static std::string  GUILib_Version();
  static std::string  GUILib_MinVersion();
  void*               GUILib_GetHelper()                { return m_helperGUI; }
  /*\__________________________________________________________________________________________
  \*/
  static void*        PVRLib_RegisterMe                  (void* addonData);
  static void*        PVRLib_RegisterLevel               (void* addonData, int level);
  static void         PVRLib_UnRegisterMe                (void* addonData, void* cbTable);
  static int          PVRLib_APILevel();
  static int          PVRLib_MinAPILevel();
  static std::string  PVRLib_Version();
  static std::string  PVRLib_MinVersion();
  void*               PVRLib_GetHelper()                { return m_helperPVR; }
  /*\__________________________________________________________________________________________
  \*/
  /*!
   * Deprecated old libraries
   */
  static void*        CodecLib_RegisterMe                (void* addonData);
  static void         CodecLib_UnRegisterMe              (void* addonData, void* cbTable);
  void*               GetHelperCODEC()                  { return m_helperCODEC; }
  /*\__________________________________________________________________________________________
  \*/
  static void*        ADSPLib_RegisterMe                 (void* addonData);
  static void         ADSPLib_UnRegisterMe               (void* addonData, void* cbTable);
  void*               GetHelperADSP()                   { return m_helperADSP; }

private:
  AddonCB*  m_callbacks;
  CAddon*   m_addon;

  friend class CAddonCallbacksAddonBase;
  friend class CAddonCallbacksAudioEngineBase;
  friend class CAddonCallbacksGUIBase;
  friend class CAddonCallbacksPVRBase;

  //static std::unordered_map<int, IAddonCallback> m_helperAddOn;
  void*     m_helperAddOn;
  void*     m_helperAudioEngine;
  void*     m_helperGUI;
  void*     m_helperPVR;

  void*     m_helperADSP;
  void*     m_helperCODEC;
};

}; /* namespace ADDON */
