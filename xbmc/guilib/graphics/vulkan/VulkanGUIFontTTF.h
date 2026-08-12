/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "guilib/GUIFontTTF.h"

#include <memory>
#include <string>
#include <vector>

#include <vulkan/vulkan_core.h>

namespace KODI::RENDERING::VULKAN
{
class CVulkanRenderSystem;
class CVulkanDeviceQueue;
class CVulkanShaderFonts;
struct VulkanData;
} // namespace KODI::RENDERING::VULKAN

namespace KODI::GUILIB::GRAPHICS::VULKAN
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
  // Vulkan resources, becomes set during constuction.
  KODI::RENDERING::VULKAN::CVulkanRenderSystem* m_renderSystem;
  KODI::RENDERING::VULKAN::CVulkanDeviceQueue* m_deviceQueue;
  KODI::RENDERING::VULKAN::CVulkanShaderFonts* m_shaderFonts;
  const KODI::RENDERING::VULKAN::VulkanData* m_vkData;

  /**
   * @brief Creates the texture resources
   *
   * @note Destroy becomes done in the virtual @ref DeleteHardwareTexture() and is called
   * when the texture is reallocated or the font is destroyed.
   */
  void CreateTextureResources();

  /**
   * @brief Sets the content of the image.
   *
   * This function is used to update the content of the texture image in Vulkan.
   * It is called when a character is copied to the texture.
   *
   * @param y1 The starting y-coordinate.
   * @param y2 The ending y-coordinate.
   * @param width The width of the image.
   * @param height The height of the image.
   * @param imageData A pointer to the image data.
   *
   * @note @ref CreateTextureResources() needs to be called before this function to
   * ensure that the texture resources are created.
   */
  void SetImageContent(
      int32_t y1, int32_t y2, uint32_t width, uint32_t height, const void* imageData);

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

} // namespace KODI::GUILIB::GRAPHICS::VULKAN
