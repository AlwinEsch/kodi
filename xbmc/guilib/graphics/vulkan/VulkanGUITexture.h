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

#include <glm/glm.hpp>

namespace KODI::RENDERING::VULKAN
{
class CVulkanRenderSystem;
class CVulkanShaderTexture;
class CVulkanDynamicBuffer;
class CVulkanDynamicBuffers;
struct Vertex;
} // KODI::RENDERING::VULKAN


namespace KODI::GUILIB::GRAPHICS::VULKAN
{

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

  CVulkanGUITexture(float posX, float posY, float width, float height, const CTextureInfo& texture);
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

  glm::vec4 m_color{1.0f, 1.0f, 1.0f, 1.0f};

  std::vector<KODI::RENDERING::VULKAN::Vertex> m_packedVertices;
  std::vector<uint32_t> m_idx;
  KODI::RENDERING::VULKAN::CVulkanRenderSystem* m_renderSystem;
  KODI::RENDERING::VULKAN::CVulkanShaderTexture* m_shaderTexture;
  KODI::RENDERING::VULKAN::CVulkanDynamicBuffers* m_dynamicBuffers;
  KODI::RENDERING::VULKAN::CVulkanDynamicBuffer* m_uniformBuffer;
  KODI::RENDERING::VULKAN::CVulkanDynamicBuffer* m_vertexBuffer;
  KODI::RENDERING::VULKAN::CVulkanDynamicBuffer* m_indexBuffer;
};

} // namespace KODI::GUILIB::GRAPHICS::VULKAN
