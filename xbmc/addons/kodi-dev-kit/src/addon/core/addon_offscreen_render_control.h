/*
 *  Copyright (C) 2005-2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "../../../include/kodi/c-api/addon_base.h"
#include "../../shared/SharedGroups.h"

namespace KODI_ADDON
{
namespace INTERNAL
{

class IAddonOffscreenRenderControl
{
public:
  IAddonOffscreenRenderControl(KODI::ADDONS::INTERFACE::PtrKodiOffscreen kodiOffsetHdl,
                               uint64_t renderId,
                               int x,
                               int y,
                               int width,
                               int height,
                               ADDON_HARDWARE_CONTEXT context)
    : m_kodiOffsetHdl(kodiOffsetHdl),
      m_renderId(renderId),
      m_x(x),
      m_y(y),
      m_width(width),
      m_height(height),
      m_context(context)
  {

  }
  virtual ~IAddonOffscreenRenderControl() = default;

  virtual bool Start() = 0;
  virtual void Stop() = 0;
  virtual void RenderBegin() = 0;
  virtual void RenderEnd() = 0;

  const KODI::ADDONS::INTERFACE::PtrKodiOffscreen m_kodiOffsetHdl;
  const uint64_t m_renderId;
  const int m_x;
  const int m_y;
  const int m_width;
  const int m_height;
  const ADDON_HARDWARE_CONTEXT m_context;
};

IAddonOffscreenRenderControl* GenerateOffscreenControl(KODI::ADDONS::INTERFACE::PtrKodiOffscreen kodiOffsetHdl,
                                                       uint64_t renderId,
                                                       int x,
                                                       int y,
                                                       int width,
                                                       int height,
                                                       ADDON_HARDWARE_CONTEXT context);

} /* namespace INTERNAL */
} /* namespace KODI_ADDON */
