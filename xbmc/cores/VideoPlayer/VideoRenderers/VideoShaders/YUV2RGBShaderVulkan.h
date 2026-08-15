/*
 *  Copyright (C) 2007-2024 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "ConversionMatrix.h"
#include "ShaderFormats.h"
#include "cores/VideoSettings.h"
#include "guilib/graphics/vulkan/VulkanShader.h"
#include "utils/TransformMatrix.h"

extern "C"
{
#include <libavutil/mastering_display_metadata.h>
#include <libavutil/pixfmt.h>
}

namespace Shaders
{
namespace VULKAN
{

class BaseYUV2RGBVulkanShader : public KODI::GUILIB::GRAPHICS::VULKAN::CVulkanSLShaderProgram
{
public:
  BaseYUV2RGBVulkanShader(EShaderFormat format,
                            AVColorPrimaries dst,
                            AVColorPrimaries src,
                            bool toneMap,
                            ETONEMAPMETHOD toneMapMethod,
                            bool dither = false);
  ~BaseYUV2RGBVulkanShader() override;
  void SetField(int field) { m_field = field; }
  void SetWidth(int w) { m_width = w; }
  void SetHeight(int h) { m_height = h; }

  void SetColParams(AVColorSpace colSpace, int bits, bool limited, int textureBits);
  void SetBlack(float black) { m_black = black; }
  void SetContrast(float contrast) { m_contrast = contrast; }
  void SetConvertFullColorRange(bool convertFullRange) { m_convertFullRange = convertFullRange; }
  void SetDitherUniforms(bool enabled, unsigned int ditherTex, unsigned int ditherDepth, int ditherSize);
  void SetDisplayMetadata(bool hasDisplayMetadata,
                          const AVMasteringDisplayMetadata& displayMetadata,
                          bool hasLightMetadata,
                          AVContentLightMetadata lightMetadata);
  void SetToneMapParam(float param) { m_toneMappingParam = param; }

  int GetVertexLoc() { return m_hVertex; }
  int GetYcoordLoc() { return m_hYcoord; }
  int GetUcoordLoc() { return m_hUcoord; }
  int GetVcoordLoc() { return m_hVcoord; }

  void SetMatrices(const float* p, const float* m)
  {
    m_proj = p;
    m_model = m;
  }
  void SetAlpha(float alpha) { m_alpha = alpha; }

protected:
  void OnCompiledAndLinked() override;
  bool OnEnabled() override;
  void OnDisabled() override;
  void Free();

  EShaderFormat m_format;
  int m_width;
  int m_height;
  int m_field;
  bool m_hasDisplayMetadata{false};
  AVMasteringDisplayMetadata m_displayMetadata;
  bool m_hasLightMetadata{false};
  AVContentLightMetadata m_lightMetadata;
  bool m_toneMapping{false};
  ETONEMAPMETHOD m_toneMappingMethod{VS_TONEMAPMETHOD_OFF};
  float m_toneMappingParam{1.0};

  bool m_colorConversion{false};

  float m_black;
  float m_contrast;

  std::string m_defines;

  CConvertMatrix m_convMatrix;

  // shader attribute handles
  int m_hYTex{-1};
  int m_hUTex{-1};
  int m_hVTex{-1};
  int m_hYuvMat{-1};
  int m_hStep{-1};
  int m_hGammaSrc{-1};
  int m_hGammaDstInv{-1};
  int m_hPrimMat{-1};
  int m_hToneP1{-1};
  int m_hCoefsDst{-1};
  int m_hLuminance = -1;

  int m_hVertex{-1};
  int m_hYcoord{-1};
  int m_hUcoord{-1};
  int m_hVcoord{-1};
  int m_hProj{-1};
  int m_hModel{-1};
  int m_hAlpha{-1};

  const float* m_proj{nullptr};
  const float* m_model{nullptr};
  float m_alpha{1.0f};

  bool m_convertFullRange;

  // dithering
  bool m_dither{false};
  bool m_ditherEnabled{false};
  int m_hDitherEnabled{-1};
  int m_hDither{-1};
  int m_hDitherQuant{-1};
  int m_hDitherSize{-1};
  unsigned int m_ditherTex{0};
  unsigned int m_ditherDepth{0};
  int m_ditherSize{0};
};

class YUV2RGBProgressiveShader : public BaseYUV2RGBVulkanShader
{
public:
  YUV2RGBProgressiveShader(EShaderFormat format,
                           AVColorPrimaries dstPrimaries,
                           AVColorPrimaries srcPrimaries,
                           bool toneMap,
                           ETONEMAPMETHOD toneMapMethod,
                           bool dither = false);
};

class YUV2RGBBobShader : public BaseYUV2RGBVulkanShader
{
public:
  YUV2RGBBobShader(EShaderFormat format,
                   AVColorPrimaries dstPrimaries,
                   AVColorPrimaries srcPrimaries,
                   bool toneMap,
                   ETONEMAPMETHOD toneMapMethod,
                   bool dither = false);
  void OnCompiledAndLinked() override;
  bool OnEnabled() override;

  int m_hStepX = -1;
  int m_hStepY = -1;
  int m_hField = -1;
};

class YUV2RGBFilterShader : public BaseYUV2RGBVulkanShader
{
public:
  YUV2RGBFilterShader(EShaderFormat format,
                      AVColorPrimaries dstPrimaries,
                      AVColorPrimaries srcPrimaries,
                      bool toneMap,
                      ETONEMAPMETHOD toneMapMethod,
                      ESCALINGMETHOD method,
                      bool dither = false);
  ~YUV2RGBFilterShader() override;

protected:
  void OnCompiledAndLinked() override;
  bool OnEnabled() override;

  unsigned int m_kernelTex = 0;
  int m_hKernTex = -1;
  ESCALINGMETHOD m_scaling = VS_SCALINGMETHOD_LANCZOS3_FAST;
};

} // namespace VULKAN
} // namespace Shaders
