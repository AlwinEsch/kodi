/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "addons/addoninfo/AddonInfo.h"

#include <memory>
#include <set>

class TiXmlElement;

namespace KODI
{
namespace ADDONS
{

/*!
 * @brief Identifier denoting default add-on instance.
 *
 * All numbers greater than 0 denote add-ons with support for multiple instances.
 */
constexpr uint32_t ADDON_SINGLETON_INSTANCE_ID = 0;

/*!
 * @brief Identifier denoting add-on instance id as unused.
 *
 * @sa ADDON::IAddonInstanceHandler
 */
constexpr uint32_t ADDON_INSTANCE_ID_UNUSED = ADDON_SINGLETON_INSTANCE_ID;

/*!
 * @brief Identifier denoting default add-on settings.xml.
 *
 * All numbers greater than 0 denote add-on instances with an individual set of settings.
 */
constexpr uint32_t ADDON_SETTINGS_ID = ADDON_SINGLETON_INSTANCE_ID;

} /* namespace ADDONS */
} /* namespace KODI */

namespace ADDON
{
  class IAddon;
  typedef std::shared_ptr<IAddon> AddonPtr;
  class CInstanceVisualization;
  typedef std::shared_ptr<CInstanceVisualization> VizPtr;
  class CSkinInfo;
  typedef std::shared_ptr<CSkinInfo> SkinPtr;
  class CPluginSource;
  typedef std::shared_ptr<CPluginSource> PluginPtr;

  class CAddonMgr;
  class CAddonSettings;

  class IAddon : public std::enable_shared_from_this<IAddon>
  {
  public:
    virtual ~IAddon() = default;
    virtual TYPE MainType() const = 0;
    virtual TYPE Type() const =0;
    virtual bool HasType(TYPE type) const = 0;
    virtual bool HasMainType(TYPE type) const = 0;
    virtual std::string ID() const =0;
    virtual std::string Name() const =0;
    virtual bool IsInUse() const =0;
    virtual bool IsBinary() const = 0;
    virtual AddonVersion Version() const =0;
    virtual AddonVersion MinVersion() const =0;
    virtual std::string Summary() const =0;
    virtual std::string Description() const =0;
    virtual std::string Path() const =0;
    virtual std::string Profile() const =0;
    virtual std::string LibPath() const =0;
    virtual std::string ChangeLog() const =0;
    virtual std::string FanArt() const =0;
    virtual ArtMap Art() const =0;
    virtual std::vector<std::string> Screenshots() const =0;
    virtual std::string Author() const =0;
    virtual std::string Icon() const =0;
    virtual std::string Disclaimer() const =0;
    virtual AddonLifecycleState LifecycleState() const = 0;
    virtual std::string LifecycleStateDescription() const = 0;
    virtual CDateTime InstallDate() const =0;
    virtual CDateTime LastUpdated() const =0;
    virtual CDateTime LastUsed() const =0;
    virtual std::string Origin() const =0;
    virtual std::string OriginName() const = 0;
    virtual uint64_t PackageSize() const =0;
    virtual const InfoMap &ExtraInfo() const =0;
    virtual bool SupportsMultipleInstances() const = 0;
    virtual AddonInstanceUse InstanceUseType() const = 0;
    virtual std::vector<uint32_t> GetKnownInstanceIds() const = 0;
    virtual bool SupportsInstanceSettings() const = 0;
    virtual bool DeleteInstanceSettings(uint32_t instance) = 0;
    virtual bool CanHaveAddonOrInstanceSettings() = 0;
    virtual bool HasSettings(uint32_t instance) = 0;
    virtual bool HasUserSettings(uint32_t instance) = 0;
    virtual void SaveSettings(uint32_t instance) = 0;
    virtual void UpdateSetting(uint32_t instance,
                               const std::string& key,
                               const std::string& value) = 0;
    virtual bool UpdateSettingBool(uint32_t instance, const std::string& key, bool value) = 0;
    virtual bool UpdateSettingInt(uint32_t instance, const std::string& key, int value) = 0;
    virtual bool UpdateSettingNumber(uint32_t instance, const std::string& key, double value) = 0;
    virtual bool UpdateSettingString(uint32_t instance,
                                     const std::string& key,
                                     const std::string& value) = 0;
    virtual std::string GetSetting(uint32_t instance, const std::string& key) = 0;
    virtual bool GetSettingBool(uint32_t instance, const std::string& key, bool& value) = 0;
    virtual bool GetSettingInt(uint32_t instance, const std::string& key, int& value) = 0;
    virtual bool GetSettingNumber(uint32_t instance, const std::string& key, double& value) = 0;
    virtual bool GetSettingString(uint32_t instance,
                                  const std::string& key,
                                  std::string& value) = 0;
    virtual std::shared_ptr<CAddonSettings> GetSettings(uint32_t instance) = 0;
    virtual const std::vector<DependencyInfo> &GetDependencies() const =0;
    virtual AddonVersion GetDependencyVersion(const std::string &dependencyID) const =0;
    virtual bool MeetsVersion(const AddonVersion& versionMin,
                              const AddonVersion& version) const = 0;
    virtual bool ReloadSettings(uint32_t instance) = 0;
    virtual void ResetSettings(uint32_t instance) = 0;
    virtual AddonPtr GetRunningInstance() const=0;
    virtual void OnPreInstall() =0;
    virtual void OnPostInstall(bool update, bool modal) =0;
    virtual void OnPreUnInstall() =0;
    virtual void OnPostUnInstall() =0;
  };
};
