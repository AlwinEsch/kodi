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

class CVulkanGUIFontTTF : public CGUIFontTTF
{
public:
  explicit CVulkanGUIFontTTF(const std::string& fontIdent);
  ~CVulkanGUIFontTTF() override;

  bool FirstBegin() override;
  void LastEnd() override;

  CVertexBuffer CreateVertexBuffer(const std::vector<SVertex>& vertices) const override;
  void DestroyVertexBuffer(CVertexBuffer& bufferHandle) const override;
  static void CreateStaticIndexBuffers();
  static void DestroyStaticIndexBuffers();

protected:
  std::unique_ptr<CTexture> ReallocTexture(unsigned int& newHeight) override;
  bool CopyCharToTexture(FT_BitmapGlyph bitGlyph,
                         unsigned int x1,
                         unsigned int y1,
                         unsigned int x2,
                         unsigned int y2) override;
  void DeleteHardwareTexture() override;

private:
  /**
   * @brief Creates a Vulkan vertex buffer.
   *
   * @param ref Pointer to the CVulkanGUIFontTTF instance.
   * @param vertices Vector of vertices to be copied to the buffer.
   * @return A CVertexBuffer object representing the created vertex buffer.
   *
   * @note Needs to be static because it is called from the base class CGUIFontTTF, which defined as const.
   * The constness of the base class method prevents it from calling non-static member functions of the derived class.
   */
  static CVertexBuffer VulkanCreateVertexBuffer(CVulkanGUIFontTTF* ref,
                                                const std::vector<SVertex>& vertices);

  /**
   * @brief Destroys a Vulkan vertex buffer.
   *
   * @param ref Pointer to the CVulkanGUIFontTTF instance.
   * @param bufferHandle The CVertexBuffer object representing the vertex buffer to be destroyed.
   *
   * @note Needs to be static because it is called from the base class CGUIFontTTF, which defined as const.
   */
  static void VulkanDestroyVertexBuffer(CVulkanGUIFontTTF* ref, CVertexBuffer& bufferHandle);

  std::unique_ptr<KODI::RENDERING::VULKAN::CVulkanShaderFonts> m_shader;

  VkSampler m_sampler{VK_NULL_HANDLE};
  VkImage m_image{VK_NULL_HANDLE};
  VkImageView m_view{VK_NULL_HANDLE};
  VkDeviceMemory m_imageMemory{VK_NULL_HANDLE};
  VkDescriptorPool m_descriptorPool{VK_NULL_HANDLE};
  VkDescriptorSet m_descriptorSet{VK_NULL_HANDLE};
  VkDescriptorSetLayout m_descriptorSetLayout{VK_NULL_HANDLE};
  VkPipelineLayout m_pipelineLayout{VK_NULL_HANDLE};
  VkPipeline m_pipeline{VK_NULL_HANDLE};

	// Passed from the sample
  VkRenderPass m_renderPass{VK_NULL_HANDLE};
  VkQueue m_queue{VK_NULL_HANDLE};

  KODI::RENDERING::VULKAN::CVulkanRenderSystem* m_renderSystem{nullptr};

  unsigned int m_updateY1{0};
  unsigned int m_updateY2{0};

  enum TextureStatus
  {
    TEXTURE_VOID = 0,
    TEXTURE_READY,
    TEXTURE_REALLOCATED,
    TEXTURE_UPDATED,
  };

  TextureStatus m_textureStatus{TEXTURE_VOID};

  struct StaticIndexBuffer
  {
    bool created{false};
    VkBuffer buffer{VK_NULL_HANDLE};
    VmaAllocation allocation{VK_NULL_HANDLE};
  };

  static StaticIndexBuffer m_staticIndexBuffer;
};

} // namespace VULKAN
} // namespace GRAPHICS
} // namespace GUILIB
} // namespace KODI
