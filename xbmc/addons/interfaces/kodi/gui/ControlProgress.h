#pragma once
/*
 *      Copyright (C) 2015-2016 Team KODI
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

struct sFuncTable_Addon;

namespace ADDON
{
extern "C"
{

  struct Interface_GUIControlProgress
  {
  public:
    static void Init(sFuncTable_Addon* funcTable);

    static void SetVisible(void* kodiInstance, void* handle, bool visible);

    static void SetPercentage(void* kodiInstance, void* handle, float percent);
    static float GetPercentage(void* kodiInstance, void* handle);
  };

} /* extern "C" */
} /* namespace ADDON */
