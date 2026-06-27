/*
 *  Copyright (C) 2019-2025 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "cores/RetroPlayer/shaders/IShaderTexture.h"

#include "system_vulkan.h"

namespace KODI::SHADER
{

/*!
 * \brief Shader texture that wraps an external texture object
 *
 * NOTE: The lifetime of the external texture object must outlast this class.
 */
class CShaderTextureVulkanRef : public IShaderTexture
{
public:
  CShaderTextureVulkanRef(uint32_t textureWidth, uint32_t textureHeight, unsigned int texture = 0);
  ~CShaderTextureVulkanRef() override = default;

  // Implementation of IShaderTexture
  float GetWidth() const override;
  float GetHeight() const override;

  // OpenVulkan interface
  void BindToUnit(unsigned int unit);
  unsigned int GetTextureID() const { return m_texture; }

private:
  const uint32_t m_textureWidth;
  const uint32_t m_textureHeight;
  unsigned int m_texture{0};
};
} // namespace KODI::SHADER
