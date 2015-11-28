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

  struct CB_GUILib;

  struct CAddonGUI_Control_Slider
  {
    static void Init(CB_GUILib *callbacks);

    static void SetVisible(void *addonData, GUIHANDLE handle, bool yesNo);
    static void GetDescription(void *addonData, GUIHANDLE handle, char &text, unsigned int &iMaxStringSize);
    static void SetIntRange(void *addonData, GUIHANDLE handle, int iStart, int iEnd);
    static void SetIntValue(void *addonData, GUIHANDLE handle, int iValue);
    static int GetIntValue(void *addonData, GUIHANDLE handle);
    static void SetIntInterval(void *addonData, GUIHANDLE handle, int iInterval);
    static void SetPercentage(void *addonData, GUIHANDLE handle, float fPercent);
    static float GetPercentage(void *addonData, GUIHANDLE handle);
    static void SetFloatRange(void *addonData, GUIHANDLE handle, float fStart, float fEnd);
    static void SetFloatValue(void *addonData, GUIHANDLE handle, float fValue);
    static float GetFloatValue(void *addonData, GUIHANDLE handle);
    static void SetFloatInterval(void *addonData, GUIHANDLE handle, float fInterval);
  };

}; /* namespace V2 */
}; /* namespace GUILIB */
