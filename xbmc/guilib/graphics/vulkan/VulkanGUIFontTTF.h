/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "guilib/GUIFontTTF.h"
#include "rendering/vulkan/shaders/VulkanShaderFonts.h"

#include <memory>
#include <string>
#include <vector>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

namespace KODI::RENDERING::VULKAN
{
class CVulkanRenderSystem;
class CVulkanDeviceQueue;
class CVulkanShaderFonts;
} // namespace KODI::RENDERING::VULKAN

namespace KODI
{
namespace GUILIB
{
namespace GRAPHICS
{
namespace VULKAN
{

class CVulkanGUIFontTTF : public CGUIFontTTF
{
public:
  explicit CVulkanGUIFontTTF(const std::string& fontIdent);
  ~CVulkanGUIFontTTF() override;

  bool FirstBegin() override;
  void LastEnd() override;

  CVertexBuffer CreateVertexBuffer(const std::vector<SVertex>& vertices) const override;
  void DestroyVertexBuffer(CVertexBuffer& bufferHandle) const override;

protected:
  std::unique_ptr<CTexture> ReallocTexture(unsigned int& newHeight) override;
  bool CopyCharToTexture(FT_BitmapGlyph bitGlyph,
                         unsigned int x1,
                         unsigned int y1,
                         unsigned int x2,
                         unsigned int y2) override;
  void DeleteHardwareTexture() override;

private:
  void CreateTextureResources();
  void SetImageContent(int32_t y1, int32_t y2, uint32_t width, uint32_t height, const void* imageData);

  KODI::RENDERING::VULKAN::CVulkanRenderSystem* m_renderSystem;
  KODI::RENDERING::VULKAN::CVulkanDeviceQueue* m_deviceQueue;
  KODI::RENDERING::VULKAN::CVulkanShaderFonts* m_shaderFonts;
  const KODI::RENDERING::VULKAN::VulkanData* m_vkData;

  VkSampler m_sampler{VK_NULL_HANDLE};
  VkImage m_image{VK_NULL_HANDLE};
  VkImageView m_imageView{VK_NULL_HANDLE};
  VkDeviceMemory m_imageMemory{VK_NULL_HANDLE};
  VkDescriptorSet m_descriptorSet{VK_NULL_HANDLE};

  enum TextureStatus
  {
    TEXTURE_VOID = 0,
    TEXTURE_READY,
    TEXTURE_REALLOCATED,
    TEXTURE_UPDATED,
  };

  TextureStatus m_textureStatus{TEXTURE_VOID};

  unsigned int m_updateY1{0};
  unsigned int m_updateY2{0};

  /**
   * @brief Determines if scissors can affect clipping.
   *
   * Values @ref m_clipFactor and @ref m_clipOffset are set by call of ScissorsCanEffectClipping.
   *
   * @return True if scissors can affect clipping, false otherwise.
   *
   * @note This function and @ref ClipRectToScissorRect are normally included on @ref CRenderSystemBase,
   * but as it only be used on fonts and Vulkan is a bit more complex than OpenGL, it is included here.
   */
  bool ScissorsCanEffectClipping();

  /**
   * @brief Clips a rectangle to the scissor rectangle.
   *
   * @param rect The rectangle to be clipped.
   * @return The clipped rectangle.
   */
  CRect ClipRectToScissorRect(const CRect& rect);

  bool m_scissorClip{false};
  glm::vec2 m_clipFactor{0.0f};
  glm::vec2 m_clipOffset{0.0f};
};

} // namespace VULKAN
} // namespace GRAPHICS
} // namespace GUILIB
} // namespace KODI
