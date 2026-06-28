/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <stdint.h>

namespace KODI
{
namespace UTILS
{
namespace RENDER
{

enum class ColorChannel
{
  A,
  R,
  G,
  B,
};

uint8_t GetChannelFromARGB(const ColorChannel colorChannel, const uint32_t argb);

} // RENDER
} // UTILS
} // KODI
