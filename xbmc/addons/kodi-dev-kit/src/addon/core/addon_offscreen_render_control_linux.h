/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "addon_offscreen_render_control.h"
#include "../../../src/shared/SharedGL.h"

#include <memory>
#include <string>

namespace KODI_ADDON
{
namespace INTERNAL
{

class COffsceenRenderBaseLinux;

class CAddonOffscreenRenderControlLinux : public IAddonOffscreenRenderControl
{
public:
  CAddonOffscreenRenderControlLinux(KODI::ADDONS::INTERFACE::PtrKodiOffscreen kodiOffsetHdl,
                                    uint64_t renderId,
                                    int x,
                                    int y,
                                    int width,
                                    int height,
                                    ADDON_HARDWARE_CONTEXT context);
  virtual ~CAddonOffscreenRenderControlLinux() = default;

  bool Start() override;
  void Stop() override;
  void RenderBegin() override;
  void RenderEnd() override;

private:
  const std::string m_socketServerFile;
  const std::string m_socketClientFile;

  PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR = nullptr;
  PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR = nullptr;
  PFNEGLEXPORTDMABUFIMAGEQUERYMESAPROC eglExportDMABUFImageQueryMESA = nullptr;
  PFNEGLEXPORTDMABUFIMAGEMESAPROC eglExportDMABUFImageMESA = nullptr;

  EGLSurface m_eglSurface = EGL_NO_SURFACE;
  EGLDisplay m_eglDisplay = EGL_NO_DISPLAY;
  EGLContext m_eglContext = EGL_NO_CONTEXT;
  EGLImageKHR m_eglImage = EGL_NO_IMAGE_KHR;
  EGLConfig m_eglConfig = EGL_NO_CONFIG_KHR;

  // EGL (extension: EGL_MESA_image_dma_buf_export): Get file descriptor (texture_dmabuf_fd) for the EGL image and get its
  // storage data (texture_storage_metadata - fourcc, stride, offset)
  int m_socket = -1;
  int m_textureDmaBufFd = -1;
  struct texture_storage_metadata_t m_textureStorageMetadata;

  GLuint m_texture = 0;
  GLuint m_framebuffer = 0;
  GLuint m_renderbuffer = 0;

  GLuint m_vertexVAO = 0;

  std::shared_ptr<COffsceenRenderBaseLinux> m_renderBase;
};

} /* namespace INTERNAL */
} /* namespace KODI_ADDON */
