/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "RenderUtils.h"

#include <stdexcept>

uint8_t KODI::UTILS::RENDER::GetChannelFromARGB(
    const KODI::UTILS::RENDER::ColorChannel colorChannel, const uint32_t argb)
{
  switch (colorChannel)
  {
    case KODI::UTILS::RENDER::ColorChannel::A:
      return (argb >> 24) & 0xFF;
    case KODI::UTILS::RENDER::ColorChannel::R:
      return (argb >> 16) & 0xFF;
    case KODI::UTILS::RENDER::ColorChannel::G:
      return (argb >> 8) & 0xFF;
    case KODI::UTILS::RENDER::ColorChannel::B:
      return (argb >> 0) & 0xFF;
    default:
      throw std::runtime_error("KODI::UTILS::RENDER::GetChannelFromARGB: ColorChannel not handled");
  };
}
