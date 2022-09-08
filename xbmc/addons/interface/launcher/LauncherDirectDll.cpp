/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "LauncherDirectDll.h"

#include "addons/IAddon.h"
#include "filesystem/File.h"
#include "filesystem/SpecialProtocol.h"
#include "utils/URIUtils.h"
#include "utils/Variant.h"
#include "utils/log.h"

using namespace std::chrono_literals;

namespace KODI
{
namespace ADDONS
{
namespace INTERFACE
{

CLauncherDirectDll::CLauncherDirectDll(std::shared_ptr<ADDON::IAddon> addon,
                                       const std::string& uuid)
  : ILauncher(addon, uuid), CThread(addon->ID().c_str())
{
}

CLauncherDirectDll::~CLauncherDirectDll()
{
  Kill(true);
}

bool CLauncherDirectDll::Launch(const std::vector<char*>& argv, bool waitForExit)
{
  std::string strFileName = argv[0];
  if (strFileName.empty())
    return false;

  m_argv = argv;

  /* Load the Dll */
  if (!m_pDll)
  {
    m_pDll = new DllAddon;
    m_pDll->SetFile(strFileName);
    m_pDll->EnableDelayedUnload(false);
    if (!m_pDll->Load())
    {
      delete m_pDll;
      m_pDll = nullptr;

      return false;
    }
  }

  if (SupportNonBlocking())
  {
    int ret = m_pDll->non_blocking_main_start(m_argv.size(), m_argv.data());
    if (ret != 0)
      return false;
  }
  else if (waitForExit)
  {
    int ret = m_pDll->main(m_argv.size(), m_argv.data());
    if (ret != 0)
      return false;
  }
  else
  {
    CThread::Create();
    while (!m_startDone)
    {
      CThread::Sleep(1ms);
    }
    CThread::Sleep(10ms);
  }

  return true;
}

bool CLauncherDirectDll::Kill(bool wait)
{
  if (!m_pDll)
    return true;

  if (SupportNonBlocking())
  {
    m_pDll->non_blocking_main_stop();
  }
  else
  {
    CThread::StopThread();
  }

  /* Unload library file */
  if (m_pDll)
  {
    m_pDll->Unload();
    delete m_pDll;
    m_pDll = nullptr;
  }

  return true;
}

ChildStatus CLauncherDirectDll::ProcessActive()
{
  return m_pDll ? ChildStatus::Running : ChildStatus::ExitedNormal;
}

std::string CLauncherDirectDll::GetDllPath(const std::string& libPath)
{
  std::string strFileName = libPath;
  std::string strLibName = URIUtils::GetFileName(strFileName);

  if (strLibName.empty())
    return "";

    /* Check if lib being loaded exists, else check in XBMC binary location */
#if defined(TARGET_ANDROID)
  if (XFILE::CFile::Exists(strFileName))
  {
    bool doCopy = true;
    std::string dstfile = URIUtils::AddFileToFolder(
        CSpecialProtocol::TranslatePath("special://xbmcaltbinaddons/"), strLibName);

    struct __stat64 dstFileStat;
    if (XFILE::CFile::Stat(dstfile, &dstFileStat) == 0)
    {
      struct __stat64 srcFileStat;
      if (XFILE::CFile::Stat(strFileName, &srcFileStat) == 0)
      {
        if (dstFileStat.st_size == srcFileStat.st_size &&
            dstFileStat.st_mtime > srcFileStat.st_mtime)
          doCopy = false;
      }
    }

    if (doCopy)
    {
      CLog::Log(LOGDEBUG, "ADDON: caching {} to {}", strFileName, dstfile);
      XFILE::CFile::Copy(strFileName, dstfile);
    }

    strFileName = dstfile;
  }
  if (!XFILE::CFile::Exists(strFileName))
  {
    std::string tempbin = getenv("KODI_ANDROID_LIBS");
    strFileName = tempbin + "/" + strLibName;
  }
#endif

  if (!XFILE::CFile::Exists(strFileName))
  {
    std::string strAltFileName;

    std::string altbin = CSpecialProtocol::TranslatePath("special://xbmcaltbinaddons/");
    if (!altbin.empty())
    {
      strAltFileName = altbin + strLibName;
      if (!XFILE::CFile::Exists(strAltFileName))
      {
        std::string temp = CSpecialProtocol::TranslatePath("special://xbmc/addons/");
        strAltFileName = strFileName;
        strAltFileName.erase(0, temp.size());
        strAltFileName = altbin + strAltFileName;
      }
      CLog::Log(LOGDEBUG, "ADDON: Trying to load {}", strAltFileName);
    }

    if (XFILE::CFile::Exists(strAltFileName))
      strFileName = strAltFileName;
    else
    {
      std::string temp = CSpecialProtocol::TranslatePath("special://xbmc/");
      std::string tempbin = CSpecialProtocol::TranslatePath("special://xbmcbin/");
      strFileName.erase(0, temp.size());
      strFileName = tempbin + strFileName;
      if (!XFILE::CFile::Exists(strFileName))
      {
        CLog::Log(LOGERROR, "ADDON: Could not locate {}", strLibName);
        strFileName.clear();
      }
    }
  }

  return strFileName;
}

void CLauncherDirectDll::Process()
{
  m_startDone = true;
  m_pDll->main(m_argv.size(), m_argv.data());
}

bool CLauncherDirectDll::SupportNonBlocking()
{
  return m_pDll->non_blocking_main_start_available();
}

bool CLauncherDirectDll::SupportNonBlocking(const std::string& path)
{
  /* Load the Dll */
  if (!m_pDll)
  {
    m_pDll = new DllAddon;
    m_pDll->SetFile(path);
    m_pDll->EnableDelayedUnload(false);
    if (!m_pDll->Load())
    {
      delete m_pDll;
      m_pDll = nullptr;

      return false;
    }
  }

  return SupportNonBlocking();
}

} /* namespace INTERFACE */
} /* namespace ADDONS */
} /* namespace KODI */
