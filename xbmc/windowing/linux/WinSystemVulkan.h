/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "utils/VulkanUtils.h"

namespace KODI
{
namespace WINDOWING
{
namespace LINUX
{

class CWinSystemVulkan
{
public:
  CWinSystemVulkan();
  ~CWinSystemVulkan() = default;

  //EGLDisplay GetEGLDisplay() const;
  //EGLSurface GetEGLSurface() const;
  //EGLContext GetEGLContext() const;
  //EGLConfig GetEGLConfig() const;

protected:
  //CEGLContextUtils m_eglContext;
};

} // namespace LINUX
} // namespace WINDOWING
} // namespace KODI
