/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "FrameBufferObjectVulkan.h"

#include "ServiceBroker.h"
#include "rendering/RenderSystem.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "utils/log.h"

//////////////////////////////////////////////////////////////////////
// CFrameBufferObjectVulkan
//////////////////////////////////////////////////////////////////////

CFrameBufferObjectVulkan::CFrameBufferObjectVulkan()
{
  m_valid = false;
  m_supported = false;
  m_bound = false;
}

bool CFrameBufferObjectVulkan::IsSupported()
{
  //if (CVulkanExtensions::IsExtensionSupported(CVulkanExtensions::EXT_framebuffer_object))
  //  m_supported = true;
  //else
  //  m_supported = false;
  //return m_supported;
  return true;
}

bool CFrameBufferObjectVulkan::Initialize()
{
  //if (!IsSupported())
  //  return false;

  //Cleanup();

  //glGenFramebuffers(1, &m_fbo);
  //VerifyVulkanState();

  //if (!m_fbo)
  //  return false;

  //m_valid = true;
  return true;
}

void CFrameBufferObjectVulkan::Cleanup()
{
  //if (!IsValid())
  //  return;

  //if (m_fbo)
  //  glDeleteFramebuffers(1, &m_fbo);

  //if (m_texid)
  //  glDeleteTextures(1, &m_texid);

  //if (m_depthBuffer)
  //  glDeleteRenderbuffers(1, &m_depthBuffer);

  //m_depthBuffer = 0;
  //m_texid = 0;
  //m_fbo = 0;
  //m_valid = false;
  //m_bound = false;
}

//bool CFrameBufferObjectVulkan::CreateAndBindToTexture(GLenum target, int width, int height, GLenum format, GLenum type,
//                                                GLenum filter, GLenum clampmode)
//{
//  if (!IsValid())
//    return false;
//
//  if (m_texid)
//    glDeleteTextures(1, &m_texid);
//
//  glGenTextures(1, &m_texid);
//  glBindTexture(target, m_texid);
//  glTexImage2D(target, 0, format,  width, height, 0, GL_RGBA, type, NULL);
//  glTexParameteri(target, GL_TEXTURE_WRAP_S, clampmode);
//  glTexParameteri(target, GL_TEXTURE_WRAP_T, clampmode);
//  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
//  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
//  VerifyVulkanState();
//
//  m_bound = false;
//  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
//  glBindTexture(target, m_texid);
//  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, m_texid, 0);
//  VerifyVulkanState();
//  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//  glBindFramebuffer(GL_FRAMEBUFFER, 0);
//  if (status != GL_FRAMEBUFFER_COMPLETE)
//  {
//    VerifyVulkanState();
//    return false;
//  }
//  m_bound = true;
//  return true;
//}

bool CFrameBufferObjectVulkan::AttachDepthBuffer(int width, int height)
{
  //if (!IsValid() || !IsBound())
  //  return false;

  //if (m_depthBuffer)
  //  glDeleteRenderbuffers(1, &m_depthBuffer);

  //glGenRenderbuffers(1, &m_depthBuffer);
  //glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
  //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
  //glBindRenderbuffer(GL_RENDERBUFFER, 0);

  //glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);
  //const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  //glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //if (status != GL_FRAMEBUFFER_COMPLETE)
  //{
  //  glDeleteRenderbuffers(1, &m_depthBuffer);
  //  m_depthBuffer = 0;
  //  return false;
  //}
  return true;
}

//void CFrameBufferObjectVulkan::SetFiltering(GLenum target, GLenum mode)
//{
//  glBindTexture(target, m_texid);
//  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mode);
//  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, mode);
//}

// Begin rendering to FBO
bool CFrameBufferObjectVulkan::BeginRender()
{
  //if (IsValid() && IsBound())
  //{
  //  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  //  return true;
  //}
  return false;
}

// Finish rendering to FBO
void CFrameBufferObjectVulkan::EndRender() const
{
  //if (IsValid())
  //  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
