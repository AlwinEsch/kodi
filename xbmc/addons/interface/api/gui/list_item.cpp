/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/* File autogenerated, see xbmc/addons/kodi-dev-kit/tools/code-generator/code_generator.py */

#include "list_item.h"

// Shared API
#include "addons/kodi-dev-kit/src/shared/api/gui/list_item.h"

// Kodi
#include "addons/interface/RunningProcess.h"
#include "utils/log.h"

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1>---*/

#include "FileItem.h"
#include "general.h"
#include "utils/StringUtils.h"
#include "utils/Variant.h"

/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1_END>---*/

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_2>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_2_END>---*/

void CHdl_kodi_gui_list_item_h::InitDirect(directFuncToKodi_gui_list_item_h* ifcToKodi,
                                           directFuncToAddon_gui_list_item_h* ifcToAddon)
{
  ifcToKodi->thisClassHdl = this;
  m_ifcToAddon = ifcToAddon;
  ifcToKodi->kodi_gui_listitem_create_v1 = kodi_gui_listitem_create_v1;
  ifcToKodi->kodi_gui_listitem_destroy_v1 = kodi_gui_listitem_destroy_v1;
  ifcToKodi->kodi_gui_listitem_get_label_v1 = kodi_gui_listitem_get_label_v1;
  ifcToKodi->kodi_gui_listitem_set_label_v1 = kodi_gui_listitem_set_label_v1;
  ifcToKodi->kodi_gui_listitem_get_label2_v1 = kodi_gui_listitem_get_label2_v1;
  ifcToKodi->kodi_gui_listitem_set_label2_v1 = kodi_gui_listitem_set_label2_v1;
  ifcToKodi->kodi_gui_listitem_get_art_v1 = kodi_gui_listitem_get_art_v1;
  ifcToKodi->kodi_gui_listitem_set_art_v1 = kodi_gui_listitem_set_art_v1;
  ifcToKodi->kodi_gui_listitem_get_path_v1 = kodi_gui_listitem_get_path_v1;
  ifcToKodi->kodi_gui_listitem_set_path_v1 = kodi_gui_listitem_set_path_v1;
  ifcToKodi->kodi_gui_listitem_get_property_v1 = kodi_gui_listitem_get_property_v1;
  ifcToKodi->kodi_gui_listitem_set_property_v1 = kodi_gui_listitem_set_property_v1;
  ifcToKodi->kodi_gui_listitem_select_v1 = kodi_gui_listitem_select_v1;
  ifcToKodi->kodi_gui_listitem_is_selected_v1 = kodi_gui_listitem_is_selected_v1;
}

bool CHdl_kodi_gui_list_item_h::HandleMessage(int funcGroup,
                                              int func,
                                              const msgpack::unpacked& in,
                                              msgpack::sbuffer& out)
{
  if (funcGroup != funcGroup_gui_list_item_h)
    return false;

  switch (func)
  {
    case funcParent_kodi_gui_listitem_create_v1:
    {
      // Original API call: ATTR_DLL_EXPORT KODI_GUI_LISTITEM_HANDLE kodi_gui_listitem_create(const char* label, const char* label2, const char* path) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<std::string, std::string, std::string> msgParent__IN_kodi_gui_listitem_create_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<PtrValue> msgParent_OUT_kodi_gui_listitem_create_v1; /* Autogenerated */
      msgParent__IN_kodi_gui_listitem_create_v1 t = in.get().as<decltype(t)>();
      const std::string& label = std::get<0>(t);
      const std::string& label2 = std::get<1>(t);
      const std::string& path = std::get<2>(t);
      KODI_GUI_LISTITEM_HANDLE auto_gen_ret =
          kodi_gui_listitem_create_v1(this, label.c_str(), label2.c_str(), path.c_str());
      msgpack::pack(out, msgParent_OUT_kodi_gui_listitem_create_v1(PtrValue(auto_gen_ret)));
      return true;
    }
    case funcParent_kodi_gui_listitem_destroy_v1:
    {
      // Original API call: ATTR_DLL_EXPORT void kodi_gui_listitem_destroy(KODI_GUI_LISTITEM_HANDLE handle) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<PtrValue> msgParent__IN_kodi_gui_listitem_destroy_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_listitem_destroy_v1; /* Autogenerated */
      msgParent__IN_kodi_gui_listitem_destroy_v1 t = in.get().as<decltype(t)>();
      KODI_GUI_LISTITEM_HANDLE handle = reinterpret_cast<KODI_GUI_LISTITEM_HANDLE>(std::get<0>(t));
      kodi_gui_listitem_destroy_v1(this, handle);
      return true;
    }
    case funcParent_kodi_gui_listitem_get_label_v1:
    {
      // Original API call: ATTR_DLL_EXPORT char* kodi_gui_listitem_get_label(KODI_GUI_LISTITEM_HANDLE handle) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<PtrValue> msgParent__IN_kodi_gui_listitem_get_label_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<std::string> msgParent_OUT_kodi_gui_listitem_get_label_v1; /* Autogenerated */
      msgParent__IN_kodi_gui_listitem_get_label_v1 t = in.get().as<decltype(t)>();
      KODI_GUI_LISTITEM_HANDLE handle = reinterpret_cast<KODI_GUI_LISTITEM_HANDLE>(std::get<0>(t));
      char* auto_gen_ret = kodi_gui_listitem_get_label_v1(this, handle);
      msgpack::pack(out,
                    msgParent_OUT_kodi_gui_listitem_get_label_v1(auto_gen_ret ? auto_gen_ret : ""));
      if (auto_gen_ret)
        free(auto_gen_ret);
      return true;
    }
    case funcParent_kodi_gui_listitem_set_label_v1:
    {
      // Original API call: ATTR_DLL_EXPORT void kodi_gui_listitem_set_label(KODI_GUI_LISTITEM_HANDLE handle, const char* label) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<PtrValue, std::string> msgParent__IN_kodi_gui_listitem_set_label_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_listitem_set_label_v1; /* Autogenerated */
      msgParent__IN_kodi_gui_listitem_set_label_v1 t = in.get().as<decltype(t)>();
      KODI_GUI_LISTITEM_HANDLE handle = reinterpret_cast<KODI_GUI_LISTITEM_HANDLE>(std::get<0>(t));
      const std::string& label = std::get<1>(t);
      kodi_gui_listitem_set_label_v1(this, handle, label.c_str());
      return true;
    }
    case funcParent_kodi_gui_listitem_get_label2_v1:
    {
      // Original API call: ATTR_DLL_EXPORT char* kodi_gui_listitem_get_label2(KODI_GUI_LISTITEM_HANDLE handle) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<PtrValue> msgParent__IN_kodi_gui_listitem_get_label2_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<std::string> msgParent_OUT_kodi_gui_listitem_get_label2_v1; /* Autogenerated */
      msgParent__IN_kodi_gui_listitem_get_label2_v1 t = in.get().as<decltype(t)>();
      KODI_GUI_LISTITEM_HANDLE handle = reinterpret_cast<KODI_GUI_LISTITEM_HANDLE>(std::get<0>(t));
      char* auto_gen_ret = kodi_gui_listitem_get_label2_v1(this, handle);
      msgpack::pack(
          out, msgParent_OUT_kodi_gui_listitem_get_label2_v1(auto_gen_ret ? auto_gen_ret : ""));
      if (auto_gen_ret)
        free(auto_gen_ret);
      return true;
    }
    case funcParent_kodi_gui_listitem_set_label2_v1:
    {
      // Original API call: ATTR_DLL_EXPORT void kodi_gui_listitem_set_label2(KODI_GUI_LISTITEM_HANDLE handle, const char* label) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<PtrValue, std::string> msgParent__IN_kodi_gui_listitem_set_label2_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_listitem_set_label2_v1; /* Autogenerated */
      msgParent__IN_kodi_gui_listitem_set_label2_v1 t = in.get().as<decltype(t)>();
      KODI_GUI_LISTITEM_HANDLE handle = reinterpret_cast<KODI_GUI_LISTITEM_HANDLE>(std::get<0>(t));
      const std::string& label = std::get<1>(t);
      kodi_gui_listitem_set_label2_v1(this, handle, label.c_str());
      return true;
    }
    case funcParent_kodi_gui_listitem_get_art_v1:
    {
      // Original API call: ATTR_DLL_EXPORT char* kodi_gui_listitem_get_art(KODI_GUI_LISTITEM_HANDLE handle, const char* type) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<PtrValue, std::string> msgParent__IN_kodi_gui_listitem_get_art_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<std::string> msgParent_OUT_kodi_gui_listitem_get_art_v1; /* Autogenerated */
      msgParent__IN_kodi_gui_listitem_get_art_v1 t = in.get().as<decltype(t)>();
      KODI_GUI_LISTITEM_HANDLE handle = reinterpret_cast<KODI_GUI_LISTITEM_HANDLE>(std::get<0>(t));
      const std::string& type = std::get<1>(t);
      char* auto_gen_ret = kodi_gui_listitem_get_art_v1(this, handle, type.c_str());
      msgpack::pack(out,
                    msgParent_OUT_kodi_gui_listitem_get_art_v1(auto_gen_ret ? auto_gen_ret : ""));
      if (auto_gen_ret)
        free(auto_gen_ret);
      return true;
    }
    case funcParent_kodi_gui_listitem_set_art_v1:
    {
      // Original API call: ATTR_DLL_EXPORT void kodi_gui_listitem_set_art(KODI_GUI_LISTITEM_HANDLE handle, const char* type, const char* image) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<PtrValue, std::string, std::string> msgParent__IN_kodi_gui_listitem_set_art_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_listitem_set_art_v1; /* Autogenerated */
      msgParent__IN_kodi_gui_listitem_set_art_v1 t = in.get().as<decltype(t)>();
      KODI_GUI_LISTITEM_HANDLE handle = reinterpret_cast<KODI_GUI_LISTITEM_HANDLE>(std::get<0>(t));
      const std::string& type = std::get<1>(t);
      const std::string& image = std::get<2>(t);
      kodi_gui_listitem_set_art_v1(this, handle, type.c_str(), image.c_str());
      return true;
    }
    case funcParent_kodi_gui_listitem_get_path_v1:
    {
      // Original API call: ATTR_DLL_EXPORT char* kodi_gui_listitem_get_path(KODI_GUI_LISTITEM_HANDLE handle) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<PtrValue> msgParent__IN_kodi_gui_listitem_get_path_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<std::string> msgParent_OUT_kodi_gui_listitem_get_path_v1; /* Autogenerated */
      msgParent__IN_kodi_gui_listitem_get_path_v1 t = in.get().as<decltype(t)>();
      KODI_GUI_LISTITEM_HANDLE handle = reinterpret_cast<KODI_GUI_LISTITEM_HANDLE>(std::get<0>(t));
      char* auto_gen_ret = kodi_gui_listitem_get_path_v1(this, handle);
      msgpack::pack(out,
                    msgParent_OUT_kodi_gui_listitem_get_path_v1(auto_gen_ret ? auto_gen_ret : ""));
      if (auto_gen_ret)
        free(auto_gen_ret);
      return true;
    }
    case funcParent_kodi_gui_listitem_set_path_v1:
    {
      // Original API call: ATTR_DLL_EXPORT void kodi_gui_listitem_set_path(KODI_GUI_LISTITEM_HANDLE handle, const char* path) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<PtrValue, std::string> msgParent__IN_kodi_gui_listitem_set_path_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_listitem_set_path_v1; /* Autogenerated */
      msgParent__IN_kodi_gui_listitem_set_path_v1 t = in.get().as<decltype(t)>();
      KODI_GUI_LISTITEM_HANDLE handle = reinterpret_cast<KODI_GUI_LISTITEM_HANDLE>(std::get<0>(t));
      const std::string& path = std::get<1>(t);
      kodi_gui_listitem_set_path_v1(this, handle, path.c_str());
      return true;
    }
    case funcParent_kodi_gui_listitem_get_property_v1:
    {
      // Original API call: ATTR_DLL_EXPORT char* kodi_gui_listitem_get_property(KODI_GUI_LISTITEM_HANDLE handle, const char* key) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<PtrValue, std::string> msgParent__IN_kodi_gui_listitem_get_property_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<std::string> msgParent_OUT_kodi_gui_listitem_get_property_v1; /* Autogenerated */
      msgParent__IN_kodi_gui_listitem_get_property_v1 t = in.get().as<decltype(t)>();
      KODI_GUI_LISTITEM_HANDLE handle = reinterpret_cast<KODI_GUI_LISTITEM_HANDLE>(std::get<0>(t));
      const std::string& key = std::get<1>(t);
      char* auto_gen_ret = kodi_gui_listitem_get_property_v1(this, handle, key.c_str());
      msgpack::pack(
          out, msgParent_OUT_kodi_gui_listitem_get_property_v1(auto_gen_ret ? auto_gen_ret : ""));
      if (auto_gen_ret)
        free(auto_gen_ret);
      return true;
    }
    case funcParent_kodi_gui_listitem_set_property_v1:
    {
      // Original API call: ATTR_DLL_EXPORT void kodi_gui_listitem_set_property(KODI_GUI_LISTITEM_HANDLE handle, const char* key, const char* value) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<PtrValue, std::string, std::string> msgParent__IN_kodi_gui_listitem_set_property_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_listitem_set_property_v1; /* Autogenerated */
      msgParent__IN_kodi_gui_listitem_set_property_v1 t = in.get().as<decltype(t)>();
      KODI_GUI_LISTITEM_HANDLE handle = reinterpret_cast<KODI_GUI_LISTITEM_HANDLE>(std::get<0>(t));
      const std::string& key = std::get<1>(t);
      const std::string& value = std::get<2>(t);
      kodi_gui_listitem_set_property_v1(this, handle, key.c_str(), value.c_str());
      return true;
    }
    case funcParent_kodi_gui_listitem_select_v1:
    {
      // Original API call: ATTR_DLL_EXPORT void kodi_gui_listitem_select(KODI_GUI_LISTITEM_HANDLE handle, bool select) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<PtrValue, bool> msgParent__IN_kodi_gui_listitem_select_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<DummyValue> msgParent_OUT_kodi_gui_listitem_select_v1; /* Autogenerated */
      msgParent__IN_kodi_gui_listitem_select_v1 t = in.get().as<decltype(t)>();
      KODI_GUI_LISTITEM_HANDLE handle = reinterpret_cast<KODI_GUI_LISTITEM_HANDLE>(std::get<0>(t));
      bool select = std::get<1>(t);
      kodi_gui_listitem_select_v1(this, handle, select);
      return true;
    }
    case funcParent_kodi_gui_listitem_is_selected_v1:
    {
      // Original API call: ATTR_DLL_EXPORT bool kodi_gui_listitem_is_selected(KODI_GUI_LISTITEM_HANDLE handle) __INTRODUCED_IN_KODI(1);
      // Tuple in:          typedef std::tuple<PtrValue> msgParent__IN_kodi_gui_listitem_is_selected_v1; /* Autogenerated */
      // Tuple out:         typedef std::tuple<bool> msgParent_OUT_kodi_gui_listitem_is_selected_v1; /* Autogenerated */
      msgParent__IN_kodi_gui_listitem_is_selected_v1 t = in.get().as<decltype(t)>();
      KODI_GUI_LISTITEM_HANDLE handle = reinterpret_cast<KODI_GUI_LISTITEM_HANDLE>(std::get<0>(t));
      bool auto_gen_ret = kodi_gui_listitem_is_selected_v1(this, handle);
      msgpack::pack(out, msgParent_OUT_kodi_gui_listitem_is_selected_v1(auto_gen_ret));
      return true;
    }
    default:
      CLog::Log(LOGERROR,
                "CHdl_kodi_gui_list_item_h::{}: addon called with unknown function id '{}' on "
                "group 'gui_list_item_h'",
                __func__, func);
  }

  return false;
}

// Function calls from Kodi to addon
/* NOTE: unused (no functions here) */

// Callbacks from addon to Kodi

KODI_GUI_LISTITEM_HANDLE CHdl_kodi_gui_list_item_h::kodi_gui_listitem_create_v1(void* thisClassHdl,
                                                                                const char* label,
                                                                                const char* label2,
                                                                                const char* path)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_gui_list_item_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return nullptr;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_CREATE>---*/

  CFileItemPtr* item = new CFileItemPtr(new CFileItem());
  if (label)
    item->get()->SetLabel(label);
  if (label2)
    item->get()->SetLabel2(label2);
  if (path)
    item->get()->SetPath(path);

  return item;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_CREATE_END>---*/
}

void CHdl_kodi_gui_list_item_h::kodi_gui_listitem_destroy_v1(void* thisClassHdl,
                                                             KODI_GUI_LISTITEM_HANDLE handle)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_gui_list_item_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_DESTROY>---*/

  CFileItemPtr* item = static_cast<CFileItemPtr*>(handle);
  if (item)
    delete item;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_DESTROY_END>---*/
}

char* CHdl_kodi_gui_list_item_h::kodi_gui_listitem_get_label_v1(void* thisClassHdl,
                                                                KODI_GUI_LISTITEM_HANDLE handle)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_gui_list_item_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return nullptr;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_GET_LABEL>---*/

  CFileItemPtr* item = static_cast<CFileItemPtr*>(handle);
  if (item->get() == nullptr)
  {
    CLog::Log(LOGERROR, "CHdl_kodi_gui_list_item_h::{} - empty list item called on addon '{}'",
              __func__, thisClass->m_process->GetAddonID());
    return nullptr;
  }

  char* ret;
  CHdl_kodi_gui_general_h::lock();
  ret = strdup(item->get()->GetLabel().c_str());
  CHdl_kodi_gui_general_h::unlock();
  return ret;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_GET_LABEL_END>---*/
}

void CHdl_kodi_gui_list_item_h::kodi_gui_listitem_set_label_v1(void* thisClassHdl,
                                                               KODI_GUI_LISTITEM_HANDLE handle,
                                                               const char* label)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_gui_list_item_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_SET_LABEL>---*/

  CFileItemPtr* item = static_cast<CFileItemPtr*>(handle);
  if (item->get() == nullptr)
  {
    CLog::Log(LOGERROR, "CHdl_kodi_gui_list_item_h::{} - empty list item called on addon '{}'",
              __func__, thisClass->m_process->GetAddonID());
    return;
  }

  CHdl_kodi_gui_general_h::lock();
  item->get()->SetLabel(label);
  CHdl_kodi_gui_general_h::unlock();

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_SET_LABEL_END>---*/
}

char* CHdl_kodi_gui_list_item_h::kodi_gui_listitem_get_label2_v1(void* thisClassHdl,
                                                                 KODI_GUI_LISTITEM_HANDLE handle)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_gui_list_item_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return nullptr;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_GET_LABEL2>---*/

  CFileItemPtr* item = static_cast<CFileItemPtr*>(handle);
  if (item->get() == nullptr)
  {
    CLog::Log(LOGERROR, "CHdl_kodi_gui_list_item_h::{} - empty list item called on addon '{}'",
              __func__, thisClass->m_process->GetAddonID());
    return nullptr;
  }

  char* ret;
  CHdl_kodi_gui_general_h::lock();
  ret = strdup(item->get()->GetLabel2().c_str());
  CHdl_kodi_gui_general_h::unlock();
  return ret;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_GET_LABEL2_END>---*/
}

void CHdl_kodi_gui_list_item_h::kodi_gui_listitem_set_label2_v1(void* thisClassHdl,
                                                                KODI_GUI_LISTITEM_HANDLE handle,
                                                                const char* label)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_gui_list_item_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_SET_LABEL2>---*/

  CFileItemPtr* item = static_cast<CFileItemPtr*>(handle);
  if (item->get() == nullptr)
  {
    CLog::Log(LOGERROR, "CHdl_kodi_gui_list_item_h::{} - empty list item called on addon '{}'",
              __func__, thisClass->m_process->GetAddonID());
    return;
  }

  CHdl_kodi_gui_general_h::lock();
  item->get()->SetLabel2(label);
  CHdl_kodi_gui_general_h::unlock();

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_SET_LABEL2_END>---*/
}

char* CHdl_kodi_gui_list_item_h::kodi_gui_listitem_get_art_v1(void* thisClassHdl,
                                                              KODI_GUI_LISTITEM_HANDLE handle,
                                                              const char* type)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_gui_list_item_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return nullptr;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_GET_ART>---*/

  CFileItemPtr* item = static_cast<CFileItemPtr*>(handle);
  if (item->get() == nullptr)
  {
    CLog::Log(LOGERROR, "CHdl_kodi_gui_list_item_h::{} - empty list item called on addon '{}'",
              __func__, thisClass->m_process->GetAddonID());
    return nullptr;
  }

  char* ret;
  CHdl_kodi_gui_general_h::lock();
  ret = strdup(item->get()->GetArt(type).c_str());
  CHdl_kodi_gui_general_h::unlock();
  return ret;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_GET_ART_END>---*/
}

void CHdl_kodi_gui_list_item_h::kodi_gui_listitem_set_art_v1(void* thisClassHdl,
                                                             KODI_GUI_LISTITEM_HANDLE handle,
                                                             const char* type,
                                                             const char* image)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_gui_list_item_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_SET_ART>---*/

  CFileItemPtr* item = static_cast<CFileItemPtr*>(handle);
  if (item->get() == nullptr)
  {
    CLog::Log(LOGERROR, "CHdl_kodi_gui_list_item_h::{} - empty list item called on addon '{}'",
              __func__, thisClass->m_process->GetAddonID());
    return;
  }

  CHdl_kodi_gui_general_h::lock();
  item->get()->SetArt(type, image);
  CHdl_kodi_gui_general_h::unlock();

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_SET_ART_END>---*/
}

char* CHdl_kodi_gui_list_item_h::kodi_gui_listitem_get_path_v1(void* thisClassHdl,
                                                               KODI_GUI_LISTITEM_HANDLE handle)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_gui_list_item_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return nullptr;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_GET_PATH>---*/

  CFileItemPtr* item = static_cast<CFileItemPtr*>(handle);
  if (item->get() == nullptr)
  {
    CLog::Log(LOGERROR, "CHdl_kodi_gui_list_item_h::{} - empty list item called on addon '{}'",
              __func__, thisClass->m_process->GetAddonID());
    return nullptr;
  }

  char* ret;
  CHdl_kodi_gui_general_h::lock();
  ret = strdup(item->get()->GetPath().c_str());
  CHdl_kodi_gui_general_h::unlock();
  return ret;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_GET_PATH_END>---*/
}

void CHdl_kodi_gui_list_item_h::kodi_gui_listitem_set_path_v1(void* thisClassHdl,
                                                              KODI_GUI_LISTITEM_HANDLE handle,
                                                              const char* path)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_gui_list_item_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_SET_PATH>---*/

  CFileItemPtr* item = static_cast<CFileItemPtr*>(handle);
  if (item->get() == nullptr)
  {
    CLog::Log(LOGERROR, "CHdl_kodi_gui_list_item_h::{} - empty list item called on addon '{}'",
              __func__, thisClass->m_process->GetAddonID());
    return;
  }

  CHdl_kodi_gui_general_h::lock();
  item->get()->SetPath(path);
  CHdl_kodi_gui_general_h::unlock();

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_SET_PATH_END>---*/
}

char* CHdl_kodi_gui_list_item_h::kodi_gui_listitem_get_property_v1(void* thisClassHdl,
                                                                   KODI_GUI_LISTITEM_HANDLE handle,
                                                                   const char* key)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_gui_list_item_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return nullptr;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_GET_PROPERTY>---*/

  CFileItemPtr* item = static_cast<CFileItemPtr*>(handle);
  if (item->get() == nullptr)
  {
    CLog::Log(LOGERROR, "CHdl_kodi_gui_list_item_h::{} - empty list item called on addon '{}'",
              __func__, thisClass->m_process->GetAddonID());
    return nullptr;
  }

  std::string lowerKey = key;
  StringUtils::ToLower(lowerKey);

  CHdl_kodi_gui_general_h::lock();
  char* ret = strdup(item->get()->GetProperty(lowerKey).asString().c_str());
  CHdl_kodi_gui_general_h::unlock();

  return ret;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_GET_PROPERTY_END>---*/
}

void CHdl_kodi_gui_list_item_h::kodi_gui_listitem_set_property_v1(void* thisClassHdl,
                                                                  KODI_GUI_LISTITEM_HANDLE handle,
                                                                  const char* key,
                                                                  const char* value)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_gui_list_item_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_SET_PROPERTY>---*/

  CFileItemPtr* item = static_cast<CFileItemPtr*>(handle);
  if (item->get() == nullptr)
  {
    CLog::Log(LOGERROR, "CHdl_kodi_gui_list_item_h::{} - empty list item called on addon '{}'",
              __func__, thisClass->m_process->GetAddonID());
    return;
  }

  std::string lowerKey = key;
  StringUtils::ToLower(lowerKey);

  CHdl_kodi_gui_general_h::lock();
  item->get()->SetProperty(lowerKey, CVariant(value));
  CHdl_kodi_gui_general_h::unlock();

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_SET_PROPERTY_END>---*/
}

void CHdl_kodi_gui_list_item_h::kodi_gui_listitem_select_v1(void* thisClassHdl,
                                                            KODI_GUI_LISTITEM_HANDLE handle,
                                                            bool select)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_gui_list_item_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_SELECT>---*/

  CFileItemPtr* item = static_cast<CFileItemPtr*>(handle);
  if (item->get() == nullptr)
  {
    CLog::Log(LOGERROR, "CHdl_kodi_gui_list_item_h::{} - empty list item called on addon '{}'",
              __func__, thisClass->m_process->GetAddonID());
    return;
  }

  CHdl_kodi_gui_general_h::lock();
  item->get()->Select(select);
  CHdl_kodi_gui_general_h::unlock();

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_SELECT_END>---*/
}

bool CHdl_kodi_gui_list_item_h::kodi_gui_listitem_is_selected_v1(void* thisClassHdl,
                                                                 KODI_GUI_LISTITEM_HANDLE handle)
{
  auto thisClass = reinterpret_cast<CHdl_kodi_gui_list_item_h*>(thisClassHdl);
  if (thisClass == nullptr)
    return false;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_IS_SELECTED>---*/

  CFileItemPtr* item = static_cast<CFileItemPtr*>(handle);
  if (item->get() == nullptr)
  {
    CLog::Log(LOGERROR, "CHdl_kodi_gui_list_item_h::{} - empty list item called on addon '{}'",
              __func__, thisClass->m_process->GetAddonID());
    return false;
  }

  CHdl_kodi_gui_general_h::lock();
  bool ret = item->get()->IsSelected();
  CHdl_kodi_gui_general_h::unlock();

  return ret;

  /*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_KODI_GUI_LISTITEM_IS_SELECTED_END>---*/
}

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_3_END>---*/

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
