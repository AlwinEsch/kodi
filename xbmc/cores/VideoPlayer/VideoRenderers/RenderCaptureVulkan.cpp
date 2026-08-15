/*
 *  Copyright (C) 2005-2021 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "RenderCaptureVulkan.h"

CRenderCaptureVulkan::~CRenderCaptureVulkan()
{
  delete[] m_pixels;
}

void CRenderCaptureVulkan::BeginRender()
{
  if (m_bufferSize != m_width * m_height * 4)
  {
    delete[] m_pixels;
    m_bufferSize = m_width * m_height * 4;
    m_pixels = new uint8_t[m_bufferSize];
  }
}

void CRenderCaptureVulkan::EndRender()
{
  SetState(CAPTURESTATE_DONE);
}
