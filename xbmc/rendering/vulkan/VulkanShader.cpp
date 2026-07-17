/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanShader.h"

using namespace KODI::GUILIB::GRAPHICS::VULKAN;

CVulkanShader::CVulkanShader(const char* shader, const std::string& prefix)
{
}

CVulkanShader::CVulkanShader(const char* vshader, const char* fshader, const std::string& prefix)
{
}

void CVulkanShader::OnCompiledAndLinked()
{
}

bool CVulkanShader::OnEnabled()
{
  return true;
}

void CVulkanShader::Free()
{
}

