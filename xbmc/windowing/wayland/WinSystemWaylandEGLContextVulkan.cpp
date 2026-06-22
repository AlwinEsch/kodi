/*
 *  Copyright (C) 2017-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "WinSystemWaylandEGLContextVulkan.h"

#include "OptionalsReg.h"
#include "cores/RetroPlayer/process/RPProcessInfo.h"
#include "cores/RetroPlayer/rendering/VideoRenderers/RPRendererDMAVulkan.h"
#include "cores/RetroPlayer/rendering/VideoRenderers/RPRendererVulkan.h"
#include "cores/VideoPlayer/DVDCodecs/Video/DVDVideoCodecDRMPRIME.h"
#include "cores/VideoPlayer/VideoRenderers/HwDecRender/RendererDRMPRIMEVulkan.h"
#include "cores/VideoPlayer/VideoRenderers/LinuxRendererVulkan.h"
#include "cores/VideoPlayer/VideoRenderers/RenderFactory.h"
#include "rendering/vulkan/VulkanScreenshotSurface.h"
#include "utils/BufferObjectFactory.h"
#include "utils/DMAHeapBufferObject.h"
#include "utils/UDMABufferObject.h"
#include "utils/log.h"
#include "windowing/WindowSystemFactory.h"

using namespace KODI::WINDOWING::WAYLAND;

void CWinSystemWaylandEGLContextVulkan::Register()
{
  CWindowSystemFactory::RegisterWindowSystem(CreateWinSystem, "wayland");
}

std::unique_ptr<CWinSystemBase> CWinSystemWaylandEGLContextVulkan::CreateWinSystem()
{
  return std::make_unique<CWinSystemWaylandEGLContextVulkan>();
}

bool CWinSystemWaylandEGLContextVulkan::InitWindowSystem()
{
  if (!CWinSystemWaylandEGLContext::InitWindowSystemEGL(EGL_OPENGL_ES2_BIT, EGL_OPENGL_ES_API))
  {
    return false;
  }

  CLinuxRendererVulkan::Register();

  CDVDVideoCodecDRMPRIME::Register();
  CRendererDRMPRIMEVulkan::Register();

  RETRO::CRPProcessInfo::RegisterRendererFactory(new RETRO::CRendererFactoryDMAVulkan);
  RETRO::CRPProcessInfo::RegisterRendererFactory(new RETRO::CRendererFactoryVulkan);

  bool general, deepColor;
  m_vaapiProxy.reset(WAYLAND::VaapiProxyCreate());
  WAYLAND::VaapiProxyConfig(m_vaapiProxy.get(), GetConnection()->GetDisplay(),
                            m_eglContext.GetEGLDisplay());
  WAYLAND::VAAPIRegisterRenderVulkan(m_vaapiProxy.get(), general, deepColor);
  if (general)
  {
    WAYLAND::VAAPIRegister(m_vaapiProxy.get(), deepColor);
  }

  CBufferObjectFactory::ClearBufferObjects();
#if defined(HAVE_LINUX_MEMFD) && defined(HAVE_LINUX_UDMABUF)
  CUDMABufferObject::Register();
#endif
#if defined(HAVE_LINUX_DMA_HEAP)
  CDMAHeapBufferObject::Register();
#endif

  CVulkanScreenshotSurface::Register();

  return true;
}

bool CWinSystemWaylandEGLContextVulkan::CreateContext()
{
  CEGLAttributesVec contextAttribs;
  contextAttribs.Add({{EGL_CONTEXT_CLIENT_VERSION, 2}});

  if (!m_eglContext.CreateContext(contextAttribs))
  {
    CLog::Log(LOGERROR, "EGL context creation failed");
    return false;
  }
  return true;
}

void CWinSystemWaylandEGLContextVulkan::SetContextSize(CSizeInt size)
{
  CWinSystemWaylandEGLContext::SetContextSize(size);

  // Propagate changed dimensions to render system if necessary
  if (CVulkanRenderSystem::m_width != size.Width() || CVulkanRenderSystem::m_height != size.Height())
  {
    CLog::LogF(LOGDEBUG, "Resetting render system to {}x{}", size.Width(), size.Height());
    CVulkanRenderSystem::ResetRenderSystem(size.Width(), size.Height());
  }
}

void CWinSystemWaylandEGLContextVulkan::SetVSyncImpl(bool enable)
{
  // Unsupported
}

void CWinSystemWaylandEGLContextVulkan::PresentRenderImpl(bool rendered)
{
  PresentFrame(rendered);
}

void CWinSystemWaylandEGLContextVulkan::delete_CVaapiProxy::operator()(CVaapiProxy *p) const
{
  WAYLAND::VaapiProxyDelete(p);
}
