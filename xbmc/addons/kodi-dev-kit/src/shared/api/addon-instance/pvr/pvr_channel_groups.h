/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

/* File autogenerated, see xbmc/addons/kodi-dev-kit/tools/code-generator/code_generator.py */

#pragma once

#include "../../../../../include/kodi/c-api/addon-instance/pvr/pvr_channel_groups.h"
#include "../../../../../src/shared/api/addon-instance/pvr/pvr_defines.h"
#include "src/shared/SharedGroups.h"

#include <string.h>
#include <string>
#include <tuple>
#include <vector>

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_1_END>---*/

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

struct IFC_PVR_CHANNEL_GROUP
{
  IFC_PVR_CHANNEL_GROUP() = default;
  IFC_PVR_CHANNEL_GROUP(const PVR_CHANNEL_GROUP* c_data)
  {
    if (c_data == nullptr)
      return;

    memcpy(strGroupName.data(), c_data->strGroupName, PVR_ADDON_NAME_STRING_LENGTH);
    bIsRadio = c_data->bIsRadio;
    iPosition = c_data->iPosition;
  }

  void SetCStructure(PVR_CHANNEL_GROUP* c_data)
  {
    if (c_data == nullptr)
      return;

    memcpy(c_data->strGroupName, strGroupName.data(), PVR_ADDON_NAME_STRING_LENGTH);
    c_data->bIsRadio = bIsRadio;
    c_data->iPosition = iPosition;
  }

  std::array<char, PVR_ADDON_NAME_STRING_LENGTH> strGroupName;
  bool bIsRadio;
  unsigned int iPosition;

  MSGPACK_DEFINE(strGroupName, bIsRadio, iPosition);
};

struct IFC_PVR_CHANNEL_GROUP_MEMBER
{
  IFC_PVR_CHANNEL_GROUP_MEMBER() = default;
  IFC_PVR_CHANNEL_GROUP_MEMBER(const PVR_CHANNEL_GROUP_MEMBER* c_data)
  {
    if (c_data == nullptr)
      return;

    memcpy(strGroupName.data(), c_data->strGroupName, PVR_ADDON_NAME_STRING_LENGTH);
    iChannelUniqueId = c_data->iChannelUniqueId;
    iChannelNumber = c_data->iChannelNumber;
    iSubChannelNumber = c_data->iSubChannelNumber;
    iOrder = c_data->iOrder;
  }

  void SetCStructure(PVR_CHANNEL_GROUP_MEMBER* c_data)
  {
    if (c_data == nullptr)
      return;

    memcpy(c_data->strGroupName, strGroupName.data(), PVR_ADDON_NAME_STRING_LENGTH);
    c_data->iChannelUniqueId = iChannelUniqueId;
    c_data->iChannelNumber = iChannelNumber;
    c_data->iSubChannelNumber = iSubChannelNumber;
    c_data->iOrder = iOrder;
  }

  std::array<char, PVR_ADDON_NAME_STRING_LENGTH> strGroupName;
  unsigned int iChannelUniqueId;
  unsigned int iChannelNumber;
  unsigned int iSubChannelNumber;
  int iOrder;

  MSGPACK_DEFINE(strGroupName, iChannelUniqueId, iChannelNumber, iSubChannelNumber, iOrder);
};

/* Code below becomes on auto generation reused, hand edited parts should included here */
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_2>---*/
/*---AUTO_GEN_PARSE<HAND_EDITED_FIELD_2_END>---*/

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
