/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "addon_offscreen_render_control.h"
#include "addon_offscreen_render_control_linux.h"

namespace KODI_ADDON
{
namespace INTERNAL
{

IAddonOffscreenRenderControl* GenerateOffscreenControl(KODI::ADDONS::INTERFACE::PtrKodiOffscreen kodiOffsetHdl,
                                                       uint64_t renderId,
                                                       int x,
                                                       int y,
                                                       int width,
                                                       int height,
                                                       ADDON_HARDWARE_CONTEXT context)
{
  CAddonOffscreenRenderControlLinux* control = new CAddonOffscreenRenderControlLinux(kodiOffsetHdl, renderId, x, y, width, height, context);
  return control;
}

} /* namespace INTERNAL */
} /* namespace KODI_ADDON */
