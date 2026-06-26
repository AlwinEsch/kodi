/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "SlideShowPicture.h"

class CTexture;

class CSlideShowPicVulkan : public CSlideShowPic
{
public:
  CSlideShowPicVulkan() = default;
  ~CSlideShowPicVulkan() override = default;

protected:
  void Render(float* x, float* y, CTexture* pTexture, KODI::UTILS::COLOR::Color color) override;
};
