/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "utils/IScreenshotSurface.h"

#include <memory>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

class CScreenshotSurface : public IScreenshotSurface
{
public:
  static void Register();
  static std::unique_ptr<IScreenshotSurface> CreateSurface();

  bool Capture() override;
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
