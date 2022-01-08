/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "addons/IAddon.h"
#include "utils/XBMCTinyXML.h"

class TiXmlElement;
class CAddonCallbacksAddon;
class CVariant;

namespace ADDON
{
  typedef std::vector<AddonPtr> VECADDONS;
  typedef std::vector<AddonPtr>::iterator IVECADDONS;

  const char* const ORIGIN_SYSTEM = "b6a50484-93a0-4afb-a01c-8d17e059feda";

  void OnPreInstall(const AddonPtr& addon);
  void OnPostInstall(const AddonPtr& addon, bool update, bool modal);
  void OnPreUnInstall(const AddonPtr& addon);
  void OnPostUnInstall(const AddonPtr& addon);

class CAddon : public IAddon
{
public:
  explicit CAddon(const AddonInfoPtr& addonInfo, TYPE addonType);
  ~CAddon() override = default;

  /**
   * @brief To get the main type of this addon
   *
   * This is the first type defined in **addon.xml** and can be different to the
   * on @ref Type() defined type.
   *
   * @return The used main type of addon
   */
  TYPE MainType() const override { return m_addonInfo->MainType(); }

  /**
   * @brief To get the on this CAddon class processed addon type
   *
   * @return For this class used addon type
   */
  TYPE Type() const override { return m_type; }

  /**
   * @brief To check complete addon (not only this) contains a type
   *
   * @note This can be overridden by a child e.g. plugin to check for subtype
   * e.g. video or music.
   *
   * @param[in] type The to checked type identifier
   * @return true in case the wanted type is supported, false if not
   */
  bool HasType(TYPE type) const override { return m_addonInfo->HasType(type); }

  /**
   * @brief To check complete addon (not only this) has a specific type
   * defined in its first extension point including the provided subcontent
   * e.g. video or audio
   *
   * @param[in] type Type identifier to be checked
   * @return true in case the wanted type is the main type, false if not
   */
  bool HasMainType(TYPE type) const override { return m_addonInfo->HasType(type, true); }

  /**
   * @brief The get for given addon type information and extension data
   *
   * @param[in] type The wanted type data
   * @return addon type class with @ref CAddonExtensions as information
   *
   * @note This function return never a "nullptr", in case the wanted type is
   * not supported, becomes a dummy of @ref CAddonType given.
   *
   * ------------------------------------------------------------------------
   *
   * **Example:**
   * ~~~~~~~~~~~~~{.cpp}
   * // To get e.g. <extension ... name="blablabla" /> from addon.xml
   * std::string name = Type(ADDON_...)->GetValue("@name").asString();
   * ~~~~~~~~~~~~~
   *
   */
  const CAddonType* Type(TYPE type) const { return m_addonInfo->Type(type); }

  std::string ID() const override{ return m_addonInfo->ID(); }
  std::string Name() const override { return m_addonInfo->Name(); }
  bool IsInUse() const override { return false; }
  bool IsBinary() const override { return m_addonInfo->IsBinary(); }
  AddonVersion Version() const override { return m_addonInfo->Version(); }
  AddonVersion MinVersion() const override { return m_addonInfo->MinVersion(); }
  std::string Summary() const override { return m_addonInfo->Summary(); }
  std::string Description() const override { return m_addonInfo->Description(); }
  std::string Path() const override { return m_addonInfo->Path(); }
  std::string Profile() const override { return m_profilePath; }
  std::string LibPath() const override;
  std::string Author() const override { return m_addonInfo->Author(); }
  std::string ChangeLog() const override { return m_addonInfo->ChangeLog(); }
  std::string Icon() const override { return m_addonInfo->Icon(); }
  ArtMap Art() const override { return m_addonInfo->Art(); }
  std::vector<std::string> Screenshots() const override { return m_addonInfo->Screenshots(); }
  std::string Disclaimer() const override { return m_addonInfo->Disclaimer(); }
  AddonLifecycleState LifecycleState() const override { return m_addonInfo->LifecycleState(); }
  std::string LifecycleStateDescription() const override
  {
    return m_addonInfo->LifecycleStateDescription();
  }
  CDateTime InstallDate() const override { return m_addonInfo->InstallDate(); }
  CDateTime LastUpdated() const override { return m_addonInfo->LastUpdated(); }
  CDateTime LastUsed() const override { return m_addonInfo->LastUsed(); }
  std::string Origin() const override { return m_addonInfo->Origin(); }
  std::string OriginName() const override { return m_addonInfo->OriginName(); }
  uint64_t PackageSize() const override { return m_addonInfo->PackageSize(); }
  const InfoMap& ExtraInfo() const override { return m_addonInfo->ExtraInfo(); }
  const std::vector<DependencyInfo>& GetDependencies() const override { return m_addonInfo->GetDependencies(); }

  std::string FanArt() const override
  {
    auto it = m_addonInfo->Art().find("fanart");
    return it != m_addonInfo->Art().end() ? it->second : "";
  }

  /*!
   * \brief Check addon for support from independent work instances.
   *
   * \return true if the add-on supports individual add-on instances, false otherwise
   */
  bool SupportsMultipleInstances() const override;

  /*!
   * \brief Return the used instance path type of the addon type.
   *
   * \return The route used to instance handling, @ref AddonInstanceUse::NONE if not supported.
   */
  AddonInstanceUse InstanceUseType() const override;

  /*!
   * \brief Gives active, independently working instance identifiers for this add-on.
   *
   * This function is supported if addon type has defined
   * @ref AddonInstanceUse::SUPPORTED_BY_SETTINGS and the associated settings
   * are available.
   *
   * \return List of active instance identifiers.
   */
  std::vector<uint32_t> GetKnownInstanceIds() const override;

  /*!
   * \brief Check whether the add-on supports individual settings per add-on instance.
   *
   * This function is supported if addon type has defined
   * @ref AddonInstanceUse::SUPPORTED_BY_SETTINGS
   *
   * \return true if the add-on supports individual settings per add-on instance, false otherwise
   */
  bool SupportsInstanceSettings() const override;

  /*!
   * \brief Delete selected instance settings from storage.
   *
   * The related instance-settings-[0-9...].xml file will be deleted by this method.
   *
   * \param[in] instance Instance identifier to use.
   * \return true on success, false otherwise.
   */
  bool DeleteInstanceSettings(uint32_t instance) override;

  /*!
   * \brief Check whether this add-on can be configured by the user.
   *
   * \return true if the addon has settings, false otherwise
   */
  bool CanHaveAddonOrInstanceSettings() override;

  /*!
   * \brief Check whether this add-on can be configured by the user.
   *
   * \param[in] instance Instance identifier to use, use @ref KODI::ADDONS::ADDON_SETTINGS_ID
   *                     to denote global addon settings from settings.xml.
   * \return true if the addon has settings, false otherwise
   *
   * \sa LoadSettings, LoadUserSettings, SaveSettings, HasUserSettings, GetSetting, UpdateSetting
   */
  bool HasSettings(uint32_t instance) override;

  /*!
   * \brief Check whether the user has configured this addon or not.
   *
   * \param[in] instance Instance identifier to use, use @ref KODI::ADDONS::ADDON_SETTINGS_ID
   *                     to denote global addon settings from settings.xml.
   * \return true if previously saved settings are found, false otherwise
   *
   * \sa LoadSettings, LoadUserSettings, SaveSettings, HasSettings, GetSetting, UpdateSetting
   */
  bool HasUserSettings(uint32_t instance) override;

  /*!
   * \brief Save any user configured settings
   *
   * \param[in] instance Instance identifier to use, use @ref KODI::ADDONS::ADDON_SETTINGS_ID
   *                     to denote global addon settings from settings.xml.
   *
   * \sa LoadSettings, LoadUserSettings, HasSettings, HasUserSettings, GetSetting, UpdateSetting
   */
  void SaveSettings(uint32_t instance) override;

  /*!
   * \brief Update a user-configured setting with a new value.
   *
   * \param[in] instance Instance identifier to use, use @ref KODI::ADDONS::ADDON_SETTINGS_ID
   *                     to denote global addon settings from settings.xml.
   * \param[in] key the id of the setting to update
   * \param[in] value the value that the setting should take
   *
   * \sa LoadSettings, LoadUserSettings, SaveSettings, HasSettings, HasUserSettings, GetSetting
   */
  void UpdateSetting(uint32_t instance, const std::string& key, const std::string& value) override;

  /*!
   * \brief Update a user-configured setting with a new boolean value.
   *
   * \param[in] instance Instance identifier to use, use @ref KODI::ADDONS::ADDON_SETTINGS_ID
   *                     to denote global addon settings from settings.xml.
   * \param[in] key the id of the setting to update
   * \param[in] value the value that the setting should take
   *
   * \sa LoadSettings, LoadUserSettings, SaveSettings, HasSettings, HasUserSettings, GetSetting
   */
  bool UpdateSettingBool(uint32_t instance, const std::string& key, bool value) override;

  /*!
   * \brief Update a user-configured setting with a new integer value.
   *
   * \param[in] instance Instance identifier to use, use @ref KODI::ADDONS::ADDON_SETTINGS_ID
   *                     to denote global addon settings from settings.xml.
   * \param[in] key the id of the setting to update
   * \param[in] value the value that the setting should take
   *
   * \sa LoadSettings, LoadUserSettings, SaveSettings, HasSettings, HasUserSettings, GetSetting
   */
  bool UpdateSettingInt(uint32_t instance, const std::string& key, int value) override;

  /*!
   * \brief Update a user-configured setting with a new number value.
   *
   * \param[in] instance Instance identifier to use, use @ref KODI::ADDONS::ADDON_SETTINGS_ID
   *                     to denote global addon settings from settings.xml.
   * \param[in] key the id of the setting to update
   * \param[in] value the value that the setting should take
   *
   * \sa LoadSettings, LoadUserSettings, SaveSettings, HasSettings, HasUserSettings, GetSetting
   */
  bool UpdateSettingNumber(uint32_t instance, const std::string& key, double value) override;

  /*!
   * \brief Update a user-configured setting with a new string value.
   *
   * \param[in] instance Instance identifier to use, use @ref KODI::ADDONS::ADDON_SETTINGS_ID
   *                     to denote global addon settings from settings.xml.
   * \param[in] key the id of the setting to update
   * \param[in] value the value that the setting should take
   *
   * \sa LoadSettings, LoadUserSettings, SaveSettings, HasSettings, HasUserSettings, GetSetting
   */
  bool UpdateSettingString(uint32_t instance,
                           const std::string& key,
                           const std::string& value) override;

  /*!
   * \brief Retrieve a particular settings value.
   *
   * If a previously configured user setting is available, we return it's value, else we return the default (if available).
   *
   * \param[in] instance Instance identifier to use, use @ref KODI::ADDONS::ADDON_SETTINGS_ID
   *                     to denote global addon settings from settings.xml.
   * \param[in] key the id of the setting to retrieve
   * \return the current value of the setting, or the default if the setting has yet to be configured.
   *
   * \sa LoadSettings, LoadUserSettings, SaveSettings, HasSettings, HasUserSettings, UpdateSetting
   */
  std::string GetSetting(uint32_t instance, const std::string& key) override;

  /*!
   * \brief Retrieve a particular settings value as boolean.
   *
   * If a previously configured user setting is available, we return it's value, else we return the default (if available).
   *
   * \param[in] instance Instance identifier to use, use @ref KODI::ADDONS::ADDON_SETTINGS_ID
   *                     to denote global addon settings from settings.xml.
   * \param[in] key the id of the setting to retrieve
   * \param[out] value the current value of the setting, or the default if the setting has yet to be configured
   * \return true if the setting's value was retrieved, false otherwise.
   *
   * \sa LoadSettings, LoadUserSettings, SaveSettings, HasSettings, HasUserSettings, UpdateSetting
   */
  bool GetSettingBool(uint32_t instance, const std::string& key, bool& value) override;

  /*!
   * \brief Retrieve a particular settings value as integer.
   *
   * If a previously configured user setting is available, we return it's value, else we return the default (if available)
   *
   * \param[in] instance Instance identifier to use, use @ref KODI::ADDONS::ADDON_SETTINGS_ID
   *                     to denote global addon settings from settings.xml.
   * \param[in] key the id of the setting to retrieve
   * \param[out] value the current value of the setting, or the default if the setting has yet to be configured
   * \return true if the setting's value was retrieved, false otherwise.
   *
   * \sa LoadSettings, LoadUserSettings, SaveSettings, HasSettings, HasUserSettings, UpdateSetting
   */
  bool GetSettingInt(uint32_t instance, const std::string& key, int& value) override;

  /*!
   * \brief Retrieve a particular settings value as number.
   *
   * If a previously configured user setting is available, we return it's value, else we return the default (if available)
   *
   * \param[in] instance Instance identifier to use, use @ref KODI::ADDONS::ADDON_SETTINGS_ID
   *                     to denote global addon settings from settings.xml.
   * \param[in] key the id of the setting to retrieve
   * \param[out] value the current value of the setting, or the default if the setting has yet to be configured
   * \return true if the setting's value was retrieved, false otherwise.
   *
   * \sa LoadSettings, LoadUserSettings, SaveSettings, HasSettings, HasUserSettings, UpdateSetting
   */
  bool GetSettingNumber(uint32_t instance, const std::string& key, double& value) override;

  /*!
   * \brief Retrieve a particular settings value as string
   *
   * If a previously configured user setting is available, we return it's value, else we return the default (if available)
   *
   * \param[in] instance Instance identifier to use, use @ref KODI::ADDONS::ADDON_SETTINGS_ID
   *                     to denote global addon settings from settings.xml.
   * \param[in] key the id of the setting to retrieve
   * \param[out] value the current value of the setting, or the default if the setting has yet to be configured
   * \return true if the setting's value was retrieved, false otherwise.
   *
   * \sa LoadSettings, LoadUserSettings, SaveSettings, HasSettings, HasUserSettings, UpdateSetting
   */
  bool GetSettingString(uint32_t instance, const std::string& key, std::string& value) override;

  std::shared_ptr<CAddonSettings> GetSettings(uint32_t instance) override;

  /*! \brief get the required version of a dependency.
   \param dependencyID the addon ID of the dependency.
   \return the version this addon requires.
   */
  AddonVersion GetDependencyVersion(const std::string &dependencyID) const override;

  /*! \brief return whether or not this addon satisfies the given version requirements
   \param version the version to meet.
   \return true if  min_version <= version <= current_version, false otherwise.
   */
  bool MeetsVersion(const AddonVersion& versionMin, const AddonVersion& version) const override
  {
    return m_addonInfo->MeetsVersion(versionMin, version);
  }
  bool ReloadSettings(uint32_t instance) override;

  void ResetSettings(uint32_t instance) override;

  /*! \brief retrieve the running instance of an add-on if it persists while running.
   */
  AddonPtr GetRunningInstance() const override { return AddonPtr(); }

  void OnPreInstall() override {};
  void OnPostInstall(bool update, bool modal) override {};
  void OnPreUnInstall() override {};
  void OnPostUnInstall() override {};

protected:
  /*!
   * \brief Whether or not the settings have been initialized.
   *
   * \param[in] instance Instance identifier to use, use @ref KODI::ADDONS::ADDON_SETTINGS_ID
   *                     to denote global addon settings from settings.xml.
   * \return true if settings initialize was successfull
   */
  virtual bool SettingsInitialized(uint32_t instance) const;

  /*!
   * \brief Whether or not the settings have been loaded.
   *
   * \param[in] instance Instance identifier to use, use @ref KODI::ADDONS::ADDON_SETTINGS_ID
   *                     to denote global addon settings from settings.xml.
   * \return true if settings are loaded correct
   */
  virtual bool SettingsLoaded(uint32_t instance) const;

  /*!
   * \brief Load the default settings and override these with any previously configured user settings
   *
   * \param[in] instance Instance identifier to use, use @ref KODI::ADDONS::ADDON_SETTINGS_ID
   *                     to denote global addon settings from settings.xml.
   * \param[in] bForce force the load of settings even if they are already loaded (reload)
   * \param[in] loadUserSettings whether or not to load user settings
   * \return true if settings exist, false otherwise
   *
   * \sa LoadUserSettings, SaveSettings, HasSettings, HasUserSettings, GetSetting, UpdateSetting
   */
  bool LoadSettings(uint32_t instance, bool bForce, bool loadUserSettings = true);

  /*!
   * \brief Load the user settings
   *
   * \param[in] instance Instance identifier to use, use @ref KODI::ADDONS::ADDON_SETTINGS_ID
   *                     to denote global addon settings from settings.xml.
   * \return true if user settings exist, false otherwise
   *
   * \sa LoadSettings, SaveSettings, HasSettings, HasUserSettings, GetSetting, UpdateSetting
   */
  virtual bool LoadUserSettings(uint32_t instance);

  /*!
   * \brief Whether there are settings to be saved
   *
   * \param[in] instance Instance identifier to use, use @ref KODI::ADDONS::ADDON_SETTINGS_ID
   *                     to denote global addon settings from settings.xml.
   * \return true if settings has to save
   *
   * \sa SaveSettings
   */
  virtual bool HasSettingsToSave(uint32_t instance) const;

  /*!
   * \brief Parse settings from an XML document
   *
   * \param[in] instance Instance identifier to use, use @ref KODI::ADDONS::ADDON_SETTINGS_ID
   *                     to denote global addon settings from settings.xml.
   * \param[in] doc XML document to parse for settings
   * \param[in] loadDefaults if true, the default attribute is used and settings are reset prior to parsing, else the value attribute is used.
   * \return true if settings are loaded, false otherwise
   *
   * \sa SettingsToXML
   */
  virtual bool SettingsFromXML(uint32_t instance,
                               const CXBMCTinyXML& doc,
                               bool loadDefaults = false);

  /*!
   * \brief Write settings into an XML document
   *
   * \param[in] instance Instance identifier to use, use @ref KODI::ADDONS::ADDON_SETTINGS_ID
   *                     to denote global addon settings from settings.xml.
   * \param[out] doc XML document to receive the settings
   * \return true if settings are saved, false otherwise
   *
   * \sa SettingsFromXML
   */
  virtual bool SettingsToXML(uint32_t instance, CXBMCTinyXML& doc) const;

  const AddonInfoPtr m_addonInfo;

private:
  struct CSettingsData
  {
    bool m_loadSettingsFailed{false};
    bool m_hasUserSettings{false};
    std::string m_addonSettingsPath;
    std::string m_userSettingsPath;
    std::shared_ptr<CAddonSettings> m_addonSettings;
  };

  bool InitSettings(uint32_t instance);

  std::string m_profilePath;
  mutable std::unordered_map<uint32_t, CSettingsData> m_settings;
  const TYPE m_type;
};

}; /* namespace ADDON */

