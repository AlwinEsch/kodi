/*
 *  Copyright (C) 2005-2021 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "c-api/addon_base.h"
#include "c-api/dl/kodi_api_1.h"

#ifdef __cplusplus

#include <assert.h>
#include <cstring>
#include <memory>
#include <string>
#include <stdexcept>

#include <dlfcn.h>
#include <unistd.h>

namespace kodi
{

using AddonLog = ADDON_LOG;

class dl;

namespace gui
{
struct IRenderHelper;
} /* namespace gui */

//==============================================================================
/// @ingroup cpp_kodi_Defs
/// @defgroup cpp_kodi_Defs_HardwareContext using HardwareContext
/// @brief **Hardware specific device context**\n
/// This defines an independent value which is used for hardware and OS specific
/// values.
///
/// This is basically a simple pointer which has to be changed to the desired
/// format at the corresponding places using <b>`static_cast<...>(...)`</b>.
///
///
///-------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <d3d11_1.h>
/// ..
/// // Note: Device() there is used inside addon child class about
/// // kodi::addon::CInstanceVisualization
/// ID3D11DeviceContext1* context = static_cast<ID3D11DeviceContext1*>(kodi::addon::CInstanceVisualization::Device());
/// ..
/// ~~~~~~~~~~~~~
///
///@{
using HardwareContext = ADDON_HARDWARE_CONTEXT;
///@}
//------------------------------------------------------------------------------

namespace addon
{

/*
 * Internally used helper class to manage processing of a "C" structure in "CPP"
 * class.
 *
 * At constant, the "C" structure is copied, otherwise the given pointer is
 * superseded and is changeable.
 *
 * -----------------------------------------------------------------------------
 *
 * Example:
 *
 * ~~~~~~~~~~~~~{.cpp}
 * extern "C" typedef struct C_SAMPLE_DATA
 * {
 *   unsigned int iUniqueId;
 * } C_SAMPLE_DATA;
 *
 * class CPPSampleData : public CStructHdl<CPPSampleData, C_SAMPLE_DATA>
 * {
 * public:
 *   CPPSampleData() = default;
 *   CPPSampleData(const CPPSampleData& sample) : CStructHdl(sample) { }
 *   CPPSampleData(const C_SAMPLE_DATA* sample) : CStructHdl(sample) { }
 *   CPPSampleData(C_SAMPLE_DATA* sample) : CStructHdl(sample) { }
 *
 *   void SetUniqueId(unsigned int uniqueId) { m_cStructure->iUniqueId = uniqueId; }
 *   unsigned int GetUniqueId() const { return m_cStructure->iUniqueId; }
 * };
 *
 * ~~~~~~~~~~~~~
 *
 * It also works with the following example:
 *
 * ~~~~~~~~~~~~~{.cpp}
 * CPPSampleData test;
 * // Some work
 * C_SAMPLE_DATA* data = test;
 * // Give "data" to Kodi
 * ~~~~~~~~~~~~~
 */
template<class CPP_CLASS, typename C_STRUCT>
class CStructHdl
{
public:
  CStructHdl() : m_cStructure(new C_STRUCT()), m_owner(true) {}

  CStructHdl(const CPP_CLASS& cppClass)
    : m_cStructure(new C_STRUCT(*cppClass.m_cStructure)), m_owner(true)
  {
  }

  CStructHdl(const C_STRUCT* cStructure) : m_cStructure(new C_STRUCT(*cStructure)), m_owner(true) {}

  CStructHdl(C_STRUCT* cStructure) : m_cStructure(cStructure) { assert(cStructure); }

  const CStructHdl& operator=(const CStructHdl& right)
  {
    assert(&right.m_cStructure);

    if (this == &right)
      return *this;

    if (m_cStructure && !m_owner)
    {
      memcpy(m_cStructure, right.m_cStructure, sizeof(C_STRUCT));
    }
    else
    {
      if (m_owner)
        delete m_cStructure;
      m_owner = true;
      m_cStructure = new C_STRUCT(*right.m_cStructure);
    }
    return *this;
  }

  const CStructHdl& operator=(const C_STRUCT& right)
  {
    assert(&right);

    if (m_cStructure == &right)
      return *this;

    if (m_cStructure && !m_owner)
    {
      memcpy(m_cStructure, &right, sizeof(C_STRUCT));
    }
    else
    {
      if (m_owner)
        delete m_cStructure;
      m_owner = true;
      m_cStructure = new C_STRUCT(*right);
    }
    return *this;
  }

  virtual ~CStructHdl()
  {
    if (m_owner)
      delete m_cStructure;
  }

  operator C_STRUCT*() { return m_cStructure; }
  operator const C_STRUCT*() const { return m_cStructure; }

  const C_STRUCT* GetCStructure() const { return m_cStructure; }

protected:
  C_STRUCT* m_cStructure = nullptr;

private:
  bool m_owner = false;
};

//==============================================================================
/// @ingroup cpp_kodi_addon_addonbase_Defs
/// @defgroup cpp_kodi_addon_addonbase_Defs_CSettingValue class CSettingValue
/// @brief **Setting value handler**\n
/// Inside addon main instance used helper class to give settings value.
///
/// This is used on @ref addon::CAddonBase::SetSetting() to inform addon about
/// settings change by used. This becomes then used to give the related value
/// name.
///
/// ----------------------------------------------------------------------------
///
/// @copydetails cpp_kodi_addon_addonbase_Defs_CSettingValue_Help
///
/// ----------------------------------------------------------------------------
///
/// **Here is a code example how this is used:**
///
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/AddonBase.h>
///
/// enum myEnumValue
/// {
///   valueA,
///   valueB,
///   valueC
/// };
///
/// std::string m_myStringValue;
/// int m_myIntegerValue;
/// bool m_myBooleanValue;
/// float m_myFloatingPointValue;
/// myEnumValue m_myEnumValue;
///
///
/// ADDON_STATUS CMyAddon::SetSetting(const std::string& settingName, const kodi::addon::CSettingValue& settingValue)
/// {
///   if (settingName == "my_string_value")
///     m_myStringValue = settingValue.GetString();
///   else if (settingName == "my_integer_value")
///     m_myIntegerValue = settingValue.GetInt();
///   else if (settingName == "my_boolean_value")
///     m_myBooleanValue = settingValue.GetBoolean();
///   else if (settingName == "my_float_value")
///     m_myFloatingPointValue = settingValue.GetFloat();
///   else if (settingName == "my_enum_value")
///     m_myEnumValue = settingValue.GetEnum<myEnumValue>();
/// }
/// ~~~~~~~~~~~~~
///
/// @note The asked type should match the type used on settings.xml.
///
///@{
class ATTR_DLL_LOCAL CSettingValue
{
public:
  explicit CSettingValue(const std::string& settingValue) : str(settingValue) {}

  bool empty() const { return str.empty(); }

  /// @defgroup cpp_kodi_addon_addonbase_Defs_CSettingValue_Help Value Help
  /// @ingroup cpp_kodi_addon_addonbase_Defs_CSettingValue
  ///
  /// <b>The following table contains values that can be set with @ref cpp_kodi_addon_addonbase_Defs_CSettingValue :</b>
  /// | Name | Type | Get call
  /// |------|------|----------
  /// | **Settings value as string** | `std::string` | @ref CSettingValue::GetString "GetString"
  /// | **Settings value as integer** | `int` | @ref CSettingValue::GetInt "GetInt"
  /// | **Settings value as unsigned integer** | `unsigned int` | @ref CSettingValue::GetUInt "GetUInt"
  /// | **Settings value as boolean** | `bool` | @ref CSettingValue::GetBoolean "GetBoolean"
  /// | **Settings value as floating point** | `float` | @ref CSettingValue::GetFloat "GetFloat"
  /// | **Settings value as enum** | `enum` | @ref CSettingValue::GetEnum "GetEnum"

  /// @addtogroup cpp_kodi_addon_addonbase_Defs_CSettingValue
  ///@{

  /// @brief To get settings value as string.
  std::string GetString() const { return str; }

  /// @brief To get settings value as integer.
  int GetInt() const { return std::atoi(str.c_str()); }

  /// @brief To get settings value as unsigned integer.
  unsigned int GetUInt() const { return std::atoi(str.c_str()); }

  /// @brief To get settings value as boolean.
  bool GetBoolean() const { return std::atoi(str.c_str()) > 0; }

  /// @brief To get settings value as floating point.
  float GetFloat() const { return static_cast<float>(std::atof(str.c_str())); }

  /// @brief To get settings value as enum.
  /// @note Inside settings.xml them stored as integer.
  template<typename enumType>
  enumType GetEnum() const
  {
    return static_cast<enumType>(GetInt());
  }

private:
  const std::string str;
};
//------------------------------------------------------------------------------

//==============================================================================
/// @ingroup cpp_kodi_addon_addonbase_Defs
/// @defgroup cpp_kodi_addon_addonbase_Defs_IInstanceInfo class IInstanceInfo
/// @brief **Instance informations**\n
/// Class to get any instance information when creating a new one.
///
///@{
class ATTR_DLL_LOCAL IInstanceInfo
{
public:
  /// @defgroup cpp_kodi_addon_addonbase_Defs_IInstanceInfo_Help Value Help
  /// @ingroup cpp_kodi_addon_addonbase_Defs_IInstanceInfo
  ///
  /// <b>The following table contains values that can be set with @ref cpp_kodi_addon_addonbase_Defs_IInstanceInfo :</b>
  /// | Name | Type | Get call
  /// |------|------|----------
  /// | **Used type identifier** | @ref KODI_ADDON_INSTANCE_TYPE | @ref CSettingValue::GetType "GetType"
  /// | **Check asked type used on this class** | `bool` | @ref CSettingValue::IsType "IsType"
  /// | **Get optional identification number (usage relate to addon type)** | `uint32_t` | @ref CSettingValue::GetNumber "GetNumber"
  /// | **Get optional identification string (usage relate to addon type)** | `std::string` | @ref CSettingValue::GetID "GetID"
  /// | **Get API version from Kodi about instance** | `std::string` | @ref CSettingValue::GetAPIVersion "GetAPIVersion"
  /// | **Check this is first created instance by Kodi** | `bool` | @ref CSettingValue::FirstInstance "FirstInstance"

  /// @addtogroup cpp_kodi_addon_addonbase_Defs_IInstanceInfo
  ///@{

  /// @brief To get settings value as string.
  KODI_ADDON_INSTANCE_TYPE GetType() const { return m_type; }

  /// @brief Check asked type used on this class.
  ///
  /// @param[in] type Type identifier to check
  /// @return True if type matches
  bool IsType(KODI_ADDON_INSTANCE_TYPE type) const { return m_type == type; }

  /// @brief Get optional identification number (usage relate to addon type).
  uint32_t GetInstanceID() const { return m_instance_id; }

  /// @brief Get optional identification string (usage relate to addon type).
  std::string GetUniqueWorkID() const { return m_unique_work_id; }

  KODI_ADDON_INSTANCE_BACKEND_HDL GetKodiHdl() const { return m_kodi; }
  KODI_ADDON_INSTANCE_HDL GetParentHdl() const { return m_parent; }

  ///@}

private:
  friend class CAddonBase;
  friend class IAddonInstance;
  friend class kodi::dl;

  IInstanceInfo() = default;
  explicit IInstanceInfo(const KODI_ADDON_INSTANCE_INFO* instance)
  {
    *this = instance;
  }

  const IInstanceInfo& operator=(const KODI_ADDON_INSTANCE_INFO* right)
  {
    assert(right);

    m_type = right->type;
    m_instance_id = right->instance_id;
    m_unique_work_id = right->unique_work_id;
    m_kodi = right->kodi;
    m_parent = right->parent;

    return *this;
  }

  KODI_ADDON_INSTANCE_TYPE m_type{ADDON_INSTANCE_UNKNOWN};
  uint32_t m_instance_id{0};
  std::string m_unique_work_id;
  KODI_ADDON_INSTANCE_BACKEND_HDL m_kodi{nullptr};
  KODI_ADDON_INSTANCE_HDL m_parent{nullptr};
};
///@}
//------------------------------------------------------------------------------

class IAddonInstance;
class CAddonBase;

} /* namespace addon */

class ATTR_DLL_LOCAL dl
{
public:
  static inline std::string get_kodi_lib_path(int argc, char* argv[])
  {
    std::string main_shared;

    for (int i = 1; i < argc; ++i)
    {
      if (argv[i] == nullptr)
        break;

      std::string arg = argv[i];
      if (arg.rfind("--dl=", 0) == 0)
      {
        main_shared = arg.substr(strlen("--dl="));
        break;
      }
      else if ((arg == "-l") || (arg == "--dl"))
      {
        if (i + 1 < argc) // Make sure we aren't at the end of argv!
        {
          main_shared =
              argv[++i]; // Increment 'i' so we don't get the argument as the next argv[i].
          break;
        }
      }
    }

    return main_shared;
  }

  static inline bool LoadDll(int argc, char** argv)
  {
    const std::string libPath = get_kodi_lib_path(argc, argv);
    local.dll = dlopen(libPath.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (local.dll == nullptr)
      return false;

    bool (*load_kodi_api_1)(struct kodi_api_1*) =
        reinterpret_cast<decltype(load_kodi_api_1)>(dlsym(local.dll, "load_kodi_api_1"));
    if (!load_kodi_api_1 || !load_kodi_api_1(&api))
    {
      fprintf(stderr, "FATAL: Failed to load required kodi dev kit lib '%s'!\n", libPath.c_str());
      dlclose(local.dll);
      return false;
    }

    return true;
  }

  static inline void UnloadDll() { dlclose(local.dll); }

  static kodi_api_1 api;
  static KODI_IFC_HDL hdl;

  struct LOCAL
  {
    KODI_DLL_HDL dll;

    kodi::addon::IAddonInstance* globalSingleInstance;
    kodi::addon::IInstanceInfo firstKodiInstance;
    kodi::addon::CAddonBase* addonBase;
  } static local;
};

namespace addon
{

//==============================================================================
/*
 * Internal class to control various instance types with general parts defined
 * here.
 *
 * Mainly is this currently used to identify requested instance types.
 *
 * @note This class is not need to know during add-on development thats why
 * commented with "*".
 */
class ATTR_DLL_LOCAL IAddonInstance
{
public:
  IAddonInstance()
  : m_instanceInfo(kodi::dl::local.firstKodiInstance),
    m_kodi(kodi::dl::local.firstKodiInstance.GetKodiHdl())
  {
    kodi::dl::local.globalSingleInstance = this;
  }

  explicit IAddonInstance(const kodi::addon::IInstanceInfo& instance)
    : m_instanceInfo(instance), m_kodi(instance.GetKodiHdl())
  {
  }
  virtual ~IAddonInstance() = default;

  virtual ADDON_STATUS CreateInstance(const kodi::addon::IInstanceInfo& instance,
                                      KODI_ADDON_INSTANCE_HDL& hdl)
  {
    return ADDON_STATUS_NOT_IMPLEMENTED;
  }

  virtual void SetAddonStruct(KODI_ADDON_INSTANCE_STRUCT* instance) = 0;

  virtual ADDON_STATUS SetInstanceSetting(const std::string& settingName,
                                          const kodi::addon::CSettingValue& settingValue)
  {
    return ADDON_STATUS_UNKNOWN;
  }

  inline bool IsInstanceSettingUsingDefault(const std::string& settingName)
  {
    return kodi::dl::api.kodi_addon_instance_is_setting_using_default(m_kodi, settingName.c_str());
  }

  inline std::string GetInstanceUserPath(const std::string& append = "")
  {
    char* str = kodi::dl::api.kodi_addon_instance_get_user_path(m_kodi);
    if (!str)
      return "";

    std::string ret = str;
    free(str);

    if (!append.empty())
    {
      if (append.at(0) != '\\' && append.at(0) != '/')
#ifdef TARGET_WINDOWS
        ret.append("\\");
#else
        ret.append("/");
#endif
      ret.append(append);
    }
    return ret;
  }

  inline bool CheckInstanceSettingString(const std::string& settingName, std::string& settingValue)
  {
    char* buffer = nullptr;
    bool ret = kodi::dl::api.kodi_addon_instance_get_setting_string(m_kodi, settingName.c_str(), &buffer);
    if (buffer)
    {
      if (ret)
        settingValue = buffer;
      free(buffer);
    }
    return ret;
  }

  inline std::string GetInstanceSettingString(const std::string& settingName,
                                              const std::string& defaultValue = "")
  {
    std::string settingValue = defaultValue;
    CheckInstanceSettingString(settingName, settingValue);
    return settingValue;
  }

  inline void SetInstanceSettingString(const std::string& settingName,
                                       const std::string& settingValue)
  {
    kodi::dl::api.kodi_addon_instance_set_setting_string(m_kodi, settingName.c_str(),
                                           settingValue.c_str());
  }

  inline bool CheckInstanceSettingInt(const std::string& settingName, int& settingValue)
  {
    return kodi::dl::api.kodi_addon_instance_get_setting_int(m_kodi, settingName.c_str(), &settingValue);
  }

  inline int GetInstanceSettingInt(const std::string& settingName, int defaultValue = 0)
  {
    int settingValue = defaultValue;
    CheckInstanceSettingInt(settingName, settingValue);
    return settingValue;
  }

  inline void SetInstanceSettingInt(const std::string& settingName, int settingValue)
  {
    kodi::dl::api.kodi_addon_instance_set_setting_int(m_kodi, settingName.c_str(), settingValue);
  }

  inline bool CheckInstanceSettingBoolean(const std::string& settingName, bool& settingValue)
  {
    return kodi::dl::api.kodi_addon_instance_get_setting_bool(m_kodi, settingName.c_str(), &settingValue);
  }

  inline bool GetInstanceSettingBoolean(const std::string& settingName, bool defaultValue = false)
  {
    bool settingValue = defaultValue;
    CheckInstanceSettingBoolean(settingName, settingValue);
    return settingValue;
  }

  inline void SetInstanceSettingBoolean(const std::string& settingName, bool settingValue)
  {
    kodi::dl::api.kodi_addon_instance_set_setting_bool(m_kodi, settingName.c_str(), settingValue);
  }

  inline bool CheckInstanceSettingFloat(const std::string& settingName, double& settingValue)
  {
    return kodi::dl::api.kodi_addon_instance_get_setting_float(m_kodi, settingName.c_str(), &settingValue);
  }

  inline double GetInstanceSettingFloat(const std::string& settingName, double defaultValue = 0.0)
  {
    double settingValue = defaultValue;
    CheckInstanceSettingFloat(settingName, settingValue);
    return settingValue;
  }

  inline void SetInstanceSettingFloat(const std::string& settingName, double settingValue)
  {
    kodi::dl::api.kodi_addon_instance_set_setting_float(m_kodi, settingName.c_str(), settingValue);
  }

  template<typename enumType>
  inline bool CheckInstanceSettingEnum(const std::string& settingName, enumType& settingValue)
  {
    using namespace kodi::addon;

    int settingValueInt = static_cast<int>(settingValue);
    bool ret =
        kodi::dl::api.kodi_addon_instance_get_setting_int(m_kodi, settingName.c_str(), &settingValueInt);
    if (ret)
      settingValue = static_cast<enumType>(settingValueInt);
    return ret;
  }

  template<typename enumType>
  inline enumType GetInstanceSettingEnum(const std::string& settingName,
                                         enumType defaultValue = static_cast<enumType>(0))
  {
    enumType settingValue = defaultValue;
    CheckInstanceSettingEnum(settingName, settingValue);
    return settingValue;
  }

  template<typename enumType>
  inline void SetInstanceSettingEnum(const std::string& settingName, enumType settingValue)
  {
    kodi::dl::api.kodi_addon_instance_set_setting_int(m_kodi, settingName.c_str(),
                                        static_cast<int>(settingValue));
  }

  const kodi::addon::IInstanceInfo& GetInfo() const { return m_instanceInfo; }
  KODI_ADDON_INSTANCE_BACKEND_HDL GetBackendInstanceHdl() const { return m_kodi; }

protected:
  kodi::addon::IInstanceInfo m_instanceInfo;
  KODI_ADDON_INSTANCE_BACKEND_HDL m_kodi;

private:
  friend class CAddonBase;

  static inline ADDON_STATUS INSTANCE_instance_setting_change_string(
      const KODI_ADDON_HDL hdl,
      const KODI_ADDON_INSTANCE_HDL instance_hdl,
      const char* name,
      const char* value)
  {
    return static_cast<IAddonInstance*>(hdl)->SetInstanceSetting(name, CSettingValue(value));
  }

  static inline ADDON_STATUS INSTANCE_instance_setting_change_boolean(
      const KODI_ADDON_HDL hdl,
      const KODI_ADDON_INSTANCE_HDL instance_hdl,
      const char* name,
      bool value)
  {
    return static_cast<IAddonInstance*>(hdl)->SetInstanceSetting(name,
                                                                 CSettingValue(value ? "1" : "0"));
  }

  static inline ADDON_STATUS INSTANCE_instance_setting_change_integer(
      const KODI_ADDON_HDL hdl,
      const KODI_ADDON_INSTANCE_HDL instance_hdl,
      const char* name,
      int value)
  {
    return static_cast<IAddonInstance*>(hdl)->SetInstanceSetting(
        name, CSettingValue(std::to_string(value)));
  }

  static inline ADDON_STATUS INSTANCE_instance_setting_change_float(
      const KODI_ADDON_HDL hdl,
      const KODI_ADDON_INSTANCE_HDL instance_hdl,
      const char* name,
      double value)
  {
    return static_cast<IAddonInstance*>(hdl)->SetInstanceSetting(
        name, CSettingValue(std::to_string(value)));
  }
};

//============================================================================
/// @addtogroup cpp_kodi_addon_addonbase
/// @brief **Add-on main instance class**\n
/// This is the addon main class, similar to an <b>`int main()`</b> in executable and
/// carries out initial work and later management of it.
///
class ATTR_DLL_LOCAL CAddonBase
{
public:
  //============================================================================
  /// @ingroup cpp_kodi_addon_addonbase
  /// @brief Addon base class constructor.
  ///
  CAddonBase() = default;
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_addon_addonbase
  /// @brief Destructor.
  ///
  virtual ~CAddonBase() = default;
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_addon_addonbase
  /// @brief Main addon creation function
  ///
  /// With this function addon can carry out necessary work which is required
  /// at later points or start necessary processes.
  ///
  /// This function is optional and necessary work can also be carried out
  /// using @ref CreateInstance (if it concerns any instance types).
  ///
  /// @return @ref ADDON_STATUS_OK if correct, for possible errors see
  ///         @ref ADDON_STATUS
  ///
  /// @note Terminating the add-on must be carried out using the class destructor
  /// given by child.
  ///
  virtual ADDON_STATUS Create() { return ADDON_STATUS_OK; }
  //----------------------------------------------------------------------------

  //============================================================================
  /// @ingroup cpp_kodi_addon_addonbase
  /// @brief To inform addon about changed settings values.
  ///
  /// This becomes called for every entry defined inside his settings.xml and
  /// as **last** call the one where last in xml (to identify end of calls).
  ///
  /// --------------------------------------------------------------------------
  ///
  /// @copydetails cpp_kodi_addon_addonbase_Defs_CSettingValue_Help
  ///
  ///
  /// --------------------------------------------------------------------------
  ///
  /// **Here is a code example how this is used:**
  ///
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/AddonBase.h>
  ///
  /// enum myEnumValue
  /// {
  ///   valueA,
  ///   valueB,
  ///   valueC
  /// };
  ///
  /// std::string m_myStringValue;
  /// int m_myIntegerValue;
  /// bool m_myBooleanValue;
  /// float m_myFloatingPointValue;
  /// myEnumValue m_myEnumValue;
  ///
  ///
  /// ADDON_STATUS CMyAddon::SetSetting(const std::string& settingName, const kodi::addon::CSettingValue& settingValue)
  /// {
  ///   if (settingName == "my_string_value")
  ///     m_myStringValue = settingValue.GetString();
  ///   else if (settingName == "my_integer_value")
  ///     m_myIntegerValue = settingValue.GetInt();
  ///   else if (settingName == "my_boolean_value")
  ///     m_myBooleanValue = settingValue.GetBoolean();
  ///   else if (settingName == "my_float_value")
  ///     m_myFloatingPointValue = settingValue.GetFloat();
  ///   else if (settingName == "my_enum_value")
  ///     m_myEnumValue = settingValue.GetEnum<myEnumValue>();
  /// }
  /// ~~~~~~~~~~~~~
  ///
  /// @note The asked type should match the type used on settings.xml.
  ///
  virtual ADDON_STATUS SetSetting(const std::string& settingName,
                                  const kodi::addon::CSettingValue& settingValue)
  {
    return ADDON_STATUS_UNKNOWN;
  }
  //----------------------------------------------------------------------------

  //==========================================================================
  /// @ingroup cpp_kodi_addon_addonbase
  /// @brief Instance created
  ///
  /// @param[in] instance Instance informations about
  /// @param[out] hdl The pointer to instance class created in addon.
  ///                 Needed to be able to identify them on calls.
  /// @return @ref ADDON_STATUS_OK if correct, for possible errors see @ref ADDON_STATUS
  ///
  ///
  /// --------------------------------------------------------------------------
  ///
  /// **Here is a code example how this is used:**
  ///
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/AddonBase.h>
  ///
  /// ...
  ///
  /// // If you use only one instance in your add-on, can be instanceType and
  /// // instanceID ignored
  /// ADDON_STATUS CMyAddon::CreateInstance(const kodi::addon::IInstanceInfo& instance,
  ///                                       KODI_ADDON_INSTANCE_HDL& hdl)
  /// {
  ///   if (instance.IsType(ADDON_INSTANCE_SCREENSAVER))
  ///   {
  ///     kodi::Log(ADDON_LOG_INFO, "Creating my Screensaver");
  ///     hdl = new CMyScreensaver(instance);
  ///     return ADDON_STATUS_OK;
  ///   }
  ///   else if (instance.IsType(ADDON_INSTANCE_VISUALIZATION))
  ///   {
  ///     kodi::Log(ADDON_LOG_INFO, "Creating my Visualization");
  ///     hdl = new CMyVisualization(instance);
  ///     return ADDON_STATUS_OK;
  ///   }
  ///   else if (...)
  ///   {
  ///     ...
  ///   }
  ///   return ADDON_STATUS_UNKNOWN;
  /// }
  ///
  /// ...
  ///
  /// ~~~~~~~~~~~~~
  ///
  virtual ADDON_STATUS CreateInstance(const kodi::addon::IInstanceInfo& instance,
                                      KODI_ADDON_INSTANCE_HDL& hdl)
  {
    return ADDON_STATUS_NOT_IMPLEMENTED;
  }
  //--------------------------------------------------------------------------

  //==========================================================================
  /// @ingroup cpp_kodi_addon_addonbase
  /// @brief Instance destroy
  ///
  /// This function is optional and intended to notify addon that the instance
  /// is terminating.
  ///
  /// @param[in] instance Instance informations about
  /// @param[in] hdl The pointer to instance class created in addon.
  ///
  /// @warning This call is only used to inform that the associated instance
  /// is terminated. The deletion is carried out in the background.
  ///
  virtual void DestroyInstance(const IInstanceInfo& instance, const KODI_ADDON_INSTANCE_HDL hdl) {}
  //--------------------------------------------------------------------------

  /* Background helper for GUI render systems, e.g. Screensaver or Visualization */
  std::shared_ptr<kodi::gui::IRenderHelper> m_renderHelper;

  static KODI_ADDON_FUNC GetAddonFunctions()
  {
    KODI_ADDON_FUNC func = {ADDONBASE_create,
                            ADDONBASE_destroy,

                            ADDONBASE_setting_change_string,
                            ADDONBASE_setting_change_boolean,
                            ADDONBASE_setting_change_integer,
                            ADDONBASE_setting_change_float,

                            ADDONBASE_create_instance,
                            ADDONBASE_destroy_instance,

                            IAddonInstance::INSTANCE_instance_setting_change_string,
                            IAddonInstance::INSTANCE_instance_setting_change_boolean,
                            IAddonInstance::INSTANCE_instance_setting_change_integer,
                            IAddonInstance::INSTANCE_instance_setting_change_float};
    return func;
  }

private:
  static ADDON_STATUS ADDONBASE_create(const struct KODI_ADDON_INSTANCE_INFO* first_instance, KODI_ADDON_HDL* hdl);

  static inline void ADDONBASE_destroy(const KODI_ADDON_HDL hdl)
  {
    delete static_cast<CAddonBase*>(hdl);
    kodi::dl::local.globalSingleInstance = nullptr;
    kodi::dl::local.addonBase = nullptr;
  }

  static inline ADDON_STATUS ADDONBASE_create_instance(const KODI_ADDON_HDL hdl,
                                                       const struct KODI_ADDON_INSTANCE_INFO* info,
                                                       struct KODI_ADDON_INSTANCE_STRUCT* instance)
  {
    ADDON_STATUS status = ADDON_STATUS_NOT_IMPLEMENTED;

    CAddonBase* base = static_cast<CAddonBase*>(hdl);
    if (base == nullptr || info == nullptr || instance == nullptr)
    {
      kodi::dl::api.kodi_log(
          ADDON_LOG_FATAL,
          "kodi::addon::CAddonBase CreateInstance called with empty instance memory");
      return ADDON_STATUS_PERMANENT_FAILURE;
    }

    IAddonInstance* instanceHdl = nullptr;

    /* Check about single instance usage:
     * 1. The kodi side instance pointer must be equal to first one
     * 2. The addon side instance pointer must be set
     * 3. And the requested type must be equal with used add-on class
     */
    if (kodi::dl::local.globalSingleInstance &&
        kodi::dl::local.globalSingleInstance->GetInfo().GetType() == info->type &&
        kodi::dl::local.globalSingleInstance->GetInfo().GetKodiHdl() == info->kodi)
    {
      /* The handling here is intended for the case of the add-on only one
       * instance and this is integrated in the add-on base class.
       */
      status = ADDON_STATUS_OK;

      instanceHdl = kodi::dl::local.globalSingleInstance;
      instance->hdl = instanceHdl;
    }
    else
    {
      /* Here it should use the CreateInstance instance function to allow
       * creation of several on one addon.
       */

      IInstanceInfo instanceInfo(info);

      /* Check first a parent is defined about (e.g. Codec within inputstream) */
      if (info->parent != nullptr)
        status =
            static_cast<IAddonInstance*>(info->parent)->CreateInstance(instanceInfo, instance->hdl);

      /* if no parent call the main instance creation function to get it */
      if (status == ADDON_STATUS_NOT_IMPLEMENTED)
      {
        if (kodi::dl::local.globalSingleInstance != nullptr)
        {
          kodi::dl::api.kodi_log(ADDON_LOG_FATAL,
                                 "kodi::addon::CAddonBase Creation of multiple together with "
                                 "single instance way is not allowed!");
          return ADDON_STATUS_PERMANENT_FAILURE;
        }
        status = base->CreateInstance(instanceInfo, instance->hdl);
      }

      instanceHdl = static_cast<IAddonInstance*>(instance->hdl);
    }

    if (instanceHdl != nullptr)
    {
      instanceHdl->SetAddonStruct(instance);
    }
    else
    {
      if (status == ADDON_STATUS_OK)
      {
        kodi::dl::api.kodi_log(ADDON_LOG_FATAL,
                               "kodi::addon::CAddonBase CreateInstance returned an "
                               "empty instance pointer, but reported OK!");
        return ADDON_STATUS_PERMANENT_FAILURE;
      }
      else
      {
        return status;
      }
    }

    if (instanceHdl->GetInfo().GetType() != info->type)
    {
      kodi::dl::api.kodi_log(
          ADDON_LOG_FATAL,
          "kodi::addon::CAddonBase CreateInstance difference between given and returned");
      delete static_cast<IAddonInstance*>(instance->hdl);
      instance->hdl = nullptr;
      return ADDON_STATUS_PERMANENT_FAILURE;
    }

    return status;
  }

  static inline void ADDONBASE_destroy_instance(const KODI_ADDON_HDL hdl,
                                                const struct KODI_ADDON_INSTANCE_INFO* info,
                                                const KODI_ADDON_INSTANCE_HDL instance_hdl)
  {
    CAddonBase* base = static_cast<CAddonBase*>(hdl);

    if (kodi::dl::local.globalSingleInstance == nullptr && instance_hdl != base)
    {
      IInstanceInfo instanceInfo(info);
      base->DestroyInstance(instanceInfo, instance_hdl);
      delete static_cast<IAddonInstance*>(instance_hdl);
    }
  }

  static inline ADDON_STATUS ADDONBASE_setting_change_string(const KODI_ADDON_HDL hdl,
                                                             const char* name,
                                                             const char* value)
  {
    return static_cast<CAddonBase*>(hdl)->SetSetting(name, CSettingValue(value));
  }

  static inline ADDON_STATUS ADDONBASE_setting_change_boolean(const KODI_ADDON_HDL hdl,
                                                              const char* name,
                                                              bool value)
  {
    return static_cast<CAddonBase*>(hdl)->SetSetting(name, CSettingValue(value ? "1" : "0"));
  }

  static inline ADDON_STATUS ADDONBASE_setting_change_integer(const KODI_ADDON_HDL hdl,
                                                              const char* name,
                                                              int value)
  {
    return static_cast<CAddonBase*>(hdl)->SetSetting(name, CSettingValue(std::to_string(value)));
  }

  static inline ADDON_STATUS ADDONBASE_setting_change_float(const KODI_ADDON_HDL hdl,
                                                            const char* name,
                                                            double value)
  {
    return static_cast<CAddonBase*>(hdl)->SetSetting(name, CSettingValue(std::to_string(value)));
  }
};

//==============================================================================
/// @ingroup cpp_kodi_addon
/// @brief To get the addon system installation folder.
///
/// @param[in] append [optional] Path to append to given string
/// @return Path where addon is installed
///
inline std::string ATTR_DLL_LOCAL GetAddonPath(const std::string& append = "")
{
  char* str = kodi::dl::api.kodi_addon_get_addon_path();
  std::string ret = str;
  free(str);
  if (!append.empty())
  {
    if (append.at(0) != '\\' && append.at(0) != '/')
#ifdef TARGET_WINDOWS
      ret.append("\\");
#else
      ret.append("/");
#endif
    ret.append(append);
  }
  return ret;
}
//------------------------------------------------------------------------------

//==============================================================================
/// @ingroup cpp_kodi_addon
/// @brief This function gives OS associated executable binary path of the addon.
///
/// With some systems this can differ from the addon path at @ref GetAddonPath.
///
/// As an example on Linux:
/// - Addon path is at `/usr/share/kodi/addons/YOUR_ADDON_ID`
/// - Library path is at `/usr/lib/kodi/addons/YOUR_ADDON_ID`
///
/// @note In addition, in this function, for a given folder, the add-on path
/// itself, but its parent.
///
/// @return Kodi's system library path where related addons are installed.
///
inline std::string ATTR_DLL_LOCAL GetLibPath(const std::string& append = "")
{
  char* str = kodi_addon_get_lib_path();
  std::string ret = str;
  free(str);
  if (!append.empty())
  {
    if (append.at(0) != '\\' && append.at(0) != '/')
#ifdef TARGET_WINDOWS
      ret.append("\\");
#else
      ret.append("/");
#endif
    ret.append(append);
  }
  return ret;
}
//------------------------------------------------------------------------------

//==============================================================================
/// @ingroup cpp_kodi_addon
/// @brief To get the user-related folder of the addon.
///
/// @note This folder is not created automatically and has to be created by the
/// addon the first time.
///
/// @param[in] append [optional] Path to append to given string
/// @return User path of addon
///
inline std::string ATTR_DLL_LOCAL GetUserPath(const std::string& append = "")
{
  char* str = kodi::dl::api.kodi_addon_get_user_path();
  std::string ret = str;
  free(str);
  if (!append.empty())
  {
    if (append.at(0) != '\\' && append.at(0) != '/')
#ifdef TARGET_WINDOWS
      ret.append("\\");
#else
      ret.append("/");
#endif
    ret.append(append);
  }
  return ret;
}
//------------------------------------------------------------------------------

//==============================================================================
/// @ingroup cpp_kodi_addon
/// @brief To get a temporary path for the addon
///
/// This gives a temporary path which the addon can use individually for its things.
///
/// The content of this folder will be deleted when Kodi is finished!
///
/// @param[in] append A string to append to returned temporary path
/// @return Individual path for the addon
///
inline std::string ATTR_DLL_LOCAL GetTempPath(const std::string& append = "")
{
  char* str = kodi::dl::api.kodi_addon_get_temp_path();
  std::string ret = str;
  free(str);
  if (!append.empty())
  {
    if (append.at(0) != '\\' && append.at(0) != '/')
#ifdef TARGET_WINDOWS
      ret.append("\\");
#else
      ret.append("/");
#endif
    ret.append(append);
  }
  return ret;
}
//------------------------------------------------------------------------------

//==============================================================================
/// @ingroup cpp_kodi_addon
/// @brief Returns an addon's localized 'unicode string'.
///
/// @param[in] labelId    string you want to localize
/// @param[in] defaultStr [opt] The default message, also helps to identify
///                       the code that is used <em>(default is
///                       <b><c>empty</c></b>)</em>
/// @return               The localized message, or default if the add-on
///                       helper fails to return a message
///
/// @note Label id's \b 30000 to \b 30999 and \b 32000 to \b 32999 are related
/// to the add-on's own included strings from
/// <b>./resources/language/resource.language.??_??/strings.po</b>
/// All other strings are from Kodi core language files.
///
///
/// ------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/General.h>
/// ...
/// std::string str = kodi::GetLocalizedString(30005, "Use me as default");
/// ...
/// ~~~~~~~~~~~~~
///
inline std::string ATTR_DLL_LOCAL GetLocalizedString(uint32_t labelId,
                                                     const std::string& defaultStr = "")
{
  std::string retString = defaultStr;
  char* strMsg = kodi::dl::api.kodi_get_localized_string(labelId);
  if (strMsg != nullptr)
  {
    if (std::strlen(strMsg))
      retString = strMsg;
    free(strMsg);
  }
  return retString;
}
//------------------------------------------------------------------------------

//##############################################################################
/// @ingroup cpp_kodi_addon
/// @defgroup cpp_kodi_settings 1. Setting control
/// @brief **Functions to handle settings access**\n
/// This can be used to get and set the addon related values inside his
/// settings.xml.
///
/// The settings style is given with installed part on e.g.
/// <b>`$HOME/.kodi/addons/myspecial.addon/resources/settings.xml`</b>. The
/// related edit becomes then stored inside
/// <b>`$HOME/.kodi/userdata/addon_data/myspecial.addon/settings.xml`</b>.
///
/*!@{*/

//==============================================================================
/// @brief Opens this Add-Ons settings dialog.
///
/// @return true if settings were changed and the dialog confirmed, false otherwise.
///
///
/// --------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/General.h>
/// ..
/// kodi::OpenSettings();
/// ..
/// ~~~~~~~~~~~~~
///
inline bool ATTR_DLL_LOCAL OpenSettings()
{
  return kodi::dl::api.kodi_addon_open_settings_dialog();
}
//------------------------------------------------------------------------------

//==============================================================================
/// @brief Check the given setting name is set to default value.
///
/// The setting name relate to names used in his <b>settings.xml</b> file.
///
/// @param[in] settingName The name of asked setting
/// @return true if setting is the default
///
inline bool ATTR_DLL_LOCAL IsSettingUsingDefault(const std::string& settingName)
{
  return kodi::dl::api.kodi_addon_is_setting_using_default(settingName.c_str());
}
//------------------------------------------------------------------------------

//==============================================================================
/// @brief Check and get a string setting value.
///
/// The setting name relate to names used in his <b>settings.xml</b> file.
///
/// @param[in] settingName The name of asked setting
/// @param[out] settingValue The given setting value
/// @return true if setting was successfully found and "settingValue" is set
///
/// @note If returns false, the "settingValue" is not changed.
///
///
/// ----------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/General.h>
///
/// std::string value;
/// if (!kodi::CheckSettingString("my_string_value", value))
///   value = "my_default_if_setting_not_work";
/// ~~~~~~~~~~~~~
///
inline bool ATTR_DLL_LOCAL CheckSettingString(const std::string& settingName,
                                              std::string& settingValue)
{
  char* buffer = nullptr;
  bool ret = kodi::dl::api.kodi_addon_get_setting_string(settingName.c_str(), &buffer);
  if (buffer)
  {
    if (ret)
      settingValue = buffer;
    free(buffer);
  }
  return ret;
}
//------------------------------------------------------------------------------

//==============================================================================
/// @brief Get string setting value.
///
/// The setting name relate to names used in his <b>settings.xml</b> file.
///
/// @param[in] settingName The name of asked setting
/// @param[in] defaultValue [opt] Default value if not found
/// @return The value of setting, empty if not found;
///
///
/// ----------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/General.h>
///
/// std::string value = kodi::GetSettingString("my_string_value");
/// ~~~~~~~~~~~~~
///
inline std::string ATTR_DLL_LOCAL GetSettingString(const std::string& settingName,
                                                   const std::string& defaultValue = "")
{
  std::string settingValue = defaultValue;
  CheckSettingString(settingName, settingValue);
  return settingValue;
}
//------------------------------------------------------------------------------

//==============================================================================
/// @brief Set string setting of addon.
///
/// The setting name relate to names used in his <b>settings.xml</b> file.
///
/// @param[in] settingName The name of setting
/// @param[in] settingValue The setting value to write
///
///
/// ----------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/General.h>
///
/// std::string value = "my_new_name for";
/// kodi::SetSettingString("my_string_value", value);
/// ~~~~~~~~~~~~~
///
inline void ATTR_DLL_LOCAL SetSettingString(const std::string& settingName,
                                            const std::string& settingValue)
{
  kodi::dl::api.kodi_addon_set_setting_string(settingName.c_str(), settingValue.c_str());
}
//------------------------------------------------------------------------------

//==============================================================================
/// @brief Check and get a integer setting value.
///
/// The setting name relate to names used in his <b>settings.xml</b> file.
///
/// @param[in] settingName The name of asked setting
/// @param[out] settingValue The given setting value
/// @return true if setting was successfully found and "settingValue" is set
///
/// @note If returns false, the "settingValue" is not changed.
///
///
/// ----------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/General.h>
///
/// int value = 0;
/// if (!kodi::CheckSettingInt("my_integer_value", value))
///   value = 123; // My default of them
/// ~~~~~~~~~~~~~
///
inline bool ATTR_DLL_LOCAL CheckSettingInt(const std::string& settingName, int& settingValue)
{
  return kodi::dl::api.kodi_addon_get_setting_int(settingName.c_str(), &settingValue);
}
//------------------------------------------------------------------------------

//==============================================================================
/// @brief Get integer setting value.
///
/// The setting name relate to names used in his <b>settings.xml</b> file.
///
/// @param[in] settingName The name of asked setting
/// @param[in] defaultValue [opt] Default value if not found
/// @return The value of setting, <b>`0`</b> or defaultValue if not found
///
///
/// ----------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/General.h>
///
/// int value = kodi::GetSettingInt("my_integer_value");
/// ~~~~~~~~~~~~~
///
inline int ATTR_DLL_LOCAL GetSettingInt(const std::string& settingName, int defaultValue = 0)
{
  int settingValue = defaultValue;
  CheckSettingInt(settingName, settingValue);
  return settingValue;
}
//------------------------------------------------------------------------------

//==============================================================================
/// @brief Set integer setting of addon.
///
/// The setting name relate to names used in his <b>settings.xml</b> file.
///
/// @param[in] settingName The name of setting
/// @param[in] settingValue The setting value to write
///
///
/// ----------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/General.h>
///
/// int value = 123;
/// kodi::SetSettingInt("my_integer_value", value);
/// ~~~~~~~~~~~~~
///
inline void ATTR_DLL_LOCAL SetSettingInt(const std::string& settingName, int settingValue)
{
  kodi::dl::api.kodi_addon_set_setting_int(settingName.c_str(), settingValue);
}
//------------------------------------------------------------------------------

//==============================================================================
/// @brief Check and get a boolean setting value.
///
/// The setting name relate to names used in his <b>settings.xml</b> file.
///
/// @param[in] settingName The name of asked setting
/// @param[out] settingValue The given setting value
/// @return true if setting was successfully found and "settingValue" is set
///
/// @note If returns false, the "settingValue" is not changed.
///
///
/// ----------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/General.h>
///
/// bool value = false;
/// if (!kodi::CheckSettingBoolean("my_boolean_value", value))
///   value = true; // My default of them
/// ~~~~~~~~~~~~~
///
inline bool ATTR_DLL_LOCAL CheckSettingBoolean(const std::string& settingName, bool& settingValue)
{
  return kodi::dl::api.kodi_addon_get_setting_bool(settingName.c_str(), &settingValue);
}
//------------------------------------------------------------------------------

//==============================================================================
/// @brief Get boolean setting value.
///
/// The setting name relate to names used in his <b>settings.xml</b> file.
///
/// @param[in] settingName The name of asked setting
/// @param[in] defaultValue [opt] Default value if not found
/// @return The value of setting, <b>`false`</b> or defaultValue if not found
///
///
/// ----------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/General.h>
///
/// bool value = kodi::GetSettingBoolean("my_boolean_value");
/// ~~~~~~~~~~~~~
///
inline bool ATTR_DLL_LOCAL GetSettingBoolean(const std::string& settingName,
                                             bool defaultValue = false)
{
  bool settingValue = defaultValue;
  CheckSettingBoolean(settingName, settingValue);
  return settingValue;
}
//------------------------------------------------------------------------------

//==============================================================================
/// @brief Set boolean setting of addon.
///
/// The setting name relate to names used in his <b>settings.xml</b> file.
///
/// @param[in] settingName The name of setting
/// @param[in] settingValue The setting value to write
///
///
/// ----------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/General.h>
///
/// bool value = true;
/// kodi::SetSettingBoolean("my_boolean_value", value);
/// ~~~~~~~~~~~~~
///
inline void ATTR_DLL_LOCAL SetSettingBoolean(const std::string& settingName, bool settingValue)
{
  kodi::dl::api.kodi_addon_set_setting_bool(settingName.c_str(), settingValue);
}
//------------------------------------------------------------------------------

//==============================================================================
/// @brief Check and get a floating point setting value.
///
/// The setting name relate to names used in his <b>settings.xml</b> file.
///
/// @param[in] settingName The name of asked setting
/// @param[out] settingValue The given setting value
/// @return true if setting was successfully found and "settingValue" is set
///
/// @note If returns false, the "settingValue" is not changed.
///
///
/// ----------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/General.h>
///
/// float value = 0.0f;
/// if (!kodi::CheckSettingBoolean("my_float_value", value))
///   value = 1.0f; // My default of them
/// ~~~~~~~~~~~~~
///
inline bool ATTR_DLL_LOCAL CheckSettingFloat(const std::string& settingName, double& settingValue)
{
  return kodi::dl::api.kodi_addon_get_setting_float(settingName.c_str(), &settingValue);
}
//------------------------------------------------------------------------------

//==============================================================================
/// @brief Get floating point setting value.
///
/// The setting name relate to names used in his <b>settings.xml</b> file.
///
/// @param[in] settingName The name of asked setting
/// @param[in] defaultValue [opt] Default value if not found
/// @return The value of setting, <b>`0.0`</b> or defaultValue if not found
///
///
/// ----------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/General.h>
///
/// double value = kodi::GetSettingFloat("my_float_value");
/// ~~~~~~~~~~~~~
///
inline float ATTR_DLL_LOCAL GetSettingFloat(const std::string& settingName,
                                            double defaultValue = 0.0f)
{
  double settingValue = defaultValue;
  CheckSettingFloat(settingName, settingValue);
  return settingValue;
}
//------------------------------------------------------------------------------

//==============================================================================
/// @brief Set floating point setting of addon.
///
/// The setting name relate to names used in his <b>settings.xml</b> file.
///
/// @param[in] settingName The name of setting
/// @param[in] settingValue The setting value to write
///
///
/// ----------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/General.h>
///
/// double value = 1.0f;
/// kodi::SetSettingFloat("my_float_value", value);
/// ~~~~~~~~~~~~~
///
inline void ATTR_DLL_LOCAL SetSettingFloat(const std::string& settingName, double settingValue)
{
  kodi::dl::api.kodi_addon_set_setting_float(settingName.c_str(), settingValue);
}
//------------------------------------------------------------------------------

//==============================================================================
/// @brief Check and get a enum setting value.
///
/// The setting name relate to names used in his <b>settings.xml</b> file.
///
/// @param[in] settingName The name of asked setting
/// @param[out] settingValue The given setting value
/// @return true if setting was successfully found and "settingValue" is set
///
/// @remark The enums are used as integer inside settings.xml.
/// @note If returns false, the "settingValue" is not changed.
///
///
/// ----------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/General.h>
///
/// enum myEnumValue
/// {
///   valueA,
///   valueB,
///   valueC
/// };
///
/// myEnumValue value;
/// if (!kodi::CheckSettingEnum<myEnumValue>("my_enum_value", value))
///   value = valueA; // My default of them
/// ~~~~~~~~~~~~~
///
template<typename enumType>
inline bool ATTR_DLL_LOCAL CheckSettingEnum(const std::string& settingName, enumType& settingValue)
{
  int settingValueInt = static_cast<int>(settingValue);
  bool ret = kodi::dl::api.kodi_addon_get_setting_int(settingName.c_str(), &settingValueInt);
  if (ret)
    settingValue = static_cast<enumType>(settingValueInt);
  return ret;
}
//------------------------------------------------------------------------------

//==============================================================================
/// @brief Get enum setting value.
///
/// The setting name relate to names used in his <b>settings.xml</b> file.
///
/// @param[in] settingName The name of asked setting
/// @param[in] defaultValue [opt] Default value if not found
/// @return The value of setting, forced to <b>`0`</b> or defaultValue if not found
///
/// @remark The enums are used as integer inside settings.xml.
///
///
/// ----------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/General.h>
///
/// enum myEnumValue
/// {
///   valueA,
///   valueB,
///   valueC
/// };
///
/// myEnumValue value = kodi::GetSettingEnum<myEnumValue>("my_enum_value");
/// ~~~~~~~~~~~~~
///
template<typename enumType>
inline enumType ATTR_DLL_LOCAL GetSettingEnum(const std::string& settingName,
                                              enumType defaultValue = static_cast<enumType>(0))
{
  enumType settingValue = defaultValue;
  CheckSettingEnum(settingName, settingValue);
  return settingValue;
}
//------------------------------------------------------------------------------

//==============================================================================
/// @brief Set enum setting of addon.
///
/// The setting name relate to names used in his <b>settings.xml</b> file.
///
/// @param[in] settingName The name of setting
/// @param[in] settingValue The setting value to write
///
/// @remark The enums are used as integer inside settings.xml.
///
///
/// ----------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/General.h>
///
/// enum myEnumValue
/// {
///   valueA,
///   valueB,
///   valueC
/// };
///
/// myEnumValue value = valueA;
/// kodi::SetSettingEnum<myEnumValue>("my_enum_value", value);
/// ~~~~~~~~~~~~~
///
template<typename enumType>
inline void ATTR_DLL_LOCAL SetSettingEnum(const std::string& settingName, enumType settingValue)
{
  kodi::dl::api.kodi_addon_set_setting_int(settingName.c_str(), static_cast<int>(settingValue));
}
//------------------------------------------------------------------------------

/*!@}*/

//==============================================================================
/// @ingroup cpp_kodi_addon
/// @brief Returns the value of an addon property as a string
///
/// @param[in] id id of the property that the module needs to access
/// |              | Choices are  |              |
/// |:------------:|:------------:|:------------:|
/// |  author      | icon         | stars        |
/// |  changelog   | id           | summary      |
/// |  description | name         | type         |
/// |  disclaimer  | path         | version      |
/// |  fanart      | profile      |              |
///
/// @return AddOn property as a string
///
///
/// ------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/General.h>
/// ...
/// std::string addonName = kodi::GetAddonInfo("name");
/// ...
/// ~~~~~~~~~~~~~
///
inline std::string ATTR_DLL_LOCAL GetAddonInfo(const std::string& id)
{
  std::string strReturn;
  char* strMsg = kodi::dl::api.kodi_addon_get_info(id.c_str());
  if (strMsg != nullptr)
  {
    if (std::strlen(strMsg))
      strReturn = strMsg;
    free(strMsg);
  }
  return strReturn;
}
//------------------------------------------------------------------------------

//============================================================================
/// @ingroup cpp_kodi_addon
/// @brief Get to related @ref ADDON_STATUS a human readable text.
///
/// @param[in] status Status value to get name for
/// @return Wanted name, as "Unknown" if status not known
///
inline std::string ATTR_DLL_LOCAL TranslateAddonStatus(ADDON_STATUS status)
{
  switch (status)
  {
    case ADDON_STATUS_OK:
      return "OK";
    case ADDON_STATUS_LOST_CONNECTION:
      return "Lost Connection";
    case ADDON_STATUS_NEED_RESTART:
      return "Need Restart";
    case ADDON_STATUS_NEED_SETTINGS:
      return "Need Settings";
    case ADDON_STATUS_UNKNOWN:
      return "Unknown error";
    case ADDON_STATUS_PERMANENT_FAILURE:
      return "Permanent failure";
    case ADDON_STATUS_NOT_IMPLEMENTED:
      return "Not implemented";
    default:
      break;
  }
  return "Unknown";
}
//----------------------------------------------------------------------------

} /* namespace addon */

//==============================================================================
/// @ingroup cpp_kodi
/// @brief Add a message to Kodi's log.
///
/// @param[in] loglevel The log level of the message.
/// @param[in] format The format of the message to pass to Kodi.
/// @param[in] ... Additional text to insert in format text
///
/// @note This method uses limited buffer (16k) for the formatted output.
/// So data, which will not fit into it, will be silently discarded.
///
///
/// ----------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
/// #include <kodi/General.h>
///
/// kodi::Log(ADDON_LOG_ERROR, "%s: There is an error occurred!", __func__);
///
/// ~~~~~~~~~~~~~
///
#ifndef SWIG
template<typename... Args>
inline void ATTR_DLL_LOCAL Log(const AddonLog loglevel, const std::string& fmt, Args&&... args)
{
  kodi::dl::api.kodi_log(loglevel, fmt.c_str(), args...);
}
#endif
//------------------------------------------------------------------------------

#ifndef SWIG
inline bool ATTR_DLL_LOCAL Init(
    unsigned int api, int argc, char* argv[], KODI_ADDON_FUNC* func, bool via_main, bool no_receive)
{
  if (!kodi::dl::LoadDll(argc, argv))
    return false;

  KODI_IFC_HDL hdl = kodi::dl::api.kodi_init(api, argc, argv, func, via_main, no_receive);
  if (!hdl)
    return false;

  kodi::dl::hdl = hdl;

  return true;
}
#endif

inline void ATTR_DLL_LOCAL Deinit()
{
  kodi::dl::api.kodi_deinit(kodi::dl::hdl);
  kodi::dl::UnloadDll();
}

} /* namespace kodi */

//==============================================================================
/// @ingroup cpp_kodi_addon_addonbase_Defs
/// @defgroup cpp_kodi_addon_addonbase_Defs_ADDONCREATORAddonClass macro ADDONCREATOR(AddonClass)
/// @brief **Addon creation macro**\n
/// This export the three mandatory "C" functions to have available for Kodi.
///
/// @note Only this macro can be used on a C++ addon, everything else is done
/// automatically.
///
/// @param[in] AddonClass Used addon class to be exported with CAddonBase as
///            parent.
///
///
/// ----------------------------------------------------------------------------
///
/// **Example:**
/// ~~~~~~~~~~~~~{.cpp}
///
/// #include <kodi/AddonBash.h>
///
/// class CMyAddon : public kodi::addon::CAddonBase
/// {
/// public:
///   CMyAddon() = default;
///   ADDON_STATUS Create() override;
/// };
///
/// ADDON_STATUS CMyAddon::Create()
/// {
///   // Some work
///
///   return ADDON_STATUS_OK;
/// }
///
/// ADDONCREATOR(CMyAddon)
/// ~~~~~~~~~~~~~
///
/// ----------------------------------------------------------------------------
///
/// As information, the following functions are exported using this macro:
/// \table_start
///   \table_h3{ Function, Use, Description }
///   \table_row3{   <b>`ADDON_Create(KODI_HANDLE addonInterface)`</b>,
///                  \anchor ADDON_Create
///                  _required_,
///     <b>Addon creation call.</b>
///     <br>
///     Like an `int main()` is this the first on addon called place on his start
///     and create within C++ API related class inside addon.
///     <br>
///     @param[in] addonInterface Handle pointer to get Kodi's given table.
///                               There have addon needed values and functions
///                               to Kodi and addon must set his functions there
///                               for Kodi.
///     @param[in] globalApiVersion This gives the main version @ref ADDON_GLOBAL_VERSION_MAIN
///                                 where currently on Kodi's side.<br>
///                                 This is unused on addon as there's also other
///                                 special callback functions for.<br>
///                                 Only thought for future use if needed earlier.
///     @param[in] unused This is a not used value\, only there to have in case of
///                       need no Major API version increase where break current.
///     @return @ref ADDON_STATUS_OK if correct\, for possible errors see
///             @ref ADDON_STATUS.
///     <p>
///   }
///   \table_row3{   <b>`const char* ADDON_GetTypeVersion(int type)`</b>,
///                  \anchor ADDON_GetTypeVersion
///                  _required_,
///     <b>Ask addon about version of given type.</b>
///     <br>
///     This is used to query its associated version in the addon before work
///     is carried out in it and the Kodi can adapt to it.
///     <br>
///     @param[in] type With @ref ADDON_TYPE defined type to ask.
///     @return Version as string of asked type.
///     <p>
///   }
///   \table_row3{   <b>`const char* ADDON_GetTypeMinVersion(int type)`</b>,
///                  \anchor ADDON_GetTypeMinVersion
///                  _optional_,
///     <b>Ask addon about minimal version of given type.</b>
///     <br>
///     This is used to query its associated min version in the addon before work
///     is carried out in it and the Kodi can adapt to it.
///     <br>
///     @note The minimum version is optional\, if it were not available\, the
///     major version is used instead.
///     <br>
///     @param[in] type With @ref ADDON_TYPE defined type to ask.
///     @return Min version as string of asked type.
///     <p>
///   }
/// \table_end
///

#define KODI_STARTER \
  kodi_api_1 kodi::dl::api{0}; \
  KODI_IFC_HDL kodi::dl::hdl{nullptr}; \
  kodi::dl::LOCAL kodi::dl::local{};
#define ADDONCREATOR(AddonClass) \
  KODI_STARTER \
  ADDON_STATUS kodi::addon::CAddonBase::ADDONBASE_create(const struct KODI_ADDON_INSTANCE_INFO* first_instance, KODI_ADDON_HDL* hdl) \
  { \
    using namespace ::kodi::addon; \
    kodi::dl::local.firstKodiInstance = first_instance; \
    kodi::dl::local.addonBase = new AddonClass; \
    *hdl = kodi::dl::local.addonBase; \
    return kodi::dl::local.addonBase->Create(); \
  } \
  extern "C" ATTR_DLL_EXPORT int main(int argc, char** argv) \
  { \
    using namespace ::kodi::addon; \
    fprintf(stderr, "TEMP DEBUG: Addon main started, PID %i\n", getpid()); \
    /*sleep(10);*/ \
    KODI_ADDON_FUNC func = CAddonBase::GetAddonFunctions(); \
    if (!kodi::Init(__KODI_API__, argc, argv, &func, true, false)) \
    { \
      fprintf(stderr, "TEMP DEBUG: Addon main start failed!!! PID %i\n", getpid()); \
      return -1; \
    } \
    while (1) \
    { \
      if (!kodi::dl::api.kodi_process(kodi::dl::hdl)) \
        break; \
    } \
    kodi::Deinit(); \
    return 0; \
  } \
  extern "C" ATTR_DLL_EXPORT int non_blocking_main_start(int argc, char** argv) \
  { \
    using namespace ::kodi::addon; \
    KODI_ADDON_FUNC func = CAddonBase::GetAddonFunctions(); \
    return kodi::Init(__KODI_API__, argc, argv, &func, true, false) ? 0 : -1; \
  } \
  extern "C" ATTR_DLL_EXPORT void non_blocking_main_stop() { kodi::Deinit(); }

#endif /* __cplusplus */
