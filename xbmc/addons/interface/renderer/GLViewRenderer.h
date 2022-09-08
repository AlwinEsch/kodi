/*
 *  Copyright (C) 2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "system_egl.h"
#include "IRenderHelper.h"
#include "guilib/Shader.h"

#include <EGL/eglext.h>

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

class CViewRendererOpenGL : public Shaders::CGLSLShaderProgram, public IRenderHelper
{
public:
  CViewRendererOpenGL(const std::string& clientIdentifier);
  ~CViewRendererOpenGL();

  bool Init() override;
  void Deinit() override;
  void Begin() override;
  void End() override;

protected:
  void OnCompiledAndLinked() override;
  bool OnEnabled() override;

private:
  void GetShaders(std::string& vert, std::string& frag);

  const std::string m_socketServerFile;
  const std::string m_socketClientFile;

  EGLDisplay m_eglDisplay = EGL_NO_DISPLAY;
  EGLImageKHR m_image = EGL_NO_IMAGE_KHR;

  GLuint m_vertexVBO = 0;
  GLuint m_indexVBO = 0;

  GLint m_aPosition = -1;
  GLint m_aCoord = -1;
  GLuint m_texture = 0;

  PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR = nullptr;
  PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR = nullptr;
  PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES = nullptr;

  bool m_inited = false;
};

using CRenderHelper = CViewRendererOpenGL;

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
