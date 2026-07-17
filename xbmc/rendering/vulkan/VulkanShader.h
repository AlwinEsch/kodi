/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "guilib/graphics/vulkan/VulkanShader.h"

#include <string>

enum class ShaderMethodVulkan
{
  SM_DEFAULT,
  SM_TEXTURE,
  SM_TEXTURE_111R,
  SM_MULTI,
  SM_MULTI_RGBA_111R,
  SM_FONTS,
  SM_FONTS_SHADER_CLIP,
  SM_TEXTURE_NOBLEND,
  SM_MULTI_BLENDCOLOR,
  SM_MULTI_RGBA_111R_BLENDCOLOR,
  SM_MULTI_111R_111R_BLENDCOLOR,
  SM_TEXTURE_RGBA,
  SM_TEXTURE_RGBA_OES,
  SM_TEXTURE_RGBA_BLENDCOLOR,
  SM_TEXTURE_RGBA_BOB,
  SM_TEXTURE_RGBA_BOB_OES,
  SM_TEXTURE_NOALPHA,
  SM_MAX
};

class CVulkanShader : public KODI::GUILIB::GRAPHICS::VULKAN::CVulkanSLShaderProgram
{
public:
  CVulkanShader(const char* shader, const std::string& prefix);
  CVulkanShader(const char* vshader, const char* fshader, const std::string& prefix);
  void OnCompiledAndLinked() override;
  bool OnEnabled() override;
  void Free();

  //GLint GetPosLoc()   { return m_hPos;   }
  //GLint GetColLoc()   { return m_hCol;   }
  //GLint GetCord0Loc() { return m_hCord0; }
  //GLint GetCord1Loc() { return m_hCord1; }
  //GLint GetDepthLoc() { return m_hDepth; }
  //GLint GetPmaLoc() { return m_hPma; }
  //GLint GetUniColLoc() { return m_hUniCol; }
  //GLint GetCoord0MatrixLoc() { return m_hCoord0Matrix; }
  //GLint GetFieldLoc() { return m_hField; }
  //GLint GetStepLoc() { return m_hStep; }
  //GLint GetContrastLoc() { return m_hContrast; }
  //GLint GetBrightnessLoc() { return m_hBrightness; }
  //GLint GetModelLoc() { return m_hModel; }
  //GLint GetMatrixLoc() { return m_hMatrix; }
  //GLint GetShaderClipLoc() { return m_hShaderClip; }
  //GLint GetShaderCoordStepLoc() { return m_hCoordStep; }
  //bool HardwareClipIsPossible() { return m_clipPossible; }
  //GLfloat GetClipXFactor() { return m_clipXFactor; }
  //GLfloat GetClipXOffset() { return m_clipXOffset; }
  //GLfloat GetClipYFactor() { return m_clipYFactor; }
  //GLfloat GetClipYOffset() { return m_clipYOffset; }

protected:
  //GLint m_hTex0 = 0;
  //GLint m_hTex1 = 0;
  //GLint m_hUniCol = 0;
  //GLint m_hProj = 0;
  //GLint m_hModel = 0;
  //GLint m_hMatrix{0}; // m_hProj * m_hModel
  //GLint m_hShaderClip{0}; // clipping rect vec4(x1,y1,x2,y2)
  //GLint m_hCoordStep{0}; // step (1/resolution) for the two textures vec4(t1.x,t1.y,t2.x,t2.y)
  //GLint m_hPos = 0;
  //GLint m_hCol = 0;
  //GLint m_hCord0 = 0;
  //GLint m_hCord1 = 0;
  //GLint m_hCoord0Matrix = 0;
  //GLint m_hField = 0;
  //GLint m_hStep = 0;
  //GLint m_hContrast = 0;
  //GLint m_hBrightness = 0;
  //GLint m_hDepth = 0;
  //GLint m_hPma = 0;

  //const GLfloat *m_proj;
  //const GLfloat *m_model;

  //bool m_clipPossible;
  //GLfloat m_clipXFactor;
  //GLfloat m_clipXOffset;
  //GLfloat m_clipYFactor;
  //GLfloat m_clipYOffset;

  //GLfloat m_sdrPeak;
};
