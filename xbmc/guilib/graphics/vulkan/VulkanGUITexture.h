/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "guilib/GUITexture.h"
#include "system_vulkan.h"
#include "utils/ColorUtils.h"

#include <array>
#include <vector>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{
class CVulkanRenderSystem;
} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI

namespace KODI
{
namespace GUILIB
{
namespace GRAPHICS
{
namespace VULKAN
{

struct PackedVertex
{
  float x, y, z;
  float u1, v1;
  float u2, v2;
};
typedef std::vector<PackedVertex> PackedVertices;

class CVulkanGUITexture : public CGUITexture
{
public:
  static void Register();
  static CGUITexture* CreateTexture(
      float posX, float posY, float width, float height, const CTextureInfo& texture);

  static void DrawQuad(const CRect& coords,
                       KODI::UTILS::COLOR::Color color,
                       CTexture* texture = nullptr,
                       const CRect* texCoords = nullptr,
                       const float depth = 1.0,
                       const bool blending = true);

  CVulkanGUITexture(
      float posX, float posY, float width, float height, const CTextureInfo& texture);
  ~CVulkanGUITexture() override = default;

  CVulkanGUITexture* Clone() const override;

protected:
  void Begin(KODI::UTILS::COLOR::Color color) override;
  void Draw(float* x,
            float* y,
            float* z,
            const CRect& texture,
            const CRect& diffuse,
            int orientation) override;
  void End() override;

private:
  CVulkanGUITexture(const CVulkanGUITexture& texture) = default;

  std::array<uint8_t, 4> m_col;

  PackedVertices m_packedVertices;
  std::vector<uint16_t> m_idx;
  KODI::RENDERING::VULKAN::CVulkanRenderSystem* m_renderSystem;
};

} // namespace VULKAN
} // namespace GRAPHICS
} // namespace GUILIB
} // namespace KODI
