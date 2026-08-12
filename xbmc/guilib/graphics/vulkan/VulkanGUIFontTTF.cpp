/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanGUIFontTTF.h"

#include "ServiceBroker.h"
#include "guilib/GUIFont.h"
#include "guilib/GUIFontManager.h"
#include "guilib/Texture.h"
#include "guilib/TextureManager.h"
#include "guilib/graphics/vulkan/VulkanTexture.h"
#include "rendering/MatrixStack.h"
#include "rendering/vulkan/DynamicBuffers.h"
#include "rendering/vulkan/VulkanRenderSystem.h"
#include "rendering/vulkan/shaders/VulkanShaderFonts.h"
#include "rendering/vulkan/utils/VulkanInitStructs.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "settings/AdvancedSettings.h"
#include "settings/SettingsComponent.h"
#include "utils/log.h"
#include "windowing/GraphicContext.h"
#include "windowing/WinSystem.h"

#include <cassert>
#include <memory>

// stuff for freetype
#include <ft2build.h>
#include <glm/gtc/type_ptr.hpp>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H

using namespace KODI::GUILIB::GRAPHICS::VULKAN;
using namespace KODI::RENDERING;
using namespace KODI::RENDERING::VULKAN;
using namespace KODI::RENDERING::VULKAN::UTILS;

namespace
{

constexpr size_t ELEMENT_ARRAY_MAX_CHAR_INDEX = 1000;

struct StaticIndexBuffer : public VulkanMemoryData
{
  bool created{false};
};

static StaticIndexBuffer staticIndexBuffer;

/**
 * @brief Creates static index buffers.
 *
 * The static index buffers are used for rendering text and are shared across all instances of
 * CVulkanGUIFontTTF. Due to the nature of Vulkan, these buffers are created once and reused to
 * avoid unnecessary overhead.
 *
 * The from here called Vulkan functions are only access CPU side and not GPU side,
 * so we not need to care about synchronization and class construction can proceed safely.
 */
static void CreateStaticIndexBuffers()
{
  if (staticIndexBuffer.created)
    return;

  auto queue = dynamic_cast<CVulkanRenderSystem*>(CServiceBroker::GetRenderSystem())->DeviceQueue();

  // Create an array holding the mesh indices to convert quads to triangles
  uint32_t index[ELEMENT_ARRAY_MAX_CHAR_INDEX][6];
  for (size_t i = 0; i < ELEMENT_ARRAY_MAX_CHAR_INDEX; i++)
  {
    index[i][0] = 4 * i;
    index[i][1] = 4 * i + 1;
    index[i][2] = 4 * i + 2;
    index[i][3] = 4 * i + 1;
    index[i][4] = 4 * i + 3;
    index[i][5] = 4 * i + 2;
  }

  VK_CHECK_RESULT(queue->CreateBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                      &staticIndexBuffer, sizeof(index), &index));
  staticIndexBuffer.created = true;
}

/**
 * @brief Destroys static index buffers.
 *
 * The from here called Vulkan functions are only access CPU side and not GPU side,
 * so we not need to care about synchronization.
 */
static void DestroyStaticIndexBuffers()
{
  if (!staticIndexBuffer.created)
    return;

  auto queue = dynamic_cast<CVulkanRenderSystem*>(CServiceBroker::GetRenderSystem())->DeviceQueue();

  queue->DestroyBuffer(&staticIndexBuffer);

  staticIndexBuffer.created = false;
}

} /* namespace */

//-----------------------------------------------------------------------------

CGUIFontTTF* CGUIFontTTF::CreateGUIFontTTF(const std::string& fontIdent)
{
  return new CVulkanGUIFontTTF(fontIdent);
}

CVulkanGUIFontTTF::CVulkanGUIFontTTF(const std::string& fontIdent) : CGUIFontTTF(fontIdent)
{
  using KODI::RENDERING::VULKAN::CVulkanRenderSystem;

  // Stored to avoid repeated dynamic_casts in the render loop and to avoid
  // having to call CServiceBroker::GetRenderSystem() repeatedly
  m_renderSystem = dynamic_cast<CVulkanRenderSystem*>(CServiceBroker::GetRenderSystem());
  m_deviceQueue = m_renderSystem->DeviceQueue();
  m_shaderFonts = dynamic_cast<CVulkanShaderFonts*>(
      m_renderSystem->ShaderControl()->GetShader(VULKAN_SM_FONTS));
  m_vkData = m_renderSystem->vkData();

  CreateStaticIndexBuffers();
}

CVulkanGUIFontTTF::~CVulkanGUIFontTTF()
{
  DestroyStaticIndexBuffers();

  // It's important that all the CGUIFontCacheEntry objects are
  // destructed before the CVulkanGUIFontTTF goes out of scope, because
  // our virtual methods won't be accessible after this point
  m_dynamicCache.Flush();
  DeleteHardwareTexture();
}

bool CVulkanGUIFontTTF::FirstBegin()
{
  m_scissorClip = ScissorsCanEffectClipping();
  if (!m_scissorClip)
  {
    m_renderSystem->ResetScissors();
  }

  if (m_textureStatus == TEXTURE_UPDATED)
  {
    // Copies one more line in case we have to sample from there
    m_updateY2 = std::min(m_updateY2 + 1, m_texture->GetHeight());

    SetImageContent(0, m_updateY1, m_texture->GetWidth(), m_updateY2 - m_updateY1,
                    m_texture->GetPixels() + m_updateY1 * m_texture->GetPitch());

    m_updateY1 = m_updateY2 = 0;
    m_textureStatus = TEXTURE_READY;
  }

  return true;
}

void CVulkanGUIFontTTF::LastEnd()
{
  // static vertex arrays are not supported anymore
  assert(m_vertex.empty());

  CWinSystemBase* const winSystem = CServiceBroker::GetWinSystem();
  if (!winSystem)
    return;

  VkCommandBuffer commandBuffer = m_renderSystem->vkCurrentCommandBuffer();

  if (!m_vertexTrans.empty())
  {
    // Store current scissor
    CGraphicContext& context = winSystem->GetGfxContext();
    CRect scissor = context.StereoCorrection(context.GetScissors());

    const uint32_t renderImageIndex = m_renderSystem->vkCurrentRenderImageIndex();

    CVulkanShaderFonts::VulkanUniform uniform{};
    uniform.projModelMatrix =
        KODI::RENDERING::globalMatrixProject * KODI::RENDERING::globalMatrixModview;
    uniform.depth = CServiceBroker::GetWinSystem()->GetGfxContext().GetTransformDepth();
    m_shaderFonts->UpdateUniformBuffer(renderImageIndex, uniform);

    for (size_t i = 0; i < m_vertexTrans.size(); i++)
    {
      if (m_vertexTrans[i].m_vertexBuffer->bufferHandle == nullptr)
      {
        continue;
      }

      // Apply the clip rectangle
      CRect clip = ClipRectToScissorRect(m_vertexTrans[i].m_clip);
      if (!clip.IsEmpty())
      {
        // intersect with current scissor
        clip.Intersect(scissor);
        // skip empty clip
        if (clip.IsEmpty())
        {
          continue;
        }
      }

      if (m_scissorClip)
      {
        // clip using scissors
        m_renderSystem->SetScissors(clip);
      }

      // calculate the fractional offset to the ideal position
      float fractX =
          context.ScaleFinalXCoord(m_vertexTrans[i].m_translateX, m_vertexTrans[i].m_translateY);
      float fractY =
          context.ScaleFinalYCoord(m_vertexTrans[i].m_translateX, m_vertexTrans[i].m_translateY);
      fractX = -fractX + std::round(fractX);
      fractY = -fractY + std::round(fractY);

      // proj * model * gui * scroll * translation * scaling * correction factor
      // Note: Projection and model matrices are already combined in the uniform buffer, so we only need to apply
      // the GUI matrix and the translation/scaling/correction here.
      glm::mat4 matrix = context.GetGUIMatrix().GetGLMMatrix();
      matrix = glm::translate(
          matrix, glm::vec3(m_vertexTrans[i].m_offsetX, m_vertexTrans[i].m_offsetY, 0.0f));
      matrix = glm::translate(
          matrix, glm::vec3(m_vertexTrans[i].m_translateX, m_vertexTrans[i].m_translateY, 0.0f));
      // the gui matrix messes with the scale. correct it here for now.
      matrix = glm::scale(matrix, glm::vec3(context.GetGUIScaleX(), context.GetGUIScaleY(), 1.0f));
      // the gui matrix doesn't align to exact pixel coords atm. correct it here for now.
      matrix = glm::translate(matrix, glm::vec3(fractX, fractY, 0.0f));

      VkPipeline pipeline = m_shaderFonts->VulkanPipeline(/*FONTS_TYPE_SCISSOR_CLIP*/);
      VkPipelineLayout pipelineLayout =
          m_shaderFonts->VulkanPipelineLayout(/*FONTS_TYPE_SCISSOR_CLIP*/);

      vkCmdPushConstants(m_renderSystem->vkCurrentCommandBuffer(), pipelineLayout,
                         VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), glm::value_ptr(matrix));

      VkDeviceSize bufferOffset = 0;
      VkBuffer vertexBuffer = m_vertexTrans[i].m_vertexBuffer->bufferHandle->buffer;
      vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &bufferOffset);

      VkBuffer indexBuffer = staticIndexBuffer.buffer;
      vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

      vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                              &m_shaderFonts->GetUniformBuffer(renderImageIndex)->descriptorSet, 0,
                              nullptr);
      vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1,
                              &m_descriptorSet, 0, nullptr);
      vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

      uint32_t size = static_cast<uint32_t>(m_vertexTrans[i].m_vertexBuffer->size);
      for (uint32_t character = 0; size > character; character += ELEMENT_ARRAY_MAX_CHAR_INDEX)
      {
        uint32_t count = size - character;
        count = std::min<uint32_t>(count, ELEMENT_ARRAY_MAX_CHAR_INDEX);

        bufferOffset = character * sizeof(SVertex) * 4;

        vkCmdDrawIndexed(commandBuffer, count * 6, 1, 0, bufferOffset, 0);

        CRenderSystemBase::m_GUIElementCount++;
      }
    }

    // Restore the original scissor rectangle
    if (m_scissorClip)
      m_renderSystem->SetScissors(scissor);
  }
}

CVertexBuffer CVulkanGUIFontTTF::CreateVertexBuffer(const std::vector<SVertex>& vertices) const
{
  assert(vertices.size() % 4 == 0);

  const VkDeviceSize buffer_size = vertices.size() * sizeof(SVertex);

  VulkanMemoryData* memData = new VulkanMemoryData();
  VK_CHECK_RESULT(m_deviceQueue->CreateBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                              memData, buffer_size, vertices.data()),
                  CVertexBuffer());

  return CVertexBuffer(CVertexBuffer::BufferHandleType(memData), vertices.size() / 4, this);
}

void CVulkanGUIFontTTF::DestroyVertexBuffer(CVertexBuffer& buffer) const
{
  m_deviceQueue->DestroyBuffer(buffer.bufferHandle);
  delete buffer.bufferHandle;
  buffer.bufferHandle = nullptr;
}

std::unique_ptr<CTexture> CVulkanGUIFontTTF::ReallocTexture(unsigned int& newHeight)
{
  newHeight = CTexture::PadPow2(newHeight);

  auto newTexture = std::make_unique<CVulkanTexture>(m_textureWidth, newHeight, XB_FMT_A8);
  if (!newTexture || !newTexture->GetPixels())
  {
    CLog::LogF(LOGERROR, "Error creating new cache texture for size {:f} ({}:{})", m_height,
               __FILENAME__, __LINE__);
    return nullptr;
  }

  m_textureHeight = newTexture->GetHeight();
  m_textureScaleY = 1.0f / m_textureHeight;
  m_textureWidth = newTexture->GetWidth();
  m_textureScaleX = 1.0f / m_textureWidth;
  if (m_textureHeight < newHeight)
    CLog::LogF(LOGWARNING, "Allocated new texture with height of {}, requested {}", m_textureHeight,
               newHeight);
  m_staticCache.Flush();
  m_dynamicCache.Flush();

  memset(newTexture->GetPixels(), 0, m_textureHeight * newTexture->GetPitch());
  if (m_texture)
  {
    m_updateY1 = 0;
    m_updateY2 = m_texture->GetHeight();

    unsigned char* src = m_texture->GetPixels();
    unsigned char* dst = newTexture->GetPixels();
    for (unsigned int y = 0; y < m_texture->GetHeight(); y++)
    {
      memcpy(dst, src, m_texture->GetPitch());
      src += m_texture->GetPitch();
      dst += newTexture->GetPitch();
    }

    DeleteHardwareTexture();
  }

  CreateTextureResources();

  m_textureStatus = TEXTURE_REALLOCATED;

  return newTexture;
}

bool CVulkanGUIFontTTF::CopyCharToTexture(
    FT_BitmapGlyph bitGlyph, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
  FT_Bitmap bitmap = bitGlyph->bitmap;

  // Copy the bitmap buffer to the texture using memcpy.
  // Is to reduce the number of Vulkan commands and avoid the overhead of creating staging
  // buffers for each character.
  //
  // Directly copy the bitmap buffer from here with call below can works as well,
  // only like described before more works.
  // ```cpp
  // SetImageContent(x1, y1, x2 - x1, y2 - y1, bitmap.buffer);
  // ```
  unsigned char* source = bitmap.buffer;
  unsigned char* target = m_texture->GetPixels() + y1 * m_texture->GetPitch() + x1;

  for (unsigned int y = y1; y < y2; y++)
  {
    memcpy(target, source, x2 - x1);
    source += bitmap.width;
    target += m_texture->GetPitch();
  }

  switch (m_textureStatus)
  {
    case TEXTURE_UPDATED:
    {
      m_updateY1 = std::min(m_updateY1, y1);
      m_updateY2 = std::max(m_updateY2, y2);
    }
    break;

    case TEXTURE_READY:
    {
      m_updateY1 = y1;
      m_updateY2 = y2;
      m_textureStatus = TEXTURE_UPDATED;
    }
    break;

    case TEXTURE_REALLOCATED:
    {
      m_updateY2 = std::max(m_updateY2, y2);
      m_textureStatus = TEXTURE_UPDATED;
    }
    break;

    case TEXTURE_VOID:
    default:
      break;
  }

  return true;
}

void CVulkanGUIFontTTF::CreateTextureResources()
{
  VkDeviceSize size = static_cast<VkDeviceSize>(m_textureWidth * m_textureHeight);
  if (size == 0)
    return;

  VkImageCreateInfo imageInfo = vkImageCreateInfo();
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.format = VK_FORMAT_R8_UNORM;
  imageInfo.extent.width = m_textureWidth;
  imageInfo.extent.height = m_textureHeight;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  VK_CHECK_RESULT(vkCreateImage(m_vkData->vkDevice, &imageInfo, nullptr, &m_image));

  VkMemoryRequirements memReqs;
  vkGetImageMemoryRequirements(m_vkData->vkDevice, m_image, &memReqs);
  uint32_t memoryTypeIndex =
      m_deviceQueue->GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  auto allocInfo = vkMemoryAllocateInfo(memReqs.size, memoryTypeIndex);
  VK_CHECK_RESULT(vkAllocateMemory(m_vkData->vkDevice, &allocInfo, nullptr, &m_imageMemory));
  VK_CHECK_RESULT(vkBindImageMemory(m_vkData->vkDevice, m_image, m_imageMemory, 0));

  const int32_t aniso =
      CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_guiAnisotropicFiltering;

  // Create sampler
  VkSamplerCreateInfo samplerInfo = vkSamplerCreateInfo();
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerInfo.anisotropyEnable = (aniso > 1 ? VK_TRUE : VK_FALSE);
  samplerInfo.maxAnisotropy = static_cast<float>(aniso);
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.maxLod = 0.0f;
  VK_CHECK_RESULT(vkCreateSampler(m_vkData->vkDevice, &samplerInfo, nullptr, &m_sampler));

  // Create image view
  VkImageViewCreateInfo view = vkImageViewCreateInfo();
  view.image = m_image;
  view.viewType = VK_IMAGE_VIEW_TYPE_2D;
  view.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
  view.format = VK_FORMAT_R8_UNORM;
  view.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_ZERO, VK_COMPONENT_SWIZZLE_ZERO,
                     VK_COMPONENT_SWIZZLE_ZERO};
  VK_CHECK_RESULT(vkCreateImageView(m_vkData->vkDevice, &view, nullptr, &m_imageView));

  //--------------------------------------------------------------------------------

  VkDescriptorImageInfo textureDescriptor{};
  textureDescriptor.imageView = m_imageView;
  textureDescriptor.sampler = m_sampler;
  textureDescriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  VkDescriptorSetAllocateInfo descAllocInfo{};
  descAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  descAllocInfo.descriptorPool = m_vkData->vkDescriptorPool;
  descAllocInfo.descriptorSetCount = 1;
  descAllocInfo.pSetLayouts = &m_vkData->vkDescriptorSetLayout_Texture;

  VK_CHECK_RESULT(vkAllocateDescriptorSets(m_vkData->vkDevice, &descAllocInfo, &m_descriptorSet));

  VkWriteDescriptorSet writeDesc{};
  writeDesc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writeDesc.dstSet = m_descriptorSet;
  writeDesc.dstBinding = 0;
  writeDesc.dstArrayElement = 0;
  writeDesc.descriptorCount = 1;
  writeDesc.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  writeDesc.pImageInfo = &textureDescriptor;

  vkUpdateDescriptorSets(m_vkData->vkDevice, 1, &writeDesc, 0, nullptr);
}

void CVulkanGUIFontTTF::SetImageContent(
    int32_t x, int32_t y, uint32_t width, uint32_t height, const void* imageData)
{
  VkDeviceSize size = static_cast<VkDeviceSize>(width * height);
  if (size == 0)
    return;

  VkBufferImageCopy region = {};
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.layerCount = 1;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageOffset = {x, y, 0};
  region.imageExtent.width = width;
  region.imageExtent.height = height;
  region.imageExtent.depth = 1;

  VkImageSubresourceRange subresourceRange = {};
  subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresourceRange.baseMipLevel = 0;
  subresourceRange.levelCount = 1;
  subresourceRange.baseArrayLayer = 0;
  subresourceRange.layerCount = 1;

  auto bufferInfo =
      vkBufferCreateInfo(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size, VK_SHARING_MODE_EXCLUSIVE);

  VkBuffer stagingBuffer{};
  VK_CHECK_RESULT(vkCreateBuffer(m_vkData->vkDevice, &bufferInfo, nullptr, &stagingBuffer));

  VkMemoryRequirements memReqs;
  vkGetBufferMemoryRequirements(m_vkData->vkDevice, stagingBuffer, &memReqs);

  uint32_t memoryTypeIndex = m_deviceQueue->GetMemoryType(memReqs.memoryTypeBits,
                                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  auto allocInfo = vkMemoryAllocateInfo(memReqs.size, memoryTypeIndex);

  VkDeviceMemory stagingMemory{};
  VK_CHECK_RESULT(vkAllocateMemory(m_vkData->vkDevice, &allocInfo, nullptr, &stagingMemory));
  VK_CHECK_RESULT(vkBindBufferMemory(m_vkData->vkDevice, stagingBuffer, stagingMemory, 0));

  void* data;
  VK_CHECK_RESULT(vkMapMemory(m_vkData->vkDevice, stagingMemory, 0, size, 0, &data));
  memcpy(data, imageData, static_cast<size_t>(size));
  vkUnmapMemory(m_vkData->vkDevice, stagingMemory);

  VkCommandBuffer copyCmd = m_deviceQueue->CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                               m_vkData->vkCommandPool, true);

  SetImageLayout(copyCmd, m_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                 subresourceRange, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
  vkCmdCopyBufferToImage(copyCmd, stagingBuffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                         &region);
  SetImageLayout(copyCmd, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange,
                 VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

  m_deviceQueue->FlushCommandBuffer(copyCmd);

  // Clean up staging resources
  vkFreeMemory(m_vkData->vkDevice, stagingMemory, nullptr);
  vkDestroyBuffer(m_vkData->vkDevice, stagingBuffer, nullptr);
}

void CVulkanGUIFontTTF::DeleteHardwareTexture()
{
  if (m_imageView != VK_NULL_HANDLE)
  {
    vkDestroyImageView(m_vkData->vkDevice, m_imageView, nullptr);
    m_imageView = VK_NULL_HANDLE;
  }
  if (m_image != VK_NULL_HANDLE)
  {
    vkDestroyImage(m_vkData->vkDevice, m_image, nullptr);
    m_image = VK_NULL_HANDLE;
  }
  if (m_sampler != VK_NULL_HANDLE)
  {
    vkDestroySampler(m_vkData->vkDevice, m_sampler, nullptr);
    m_sampler = VK_NULL_HANDLE;
  }
  if (m_imageMemory != VK_NULL_HANDLE)
  {
    vkFreeMemory(m_vkData->vkDevice, m_imageMemory, nullptr);
    m_imageMemory = VK_NULL_HANDLE;
  }
}

/**
 * vkCmdSetScissor or glScissor operates in window coordinates.
 * In order that we can use it to perform clipping, we must ensure that there is an independent
 * linear transformation from the coordinate system used by @ref CGraphicContext::ClipRect
 * to window coordinates, separately for X and Y (in other words, no rotation or shear is
 * introduced at any stage). To do, this, we need to check that zeros are present in the
 * following locations:
 *
 * GUI matrix:
 * / * 0 * * \
 * | 0 * * * |
 * \ 0 0 * * /
 *       ^ TransformMatrix::TransformX/Y/ZCoord are only ever called with
 *         input z = 0, so this column doesn't matter
 * Model-view matrix:
 * / * 0 0 * \
 * | 0 * 0 * |
 * | 0 0 * * |
 * \ * * * * /  <- eye w has no influence on window x/y (last column below
 *                                                       is either 0 or ignored)
 * Projection matrix:
 * / * 0 0 0 \
 * | 0 * 0 0 |
 * | * * * * |  <- normalised device coordinate z has no influence on window x/y
 * \ 0 0 * 0 /
 *
 * Some of these zeros are not strictly required to ensure this, but they tend
 * to be zeroed in the common case, so by checking for zeros here, we simplify
 * the calculation of the window x/y coordinates further down the line.
 *
 * (Minor detail: we don't quite deal in window coordinates as defined by
 * Vulkan or OpenGL, because CRenderSystemGLES::SetScissors flips the Y axis. But all
 * that's needed to handle that is an effective negation at the stage where
 * Y is in normalised device coordinates.)
 */
bool CVulkanGUIFontTTF::ScissorsCanEffectClipping()
{
  const glm::mat4& projMatrix = globalMatrixProject;
  const glm::mat4& modelMatrix = globalMatrixModview;
  const TransformMatrix& guiMatrix = CServiceBroker::GetWinSystem()->GetGfxContext().GetGUIMatrix();

  CRect viewPort; // absolute positions of corners
  CServiceBroker::GetRenderSystem()->GetViewPort(viewPort);

  const bool clipPossible =
      guiMatrix.m[0][1] == 0 && guiMatrix.m[1][0] == 0 && guiMatrix.m[2][0] == 0 &&
      guiMatrix.m[2][1] == 0 && modelMatrix[1][0] == 0 && modelMatrix[2][0] == 0 &&
      modelMatrix[0][1] == 0 && modelMatrix[2][1] == 0 && modelMatrix[0][2] == 0 &&
      modelMatrix[1][2] == 0 && projMatrix[1][0] == 0 && projMatrix[2][0] == 0 &&
      projMatrix[3][0] == 0 && projMatrix[0][1] == 0 && projMatrix[2][1] == 0 &&
      projMatrix[3][1] == 0 && projMatrix[0][3] == 0 && projMatrix[1][3] == 0 &&
      projMatrix[3][3] == 0;

  m_clipFactor = glm::vec2(0.0);
  m_clipOffset = glm::vec2(0.0);

  if (clipPossible)
  {
    // Note: Vulkan uses a right-handed coordinate system, so the Y axis is inverted compared to OpenGL.
    // This is why we negate the Y component of the clip factor and offset.
    m_clipFactor.x = guiMatrix.m[0][0] * modelMatrix[0][0] * projMatrix[0][0];
    m_clipOffset.x = (guiMatrix.m[0][3] * modelMatrix[0][0] + modelMatrix[3][0]) * projMatrix[0][0];
    m_clipFactor.y = guiMatrix.m[1][1] * modelMatrix[1][1] * -projMatrix[1][1];
    m_clipOffset.y =
        (guiMatrix.m[1][3] * modelMatrix[1][1] + modelMatrix[3][1]) * -projMatrix[1][1];

    // correct for inverted window coordinate scheme
    const float clipW =
        (guiMatrix.m[2][3] * modelMatrix[2][2] + modelMatrix[3][2]) * projMatrix[2][3];
    float xMult = (viewPort.x2 - viewPort.x1) / (2 * clipW);
    float yMult = (viewPort.y1 - viewPort.y2) / (2 * clipW);

    m_clipFactor.x = m_clipFactor.x * xMult;
    m_clipOffset.x = m_clipOffset.x * xMult + (viewPort.x2 + viewPort.x1) / 2;
    m_clipFactor.y = m_clipFactor.y * yMult;
    m_clipOffset.y = m_clipOffset.y * yMult + (viewPort.y2 + viewPort.y1) / 2;
  }

  return clipPossible;
}

CRect CVulkanGUIFontTTF::ClipRectToScissorRect(const CRect& rect)
{
  return CRect(rect.x1 * m_clipFactor.x + m_clipOffset.x, rect.y1 * m_clipFactor.y + m_clipOffset.y,
               rect.x2 * m_clipFactor.x + m_clipOffset.x,
               rect.y2 * m_clipFactor.y + m_clipOffset.y);
}
