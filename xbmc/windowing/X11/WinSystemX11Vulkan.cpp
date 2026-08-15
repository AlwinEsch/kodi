/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "WinSystemX11Vulkan.h"

#include "OptionalsReg.h"
#include "ServiceBroker.h"
#include "X11DPMSSupport.h"
#include "application/ApplicationComponents.h"
#include "application/ApplicationSkinHandling.h"
#include "cores/RetroPlayer/process/X11/RPProcessInfoX11.h"
#include "cores/RetroPlayer/rendering/VideoRenderers/RPRendererVulkan.h"
#include "cores/VideoPlayer/DVDCodecs/DVDFactoryCodec.h"
#include "cores/VideoPlayer/Process/X11/ProcessInfoX11.h"
#include "cores/VideoPlayer/VideoRenderers/LinuxRendererVulkan.h"
#include "cores/VideoPlayer/VideoRenderers/RenderFactory.h"
#include "guilib/DispResource.h"
#include "utils/log.h"
#include "windowing/GraphicContext.h"
#include "windowing/WindowSystemFactory.h"

#include <mutex>

using namespace KODI;
using namespace KODI::WINDOWING::X11;

void CWinSystemX11Vulkan::Register()
{
  KODI::WINDOWING::CWindowSystemFactory::RegisterWindowSystem(CreateWinSystem, "x11");
}

std::unique_ptr<CWinSystemBase> CWinSystemX11Vulkan::CreateWinSystem()
{
  return std::make_unique<CWinSystemX11Vulkan>();
}

CWinSystemX11Vulkan::~CWinSystemX11Vulkan()
{
  //delete m_pGLContext;
}

void CWinSystemX11Vulkan::PresentRenderImpl(bool rendered)
{
  //if (rendered && m_pGLContext)
  //  m_pGLContext->SwapBuffers();

  if (m_delayDispReset && m_dispResetTimer.IsTimePast())
  {
    m_delayDispReset = false;
    std::unique_lock lock(m_resourceSection);
    // tell any shared resources
    for (std::vector<IDispResource*>::iterator i = m_resources.begin(); i != m_resources.end(); ++i)
      (*i)->OnResetDisplay();
  }
}

void CWinSystemX11Vulkan::SetVSyncImpl(bool enable)
{
  //m_pGLContext->SetVSync(enable);
}

bool CWinSystemX11Vulkan::IsExtSupported(const char* extension) const
{
  //if (strncmp(extension, m_pGLContext->ExtPrefix().c_str(), 4) != 0)
  //  return CVulkanRenderSystem::IsExtSupported(extension);

  //return m_pGLContext->IsExtSupported(extension);
  return CVulkanRenderSystem::IsExtSupported(extension);
}

//EGLDisplay CWinSystemX11Vulkan::GetEGLDisplay() const
//{
//  return m_pGLContext->m_eglDisplay;
//}
//
//EGLSurface CWinSystemX11Vulkan::GetEGLSurface() const
//{
//  return m_pGLContext->m_eglSurface;
//}
//
//EGLContext CWinSystemX11Vulkan::GetEGLContext() const
//{
//  return m_pGLContext->m_eglContext;
//}
//
//EGLConfig CWinSystemX11Vulkan::GetEGLConfig() const
//{
//  return m_pGLContext->m_eglConfig;
//}

bool CWinSystemX11Vulkan::BindTextureUploadContext()
{
  //if (m_pGLContext)
  //  return static_cast<CGLContextEGL*>(m_pGLContext)->BindTextureUploadContext();
  //else
  return false;
}

bool CWinSystemX11Vulkan::UnbindTextureUploadContext()
{
  //if (m_pGLContext)
  //  return static_cast<CGLContextEGL*>(m_pGLContext)->UnbindTextureUploadContext();
  //else
  return false;
}

bool CWinSystemX11Vulkan::HasContext()
{
  //if (m_pGLContext)
  //  return static_cast<CGLContextEGL*>(m_pGLContext)->HasContext();
  //else
  return false;
}

bool CWinSystemX11Vulkan::SetWindow(
    int width, int height, bool fullscreen, const std::string& output, int* winstate)
{
  int newwin = 0;

  CWinSystemX11::SetWindow(width, height, fullscreen, output, &newwin);
  if (newwin)
  {
    RefreshGLContext(m_currentOutput.compare(output) != 0);
    XSync(m_dpy, false);
    CServiceBroker::GetWinSystem()->GetGfxContext().Clear(0);
    CServiceBroker::GetWinSystem()->GetGfxContext().Flip(true, false);
    ResetVSync();

    m_windowDirty = false;
    m_bIsInternalXrr = false;

    if (!m_delayDispReset)
    {
      std::unique_lock lock(m_resourceSection);
      // tell any shared resources
      for (std::vector<IDispResource*>::iterator i = m_resources.begin(); i != m_resources.end();
           ++i)
        (*i)->OnResetDisplay();
    }
  }
  return true;
}

bool CWinSystemX11Vulkan::CreateNewWindow(const std::string& name,
                                          bool fullScreen,
                                          RESOLUTION_INFO& res)
{
  //CLog::Log(LOGINFO, "CWinSystemX11Vulkan::CreateNewWindow");
  //if (!CWinSystemX11::CreateNewWindow(name, fullScreen, res) || !m_pGLContext)
  //  return false;

  //m_pGLContext->QueryExtensions();
  //m_bufferAgeSupport = m_pGLContext->IsBufferAgeSupported();
  return true;
}

bool CWinSystemX11Vulkan::ResizeWindow(int newWidth, int newHeight, int newLeft, int newTop)
{
  m_newGlContext = false;
  CWinSystemX11::ResizeWindow(newWidth, newHeight, newLeft, newTop);
  CVulkanRenderSystem::ResetRenderSystem(newWidth, newHeight);

  if (m_newGlContext)
  {
    auto& components = CServiceBroker::GetAppComponents();
    const auto appSkin = components.GetComponent<CApplicationSkinHandling>();
    appSkin->ReloadSkin();
  }

  return true;
}

void CWinSystemX11Vulkan::FinishWindowResize(int newWidth, int newHeight)
{
  m_newGlContext = false;
  CWinSystemX11::FinishWindowResize(newWidth, newHeight);
  CVulkanRenderSystem::ResetRenderSystem(newWidth, newHeight);

  if (m_newGlContext)
  {
    auto& components = CServiceBroker::GetAppComponents();
    const auto appSkin = components.GetComponent<CApplicationSkinHandling>();
    appSkin->ReloadSkin();
  }
}

bool CWinSystemX11Vulkan::SetFullScreen(bool fullScreen,
                                        RESOLUTION_INFO& res,
                                        bool blankOtherDisplays)
{
  m_newGlContext = false;
  CWinSystemX11::SetFullScreen(fullScreen, res, blankOtherDisplays);
  CVulkanRenderSystem::ResetRenderSystem(res.iWidth, res.iHeight);

  if (m_newGlContext)
  {
    auto& components = CServiceBroker::GetAppComponents();
    const auto appSkin = components.GetComponent<CApplicationSkinHandling>();
    appSkin->ReloadSkin();
  }

  return true;
}

bool CWinSystemX11Vulkan::DestroyWindowSystem()
{
  //if (m_pGLContext)
  //  m_pGLContext->Destroy();
  return CWinSystemX11::DestroyWindowSystem();
}

bool CWinSystemX11Vulkan::DestroyWindow()
{
  //if (m_pGLContext)
  //  m_pGLContext->Detach();
  return CWinSystemX11::DestroyWindow();
}

XVisualInfo* CWinSystemX11Vulkan::GetVisual()
{
  //EGLDisplay eglDisplay;

  //PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT =
  //  reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));
  //if (eglGetPlatformDisplayEXT)
  //{
  //  EGLint attribs[] =
  //  {
  //    EGL_PLATFORM_X11_SCREEN_EXT, m_screen,
  //    EGL_NONE
  //  };
  //  eglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_X11_EXT,static_cast<EGLNativeDisplayType>(m_dpy), attribs);
  //}
  //else
  //  eglDisplay = eglGetDisplay(static_cast<EGLNativeDisplayType>(m_dpy));

  //if (eglDisplay == EGL_NO_DISPLAY)
  //{
  //  CLog::Log(LOGERROR, "failed to get egl display");
  //  return nullptr;
  //}
  //if (!eglInitialize(eglDisplay, nullptr, nullptr))
  //{
  //  CLog::Log(LOGERROR, "failed to initialize egl display");
  //  return nullptr;
  //}

  //GLint att[] =
  //{
  //  EGL_RED_SIZE, 8,
  //  EGL_GREEN_SIZE, 8,
  //  EGL_BLUE_SIZE, 8,
  //  EGL_ALPHA_SIZE, 8,
  //  EGL_BUFFER_SIZE, 32,
  //  EGL_DEPTH_SIZE, 24,
  //  EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
  //  EGL_NONE
  //};
  //EGLint numConfigs;
  //EGLConfig eglConfig = 0;
  //if (!eglChooseConfig(eglDisplay, att, &eglConfig, 1, &numConfigs) || numConfigs == 0)
  //{
  //  CLog::Log(LOGERROR, "Failed to choose a config {}", eglGetError());
  //  return nullptr;
  //}

  //XVisualInfo x11_visual_info_template;
  //memset(&x11_visual_info_template, 0, sizeof(XVisualInfo));

  //if (!eglGetConfigAttrib(eglDisplay, eglConfig,
  //  EGL_NATIVE_VISUAL_ID, reinterpret_cast<EGLint*>(&x11_visual_info_template.visualid)))
  //{
  //  CLog::Log(LOGERROR, "Failed to query native visual id");
  //  return nullptr;
  //}
  //int num_visuals;
  //XVisualInfo* visual =
  //    XGetVisualInfo(m_dpy, VisualIDMask, &x11_visual_info_template, &num_visuals);
  //return visual;
  return nullptr;
}

bool CWinSystemX11Vulkan::RefreshGLContext(bool force)
{
  bool success = false;
  //if (m_pGLContext)
  //{
  //  success = m_pGLContext->Refresh(force, m_screen, m_glWindow, m_newGlContext);
  //  if (!success)
  //  {
  //    success = m_pGLContext->CreatePB();
  //    m_newGlContext = true;
  //  }
  //  return success;
  //}

  //m_dpms = std::make_shared<CX11DPMSSupport>();
  //VIDEOPLAYER::CProcessInfoX11::Register();
  //RETRO::CRPProcessInfoX11::Register();
  //RETRO::CRPProcessInfoX11::RegisterRendererFactory(new RETRO::CRendererFactoryOpenVulkan);
  //CDVDFactoryCodec::ClearHWAccels();
  //VIDEOPLAYER::CRendererFactory::ClearRenderer();
  //CLinuxRendererVulkan::Register();

  //std::string gli = (getenv("KODI_GL_INTERFACE") != nullptr) ? getenv("KODI_GL_INTERFACE") : "";

  //m_pGLContext = new CGLContextEGL(m_dpy, EGL_OPENGL_ES_API);
  //success = m_pGLContext->Refresh(force, m_screen, m_glWindow, m_newGlContext);
  //if (!success && gli == "EGL_PB")
  //{
  //  success = m_pGLContext->CreatePB();
  //  m_newGlContext = true;
  //}

  //if (!success)
  //{
  //  delete m_pGLContext;
  //  m_pGLContext = nullptr;
  //}
  return success;
}
