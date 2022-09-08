/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "addon_offscreen_render_control_linux.h"

#include "addon_control.h"
#include "addon_offscreen_render_base_linux.h"
#include "../../../src/shared/OpenGLExtensionCheck.h"
#include "../../../src/shared/socket.h"
#include "../../../include/kodi/tools/StringUtils.h"

namespace KODI_ADDON
{
namespace INTERNAL
{

CAddonOffscreenRenderControlLinux::CAddonOffscreenRenderControlLinux(KODI::ADDONS::INTERFACE::PtrKodiOffscreen kodiOffsetHdl,
                                                                     uint64_t renderId,
                                                                     int x,
                                                                     int y,
                                                                     int width,
                                                                     int height,
                                                                     ADDON_HARDWARE_CONTEXT context)
  : IAddonOffscreenRenderControl(kodiOffsetHdl, renderId, x, y, width, height, context),
    m_socketServerFile(kodi::tools::StringUtils::Format("/tmp/%s-%04i-renderproc", AddonIfc::g_ifc->identifier.c_str(), renderId)),
    m_socketClientFile(kodi::tools::StringUtils::Format("/tmp/%s-%04i-renderview", AddonIfc::g_ifc->identifier.c_str(), renderId)),
    m_renderBase(std::dynamic_pointer_cast<COffsceenRenderBaseLinux>(AddonIfc::g_ifc->control->GetOffscreenRenderBase()))
{
}

bool CAddonOffscreenRenderControlLinux::Start()
{
  using namespace KODI::ADDONS::INTERFACE;

  m_eglDisplay = m_renderBase->GetEGLDisplay();
  m_eglContext = m_renderBase->GetEGLContext();

  static const EGLint pbufferAttribs[] = {
      EGL_WIDTH,          9,
      EGL_HEIGHT,         9,
      EGL_TEXTURE_TARGET, EGL_TEXTURE_2D,
      EGL_TEXTURE_FORMAT, EGL_TEXTURE_RGBA,
      EGL_NONE,
  };

  // create an EGL window surface
  m_eglSurface = eglCreatePbufferSurface(m_eglDisplay, m_renderBase->GetEGLConfig(), pbufferAttribs);

  // connect the context to the surface
  eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext);

  glGenVertexArrays(1, &m_vertexVAO);
  glBindVertexArray(m_vertexVAO);

  if (!CheckExtensionSupported("EGL_KHR_image_base"))
  {
    kodi_log(ADDON_LOG_ERROR, "Needed EGL extension \"EGL_KHR_image_base\" not available");
    return false;
  }

  if (!CheckExtensionSupported("EGL_MESA_image_dma_buf_export"))
  {
    kodi_log(ADDON_LOG_ERROR,
              "Needed EGL extension \"EGL_MESA_image_dma_buf_export\" not available");
    return false;
  }

  eglCreateImageKHR = (decltype(eglCreateImageKHR))GetEGLFunc("eglCreateImageKHR");
  eglDestroyImageKHR = (decltype(eglDestroyImageKHR))GetEGLFunc("eglDestroyImageKHR");
  eglExportDMABUFImageQueryMESA = (decltype(eglExportDMABUFImageQueryMESA))GetEGLFunc(
      "eglExportDMABUFImageQueryMESA");
  eglExportDMABUFImageMESA =
      (decltype(eglExportDMABUFImageMESA))GetEGLFunc("eglExportDMABUFImageMESA");

  // GL: Create and populate the texture
  glGenTextures(1, &m_texture);
  glBindTexture(GL_TEXTURE_2D, m_texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, m_width);
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
  glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

  static const EGLint attrib[] = {EGL_IMAGE_PRESERVED_KHR, EGL_FALSE, EGL_GL_TEXTURE_LEVEL_KHR, 0,
                                  EGL_NONE};

  // EGL: Create EGL image from the GL texture
  m_eglImage = eglCreateImageKHR(m_eglDisplay, m_eglContext, EGL_GL_TEXTURE_2D_KHR,
                                 reinterpret_cast<EGLClientBuffer>(m_texture), attrib);
  if (m_eglImage == EGL_NO_IMAGE)
  {
    kodi_log(ADDON_LOG_ERROR, "Failed to get EGL image (ERROR: %i)", eglGetError());
    return false;
  }

  // The next line works around an issue in radeonsi driver (fixed in master at the time of writing). If you are
  // not having problems with texture rendering until the first texture update you can omit this line
  glFinish();

  EGLBoolean queried = eglExportDMABUFImageQueryMESA(
      m_eglDisplay, m_eglImage, &m_textureStorageMetadata.fourcc,
      &m_textureStorageMetadata.num_planes, &m_textureStorageMetadata.modifiers);
  if (!queried)
  {
    kodi_log(ADDON_LOG_ERROR, "Failed to export DMABUF image query");
    return false;
  }

  EGLBoolean exported = eglExportDMABUFImageMESA(m_eglDisplay, m_eglImage, &m_textureDmaBufFd,
                                                 &m_textureStorageMetadata.stride, &m_textureStorageMetadata.offset);
  if (!exported)
  {
    kodi_log(ADDON_LOG_ERROR, "Failed to export DMABUF image");
    return false;
  }

  eglDestroyImageKHR(m_eglDisplay, m_eglImage);

  m_textureStorageMetadata.width = m_width;
  m_textureStorageMetadata.height = m_height;

  glGenFramebuffers(1, &m_framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
  //Attach 2D texture to this FBO
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

  glGenRenderbuffers(1, &m_renderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);

  //Attach depth buffer to FBO
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_renderbuffer);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

   glScissor(0, 0, m_width, m_height);
  glViewport(0, 0, m_width, m_height);

  // Unix Domain Socket: Send file descriptor (texture_dmabuf_fd) and texture storage data (texture_storage_metadata)
  m_socket = create_socket(m_socketServerFile.c_str());
  std::thread thread = std::thread([&] {
    while (m_socket >= 0 && connect_socket(m_socket, m_socketClientFile.c_str()) != 0)
      ;
    write_fd(m_socket, m_textureDmaBufFd, &m_textureStorageMetadata,
             sizeof(m_textureStorageMetadata));
    close(m_socket);
    m_socket = -1;
    close(m_textureDmaBufFd);
    m_textureDmaBufFd = -1;
  });

  thread.detach();

  return true;
}

void CAddonOffscreenRenderControlLinux::Stop()
{
  if (m_socket >= 0)
  {
    int socket = m_socket;
    m_socket = -1;
    close(socket);
    if (m_textureDmaBufFd >= 0)
    {
      m_textureDmaBufFd = -1;
      close(m_textureDmaBufFd);
    }
  }

  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glDeleteRenderbuffers(1, &m_renderbuffer);
  m_renderbuffer = 0;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDeleteFramebuffers(1, &m_framebuffer);
  m_framebuffer = 0;

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDeleteTextures(1, &m_texture);
  m_texture = 0;

  glDeleteVertexArrays(1, &m_vertexVAO);
  m_vertexVAO = 0;
}

void CAddonOffscreenRenderControlLinux::RenderBegin()
{
  eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext);
  glBindVertexArray(m_vertexVAO);
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

void CAddonOffscreenRenderControlLinux::RenderEnd()
{
  glFlush();
}

} /* namespace INTERNAL */
} /* namespace KODI_ADDON */
