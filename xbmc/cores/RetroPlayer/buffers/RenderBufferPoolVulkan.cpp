/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "RenderBufferPoolVulkan.h"

#include "RenderBufferVulkan.h"
#include "cores/RetroPlayer/rendering/RenderContext.h"
#include "cores/RetroPlayer/rendering/RenderVideoSettings.h"
#include "cores/RetroPlayer/rendering/VideoRenderers/RPRendererVulkan.h"
#include "utils/VulkanUtils.h"

using namespace KODI;
using namespace RETRO;

CRenderBufferPoolVulkan::CRenderBufferPoolVulkan(CRenderContext& context) : m_context(context)
{
}

bool CRenderBufferPoolVulkan::IsCompatible(const CRenderVideoSettings& renderSettings) const
{
  return CRPRendererVulkan::SupportsScalingMethod(renderSettings.GetScalingMethod());
}

IRenderBuffer* CRenderBufferPoolVulkan::CreateRenderBuffer(void* header /* = nullptr */)
{
  return new CRenderBufferVulkan(m_context, m_pixelType, m_internalFormat, m_pixelFormat, m_bpp);
}

bool CRenderBufferPoolVulkan::ConfigureInternal()
{
  // Configure CRenderBufferPoolVulkan
  switch (m_format)
  {
    case AV_PIX_FMT_0RGB32:
    {
      m_pixelType = GL_UNSIGNED_BYTE;
      if (m_context.IsExtSupported("GL_EXT_texture_format_BGRA8888") ||
          m_context.IsExtSupported("GL_IMG_texture_format_BGRA8888"))
      {
        m_internalFormat = GL_BGRA_EXT;
        m_pixelFormat = GL_BGRA_EXT;
      }
      else if (m_context.IsExtSupported("GL_APPLE_texture_format_BGRA8888"))
      {
        // Apple's implementation does not conform to spec. Instead, they require
        // differing format/internalformat, more like GL.
        m_internalFormat = GL_RGBA;
        m_pixelFormat = GL_BGRA_EXT;
      }
      else
      {
        m_internalFormat = GL_RGBA;
        m_pixelFormat = GL_RGBA;
      }
      m_bpp = sizeof(uint32_t);
      return true;
    }
    case AV_PIX_FMT_RGB555:
    case AV_PIX_FMT_RGB565:
    {
      m_pixelType = GL_UNSIGNED_SHORT_5_6_5;
      m_internalFormat = GL_RGB;
      m_pixelFormat = GL_RGB;
      m_bpp = sizeof(uint16_t);
      return true;
    }
    default:
      break;
  }

  return false;
}
