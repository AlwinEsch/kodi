/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "Addon.h"

#include <algorithm>
#include <string.h>
#include <ostream>
#include <utility>
#include <vector>

#include "AddonManager.h"
#include "addons/settings/AddonSettings.h"
#include "filesystem/Directory.h"
#include "filesystem/File.h"
#include "RepositoryUpdater.h"
#include "settings/Settings.h"
#include "ServiceBroker.h"
#include "utils/log.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "utils/XMLUtils.h"

#ifdef HAS_PYTHON
#include "interfaces/python/XBPython.h"
#endif

using XFILE::CDirectory;
using XFILE::CFile;
using namespace KODI::ADDONS;

namespace ADDON
{

CAddon::CAddon(const AddonInfoPtr& addonInfo, TYPE addonType)
  : m_addonInfo(addonInfo),
    m_userSettingsPath(),
    m_loadSettingsFailed(false),
    m_hasUserSettings(false),
    m_profilePath(StringUtils::Format("special://profile/addon_data/{}/", m_addonInfo->ID())),
    m_settings(nullptr),
    m_type(addonType == ADDON_UNKNOWN ? addonInfo->MainType() : addonType)
{
  m_userSettingsPath = URIUtils::AddFileToFolder(m_profilePath, "settings.xml");
}

/**
 * Settings Handling
 */

bool CAddon::CanHaveAddonOrInstanceSettings()
{
  return HasSettings(ADDON_SETTINGS_ID);
}

bool CAddon::HasSettings(uint32_t instance)
{
  return LoadSettings(instance, false) && m_settings->HasSettings();
}

bool CAddon::SettingsInitialized(uint32_t instance) const
{
  return m_settings != nullptr && m_settings->IsInitialized();
}

bool CAddon::SettingsLoaded(uint32_t instance) const
{
  return m_settings != nullptr && m_settings->IsLoaded();
}

bool CAddon::LoadSettings(uint32_t instance, bool bForce, bool loadUserSettings /* = true */)
{
  if (!bForce && SettingsInitialized(instance))
    return true;

  if (m_loadSettingsFailed)
    return false;

  // assume loading settings fails
  m_loadSettingsFailed = true;

  // reset the settings if we are forced to
  if (bForce && SettingsInitialized(instance))
    GetSettings(instance)->Uninitialize();

  // load the settings definition XML file
  auto addonSettingsDefinitionFile = URIUtils::AddFileToFolder(m_addonInfo->Path(), "resources", "settings.xml");
  CXBMCTinyXML addonSettingsDefinitionDoc;
  if (!addonSettingsDefinitionDoc.LoadFile(addonSettingsDefinitionFile))
  {
    if (CFile::Exists(addonSettingsDefinitionFile))
    {
      CLog::Log(LOGERROR, "CAddon[{}]: unable to load: {}, Line {}\n{}", ID(),
                addonSettingsDefinitionFile, addonSettingsDefinitionDoc.ErrorRow(),
                addonSettingsDefinitionDoc.ErrorDesc());
    }

    return false;
  }

  // initialize the settings definition
  if (!GetSettings(instance)->Initialize(addonSettingsDefinitionDoc))
  {
    CLog::Log(LOGERROR, "CAddon[{}]: failed to initialize addon settings", ID());
    return false;
  }

  // loading settings didn't fail
  m_loadSettingsFailed = false;

  // load user settings / values
  if (loadUserSettings)
    LoadUserSettings(instance);

  return true;
}

bool CAddon::HasUserSettings(uint32_t instance)
{
  if (!LoadSettings(instance, false))
    return false;

  return SettingsLoaded(instance) && m_hasUserSettings;
}

bool CAddon::ReloadSettings(uint32_t instance)
{
  return LoadSettings(instance, true);
}

void CAddon::ResetSettings(uint32_t instance)
{
  m_settings.reset();
}

bool CAddon::LoadUserSettings(uint32_t instance)
{
  if (!SettingsInitialized(instance))
    return false;

  m_hasUserSettings = false;

  // there are no user settings
  if (!CFile::Exists(m_userSettingsPath))
  {
    // mark the settings as loaded
    GetSettings(instance)->SetLoaded();
    return true;
  }

  CXBMCTinyXML doc;
  if (!doc.LoadFile(m_userSettingsPath))
  {
    CLog::Log(LOGERROR, "CAddon[{}]: failed to load addon settings from {}", ID(),
              m_userSettingsPath);
    return false;
  }

  return SettingsFromXML(instance, doc);
}

bool CAddon::HasSettingsToSave(uint32_t instance) const
{
  return SettingsLoaded(instance);
}

void CAddon::SaveSettings(uint32_t instance)
{
  if (!HasSettingsToSave(instance))
    return; // no settings to save

  // break down the path into directories
  std::string strAddon = URIUtils::GetDirectory(m_userSettingsPath);
  std::string strRoot = URIUtils::GetDirectory(strAddon);

  // create the individual folders
  if (!CDirectory::Exists(strRoot))
    CDirectory::Create(strRoot);
  if (!CDirectory::Exists(strAddon))
    CDirectory::Create(strAddon);

  // create the XML file
  CXBMCTinyXML doc;
  if (SettingsToXML(instance, doc))
    doc.SaveFile(m_userSettingsPath);

  m_hasUserSettings = true;

  //push the settings changes to the running addon instance
  CServiceBroker::GetAddonMgr().ReloadSettings(ID(), instance);
#ifdef HAS_PYTHON
  CServiceBroker::GetXBPython().OnSettingsChanged(ID());
#endif
}

std::string CAddon::GetSetting(uint32_t instance, const std::string& key)
{
  if (key.empty() || !LoadSettings(instance, false))
    return ""; // no settings available

  auto setting = m_settings->GetSetting(key);
  if (setting != nullptr)
    return setting->ToString();

  return "";
}

template<class TSetting>
bool GetSettingValue(CAddon& addon,
                     uint32_t instance,
                     const std::string& key,
                     typename TSetting::Value& value)
{
  if (key.empty() || !addon.HasSettings(instance))
    return false;

  auto setting = addon.GetSettings(instance)->GetSetting(key);
  if (setting == nullptr || setting->GetType() != TSetting::Type())
    return false;

  value = std::static_pointer_cast<TSetting>(setting)->GetValue();
  return true;
}

bool CAddon::GetSettingBool(uint32_t instance, const std::string& key, bool& value)
{
  return GetSettingValue<CSettingBool>(*this, instance, key, value);
}

bool CAddon::GetSettingInt(uint32_t instance, const std::string& key, int& value)
{
  return GetSettingValue<CSettingInt>(*this, instance, key, value);
}

bool CAddon::GetSettingNumber(uint32_t instance, const std::string& key, double& value)
{
  return GetSettingValue<CSettingNumber>(*this, instance, key, value);
}

bool CAddon::GetSettingString(uint32_t instance, const std::string& key, std::string& value)
{
  return GetSettingValue<CSettingString>(*this, instance, key, value);
}

void CAddon::UpdateSetting(uint32_t instance, const std::string& key, const std::string& value)
{
  if (key.empty() || !LoadSettings(instance, false))
    return;

  // try to get the setting
  auto setting = m_settings->GetSetting(key);

  // if the setting doesn't exist, try to add it
  if (setting == nullptr)
  {
    setting = m_settings->AddSetting(key, value);
    if (setting == nullptr)
    {
      CLog::Log(LOGERROR, "CAddon[{}]: failed to add undefined setting \"{}\"", ID(), key);
      return;
    }
  }

  setting->FromString(value);
}

template<class TSetting>
bool UpdateSettingValue(CAddon& addon,
                        uint32_t instance,
                        const std::string& key,
                        typename TSetting::Value value)
{
  if (key.empty() || !addon.HasSettings(instance))
    return false;

  // try to get the setting
  auto setting = addon.GetSettings(instance)->GetSetting(key);

  // if the setting doesn't exist, try to add it
  if (setting == nullptr)
  {
    setting = addon.GetSettings(instance)->AddSetting(key, value);
    if (setting == nullptr)
    {
      CLog::Log(LOGERROR, "CAddon[{}]: failed to add undefined setting \"{}\"", addon.ID(), key);
      return false;
    }
  }

  if (setting->GetType() != TSetting::Type())
    return false;

  return std::static_pointer_cast<TSetting>(setting)->SetValue(value);
}

bool CAddon::UpdateSettingBool(uint32_t instance, const std::string& key, bool value)
{
  return UpdateSettingValue<CSettingBool>(*this, instance, key, value);
}

bool CAddon::UpdateSettingInt(uint32_t instance, const std::string& key, int value)
{
  return UpdateSettingValue<CSettingInt>(*this, instance, key, value);
}

bool CAddon::UpdateSettingNumber(uint32_t instance, const std::string& key, double value)
{
  return UpdateSettingValue<CSettingNumber>(*this, instance, key, value);
}

bool CAddon::UpdateSettingString(uint32_t instance,
                                 const std::string& key,
                                 const std::string& value)
{
  return UpdateSettingValue<CSettingString>(*this, instance, key, value);
}

bool CAddon::SettingsFromXML(uint32_t instance,
                             const CXBMCTinyXML& doc,
                             bool loadDefaults /* = false */)
{
  if (doc.RootElement() == nullptr)
    return false;

  // if the settings haven't been initialized yet, try it from the given XML
  if (!SettingsInitialized(instance))
  {
    if (!GetSettings(instance)->Initialize(doc))
    {
      CLog::Log(LOGERROR, "CAddon[{}]: failed to initialize addon settings", ID());
      return false;
    }
  }

  // reset all setting values to their default value
  if (loadDefaults)
    GetSettings(instance)->SetDefaults();

  // try to load the setting's values from the given XML
  if (!GetSettings(instance)->Load(doc))
  {
    CLog::Log(LOGERROR, "CAddon[{}]: failed to load user settings", ID());
    return false;
  }

  m_hasUserSettings = true;

  return true;
}

bool CAddon::SettingsToXML(uint32_t instance, CXBMCTinyXML& doc) const
{
  if (!SettingsInitialized(instance))
    return false;

  if (!m_settings->Save(doc))
  {
    CLog::Log(LOGERROR, "CAddon[{}]: failed to save addon settings", ID());
    return false;
  }

  return true;
}

std::shared_ptr<CAddonSettings> CAddon::GetSettings(uint32_t instance)
{
  // initialize addon settings if necessary
  if (m_settings == nullptr)
  {
    m_settings = std::make_shared<CAddonSettings>(enable_shared_from_this::shared_from_this());
    LoadSettings(instance, false);
  }

  return m_settings;
}

std::string CAddon::LibPath() const
{
  // Get library related to given type on construction
  std::string libName = m_addonInfo->Type(m_type)->LibName();
  if (libName.empty())
  {
    // If not present fallback to master library
    libName = m_addonInfo->LibName();
    if (libName.empty())
      return "";
  }
  return URIUtils::AddFileToFolder(m_addonInfo->Path(), libName);
}

AddonVersion CAddon::GetDependencyVersion(const std::string &dependencyID) const
{
  return m_addonInfo->DependencyVersion(dependencyID);
}

void OnPreInstall(const AddonPtr& addon)
{
  //Fallback to the pre-install callback in the addon.
  //! @bug If primary extension point have changed we're calling the wrong method.
  addon->OnPreInstall();
}

void OnPostInstall(const AddonPtr& addon, bool update, bool modal)
{
  addon->OnPostInstall(update, modal);
}

void OnPreUnInstall(const AddonPtr& addon)
{
  addon->OnPreUnInstall();
}

void OnPostUnInstall(const AddonPtr& addon)
{
  addon->OnPostUnInstall();
}

} /* namespace ADDON */

