/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "BaseRenderBufferPool.h"
#include "IRenderBuffer.h"
#include "system_vulkan.h"

namespace KODI
{
namespace RETRO
{
class CRenderContext;
class CRenderVideoSettings;

class CRenderBufferPoolVulkan : public CBaseRenderBufferPool
{
public:
  CRenderBufferPoolVulkan(CRenderContext& context);
  ~CRenderBufferPoolVulkan() override = default;

  // Implementation of IRenderBufferPool via CBaseRenderBufferPool
  bool IsCompatible(const CRenderVideoSettings& renderSettings) const override;

protected:
  // Implementation of CBaseRenderBufferPool
  IRenderBuffer* CreateRenderBuffer(void* header = nullptr) override;
  bool ConfigureInternal() override;

private:
  // Construction parameters
  CRenderContext& m_context;

  // Configuration parameters
  unsigned int m_pixelType = 0;
  unsigned int m_internalFormat = 0;
  unsigned int m_pixelFormat = 0;
  unsigned int m_bpp = 0;
};
} // namespace RETRO
} // namespace KODI
