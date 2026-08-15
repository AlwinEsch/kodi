/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "cores/RetroPlayer/buffers/video/RenderBufferSysMem.h"
#include "system_vulkan.h"

namespace KODI
{
namespace RETRO
{
class CRenderContext;

class CRenderBufferVulkan : public CRenderBufferSysMem
{
public:
  CRenderBufferVulkan(CRenderContext& context,
                      unsigned int pixelType,
                      unsigned int internalFormat,
                      unsigned int pixelFormat,
                      unsigned int bpp);
  ~CRenderBufferVulkan() override;

  // Implementation of IRenderBuffer via CRenderBufferSysMem
  bool UploadTexture() override;

  //GLuint TextureID() const { return m_textureId; }

private:
  // Construction parameters
  CRenderContext& m_context;
  const unsigned int m_pixelType;
  const unsigned int m_internalFormat;
  const unsigned int m_pixelFormat;
  const unsigned int m_bpp;

  //const GLenum m_textureTarget = GL_TEXTURE_2D; //! @todo
  //GLuint m_textureId = 0;

  //void CreateTexture();
  //void DeleteTexture();
};
} // namespace RETRO
} // namespace KODI
