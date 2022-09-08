/*
 *  Copyright (C) 2005-2021 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "RunningProcess.h"
#include "addons/AddonVersion.h"
#include "addons/IAddon.h"
#include "addons/kodi-dev-kit/include/kodi/c-api/addon_base.h"

#include <memory>
#include <mutex>

class CSetting;

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

class CInterface;
typedef void* ADDON_HANDLE;
typedef void* KODI_HANDLE;

class IInstanceHandler
{
public:
  IInstanceHandler(const KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                   KODI_ADDON_INSTANCE_TYPE type,
                   const std::shared_ptr<ADDON::CAddonInfo>& addonInfo,
                   ADDON::AddonInstanceId instanceId = ADDON::ADDON_INSTANCE_ID_UNUSED,
                   ADDON_HANDLE parentInstance = nullptr,
                   const std::string& uniqueWorkID = "");
  virtual ~IInstanceHandler();

  KODI_ADDON_INSTANCE_TYPE UsedType() const { return m_type; }
  ADDON::AddonInstanceId InstanceId() const { return m_instanceId; }
  const std::string& UniqueWorkID() { return m_uniqueWorkID; }

  std::string ID() const;
  std::string Name() const;
  std::string Author() const;
  std::string Icon() const;
  std::string Path() const;
  std::string Profile() const;
  ADDON::AddonVersion Version() const;

  ADDON_STATUS CreateInstance();
  void DestroyInstance();
  const std::shared_ptr<ADDON::IAddon>& Addon() const { return m_addon; }
  const std::shared_ptr<ADDON::CAddonInfo>& GetAddonInfo() const { return m_addonInfo; }
  const std::shared_ptr<CRunningProcess>& GetProcess() const { return m_process; }
  const KODI_ADDON_INSTANCE_INFO* GetCInfo() const { return &m_info; }

  virtual void StopReport(ChildStatus status) {}
  virtual void OnPreInstall() {}
  virtual void OnPostInstall(bool update, bool modal) {}
  virtual void OnPreUnInstall() {}
  virtual void OnPostUnInstall() {}

  char* get_instance_user_path();
  /*---AUTO_GEN_PARSE<CB:kodi_addon_instance_get_user_path>---*/
  bool is_instance_setting_using_default(const char* id);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_instance_is_setting_using_default>---*/
  bool get_instance_setting_bool(const char* id, bool* value);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_instance_get_setting_bool>---*/
  bool get_instance_setting_int(const char* id, int* value);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_instance_get_setting_int>---*/
  bool get_instance_setting_float(const char* id, double* value);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_instance_get_setting_float>---*/
  bool get_instance_setting_string(const char* id, char** value);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_instance_get_setting_string>---*/
  bool set_instance_setting_bool(const char* id, bool value);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_instance_set_setting_bool>---*/
  bool set_instance_setting_int(const char* id, int value);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_instance_set_setting_int>---*/
  bool set_instance_setting_float(const char* id, double value);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_instance_set_setting_float>---*/
  bool set_instance_setting_string(const char* id, const char* value);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_instance_set_setting_string>---*/

protected:
  CInterface* m_ifc;
  KODI_ADDON_INSTANCE_INFO m_info{};
  KODI_HANDLE m_instance{};

private:
  std::shared_ptr<CSetting> GetSetting(const std::string& setting);

  const KODI_ADDON_INSTANCE_TYPE m_type;
  const ADDON::AddonInstanceId m_instanceId;
  std::string m_uniqueWorkID;
  ADDON_HANDLE m_parentInstance;
  std::shared_ptr<CRunningProcess> m_process;
  std::shared_ptr<ADDON::CAddonInfo> m_addonInfo;
  std::shared_ptr<ADDON::IAddon> m_addon;
  static std::mutex m_cdSec;
};

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
