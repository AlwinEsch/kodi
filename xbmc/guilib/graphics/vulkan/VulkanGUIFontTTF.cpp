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

namespace
{

struct StaticIndexBuffer
{
  bool created{false};
  VulkanMemoryData memoryData;
};

static StaticIndexBuffer staticIndexBuffer;

constexpr size_t ELEMENT_ARRAY_MAX_CHAR_INDEX = 1000;
} /* namespace */

CGUIFontTTF* CGUIFontTTF::CreateGUIFontTTF(const std::string& fontIdent)
{
  return new CVulkanGUIFontTTF(fontIdent);
}

CVulkanGUIFontTTF::CVulkanGUIFontTTF(const std::string& fontIdent) : CGUIFontTTF(fontIdent)
{
  using KODI::RENDERING::VULKAN::CVulkanRenderSystem;
  m_renderSystem = dynamic_cast<CVulkanRenderSystem*>(CServiceBroker::GetRenderSystem());
  m_shaderFonts = dynamic_cast<CVulkanShaderFonts*>(
      m_renderSystem->ShaderControl()->GetShader(VULKAN_SM_FONTS));

  CVulkanDynamicBuffers* dynamicBuffers = m_renderSystem->DynamicBuffers();
  m_uniformBuffer = dynamicBuffers->GetBuffer(BUFFER_TYPE_UNIFORM);
  m_vertexBuffer = dynamicBuffers->GetBuffer(BUFFER_TYPE_VERTEX);
  m_indexBuffer = dynamicBuffers->GetBuffer(BUFFER_TYPE_INDEX);
}

CVulkanGUIFontTTF::~CVulkanGUIFontTTF()
{
  // It's important that all the CGUIFontCacheEntry objects are
  // destructed before the CVulkanGUIFontTTF goes out of scope, because
  // our virtual methods won't be accessible after this point
  m_dynamicCache.Flush();
  DeleteHardwareTexture();
}

bool CVulkanGUIFontTTF::FirstBegin()
{
  m_scissorClip = true;
  ScissorsCanEffectClipping(m_clipFactor, m_clipOffset);
  if (m_scissorClip)
  {
    m_vkPipelineUsed = m_shaderFonts->VulkanPipeline(FONTS_TYPE_SCISSOR_CLIP);
  }
  else
  {
    m_vkPipelineUsed = m_shaderFonts->VulkanPipeline(FONTS_TYPE_SHADER_CLIP);
  }

  if (m_textureStatus == TEXTURE_REALLOCATED)
  {

    m_textureStatus = TEXTURE_VOID;
  }

  if (m_textureStatus == TEXTURE_VOID)
  {

    m_textureStatus = TEXTURE_UPDATED;
  }

  if (m_textureStatus == TEXTURE_UPDATED)
  {

    m_textureStatus = TEXTURE_READY;
  }

  return true;
}

void MultMatrixf(glm::mat4& matrixGLM, const float* matrix) noexcept
{
  float* m_pMatrix = glm::value_ptr(matrixGLM);

  float a = (matrix[0] * m_pMatrix[0]) + (matrix[1] * m_pMatrix[4]) +
              (matrix[2] * m_pMatrix[8]) + (matrix[3] * m_pMatrix[12]);
  float b = (matrix[0] * m_pMatrix[1]) + (matrix[1] * m_pMatrix[5]) +
              (matrix[2] * m_pMatrix[9]) + (matrix[3] * m_pMatrix[13]);
  float c = (matrix[0] * m_pMatrix[2]) + (matrix[1] * m_pMatrix[6]) +
              (matrix[2] * m_pMatrix[10]) + (matrix[3] * m_pMatrix[14]);
  float d = (matrix[0] * m_pMatrix[3]) + (matrix[1] * m_pMatrix[7]) +
              (matrix[2] * m_pMatrix[11]) + (matrix[3] * m_pMatrix[15]);
  float e = (matrix[4] * m_pMatrix[0]) + (matrix[5] * m_pMatrix[4]) +
              (matrix[6] * m_pMatrix[8]) + (matrix[7] * m_pMatrix[12]);
  float f = (matrix[4] * m_pMatrix[1]) + (matrix[5] * m_pMatrix[5]) +
              (matrix[6] * m_pMatrix[9]) + (matrix[7] * m_pMatrix[13]);
  float g = (matrix[4] * m_pMatrix[2]) + (matrix[5] * m_pMatrix[6]) +
              (matrix[6] * m_pMatrix[10]) + (matrix[7] * m_pMatrix[14]);
  float h = (matrix[4] * m_pMatrix[3]) + (matrix[5] * m_pMatrix[7]) +
              (matrix[6] * m_pMatrix[11]) + (matrix[7] * m_pMatrix[15]);
  float i = (matrix[8] * m_pMatrix[0]) + (matrix[9] * m_pMatrix[4]) +
              (matrix[10] * m_pMatrix[8]) + (matrix[11] * m_pMatrix[12]);
  float j = (matrix[8] * m_pMatrix[1]) + (matrix[9] * m_pMatrix[5]) +
              (matrix[10] * m_pMatrix[9]) + (matrix[11] * m_pMatrix[13]);
  float k = (matrix[8] * m_pMatrix[2]) + (matrix[9] * m_pMatrix[6]) +
              (matrix[10] * m_pMatrix[10]) + (matrix[11] * m_pMatrix[14]);
  float l = (matrix[8] * m_pMatrix[3]) + (matrix[9] * m_pMatrix[7]) +
              (matrix[10] * m_pMatrix[11]) + (matrix[11] * m_pMatrix[15]);
  float m = (matrix[12] * m_pMatrix[0]) + (matrix[13] * m_pMatrix[4]) +
              (matrix[14] * m_pMatrix[8]) + (matrix[15] * m_pMatrix[12]);
  float n = (matrix[12] * m_pMatrix[1]) + (matrix[13] * m_pMatrix[5]) +
              (matrix[14] * m_pMatrix[9]) + (matrix[15] * m_pMatrix[13]);
  float o = (matrix[12] * m_pMatrix[2]) + (matrix[13] * m_pMatrix[6]) +
              (matrix[14] * m_pMatrix[10]) + (matrix[15] * m_pMatrix[14]);
  float p = (matrix[12] * m_pMatrix[3]) + (matrix[13] * m_pMatrix[7]) +
              (matrix[14] * m_pMatrix[11]) + (matrix[15] * m_pMatrix[15]);
  m_pMatrix[0] = a;
  m_pMatrix[4] = e;
  m_pMatrix[8] = i;
  m_pMatrix[12] = m;
  m_pMatrix[1] = b;
  m_pMatrix[5] = f;
  m_pMatrix[9] = j;
  m_pMatrix[13] = n;
  m_pMatrix[2] = c;
  m_pMatrix[6] = g;
  m_pMatrix[10] = k;
  m_pMatrix[14] = o;
  m_pMatrix[3] = d;
  m_pMatrix[7] = h;
  m_pMatrix[11] = l;
  m_pMatrix[15] = p;


}

void Translatef(glm::mat4& matrixGLM, float x, float y, float z)
{
  const float matrix[16]{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                         0.0f, 0.0f, 1.0f, 0.0f, x,    y,    z,    1.0f};

  float* m_pMatrix = glm::value_ptr(matrixGLM);

  float a = (matrix[0] * m_pMatrix[0]) + (matrix[1] * m_pMatrix[4]) + (matrix[2] * m_pMatrix[8]) +
            (matrix[3] * m_pMatrix[12]);
  float b = (matrix[0] * m_pMatrix[1]) + (matrix[1] * m_pMatrix[5]) + (matrix[2] * m_pMatrix[9]) +
            (matrix[3] * m_pMatrix[13]);
  float c = (matrix[0] * m_pMatrix[2]) + (matrix[1] * m_pMatrix[6]) + (matrix[2] * m_pMatrix[10]) +
            (matrix[3] * m_pMatrix[14]);
  float d = (matrix[0] * m_pMatrix[3]) + (matrix[1] * m_pMatrix[7]) + (matrix[2] * m_pMatrix[11]) +
            (matrix[3] * m_pMatrix[15]);
  float e = (matrix[4] * m_pMatrix[0]) + (matrix[5] * m_pMatrix[4]) + (matrix[6] * m_pMatrix[8]) +
            (matrix[7] * m_pMatrix[12]);
  float f = (matrix[4] * m_pMatrix[1]) + (matrix[5] * m_pMatrix[5]) + (matrix[6] * m_pMatrix[9]) +
            (matrix[7] * m_pMatrix[13]);
  float g = (matrix[4] * m_pMatrix[2]) + (matrix[5] * m_pMatrix[6]) + (matrix[6] * m_pMatrix[10]) +
            (matrix[7] * m_pMatrix[14]);
  float h = (matrix[4] * m_pMatrix[3]) + (matrix[5] * m_pMatrix[7]) + (matrix[6] * m_pMatrix[11]) +
            (matrix[7] * m_pMatrix[15]);
  float i = (matrix[8] * m_pMatrix[0]) + (matrix[9] * m_pMatrix[4]) + (matrix[10] * m_pMatrix[8]) +
            (matrix[11] * m_pMatrix[12]);
  float j = (matrix[8] * m_pMatrix[1]) + (matrix[9] * m_pMatrix[5]) + (matrix[10] * m_pMatrix[9]) +
            (matrix[11] * m_pMatrix[13]);
  float k = (matrix[8] * m_pMatrix[2]) + (matrix[9] * m_pMatrix[6]) + (matrix[10] * m_pMatrix[10]) +
            (matrix[11] * m_pMatrix[14]);
  float l = (matrix[8] * m_pMatrix[3]) + (matrix[9] * m_pMatrix[7]) + (matrix[10] * m_pMatrix[11]) +
            (matrix[11] * m_pMatrix[15]);
  float m = (matrix[12] * m_pMatrix[0]) + (matrix[13] * m_pMatrix[4]) +
            (matrix[14] * m_pMatrix[8]) + (matrix[15] * m_pMatrix[12]);
  float n = (matrix[12] * m_pMatrix[1]) + (matrix[13] * m_pMatrix[5]) +
            (matrix[14] * m_pMatrix[9]) + (matrix[15] * m_pMatrix[13]);
  float o = (matrix[12] * m_pMatrix[2]) + (matrix[13] * m_pMatrix[6]) +
            (matrix[14] * m_pMatrix[10]) + (matrix[15] * m_pMatrix[14]);
  float p = (matrix[12] * m_pMatrix[3]) + (matrix[13] * m_pMatrix[7]) +
            (matrix[14] * m_pMatrix[11]) + (matrix[15] * m_pMatrix[15]);
  m_pMatrix[0] = a;
  m_pMatrix[4] = e;
  m_pMatrix[8] = i;
  m_pMatrix[12] = m;
  m_pMatrix[1] = b;
  m_pMatrix[5] = f;
  m_pMatrix[9] = j;
  m_pMatrix[13] = n;
  m_pMatrix[2] = c;
  m_pMatrix[6] = g;
  m_pMatrix[10] = k;
  m_pMatrix[14] = o;
  m_pMatrix[3] = d;
  m_pMatrix[7] = h;
  m_pMatrix[11] = l;
  m_pMatrix[15] = p;
}

void CVulkanGUIFontTTF::LastEnd()
{
  // static vertex arrays are not supported anymore
  assert(m_vertex.empty());

  CWinSystemBase* const winSystem = CServiceBroker::GetWinSystem();
  if (!winSystem)
    return;

  VkCommandBuffer commandBuffer = m_renderSystem->vkCurrentCommandBuffer();

  CreateStaticIndexBuffers();

  if (!m_vertexTrans.empty())
  {
    // Store current scissor
    CGraphicContext& context = winSystem->GetGfxContext();
    //CRect scissor = context.StereoCorrection(context.GetScissors());
    CVulkanTexture* vkTexture = dynamic_cast<CVulkanTexture*>(m_texture.get());

    const uint32_t renderImageIndex = m_renderSystem->vkCurrentRenderImageIndex();

    for (size_t i = 0; i < m_vertexTrans.size(); i++)
    {
      if (m_vertexTrans[i].m_vertexBuffer->bufferHandle == nullptr)
      {
        fprintf(stderr, "CVulkanGUIFontTTF::LastEnd: Skipping font %s with empty vertex buffer\n",
                GetFontIdent().c_str());
        continue;
      }

      // Apply the clip rectangle
      //CRect clip = ClipRectToScissorRect(m_vertexTrans[i].m_clip);
      //if (!clip.IsEmpty())
      //{
      //  // intersect with current scissor
      //  clip.Intersect(scissor);
      //  // skip empty clip
      //  if (clip.IsEmpty())
      //  {
      //    continue;
      //  }
      //}

      // calculate the fractional offset to the ideal position
      float fractX =
          context.ScaleFinalXCoord(m_vertexTrans[i].m_translateX, m_vertexTrans[i].m_translateY);
      float fractY =
          context.ScaleFinalYCoord(m_vertexTrans[i].m_translateX, m_vertexTrans[i].m_translateY);
      fractX = -fractX + std::round(fractX);
      fractY = -fractY + std::round(fractY);

      // proj * model * gui * scroll * translation * scaling * correction factor
      glm::mat4 matrix2 = context.GetGUIMatrix().GetGLMMatrix();

      glm::mat4 matrix = glm::mat4(1.0f);
      matrix = matrix * KODI::RENDERING::globalMatrixProject;
      matrix = matrix * KODI::RENDERING::globalMatrixModview;
      //matrix[3][0] += matrix2[3][0];
      matrix[3][1] += matrix2[3][1];
      matrix[3][1] += 512.0f;
      matrix[3][1] += (i * 25);
      //matrix = matrix * context.GetGUIMatrix().GetGLMMatrix();

      //Translatef(matrix, m_vertexTrans[i].m_offsetX, m_vertexTrans[i].m_offsetY, 0.0f);
      //Translatef(matrix, m_vertexTrans[i].m_translateX, m_vertexTrans[i].m_translateY, 0.0f);
      //fprintf(stderr,
      //        "CVulkanGUIFontTTF::LastEnd: Using font %s with "
      //        "translation (%f, %f) and offset (%f, %f) and fractional offset (%f, %f)\n",
      //        GetFontIdent().c_str(), double(m_vertexTrans[i].m_translateX),
      //        double(m_vertexTrans[i].m_translateY), double(m_vertexTrans[i].m_offsetX),
      //        double(m_vertexTrans[i].m_offsetY), double(fractX), double(fractY));
      //matrix = glm::translate(
      //    matrix, glm::vec3(m_vertexTrans[i].m_offsetX, m_vertexTrans[i].m_offsetY, 0.0f));
      //matrix = glm::translate(
      //    matrix, glm::vec3(m_vertexTrans[i].m_translateX, m_vertexTrans[i].m_translateY, 0.0f));
      fprintf(stderr, "1- (%f, %f, %f, %f) (%f, %f, %f, %f) (%f, %f, %f, %f) (%f, %f, %f, %f)\n",
              double(matrix[0][0]), double(matrix[0][1]), double(matrix[0][2]),
              double(matrix[0][3]), double(matrix[1][0]), double(matrix[1][1]),
              double(matrix[1][2]), double(matrix[1][3]), double(matrix[2][0]),
              double(matrix[2][1]), double(matrix[2][2]), double(matrix[2][3]),
              double(matrix[3][0]), double(matrix[3][1]), double(matrix[3][2]),
              double(matrix[3][3]));
      //fprintf(
      //    stderr, "2- (%f, %f, %f, %f) (%f, %f, %f, %f) (%f, %f, %f, %f) (%f, %f, %f, %f)\n",
      //    double(matrix2[0][0]), double(matrix2[0][1]), double(matrix2[0][2]), double(matrix2[0][3]),
      //    double(matrix2[1][0]), double(matrix2[1][1]), double(matrix2[1][2]), double(matrix2[1][3]),
      //    double(matrix2[2][0]), double(matrix2[2][1]), double(matrix2[2][2]), double(matrix2[2][3]),
      //    double(matrix2[3][0]), double(matrix2[3][1]), double(matrix2[3][2]), double(matrix2[3][3]));
      //// the gui matrix messes with the scale. correct it here for now.
      //matrix = glm::scale(matrix, glm::vec3(context.GetGUIScaleX(), context.GetGUIScaleY(), 1.0f));
      //// the gui matrix doesn't align to exact pixel coords atm. correct it here for now.
      //matrix = glm::translate(matrix, glm::vec3(fractX, fractY, 0.0f));

      const float depth = CServiceBroker::GetWinSystem()->GetGfxContext().GetTransformDepth();

      VkPipeline pipeline;
      VkPipelineLayout pipelineLayout;
      //if (m_scissorClip)
      //{
      // clip using scissors
      //fprintf(stderr,
      //        "CVulkanGUIFontTTF::LastEnd: Using scissor clip for font %s with clip rect (%f, %f, "
      //        "%f, %f)\n",
      //        GetFontIdent().c_str(), double(clip.x1), double(clip.y1), double(clip.x2),
      //        double(clip.y2));
      //m_renderSystem->SetScissors(clip);

      CVulkanShaderFonts::VulkanUniformScissorClip uniform{};

      uniform.matrix = matrix;
      uniform.depth = depth;

      m_shaderFonts->UpdateUniformBuffer(renderImageIndex, uniform);
      pipeline = m_shaderFonts->VulkanPipeline(FONTS_TYPE_SCISSOR_CLIP);
      pipelineLayout = m_shaderFonts->VulkanPipelineLayout(FONTS_TYPE_SCISSOR_CLIP);
      //}
      //else
      //{
      //  // clip using vertex shader
      //  m_renderSystem->ResetScissors();

      //  CVulkanShaderFonts::VulkanUniformShaderClip uniform{};

      //  const glm::vec4 clipBoundaries = {
      //      (m_vertexTrans[i].m_clip.x1 - m_vertexTrans[i].m_translateX -
      //       m_vertexTrans[i].m_offsetX) /
      //          context.GetGUIScaleX(),
      //      (m_vertexTrans[i].m_clip.y1 - m_vertexTrans[i].m_translateY -
      //       m_vertexTrans[i].m_offsetY) /
      //          context.GetGUIScaleY(),
      //      (m_vertexTrans[i].m_clip.x2 - m_vertexTrans[i].m_translateX -
      //       m_vertexTrans[i].m_offsetX) /
      //          context.GetGUIScaleX(),
      //      (m_vertexTrans[i].m_clip.y2 - m_vertexTrans[i].m_translateY -
      //       m_vertexTrans[i].m_offsetY) /
      //          context.GetGUIScaleY()};

      //  const glm::vec4 textureSteps = {1.f / static_cast<float>(m_textureWidth),
      //                                  1.f / static_cast<float>(m_textureHeight), 1.f, 1.f};

      //  uniform.matrix = matrix;
      //  uniform.shaderClip = clipBoundaries;
      //  uniform.cordStep = textureSteps;
      //  uniform.depth = depth;

      //  m_shaderFonts->UpdateUniformBuffer(renderImageIndex, uniform);
      //  pipeline = m_shaderFonts->VulkanPipeline(FONTS_TYPE_SHADER_CLIP);
      //  pipelineLayout = m_shaderFonts->VulkanPipelineLayout(FONTS_TYPE_SHADER_CLIP);
      //}

      VkDeviceSize bufferOffset = 0;
      VkBuffer vertexBuffer = m_vertexTrans[i].m_vertexBuffer->bufferHandle->buffer;
      vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &bufferOffset);

      VkBuffer indexBuffer = staticIndexBuffer.memoryData.buffer;
      vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

      vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                              &m_shaderFonts->GetUniformBuffer(renderImageIndex)->descriptorSet, 0,
                              nullptr);
      vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1,
                              vkTexture->vkDescriptorSet(), 0, nullptr);
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
    //if (m_scissorClip)
    //  m_renderSystem->SetScissors(scissor);
  }
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

  const VkDeviceSize buffer_size = vertices.size() * sizeof(SVertex);

  VulkanMemoryData* memData = new VulkanMemoryData();
  VK_CHECK_RESULT(ref->m_renderSystem->DeviceQueue()->CreateBuffer(
                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      memData, buffer_size, vertices.data()),
                  CVertexBuffer());

  //  // We map the buffer once, so we can update it without having to map it again
  //VK_CHECK_RESULT(vkMapMemory(ref->m_renderSystem->vkDevice(), memData->memory, 0, buffer_size, 0,
  //                            (void**)&memData->mapped),
  //                CVertexBuffer());

  return CVertexBuffer(CVertexBuffer::BufferHandleType(memData), vertices.size() / 4, ref);
}

void CVulkanGUIFontTTF::VulkanDestroyVertexBuffer(CVulkanGUIFontTTF* ref, CVertexBuffer& buffer)
{
  ref->m_renderSystem->DeviceQueue()->DestroyBuffer(buffer.bufferHandle);
  delete buffer.bufferHandle;
  buffer.bufferHandle = nullptr;
}

std::unique_ptr<CTexture> CVulkanGUIFontTTF::ReallocTexture(unsigned int& newHeight)
{
  fprintf(stderr, "CVulkanGUIFontTTF::ReallocTexture: Reallocating texture for %s\n",
          GetFontIdent().c_str());
  newHeight = CTexture::PadPow2(newHeight);

  std::unique_ptr<CTexture> newTexture =
      CTexture::CreateTexture(m_textureWidth, newHeight, XB_FMT_A8);

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
  }

  m_textureStatus = TEXTURE_REALLOCATED;

  return newTexture;
}

bool CVulkanGUIFontTTF::CopyCharToTexture(
    FT_BitmapGlyph bitGlyph, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
  fprintf(stderr,
          "CVulkanGUIFontTTF::CopyCharToTexture: Copying char to texture for %s (%u, %u, %u, %u)\n",
          GetFontIdent().c_str(), x1, y1, x2, y2);
  FT_Bitmap bitmap = bitGlyph->bitmap;

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
    }
    break;

    case TEXTURE_VOID:
    default:
      break;
  }

  return true;
}

void CVulkanGUIFontTTF::DeleteHardwareTexture()
{
  //fprintf(stderr, "CVulkanGUIFontTTF::DeleteHardwareTexture: Deleting hardware texture for %s\n",
  //        GetFontIdent().c_str());
}

void CVulkanGUIFontTTF::CreateStaticIndexBuffers()
{
  if (staticIndexBuffer.created)
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

  VK_CHECK_RESULT(renderSystem->DeviceQueue()->CreateBuffer(
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      &staticIndexBuffer.memoryData, sizeof(index), &index));
  staticIndexBuffer.created = true;
}

void CVulkanGUIFontTTF::DestroyStaticIndexBuffers()
{
  if (!staticIndexBuffer.created)
    return;

  const auto renderSystem = dynamic_cast<CVulkanRenderSystem*>(CServiceBroker::GetRenderSystem());
  renderSystem->DeviceQueue()->DestroyBuffer(&staticIndexBuffer.memoryData);

  staticIndexBuffer.created = false;
}

CRect CVulkanGUIFontTTF::ClipRectToScissorRect(const CRect& rect)
{
  //fprintf(
  //    stderr,
  //    "CVulkanGUIFontTTF::ClipRectToScissorRect: Clipping rect (%f, %f, %f, %f) to scissor rect\n",
  //    double(rect.x1), double(rect.y1), double(rect.x2), double(rect.y2));
  //fprintf(
  //    stderr,
  //    "CVulkanGUIFontTTF::ClipRectToScissorRect: Using clip factor (%f, %f) and offset (%f, %f)\n",
  //    double(m_clipFactor.x), double(m_clipFactor.y), double(m_clipOffset.x),
  //    double(m_clipOffset.y));
  return CRect(rect.x1 * m_clipFactor.x + m_clipOffset.x, rect.y1 * m_clipFactor.y + m_clipOffset.y,
               rect.x2 * m_clipFactor.x + m_clipOffset.x,
               rect.y2 * m_clipFactor.y + m_clipOffset.y);
}

bool CVulkanGUIFontTTF::ScissorsCanEffectClipping(glm::vec2& factor, glm::vec2& offset)
{
  //const glm::mat4& projMatrix = globalMatrixProject;
  //const glm::mat4& modelMatrix = globalMatrixModview;
  const TransformMatrix& guiMatrix = CServiceBroker::GetWinSystem()->GetGfxContext().GetGUIMatrix();

  CRect viewPort; // absolute positions of corners
  CServiceBroker::GetRenderSystem()->GetViewPort(viewPort);

  ///* glScissor operates in window coordinates. In order that we can use it to
  // * perform clipping, we must ensure that there is an independent linear
  // * transformation from the coordinate system used by CGraphicContext::ClipRect
  // * to window coordinates, separately for X and Y (in other words, no
  // * rotation or shear is introduced at any stage). To do, this, we need to
  // * check that zeros are present in the following locations:
  // *
  // * GUI matrix:
  // * / * 0 * * \
  // * | 0 * * * |
  // * \ 0 0 * * /
  // *       ^ TransformMatrix::TransformX/Y/ZCoord are only ever called with
  // *         input z = 0, so this column doesn't matter
  // * Model-view matrix:
  // * / * 0 0 * \
  // * | 0 * 0 * |
  // * | 0 0 * * |
  // * \ * * * * /  <- eye w has no influence on window x/y (last column below
  // *                                                       is either 0 or ignored)
  // * Projection matrix:
  // * / * 0 0 0 \
  // * | 0 * 0 0 |
  // * | * * * * |  <- normalised device coordinate z has no influence on window x/y
  // * \ 0 0 * 0 /
  // *
  // * Some of these zeros are not strictly required to ensure this, but they tend
  // * to be zeroed in the common case, so by checking for zeros here, we simplify
  // * the calculation of the window x/y coordinates further down the line.
  // *
  // * (Minor detail: we don't quite deal in window coordinates as defined by
  // * OpenGL, because CRenderSystemGLES::SetScissors flips the Y axis. But all
  // * that's needed to handle that is an effective negation at the stage where
  // * Y is in normalised device coordinates.)
  // */
  //const bool clipPossible =
  //    guiMatrix.m[0][1] == 0 && guiMatrix.m[1][0] == 0 && guiMatrix.m[2][0] == 0 &&
  //    guiMatrix.m[2][1] == 0 && modelMatrix[1][0] == 0 && modelMatrix[2][0] == 0 &&
  //    modelMatrix[0][1] == 0 && modelMatrix[2][1] == 0 && modelMatrix[0][2] == 0 &&
  //    modelMatrix[1][2] == 0 && projMatrix[1][0] == 0 && projMatrix[2][0] == 0 &&
  //    projMatrix[3][0] == 0 && projMatrix[0][1] == 0 && projMatrix[2][1] == 0 &&
  //    projMatrix[3][1] == 0 && projMatrix[0][3] == 0 && projMatrix[1][3] == 0 &&
  //    projMatrix[3][3] == 0;

  //factor = glm::vec2(0.0f);
  //offset = glm::vec2(0.0f);

  //if (clipPossible)
  //{
  //  factor.x = guiMatrix.m[0][0] * modelMatrix[0][0] * projMatrix[0][0];
  //  offset.x = (guiMatrix.m[0][3] * modelMatrix[0][0] + modelMatrix[3][0]) * projMatrix[0][0];
  //  factor.y = guiMatrix.m[1][1] * modelMatrix[1][1] * projMatrix[1][1];
  //  offset.y = (guiMatrix.m[1][3] * modelMatrix[1][1] + modelMatrix[3][1]) * projMatrix[1][1];

  //  const float clipW =
  //      (guiMatrix.m[2][3] * modelMatrix[2][2] + modelMatrix[3][2]) * projMatrix[2][3];
  //  const float xMult = (viewPort.x2 - viewPort.x1) / (2 * clipW);
  //  const float yMult =
  //      (viewPort.y1 - viewPort.y2) / (2 * clipW); // correct for inverted window coordinate scheme
  //  factor *= glm::vec2(xMult, yMult);
  //  offset *= glm::vec2(xMult, yMult) +
  //            glm::vec2((viewPort.x2 + viewPort.x1) / 2, (viewPort.y2 + viewPort.y1) / 2);
  //}

  //return clipPossible;

  const float* projMatrix = glm::value_ptr(globalMatrixProject.Get());
  const float* modelMatrix = glm::value_ptr(globalMatrixModview.Get());

  const bool clipPossible =
      guiMatrix.m[0][1] == 0 && guiMatrix.m[1][0] == 0 && guiMatrix.m[2][0] == 0 &&
      guiMatrix.m[2][1] == 0 && modelMatrix[0 + 1 * 4] == 0 && modelMatrix[0 + 2 * 4] == 0 &&
      modelMatrix[1 + 0 * 4] == 0 && modelMatrix[1 + 2 * 4] == 0 && modelMatrix[2 + 0 * 4] == 0 &&
      modelMatrix[2 + 1 * 4] == 0 && projMatrix[0 + 1 * 4] == 0 && projMatrix[0 + 2 * 4] == 0 &&
      projMatrix[0 + 3 * 4] == 0 && projMatrix[1 + 0 * 4] == 0 && projMatrix[1 + 2 * 4] == 0 &&
      projMatrix[1 + 3 * 4] == 0 && projMatrix[3 + 0 * 4] == 0 && projMatrix[3 + 1 * 4] == 0 &&
      projMatrix[3 + 3 * 4] == 0;

  if (clipPossible)
  {
    factor.x = guiMatrix.m[0][0] * modelMatrix[0 + 0 * 4] * projMatrix[0 + 0 * 4];
    offset.x = (guiMatrix.m[0][3] * modelMatrix[0 + 0 * 4] + modelMatrix[0 + 3 * 4]) *
               projMatrix[0 + 0 * 4];
    factor.y = guiMatrix.m[1][1] * modelMatrix[1 + 1 * 4] * projMatrix[1 + 1 * 4];
    offset.y = (guiMatrix.m[1][3] * modelMatrix[1 + 1 * 4] + modelMatrix[1 + 3 * 4]) *
               projMatrix[1 + 1 * 4];
    float clipW = (guiMatrix.m[2][3] * modelMatrix[2 + 2 * 4] + modelMatrix[2 + 3 * 4]) *
                  projMatrix[3 + 2 * 4];
    float xMult = (viewPort.x2 - viewPort.x1) / (2 * clipW);
    float yMult =
        (viewPort.y1 - viewPort.y2) / (2 * clipW); // correct for inverted window coordinate scheme
    factor.x = factor.x * xMult;
    offset.x = offset.x * xMult + (viewPort.x2 + viewPort.x1) / 2;
    factor.y = factor.y * yMult;
    offset.y = offset.y * yMult + (viewPort.y2 + viewPort.y1) / 2;
  }

  return clipPossible;
}
