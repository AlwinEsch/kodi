/*
 *  Copyright (C) 2014-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "addons/interface/InstanceHandler.h"
#include "addons/kodi-dev-kit/include/kodi/addon-instance/Peripheral.h"
#include "input/joysticks/JoystickTypes.h"
#include "peripherals/PeripheralTypes.h"
#include "threads/CriticalSection.h"
#include "threads/SharedSection.h"

#include <map>
#include <memory>
#include <vector>

class CFileItemList;

namespace KODI
{
namespace JOYSTICK
{
class IButtonMap;
class IDriverHandler;
} // namespace JOYSTICK
} // namespace KODI

namespace PERIPHERALS
{
class CPeripheral;
class CPeripheralJoystick;
class CPeripherals;

typedef std::vector<kodi::addon::DriverPrimitive> PrimitiveVector;
typedef std::map<KODI::JOYSTICK::FeatureName, kodi::addon::JoystickFeature> FeatureMap;

class CPeripheralAddon : public KODI::ADDONS::INTERFACE::IInstanceHandler
{
public:
  explicit CPeripheralAddon(const ADDON::AddonInfoPtr& addonInfo, CPeripherals& manager);
  ~CPeripheralAddon(void) override;

  /*!
   * @brief Initialise the instance of this add-on
   */
  bool CreateAddon(void);

  /*!
   * \brief Deinitialize the instance of this add-on
   */
  void DestroyAddon();

  bool Register(unsigned int peripheralIndex, const PeripheralPtr& peripheral);
  void UnregisterRemovedDevices(const PeripheralScanResults& results,
                                PeripheralVector& removedPeripherals);
  void GetFeatures(std::vector<PeripheralFeature>& features) const;
  bool HasFeature(const PeripheralFeature feature) const;
  PeripheralPtr GetPeripheral(unsigned int index) const;
  PeripheralPtr GetByPath(const std::string& strPath) const;
  bool SupportsFeature(PeripheralFeature feature) const;
  unsigned int GetPeripheralsWithFeature(PeripheralVector& results,
                                         const PeripheralFeature feature) const;
  unsigned int GetNumberOfPeripherals(void) const;
  unsigned int GetNumberOfPeripheralsWithId(const int iVendorId, const int iProductId) const;
  void GetDirectory(const std::string& strPath, CFileItemList& items) const;

  /** @name Peripheral add-on methods */
  //@{
  bool PerformDeviceScan(PeripheralScanResults& results);
  bool ProcessEvents(void);
  bool SendRumbleEvent(unsigned int index, unsigned int driverIndex, float magnitude);
  //@}

  /** @name Joystick methods */
  //@{
  bool GetJoystickProperties(unsigned int index, CPeripheralJoystick& joystick);
  bool HasButtonMaps(void) const { return m_bProvidesButtonMaps; }
  bool GetFeatures(const CPeripheral* device,
                   const std::string& strControllerId,
                   FeatureMap& features);
  bool MapFeature(const CPeripheral* device,
                  const std::string& strControllerId,
                  const kodi::addon::JoystickFeature& feature);
  bool GetIgnoredPrimitives(const CPeripheral* device, PrimitiveVector& primitives);
  bool SetIgnoredPrimitives(const CPeripheral* device, const PrimitiveVector& primitives);
  void SaveButtonMap(const CPeripheral* device);
  void RevertButtonMap(const CPeripheral* device);
  void ResetButtonMap(const CPeripheral* device, const std::string& strControllerId);
  void PowerOffJoystick(unsigned int index);
  //@}

  void RegisterButtonMap(CPeripheral* device, KODI::JOYSTICK::IButtonMap* buttonMap);
  void UnregisterButtonMap(KODI::JOYSTICK::IButtonMap* buttonMap);

  static inline bool ProvidesJoysticks(const ADDON::AddonInfoPtr& addonInfo)
  {
    return addonInfo->Type(ADDON::ADDON_PERIPHERALDLL)->GetValue("@provides_joysticks").asBoolean();
  }

  static inline bool ProvidesButtonMaps(const ADDON::AddonInfoPtr& addonInfo)
  {
    return addonInfo->Type(ADDON::ADDON_PERIPHERALDLL)
        ->GetValue("@provides_buttonmaps")
        .asBoolean();
  }

  /*!
   * @brief Callback functions from addon to kodi
   */
  //@{
  void TriggerDeviceScan();
  /*---AUTO_GEN_PARSE<CB:kodi_addon_peripheral_trigger_scan>---*/
  void RefreshButtonMaps(const std::string& strDeviceName, const std::string& controllerId = "");
  /*---AUTO_GEN_PARSE<CB:kodi_addon_peripheral_refresh_button_maps>---*/
  unsigned int FeatureCount(const std::string& controllerId, JOYSTICK_FEATURE_TYPE type) const;
  /*---AUTO_GEN_PARSE<CB:kodi_addon_peripheral_feature_count>---*/
  JOYSTICK_FEATURE_TYPE FeatureType(const std::string& controllerId,
                                    const std::string& featureName) const;
  /*---AUTO_GEN_PARSE<CB:kodi_addon_peripheral_feature_type>---*/
  //@}

private:
  void UnregisterButtonMap(CPeripheral* device);

  /*!
   * @brief Helper functions
   */
  static void GetPeripheralInfo(const CPeripheral* device, kodi::addon::Peripheral& peripheralInfo);

  static void GetJoystickInfo(const CPeripheral* device, kodi::addon::Joystick& joystickInfo);
  static void SetJoystickInfo(CPeripheralJoystick& joystick,
                              const kodi::addon::Joystick& joystickInfo);

  /*!
   * @brief Retrieve add-on properties from the add-on
   */
  bool GetAddonProperties(void);

  bool LogError(const PERIPHERAL_ERROR error, const char* strMethod) const;

  static std::string GetDeviceName(PeripheralType type);
  static std::string GetProvider(PeripheralType type);

  // Construction parameters
  CPeripherals& m_manager;

  /* @brief Add-on properties */
  bool m_bProvidesJoysticks;
  bool m_bSupportsJoystickRumble;
  bool m_bSupportsJoystickPowerOff;
  bool m_bProvidesButtonMaps;

  /* @brief Map of peripherals belonging to the add-on */
  std::map<unsigned int, PeripheralPtr> m_peripherals;

  /* @brief Button map observers */
  std::vector<std::pair<CPeripheral*, KODI::JOYSTICK::IButtonMap*>> m_buttonMaps;
  CCriticalSection m_buttonMapMutex;

  /* @brief Thread synchronization */
  mutable CCriticalSection m_critSection;

  CSharedSection m_dllSection;
};
} // namespace PERIPHERALS
