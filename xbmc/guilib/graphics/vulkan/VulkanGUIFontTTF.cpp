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
#include "rendering/vulkan/VulkanMatrix.h"
#include "rendering/vulkan/VulkanRenderSystem.h"
#include "rendering/vulkan/shaders/VulkanShaderFonts.h"
#include "settings/AdvancedSettings.h"
#include "settings/SettingsComponent.h"
#include "utils/log.h"
#include "windowing/GraphicContext.h"
#include "windowing/WinSystem.h"

#include <cassert>
#include <memory>

// stuff for freetype
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H

using namespace KODI::GUILIB::GRAPHICS::VULKAN;
using namespace KODI::RENDERING::VULKAN;

namespace
{
constexpr size_t ELEMENT_ARRAY_MAX_CHAR_INDEX = 1000;
} /* namespace */

CGUIFontTTF* CGUIFontTTF::CreateGUIFontTTF(const std::string& fontIdent)
{
  fprintf(stderr, "CGUIFontTTF::CreateGUIFontTTF: Creating Vulkan font for %s\n",
          fontIdent.c_str());
  return new CVulkanGUIFontTTF(fontIdent);
}

CVulkanGUIFontTTF::CVulkanGUIFontTTF(const std::string& fontIdent) : CGUIFontTTF(fontIdent)
{
  m_renderSystem = dynamic_cast<CVulkanRenderSystem*>(CServiceBroker::GetRenderSystem());
  //m_shader = std::make_unique<KODI::RENDERING::VULKAN::CVulkanShaderFonts>(
  //    m_renderSystem->vkDevice(), m_renderSystem->vkPipelineLayout(),
  //    m_renderSystem->vkRenderPass());
}

CVulkanGUIFontTTF::~CVulkanGUIFontTTF()
{
  fprintf(stderr, "--------------> %s\n", __func__);
  // It's important that all the CGUIFontCacheEntry objects are
  // destructed before the CVulkanGUIFontTTF goes out of scope, because
  // our virtual methods won't be accessible after this point
  m_dynamicCache.Flush();
  DeleteHardwareTexture();
}

bool CVulkanGUIFontTTF::FirstBegin()
{
  m_renderSystem->EnableShader(VULKAN_SM_FONTS);

  return true;
}

void CVulkanGUIFontTTF::LastEnd()
{

  CreateStaticIndexBuffers();

  m_renderSystem->DisableShader();
}

CVertexBuffer CVulkanGUIFontTTF::CreateVertexBuffer(const std::vector<SVertex>& vertices) const
{
  // We need to cast away constness here because the Vulkan vertex buffer creation modifies the
  // internal state of the font object
  return VulkanCreateVertexBuffer(const_cast<CVulkanGUIFontTTF*>(this), vertices);
}

void CVulkanGUIFontTTF::DestroyVertexBuffer(CVertexBuffer& buffer) const
{
  VulkanDestroyVertexBuffer(const_cast<CVulkanGUIFontTTF*>(this), buffer);
}

CVertexBuffer CVulkanGUIFontTTF::VulkanCreateVertexBuffer(CVulkanGUIFontTTF* ref,
                                                          const std::vector<SVertex>& vertices)
{
  assert(vertices.size() % 4 == 0);

  const VkDeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();

  // Copy Vertex data to a buffer accessible by the device
  VkBufferCreateInfo buffer_info{
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .size = buffer_size,
      .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 0,
      .pQueueFamilyIndices = nullptr,
  };

  // We use the Vulkan Memory Allocator to find a memory type that can be written and mapped from the host
  // On most setups this will return a memory type that resides in VRAM and is accessible from the host
  VmaAllocationCreateInfo buffer_alloc_ci{
      .flags =
          VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
      .usage = VMA_MEMORY_USAGE_AUTO,
      .requiredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      .preferredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
      .memoryTypeBits = 0,
      .pool = nullptr,
      .pUserData = nullptr,
      .priority = 0.0f,
  };

  VmaAllocationInfo buffer_alloc_info{};
  VkBuffer vertex_buffer{VK_NULL_HANDLE};
  VmaAllocation vertex_buffer_allocation{VK_NULL_HANDLE};
  vmaCreateBuffer(ref->m_renderSystem->DeviceQueue()->VMAAllocator(), &buffer_info,
                  &buffer_alloc_ci, &vertex_buffer, &vertex_buffer_allocation, &buffer_alloc_info);
  if (buffer_alloc_info.pMappedData)
  {
    memcpy(buffer_alloc_info.pMappedData, vertices.data(), buffer_size);
  }
  else
  {
    CLog::Log(LOGERROR, "Vulkan: Could not map vertex buffer ({0}:{1})", __FILENAME__, __LINE__);
  }

  return CVertexBuffer(CVertexBuffer::BufferHandleType(vertex_buffer, vertex_buffer_allocation),
                       vertices.size() / 4, ref);
}

void CVulkanGUIFontTTF::VulkanDestroyVertexBuffer(CVulkanGUIFontTTF* ref, CVertexBuffer& buffer)
{
  vmaDestroyBuffer(ref->m_renderSystem->DeviceQueue()->VMAAllocator(), buffer.bufferHandle.buffer,
                   buffer.bufferHandle.allocation);
  buffer.bufferHandle.allocation = VK_NULL_HANDLE;
  buffer.bufferHandle.buffer = VK_NULL_HANDLE;
}

std::unique_ptr<CTexture> CVulkanGUIFontTTF::ReallocTexture(unsigned int& newHeight)
{
  //fprintf(stderr, "CVulkanGUIFontTTF::ReallocTexture: Reallocating texture for %s\n",
  //        GetFontIdent().c_str());
  newHeight = CTexture::PadPow2(newHeight);

  std::unique_ptr<CTexture> newTexture =
      CTexture::CreateTexture(m_textureWidth, newHeight, XB_FMT_A8);

  return newTexture;
}

bool CVulkanGUIFontTTF::CopyCharToTexture(
    FT_BitmapGlyph bitGlyph, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
  //fprintf(stderr, "CVulkanGUIFontTTF::CopyCharToTexture: Copying char to texture for %s\n",
  //        GetFontIdent().c_str());
  return true;
}

void CVulkanGUIFontTTF::DeleteHardwareTexture()
{
  //fprintf(stderr, "CVulkanGUIFontTTF::DeleteHardwareTexture: Deleting hardware texture for %s\n",
  //        GetFontIdent().c_str());
}

void CVulkanGUIFontTTF::CreateStaticIndexBuffers()
{
  if (m_staticIndexBuffer.created)
    return;

  const auto renderSystem = dynamic_cast<CVulkanRenderSystem*>(CServiceBroker::GetRenderSystem());

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

  // Copy Vertex data to a buffer accessible by the device
  VkBufferCreateInfo buffer_info{
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .size = sizeof(index),
      .usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 0,
      .pQueueFamilyIndices = nullptr,
  };

  // We use the Vulkan Memory Allocator to find a memory type that can be written and mapped from the host
  // On most setups this will return a memory type that resides in VRAM and is accessible from the host
  VmaAllocationCreateInfo buffer_alloc_ci{
      .flags =
          VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
      .usage = VMA_MEMORY_USAGE_AUTO,
      .requiredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      .preferredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
      .memoryTypeBits = 0,
      .pool = nullptr,
      .pUserData = nullptr,
      .priority = 0.0f,
  };

  VmaAllocationInfo buffer_alloc_info{};
  VkBuffer vertex_buffer{VK_NULL_HANDLE};
  VmaAllocation vertex_buffer_allocation{VK_NULL_HANDLE};
  vmaCreateBuffer(renderSystem->DeviceQueue()->VMAAllocator(), &buffer_info, &buffer_alloc_ci,
                  &vertex_buffer, &vertex_buffer_allocation, &buffer_alloc_info);
  if (buffer_alloc_info.pMappedData)
  {
    memcpy(buffer_alloc_info.pMappedData, index, sizeof(index));
  }
  else
  {
    CLog::Log(LOGERROR, "Vulkan: Could not map vertex buffer ({0}:{1})", __FILENAME__, __LINE__);
  }

  m_staticIndexBuffer.buffer = vertex_buffer;
  m_staticIndexBuffer.allocation = vertex_buffer_allocation;
  m_staticIndexBuffer.created = true;
}

void CVulkanGUIFontTTF::DestroyStaticIndexBuffers()
{
  if (!m_staticIndexBuffer.created)
    return;

  const auto renderSystem = dynamic_cast<CVulkanRenderSystem*>(CServiceBroker::GetRenderSystem());

  vmaDestroyBuffer(renderSystem->DeviceQueue()->VMAAllocator(), m_staticIndexBuffer.buffer,
                   m_staticIndexBuffer.allocation);

  m_staticIndexBuffer.allocation = VK_NULL_HANDLE;
  m_staticIndexBuffer.buffer = VK_NULL_HANDLE;
  m_staticIndexBuffer.created = false;
}

CVulkanGUIFontTTF::StaticIndexBuffer CVulkanGUIFontTTF::m_staticIndexBuffer;
