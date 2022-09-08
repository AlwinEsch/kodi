/*
 *  Copyright (C) 2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "GLViewRenderer.h"

#include "ServiceBroker.h"
#include "addons/kodi-dev-kit/src/shared/SharedGL.h"
#include "filesystem/File.h"
#include "rendering/RenderSystem.h"
#include "utils/EGLUtils.h"
#include "utils/StringUtils.h"
#include "utils/log.h"
#include "../kodi-dev-kit/src/shared/SharedData.h"
#include "../kodi-dev-kit/src/shared/socket.h"

using namespace KODI::ADDONS::INTERFACE;

namespace
{

constexpr const float render_vertices[20] = {
  -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // top left
  1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, // top right
  1.0f,  1.0f,  0.0f, 1.0f, 1.0f, // bottom right
  -1.0f, 1.0f,  0.0f, 0.0f, 1.0f // bottom left
};

constexpr const unsigned char render_indices[4] = {0, 1, 3, 2};

} // namespace

CViewRendererOpenGL::CViewRendererOpenGL(const std::string& clientIdentifier)
  : m_socketServerFile("/tmp/" + clientIdentifier + "-renderproc"),
    m_socketClientFile("/tmp/" + clientIdentifier + "-renderview")
{
}

CViewRendererOpenGL::~CViewRendererOpenGL()
{
  Deinit();
}

bool CViewRendererOpenGL::Init()
{
  if (m_inited)
    return true;

  std::string vertShader, fraqShader;
  GetShaders(vertShader, fraqShader);
  if (!LoadShaderFiles(vertShader, fraqShader) || !CompileAndLink())
  {
    CLog::Log(LOGERROR, "Failed to create or compile shader");
    return false;
  }

  m_eglDisplay = eglGetCurrentDisplay();
  if (m_eglDisplay == EGL_NO_DISPLAY)
  {
    CLog::Log(LOGERROR, "Not get EGL display");
    return false;
  }

  if (!CEGLUtils::HasExtension(m_eglDisplay, "EGL_KHR_image_base"))
  {
    CLog::Log(LOGERROR, "Needed EGL extension \"EGL_KHR_image_base\" not available");
    return false;
  }

  if (!CServiceBroker::GetRenderSystem()->IsExtSupported("GL_OES_EGL_image"))
  {
    CLog::Log(LOGERROR, "Needed EGL extension \"GL_OES_EGL_image\" not available");
    return false;
  }

  eglCreateImageKHR = CEGLUtils::GetRequiredProcAddress<decltype(eglCreateImageKHR)>("eglCreateImageKHR");
  eglDestroyImageKHR = CEGLUtils::GetRequiredProcAddress<decltype(eglDestroyImageKHR)>("eglDestroyImageKHR");
  glEGLImageTargetTexture2DOES = CEGLUtils::GetRequiredProcAddress<decltype(glEGLImageTargetTexture2DOES)>("glEGLImageTargetTexture2DOES");

  glEnable(GL_BLEND);

  // Unix Domain Socket: Receive file descriptor (texture_dmabuf_fd) and texture storage data (dmabuf)
  int texture_dmabuf_fd;
  struct texture_storage_metadata_t dmabuf;

  int sock = create_socket(m_socketClientFile.c_str());
  read_fd(sock, &texture_dmabuf_fd, &dmabuf, sizeof(dmabuf));
  close(sock);

  int i = 0;
  EGLint attribute_list[64];

  attribute_list[i++] = EGL_WIDTH;
  attribute_list[i++] = dmabuf.width;
  attribute_list[i++] = EGL_HEIGHT;
  attribute_list[i++] = dmabuf.height;
  attribute_list[i++] = EGL_LINUX_DRM_FOURCC_EXT;
  attribute_list[i++] = dmabuf.fourcc;

  attribute_list[i++] = EGL_DMA_BUF_PLANE0_FD_EXT;
  attribute_list[i++] = texture_dmabuf_fd;
  attribute_list[i++] = EGL_DMA_BUF_PLANE0_PITCH_EXT;
  attribute_list[i++] = dmabuf.stride;
  attribute_list[i++] = EGL_DMA_BUF_PLANE0_OFFSET_EXT;
  attribute_list[i++] = dmabuf.offset;
#ifdef EGL_DMA_BUF_PLANE0_MODIFIER_LO_EXT
  if (dmabuf.modifiers)
  {
    attribute_list[i++] = EGL_DMA_BUF_PLANE0_MODIFIER_LO_EXT;
    attribute_list[i++] = static_cast<EGLint>(dmabuf.modifiers & 0xFFFFFFFF);
    attribute_list[i++] = EGL_DMA_BUF_PLANE0_MODIFIER_HI_EXT;
    attribute_list[i++] = static_cast<EGLint>(dmabuf.modifiers >> 32);
  }
#endif
  attribute_list[i++] = EGL_NONE;

  m_image = eglCreateImageKHR(m_eglDisplay, EGL_NO_CONTEXT, EGL_LINUX_DMA_BUF_EXT, nullptr,
                              attribute_list);
  if (m_image == EGL_NO_IMAGE)
  {
    CLog::Log(LOGERROR, "Failed to get EGL image");
    return false;
  }

  XFILE::CFile::Delete(m_socketServerFile);
  XFILE::CFile::Delete(m_socketClientFile);

  // GL texture that will be shared
  glGenTextures(1, &m_texture);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, m_image);
  eglDestroyImageKHR(m_eglDisplay, m_image);

  glBindTexture(GL_TEXTURE_2D, 0);

  close(texture_dmabuf_fd);

  glGenBuffers(1, &m_vertexVBO);
  glGenBuffers(1, &m_indexVBO);

  m_inited = true;
  return true;
}

void CViewRendererOpenGL::Deinit()
{
  if (!m_inited)
    return;

  m_inited = false;

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDeleteTextures(1, &m_texture);
  m_texture = 0;

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &m_vertexVBO);
  m_vertexVBO = 0;

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &m_indexVBO);
  m_indexVBO = 0;
}

void CViewRendererOpenGL::Begin()
{

}

void CViewRendererOpenGL::End()
{
  if (!m_inited)
    return;

  glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(render_vertices), render_vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexVBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(render_indices), render_indices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

  Enable();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, 0);

  Disable();

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CViewRendererOpenGL::OnCompiledAndLinked()
{
  m_aPosition = glGetAttribLocation(ProgramHandle(), "aPos");
  m_aCoord = glGetAttribLocation(ProgramHandle(), "aTexCoords");
}

bool CViewRendererOpenGL::OnEnabled()
{
  return true;
}

void CViewRendererOpenGL::GetShaders(std::string& vert, std::string& frag)
{
#if defined(HAS_GL)
  vert = "gl_shader_vert_addon_backend.glsl";
  frag = "gl_shader_frag_addon_backend.glsl";
#else
  vert = "gles_shader_vert_addon_backend.glsl";
  frag = "gles_shader_frag_addon_backend.glsl";
#endif
}
