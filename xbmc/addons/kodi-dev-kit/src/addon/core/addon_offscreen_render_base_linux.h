/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "addon_offscreen_render_base.h"

#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace KODI_ADDON
{
namespace INTERNAL
{

class COffsceenRenderBaseLinux : public IOffsceenRenderBase
{
public:
  COffsceenRenderBaseLinux() = default;

  bool Create() override;
  void Destroy() override;

  EGLDisplay GetEGLDisplay() { return m_eglDisplay; }
  EGLConfig GetEGLConfig() { return m_eglConfig; }
  EGLContext GetEGLContext() { return m_eglContext; }

private:
  bool m_started = false;

  EGLDisplay m_eglDisplay = EGL_NO_DISPLAY;
  EGLConfig m_eglConfig = EGL_NO_CONFIG_KHR;
  EGLContext m_eglContext = EGL_NO_CONTEXT;
};

} /* namespace INTERNAL */
} /* namespace KODI_ADDON */


