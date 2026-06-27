/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanInstance.h"

#include "utils/log.h"
#include "utils/VulkanUtils.h"

using namespace KODI::RENDERING::VULKAN;

CVulkanInstance::~CVulkanInstance()
{
  Destroy();
}

bool CVulkanInstance::Create()
{
  return true;
}

void CVulkanInstance::Destroy()
{

}
