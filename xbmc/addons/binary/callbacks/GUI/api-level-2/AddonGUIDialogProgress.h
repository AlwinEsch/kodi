#pragma once
/*
 *      Copyright (C) 2015 Team KODI
 *      http://kodi.tv
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
 *  along with KODI; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "../../../../../../addons/library.kodi.guilib/gui.api2/GUILib.h"

namespace GUILIB
{
namespace V2
{

  struct CAddonGUI_Dialog_Progress
  {
    static void Init(CB_GUILib *callbacks);

    static GUIHANDLE    New(void *addonData);
    static void         Delete(void *addonData, GUIHANDLE handle);
    static void         Open(void *addonData, GUIHANDLE handle);
    static void         SetHeading(void *addonData, GUIHANDLE handle, const char *heading);
    static void         SetLine(void *addonData, GUIHANDLE handle, unsigned int iLine, const char *line);
    static void         SetCanCancel(void *addonData, GUIHANDLE handle, bool bCanCancel);
    static bool         IsCanceled(void *addonData, GUIHANDLE handle);
    static void         SetPercentage(void *addonData, GUIHANDLE handle, int iPercentage);
    static int          GetPercentage(void *addonData, GUIHANDLE handle);
    static void         ShowProgressBar(void *addonData, GUIHANDLE handle, bool bOnOff);
    static void         SetProgressMax(void *addonData, GUIHANDLE handle, int iMax);
    static void         SetProgressAdvance(void *addonData, GUIHANDLE handle, int nSteps);
    static bool         Abort(void *addonData, GUIHANDLE handle);
  };

}; /* namespace V2 */
}; /* namespace GUILIB */
