/*
 *  Copyright (C) 2005-2021 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "InstanceHandler.h"

#include "ServiceBroker.h"
#include "addons/interface/Controller.h"
#include "addons/interface/api/addon_base.h"
#include "addons/settings/AddonSettings.h"
#include "filesystem/Directory.h"
#include "filesystem/SpecialProtocol.h"
#include "utils/StringUtils.h"
#include "utils/log.h"

using namespace ADDON;
using namespace KODI::ADDONS::INTERFACE;

std::mutex IInstanceHandler::m_cdSec;

IInstanceHandler::IInstanceHandler(const KODI_ADDON_INSTANCE_BACKEND_HDL hdl,
                                   KODI_ADDON_INSTANCE_TYPE type,
                                   const AddonInfoPtr& addonInfo,
                                   AddonInstanceId instanceId /* = ADDON_INSTANCE_ID_UNUSED */,
                                   KODI_HANDLE parentInstance /* = nullptr*/,
                                   const std::string& uniqueWorkID /* = ""*/)
  : m_type(type), m_instanceId(instanceId), m_parentInstance(parentInstance), m_addonInfo(addonInfo)
{
  // if no special instance ID is given generate one from class pointer (is
  // faster as unique id and also safe enough for them).
  m_uniqueWorkID = !uniqueWorkID.empty() ? uniqueWorkID : StringUtils::Format("{}", fmt::ptr(this));

  m_info.instance_id = instanceId;
  m_info.unique_work_id = m_uniqueWorkID.c_str();
  m_info.type = m_type;
  m_info.kodi = hdl;
  m_info.parent = m_parentInstance;
}

IInstanceHandler::~IInstanceHandler()
{
}

std::string IInstanceHandler::ID() const
{
  return m_addonInfo ? m_addonInfo->ID() : "";
}

std::string IInstanceHandler::Name() const
{
  return m_addonInfo ? m_addonInfo->Name() : "";
}

std::string IInstanceHandler::Author() const
{
  return m_addonInfo ? m_addonInfo->Author() : "";
}

std::string IInstanceHandler::Icon() const
{
  return m_addonInfo ? m_addonInfo->Icon() : "";
}

std::string IInstanceHandler::Path() const
{
  return m_addonInfo ? m_addonInfo->Path() : "";
}

std::string IInstanceHandler::Profile() const
{
  return m_addonInfo ? m_addonInfo->ProfilePath() : "";
}

AddonVersion IInstanceHandler::Version() const
{
  return m_addonInfo ? m_addonInfo->Version() : AddonVersion();
}

ADDON_STATUS IInstanceHandler::CreateInstance()
{
  m_process =
      CServiceBroker::GetAddonIfcCtrl().GetAddonProcess(m_addonInfo, this, &m_info, m_addon);
  if (!m_process)
  {
    CLog::Log(LOGERROR,
              "IInstanceHandler::{}: {} failed to get addon process during instance creation",
              __func__, m_addonInfo->ID());
    return ADDON_STATUS_UNKNOWN;
  }
  m_ifc = &m_process->GetIfc();

  if (!m_addon || !m_ifc)
    return ADDON_STATUS_UNKNOWN;

  std::unique_lock<std::mutex> lock(m_cdSec);

  ADDON_STATUS status = m_ifc->kodi_addon_base_h->kodi_addon_create_instance_v1(
      m_process->m_hdl, &m_info, &m_instance);
  if (status != ADDON_STATUS_OK)
  {
    CLog::Log(LOGERROR,
              "IInstanceHandler::{}: {} returned bad status \"{}\" during instance creation",
              __func__, m_addonInfo->ID(), status);
  }
  return status;
}

void IInstanceHandler::DestroyInstance()
{
  std::unique_lock<std::mutex> lock(m_cdSec);
  if (m_addon && m_ifc)
  {
    m_ifc->kodi_addon_base_h->kodi_addon_destroy_instance_v1(m_process->m_hdl, &m_info, m_instance);
    m_ifc = nullptr;
  }

  CServiceBroker::GetAddonIfcCtrl().ReleaseAddonProcess(m_process, this);
  m_process = nullptr;
}

std::shared_ptr<CSetting> IInstanceHandler::GetSetting(const std::string& setting)
{
  if (!m_addon->HasSettings())
  {
    CLog::Log(LOGERROR, "IInstanceHandler::{} - couldn't get settings for add-on '{}'", __func__,
              Name());
    return nullptr;
  }

  auto value = m_addon->GetSettings()->GetSetting(setting);
  if (value == nullptr)
  {
    CLog::Log(LOGERROR, "IInstanceHandler::{} - can't find setting '{}' in '{}'", __func__, setting,
              Name());
    return nullptr;
  }

  return value;
}

char* IInstanceHandler::get_instance_user_path()
{
  const std::string path = CSpecialProtocol::TranslatePath(m_addon->Profile());

  XFILE::CDirectory::Create(path);
  return strdup(path.c_str());
}

bool IInstanceHandler::is_instance_setting_using_default(const char* id)
{
  if (!id)
    return false;

  auto setting = GetSetting(id);
  if (setting == nullptr)
    return false;

  return setting->IsDefault();
}

bool IInstanceHandler::get_instance_setting_bool(const char* id, bool* value)
{
  if (!id || !value)
    return false;

  auto setting = GetSetting(id);
  if (setting == nullptr)
    return false;

  if (setting->GetType() != SettingType::Boolean)
  {
    CLog::Log(LOGERROR, "IInstanceHandler::{} - setting '{}' is not a boolean in '{}'", __func__,
              id, Name());
    return false;
  }

  *value = std::static_pointer_cast<CSettingBool>(setting)->GetValue();
  return true;
}

bool IInstanceHandler::get_instance_setting_int(const char* id, int* value)
{
  if (!id || !value)
    return false;

  auto setting = GetSetting(id);
  if (setting == nullptr)
    return false;

  if (setting->GetType() != SettingType::Integer && setting->GetType() != SettingType::Number)
  {
    CLog::Log(LOGERROR, "IInstanceHandler::{} - setting '{}' is not a integer in '{}'", __func__,
              id, Name());
    return false;
  }

  if (setting->GetType() == SettingType::Integer)
    *value = std::static_pointer_cast<CSettingInt>(setting)->GetValue();
  else
    *value = static_cast<int>(std::static_pointer_cast<CSettingNumber>(setting)->GetValue());
  return true;
}

bool IInstanceHandler::get_instance_setting_float(const char* id, double* value)
{
  if (!id || !value)
    return false;

  auto setting = GetSetting(id);
  if (setting == nullptr)
    return false;

  if (setting->GetType() != SettingType::Number)
  {
    CLog::Log(LOGERROR, "IInstanceHandler::{} - setting '{}' is not a number in '{}'", __func__, id,
              Name());
    return false;
  }

  *value = std::static_pointer_cast<CSettingNumber>(setting)->GetValue();
  return true;
}

bool IInstanceHandler::get_instance_setting_string(const char* id, char** value)
{
  if (!id || !value)
    return false;

  auto setting = GetSetting(id);
  if (setting == nullptr)
    return false;

  if (setting->GetType() != SettingType::String)
  {
    CLog::Log(LOGERROR, "IInstanceHandler::{} - setting '{}' is not a string in '{}'", __func__, id,
              Name());
    return false;
  }

  *value = strdup(std::static_pointer_cast<CSettingString>(setting)->GetValue().c_str());
  return true;
}

bool IInstanceHandler::set_instance_setting_bool(const char* id, bool value)
{
  if (!id)
    return false;

  if (CHdl_kodi_addon_base_h::UpdateSettingInActiveDialog(ID(), id, value ? "true" : "false"))
    return true;

  if (!m_addon->UpdateSettingBool(id, value))
  {
    CLog::Log(LOGERROR, "IInstanceHandler::{} - invalid setting type", __func__);
    return false;
  }

  m_addon->SaveSettings();

  return true;
}

bool IInstanceHandler::set_instance_setting_int(const char* id, int value)
{
  if (!id)
    return false;

  if (CHdl_kodi_addon_base_h::UpdateSettingInActiveDialog(ID(), id, std::to_string(value)))
    return true;

  if (!m_addon->UpdateSettingInt(id, value))
  {
    CLog::Log(LOGERROR, "IInstanceHandler::{} - invalid setting type", __func__);
    return false;
  }

  m_addon->SaveSettings();

  return true;
}

bool IInstanceHandler::set_instance_setting_float(const char* id, double value)
{
  if (!id)
    return false;

  if (CHdl_kodi_addon_base_h::UpdateSettingInActiveDialog(ID(), id,
                                                          StringUtils::Format("{:f}", value)))
    return true;

  if (!m_addon->UpdateSettingNumber(id, static_cast<double>(value)))
  {
    CLog::Log(LOGERROR, "IInstanceHandler::{} - invalid setting type", __func__);
    return false;
  }

  m_addon->SaveSettings();

  return true;
}

bool IInstanceHandler::set_instance_setting_string(const char* id, const char* value)
{
  if (!id || !value)
    return false;

  if (CHdl_kodi_addon_base_h::UpdateSettingInActiveDialog(ID(), id, value))
    return true;

  if (!m_addon->UpdateSettingString(id, value))
  {
    CLog::Log(LOGERROR, "IInstanceHandler::{} - invalid setting type", __func__);
    return false;
  }

  m_addon->SaveSettings();

  return true;
}
