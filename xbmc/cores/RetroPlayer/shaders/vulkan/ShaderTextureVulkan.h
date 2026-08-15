/*
 *  Copyright (C) 2019-2025 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "cores/RetroPlayer/shaders/IShaderTexture.h"
#include "system_vulkan.h"

#include <cstdint>

namespace KODI::SHADER
{

class CShaderTextureVulkan : public IShaderTexture
{
public:
  CShaderTextureVulkan(uint32_t textureWidth,
                       uint32_t textureHeight,
                       unsigned int pixelType,
                       unsigned int internalFormat,
                       unsigned int pixelFormat,
                       bool bUseAlpha);
  ~CShaderTextureVulkan() override;

  // Disallow copy and move (this object owns raw GL IDs)
  CShaderTextureVulkan(const CShaderTextureVulkan&) = delete;
  CShaderTextureVulkan& operator=(const CShaderTextureVulkan&) = delete;
  CShaderTextureVulkan(CShaderTextureVulkan&&) = delete;
  CShaderTextureVulkan& operator=(CShaderTextureVulkan&&) = delete;

  // Implementation of IShaderTexture
  float GetWidth() const override;
  float GetHeight() const override;

  // OpenVulkan interface
  void CreateTexture();
  void DeleteTexture();
  void BindToUnit(unsigned int unit);
  //GLuint GetTextureID() const { return m_texture; }

  void SetSRGBFramebuffer() { m_sRgbFramebuffer = true; }
  bool IsSRGBFramebuffer() const { return m_sRgbFramebuffer; }

  void SetMipmapping() { m_mipmapping = true; }
  bool IsMipmapped() const { return m_mipmapping; }

  // Frame buffer interface
  void CreateFBO();
  void DeleteFBO();
  bool BindFBO();
  void UnbindFBO() const;

private:
  // Construction parameters
  const unsigned int m_textureWidth;
  const unsigned int m_textureHeight;
  const unsigned int m_pixelType;
  const unsigned int m_internalFormat;
  const unsigned int m_pixelFormat;
  const bool m_useAlpha;

  //const GLenum m_textureTarget = GL_TEXTURE_2D; //! @todo
  unsigned int m_texture{0};
  unsigned int m_FBO{0};
  bool m_sRgbFramebuffer{false};
  bool m_mipmapping{false};
};
} // namespace KODI::SHADER
