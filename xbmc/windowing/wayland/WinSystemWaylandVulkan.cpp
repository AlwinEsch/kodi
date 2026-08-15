/*
 *  Copyright (C) 2017-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "WinSystemWaylandVulkan.h"

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

void CWinSystemWaylandVulkan::Register()
{
  CWindowSystemFactory::RegisterWindowSystem(CreateWinSystem, "wayland");
}

std::unique_ptr<CWinSystemBase> CWinSystemWaylandVulkan::CreateWinSystem()
{
  return std::make_unique<CWinSystemWaylandVulkan>();
}

bool CWinSystemWaylandVulkan::InitWindowSystem()
{
  if (!CWinSystemWayland::InitWindowSystem())
  {
    return false;
  }

  CLinuxRendererVulkan::Register();

  //  CDVDVideoCodecDRMPRIME::Register();
  //  CRendererDRMPRIMEVulkan::Register();
  //
  //  RETRO::CRPProcessInfo::RegisterRendererFactory(new RETRO::CRendererFactoryDMAVulkan);
  //  RETRO::CRPProcessInfo::RegisterRendererFactory(new RETRO::CRendererFactoryVulkan);
  //
  //  bool general, deepColor;
  //  m_vaapiProxy.reset(WAYLAND::VaapiProxyCreate());
  //  WAYLAND::VaapiProxyConfig(m_vaapiProxy.get(), GetConnection()->GetDisplay(),
  //                            m_eglContext.GetEGLDisplay());
  //  WAYLAND::VAAPIRegisterRenderVulkan(m_vaapiProxy.get(), general, deepColor);
  //  if (general)
  //  {
  //    WAYLAND::VAAPIRegister(m_vaapiProxy.get(), deepColor);
  //  }
  //
    CBufferObjectFactory::ClearBufferObjects();
  //#if defined(HAVE_LINUX_MEMFD) && defined(HAVE_LINUX_UDMABUF)
  //  CUDMABufferObject::Register();
  //#endif
  //#if defined(HAVE_LINUX_DMA_HEAP)
  //  CDMAHeapBufferObject::Register();
  //#endif

  KODI::RENDERING::VULKAN::CScreenshotSurface::Register();

  return true;
}

bool CWinSystemWaylandVulkan::CreateNewWindow(const std::string& name,
                                              bool fullScreen,
                                              RESOLUTION_INFO& res)
{
  if (!CWinSystemWayland::CreateNewWindow(name, fullScreen, res))
  {
    return false;
  }

  wl_display* display = GetConnection()->GetDisplay();
  wl_surface* surface = GetMainSurface();

  SetRenderSystemWaylandInfo(display, surface, res.iWidth, res.iHeight);

  //if (!CreateContext())
  //{
  //  return false;
  //}

  return true;
}

bool CWinSystemWaylandVulkan::DestroyWindow()
{
  return CWinSystemWayland::DestroyWindow();
}

bool CWinSystemWaylandVulkan::DestroyWindowSystem()
{
  return CWinSystemWayland::DestroyWindowSystem();
}

bool CWinSystemWaylandVulkan::BindTextureUploadContext()
{
  return true;
}

bool CWinSystemWaylandVulkan::UnbindTextureUploadContext()
{
  return true;
}

bool CWinSystemWaylandVulkan::HasContext()
{
  return true;
}

//bool CWinSystemWaylandVulkan::CreateContext()
//{
////  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
////  //CEGLAttributesVec contextAttribs;
////  //contextAttribs.Add({{EGL_CONTEXT_CLIENT_VERSION, 2}});
////
////  //if (!m_eglContext.CreateContext(contextAttribs))
////  //{
////  //  CLog::Log(LOGERROR, "EGL context creation failed");
////  //  return false;
////  //}
//  return true;
//}

void CWinSystemWaylandVulkan::SetContextSize(CSizeInt size)
{
  fprintf(stderr, "---> %s (%d x %d) -> (%d x %d)\n", __PRETTY_FUNCTION__,
          CVulkanRenderSystem::m_width, CVulkanRenderSystem::m_height, size.Width(), size.Height());

  //CWinSystemWaylandEGLContext::SetContextSize(size);
  ////// Change EGL surface size if necessary
  ////if (GetNativeWindowAttachedSize() != size)
  ////{
  ////  CLog::LogF(LOGDEBUG, "Updating egl_window size to {}x{}", size.Width(), size.Height());
  ////  m_nativeWindow.resize(size.Width(), size.Height(), 0, 0);
  ////}

  // Propagate changed dimensions to render system if necessary
  if (CVulkanRenderSystem::m_width != static_cast<uint32_t>(size.Width()) ||
      CVulkanRenderSystem::m_height != static_cast<uint32_t>(size.Height()))
  {
    CLog::LogF(LOGDEBUG, "Resetting render system to {}x{}", size.Width(), size.Height());
    CVulkanRenderSystem::ResetRenderSystem(size.Width(), size.Height());
  }
}

