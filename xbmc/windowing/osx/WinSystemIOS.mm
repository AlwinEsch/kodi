/*
 *      Copyright (C) 2010-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "VideoSyncIos.h"
#include "WinEventsIOS.h"
#include "WinSystemIOS.h"
#include "cores/AudioEngine/Sinks/AESinkDARWINIOS.h"
#include "cores/RetroPlayer/process/ios/RPProcessInfoIOS.h"
#include "cores/RetroPlayer/rendering/VideoRenderers/RPRendererOpenGLES.h"
#include "cores/VideoPlayer/DVDCodecs/DVDFactoryCodec.h"
#include "cores/VideoPlayer/DVDCodecs/Video/VTB.h"
#include "cores/VideoPlayer/Process/ios/ProcessInfoIOS.h"
#include "cores/VideoPlayer/VideoRenderers/RenderFactory.h"
#include "cores/VideoPlayer/VideoRenderers/LinuxRendererGLES.h"
#include "cores/VideoPlayer/VideoRenderers/HwDecRender/RendererVTBGLES.h"
#include "utils/log.h"
#include "filesystem/SpecialProtocol.h"
#include "settings/DisplaySettings.h"
#include "windowing/GraphicContext.h"
#include "messaging/ApplicationMessenger.h"
#include "guilib/Texture.h"
#include "utils/StringUtils.h"
#include "guilib/DispResource.h"
#include "threads/SingleLock.h"
#include "VideoSyncIos.h"
#include <vector>

#import <Foundation/Foundation.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <QuartzCore/CADisplayLink.h>

#import "platform/darwin/ios/XBMCController.h"
#import "platform/darwin/ios/IOSScreenManager.h"
#include "platform/darwin/DarwinUtils.h"
#include "settings/Settings.h"
#import <dlfcn.h>

#define CONST_TOUCHSCREEN "Touchscreen"
#define CONST_EXTERNAL "External"

// IOSDisplayLinkCallback is declared in the lower part of the file
@interface IOSDisplayLinkCallback : NSObject
{
@private CVideoSyncIos *_videoSyncImpl;
}
@property (nonatomic, setter=SetVideoSyncImpl:) CVideoSyncIos *_videoSyncImpl;
- (void) runDisplayLink;
@end

using namespace KODI;
using namespace MESSAGING;

struct CADisplayLinkWrapper
{
  CADisplayLink* impl;
  IOSDisplayLinkCallback *callbackClass;
};

std::unique_ptr<CWinSystemBase> CWinSystemBase::CreateWinSystem()
{
  std::unique_ptr<CWinSystemBase> winSystem(new CWinSystemIOS());
  return winSystem;
}

int CWinSystemIOS::GetDisplayIndexFromSettings()
{
  std::string currentScreen = CServiceBroker::GetSettings().GetString(CSettings::SETTING_VIDEOSCREEN_MONITOR);
  
  int screenIdx = 0;
  if (currentScreen == CONST_EXTERNAL)
  {
    if ([[UIScreen screens] count] > 1)
    {
      screenIdx = 1;
    }
    else// screen 1 is setup but not connected
    {
      // force internal screen
      CDisplaySettings::GetInstance().SetMonitor(CONST_TOUCHSCREEN);
      UpdateResolutions();
    }
  }
  
  return screenIdx;
}

CWinSystemIOS::CWinSystemIOS() : CWinSystemBase()
{
  m_iVSyncErrors = 0;
  m_bIsBackgrounded = false;
  m_pDisplayLink = new CADisplayLinkWrapper;
  m_pDisplayLink->callbackClass = [[IOSDisplayLinkCallback alloc] init];
  m_winEvents.reset(new CWinEventsIOS());

  CAESinkDARWINIOS::Register();
}

CWinSystemIOS::~CWinSystemIOS()
{
  [m_pDisplayLink->callbackClass release];
  delete m_pDisplayLink;
}

bool CWinSystemIOS::InitWindowSystem()
{
	return CWinSystemBase::InitWindowSystem();
}

bool CWinSystemIOS::DestroyWindowSystem()
{
  return true;
}

bool CWinSystemIOS::CreateNewWindow(const std::string& name, bool fullScreen, RESOLUTION_INFO& res)
{
  //NSLog(@"%s", __PRETTY_FUNCTION__);

  if(!SetFullScreen(fullScreen, res, false))
    return false;

  [g_xbmcController setFramebuffer];

  m_bWindowCreated = true;

  m_eglext  = " ";

  const char *tmpExtensions = (const char*) glGetString(GL_EXTENSIONS);
  if (tmpExtensions != NULL)
  {
    m_eglext += tmpExtensions;
  }

  m_eglext += " ";

  CLog::Log(LOGDEBUG, "EGL_EXTENSIONS:%s", m_eglext.c_str());

  // register platform dependent objects
  CDVDFactoryCodec::ClearHWAccels();
  VTB::CDecoder::Register();
  VIDEOPLAYER::CRendererFactory::ClearRenderer();
  CLinuxRendererGLES::Register();
  CRendererVTB::Register();
  VIDEOPLAYER::CProcessInfoIOS::Register();
  RETRO::CRPProcessInfoIOS::Register();
  RETRO::CRPProcessInfoIOS::RegisterRendererFactory(new RETRO::CRendererFactoryOpenGLES);

  return true;
}

bool CWinSystemIOS::DestroyWindow()
{
  return true;
}

bool CWinSystemIOS::ResizeWindow(int newWidth, int newHeight, int newLeft, int newTop)
{
  //NSLog(@"%s", __PRETTY_FUNCTION__);

  if (m_nWidth != newWidth || m_nHeight != newHeight)
  {
    m_nWidth  = newWidth;
    m_nHeight = newHeight;
  }

  CRenderSystemGLES::ResetRenderSystem(newWidth, newHeight);

  return true;
}

bool CWinSystemIOS::SetFullScreen(bool fullScreen, RESOLUTION_INFO& res, bool blankOtherDisplays)
{
  //NSLog(@"%s", __PRETTY_FUNCTION__);

  m_nWidth      = res.iWidth;
  m_nHeight     = res.iHeight;
  m_bFullScreen = fullScreen;

  CLog::Log(LOGDEBUG, "About to switch to %i x %i on screen %i",m_nWidth, m_nHeight, res.iScreen);
  SwitchToVideoMode(res.iWidth, res.iHeight, res.fRefreshRate);
  CRenderSystemGLES::ResetRenderSystem(res.iWidth, res.iHeight);

  return true;
}

UIScreenMode *getModeForResolution(int width, int height, unsigned int screenIdx)
{
  UIScreen *aScreen = [[UIScreen screens]objectAtIndex:screenIdx];
  for ( UIScreenMode *mode in [aScreen availableModes] )
  {
    //for main screen also find modes where width and height are
    //exchanged (because of the 90°degree rotated buildinscreens)
    if((mode.size.width == width && mode.size.height == height) ||
        (screenIdx == 0 && mode.size.width == height && mode.size.height == width)
       || screenIdx == 0) // for screenIdx == 0 - which is the mainscreen - we only have one resolution - match it every time
    {
      CLog::Log(LOGDEBUG,"Found matching mode");
      return mode;
    }
  }
  CLog::Log(LOGERROR,"No matching mode found!");
  return NULL;
}

bool CWinSystemIOS::SwitchToVideoMode(int width, int height, double refreshrate)
{
  bool ret = false;
  int screenIdx = GetDisplayIndexFromSettings();

  //get the mode to pass to the controller
  UIScreenMode *newMode = getModeForResolution(width, height, screenIdx);

  if(newMode)
  {
    ret = [g_xbmcController changeScreen:screenIdx withMode:newMode];
  }
  return ret;
}

bool CWinSystemIOS::GetScreenResolution(int* w, int* h, double* fps, int screenIdx)
{
  UIScreen *screen = [[UIScreen screens] objectAtIndex:screenIdx];
  CGSize screenSize = [screen currentMode].size;
  *w = screenSize.width;
  *h = screenSize.height;
  *fps = 0.0;
  //if current mode is 0x0 (happens with external screens which aren't active)
  //then use the preferred mode
  if(*h == 0 || *w ==0)
  {
    UIScreenMode *firstMode = [screen preferredMode];
    *w = firstMode.size.width;
    *h = firstMode.size.height;
  }

  //for mainscreen exchange w and h
  //because mainscreen is build in
  //in 90° rotated
  if(screenIdx == 0)
  {
    int tmp = *w;
    *w = *h;
    *h = tmp;
  }
  CLog::Log(LOGDEBUG,"Current resolution Screen: %i with %i x %i",screenIdx, *w, *h);
  return true;
}

void CWinSystemIOS::UpdateResolutions()
{
  // Add display resolution
  int w, h;
  double fps;
  CWinSystemBase::UpdateResolutions();

  int screenIdx = GetDisplayIndexFromSettings();

  //first screen goes into the current desktop mode
  if(GetScreenResolution(&w, &h, &fps, screenIdx))
  {
    UpdateDesktopResolution(CDisplaySettings::GetInstance().GetResolutionInfo(RES_DESKTOP), 0, w, h, fps);
    CDisplaySettings::GetInstance().GetResolutionInfo(RES_DESKTOP).strOutput = screenIdx == 0 ? CONST_TOUCHSCREEN : CONST_EXTERNAL;
  }
  
  CDisplaySettings::GetInstance().ClearCustomResolutions();
  
  //now just fill in the possible resolutions for the attached screens
  //and push to the resolution info vector
  FillInVideoModes(screenIdx);
}

void CWinSystemIOS::FillInVideoModes(int screenIdx)
{
  // Add full screen settings for additional monitors
  RESOLUTION_INFO res;
  int w, h;
  // atm we don't get refreshrate info from iOS
  // but this may change in the future. In that case
  // we will adapt this code for filling some
  // useful info into this local var :)
  double refreshrate = 0.0;
  //screen 0 is mainscreen - 1 has to be the external one...
  UIScreen *aScreen = [[UIScreen screens]objectAtIndex:screenIdx];
  //found external screen
  for ( UIScreenMode *mode in [aScreen availableModes] )
  {
    w = mode.size.width;
    h = mode.size.height;
    
    if (screenIdx == 0)
    {
      res.strOutput = CONST_TOUCHSCREEN;
    }
    else
    {
      res.strOutput = CONST_EXTERNAL;
    }
    
    UpdateDesktopResolution(res, 0, w, h, refreshrate);
    CLog::Log(LOGNOTICE, "Found possible resolution for display %d with %d x %d\n", screenIdx, w, h);

    //overwrite the mode str because  UpdateDesktopResolution adds a
    //"Full Screen". Since the current resolution is there twice
    //this would lead to 2 identical resolution entrys in the guisettings.xml.
    //That would cause problems with saving screen overscan calibration
    //because the wrong entry is picked on load.
    //So we just use UpdateDesktopResolutions for the current DESKTOP_RESOLUTIONS
    //in UpdateResolutions. And on all other resolutions make a unique
    //mode str by doing it without appending "Full Screen".
    //this is what linux does - though it feels that there shouldn't be
    //the same resolution twice... - thats why i add a FIXME here.
    res.strMode = StringUtils::Format("%dx%d @ %.2f", w, h, refreshrate);

    CServiceBroker::GetWinSystem()->GetGfxContext().ResetOverscan(res);
    CDisplaySettings::GetInstance().AddResolutionInfo(res);
  }
}

bool CWinSystemIOS::IsExtSupported(const char* extension) const
{
  if(strncmp(extension, "EGL_", 4) != 0)
    return CRenderSystemGLES::IsExtSupported(extension);

  std::string name;

  name  = " ";
  name += extension;
  name += " ";

  return m_eglext.find(name) != std::string::npos;
}

bool CWinSystemIOS::BeginRender()
{
  bool rtn;

  [g_xbmcController setFramebuffer];

  rtn = CRenderSystemGLES::BeginRender();
  return rtn;
}

bool CWinSystemIOS::EndRender()
{
  bool rtn;

  rtn = CRenderSystemGLES::EndRender();
  return rtn;
}

void CWinSystemIOS::Register(IDispResource *resource)
{
  CSingleLock lock(m_resourceSection);
  m_resources.push_back(resource);
}

void CWinSystemIOS::Unregister(IDispResource* resource)
{
  CSingleLock lock(m_resourceSection);
  std::vector<IDispResource*>::iterator i = find(m_resources.begin(), m_resources.end(), resource);
  if (i != m_resources.end())
    m_resources.erase(i);
}

void CWinSystemIOS::OnAppFocusChange(bool focus)
{
  CSingleLock lock(m_resourceSection);
  m_bIsBackgrounded = !focus;
  CLog::Log(LOGDEBUG, "CWinSystemIOS::OnAppFocusChange: %d", focus ? 1 : 0);
  for (std::vector<IDispResource *>::iterator i = m_resources.begin(); i != m_resources.end(); i++)
    (*i)->OnAppFocusChange(focus);
}

//--------------------------------------------------------------
//-------------------DisplayLink stuff
@implementation IOSDisplayLinkCallback
@synthesize _videoSyncImpl;
//--------------------------------------------------------------
- (void) runDisplayLink
{
  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
  if (_videoSyncImpl != nil)
  {
    _videoSyncImpl->IosVblankHandler();
  }
  [pool release];
}
@end

bool CWinSystemIOS::InitDisplayLink(CVideoSyncIos *syncImpl)
{
  //init with the appropriate display link for the
  //used screen
  if([[IOSScreenManager sharedInstance] isExternalScreen])
  {
    fprintf(stderr,"InitDisplayLink on external");
  }
  else
  {
    fprintf(stderr,"InitDisplayLink on internal");
  }

  unsigned int currentScreenIdx = [[IOSScreenManager sharedInstance] GetScreenIdx];
  UIScreen * currentScreen = [[UIScreen screens] objectAtIndex:currentScreenIdx];
  [m_pDisplayLink->callbackClass SetVideoSyncImpl:syncImpl];
  m_pDisplayLink->impl = [currentScreen displayLinkWithTarget:m_pDisplayLink->callbackClass selector:@selector(runDisplayLink)];

  [m_pDisplayLink->impl setFrameInterval:1];
  [m_pDisplayLink->impl addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
  return m_pDisplayLink->impl != nil;
}

void CWinSystemIOS::DeinitDisplayLink(void)
{
  if (m_pDisplayLink->impl)
  {
    [m_pDisplayLink->impl invalidate];
    m_pDisplayLink->impl = nil;
    [m_pDisplayLink->callbackClass SetVideoSyncImpl:nil];
  }
}
//------------DisplayLink stuff end
//--------------------------------------------------------------

void CWinSystemIOS::PresentRenderImpl(bool rendered)
{
  //glFlush;
  if (rendered)
    [g_xbmcController presentFramebuffer];
}

void CWinSystemIOS::SetVSyncImpl(bool enable)
{
  #if 0
    // set swapinterval if possible
    void *eglSwapInterval;
    eglSwapInterval = dlsym( RTLD_DEFAULT, "eglSwapInterval" );
    if ( eglSwapInterval )
    {
      ((void(*)(int))eglSwapInterval)( 1 ) ;
    }
  #endif
  m_iVSyncMode = 10;
}

void CWinSystemIOS::ShowOSMouse(bool show)
{
}

bool CWinSystemIOS::HasCursor()
{
  // apple touch devices
  return false;
}

void CWinSystemIOS::NotifyAppActiveChange(bool bActivated)
{
  if (bActivated && m_bWasFullScreenBeforeMinimize && !CServiceBroker::GetWinSystem()->GetGfxContext().IsFullScreenRoot())
    CApplicationMessenger::GetInstance().PostMsg(TMSG_TOGGLEFULLSCREEN);
}

bool CWinSystemIOS::Minimize()
{
  m_bWasFullScreenBeforeMinimize = CServiceBroker::GetWinSystem()->GetGfxContext().IsFullScreenRoot();
  if (m_bWasFullScreenBeforeMinimize)
    CApplicationMessenger::GetInstance().PostMsg(TMSG_TOGGLEFULLSCREEN);

  return true;
}

bool CWinSystemIOS::Restore()
{
  return false;
}

bool CWinSystemIOS::Hide()
{
  return true;
}

bool CWinSystemIOS::Show(bool raise)
{
  return true;
}

void* CWinSystemIOS::GetEAGLContextObj()
{
  return [g_xbmcController getEAGLContextObj];
}

void CWinSystemIOS::GetConnectedOutputs(std::vector<std::string> *outputs)
{
  outputs->push_back("Default");
  outputs->push_back(CONST_TOUCHSCREEN);
  if ([[UIScreen screens] count] > 1)
  {
    outputs->push_back(CONST_EXTERNAL);
  }
}

std::unique_ptr<CVideoSync> CWinSystemIOS::GetVideoSync(void *clock)
{
  std::unique_ptr<CVideoSync> pVSync(new CVideoSyncIos(clock, *this));
  return pVSync;
}

bool CWinSystemIOS::MessagePump()
{
  return m_winEvents->MessagePump();
}
