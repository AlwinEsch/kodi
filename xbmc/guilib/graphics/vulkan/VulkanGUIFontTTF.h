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
class CVulkanDynamicBuffer;
class CVulkanShaderFonts;
struct VulkanMemoryData;
struct ShaderFontsVertex;
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
  void CreateTextureResources();
  void SetImageContent(int32_t y1, int32_t y2, uint32_t width, uint32_t height, const void* imageData);

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

  CRect ClipRectToScissorRect(const CRect& rect);
  static bool ScissorsCanEffectClipping(glm::vec2 &factor, glm::vec2 &offset);

  const KODI::RENDERING::VULKAN::VulkanData* m_vkData;
  std::unique_ptr<KODI::RENDERING::VULKAN::CVulkanShaderFonts> m_shader;

  VkSampler m_sampler{VK_NULL_HANDLE};
  VkImage m_image{VK_NULL_HANDLE};
  VkImageView m_imageView{VK_NULL_HANDLE};
  VkDeviceMemory m_imageMemory{VK_NULL_HANDLE};
  VkDescriptorPool m_descriptorPool{VK_NULL_HANDLE};
  VkDescriptorSet m_descriptorSet{VK_NULL_HANDLE};
  VkDescriptorSetLayout m_descriptorSetLayout{VK_NULL_HANDLE};
  VkPipelineLayout m_pipelineLayout{VK_NULL_HANDLE};
  VkPipeline m_pipeline{VK_NULL_HANDLE};

	// Passed from the sample
  VkRenderPass m_renderPass{VK_NULL_HANDLE};
  VkQueue m_queue{VK_NULL_HANDLE};

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

  using Vertex = KODI::RENDERING::VULKAN::ShaderFontsVertex;

  std::vector<Vertex> m_packedVertices;
  std::vector<uint32_t> m_idx;
  KODI::RENDERING::VULKAN::CVulkanRenderSystem* m_renderSystem;
  KODI::RENDERING::VULKAN::CVulkanShaderFonts* m_shaderFonts;
  KODI::RENDERING::VULKAN::CVulkanDynamicBuffer* m_uniformBuffer;
  KODI::RENDERING::VULKAN::CVulkanDynamicBuffer* m_vertexBuffer;
  KODI::RENDERING::VULKAN::CVulkanDynamicBuffer* m_indexBuffer;

  VkPipeline m_vkPipelineUsed{};

  // clip to scissors params
  bool m_scissorClip{false};
  glm::vec2 m_clipFactor{0.0f};
  glm::vec2 m_clipOffset{0.0f};
};

} // namespace VULKAN
} // namespace GRAPHICS
} // namespace GUILIB
} // namespace KODI
