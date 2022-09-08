%include global.i

%module kodi

%rename("%s") ADDON_STATUS;

%{
#include "c-api/version.h"
#include "c-api/addon_base.h"
#include "AddonBase.h"

using namespace kodi;

kodi_api_1 kodi::dl::api;
KODI_IFC_HDL kodi::dl::hdl = nullptr;
KODI_DLL_HDL kodi::dl::dll = nullptr;
kodi::addon::CAddonBase* kodi::addon::CAddonBase::addonBase = nullptr;
kodi::addon::IInstanceInfo kodi::addon::CAddonBase::firstInstance;
KODI_ADDON_HDL kodi::addon::CAddonBase::globalSingleInstance = nullptr;

ADDON_STATUS kodi::addon::CAddonBase::ADDONBASE_create(const struct KODI_ADDON_INSTANCE_INFO* first_instance, KODI_ADDON_HDL* hdl)
{
  return ADDON_STATUS_NOT_IMPLEMENTED;
}

%}

%include "std_string.i"
%include "c-api/version.h"
%include "c-api/addon_base.h"
%include "AddonBase.h"
