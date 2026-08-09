/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanGUITexture.h"

#include "ServiceBroker.h"
#include "guilib/Texture.h"
#include "guilib/TextureFormats.h"
#include "guilib/graphics/vulkan/VulkanTexture.h"
#include "rendering/MatrixStack.h"
#include "rendering/vulkan/DynamicBuffers.h"
#include "rendering/vulkan/VulkanRenderSystem.h"
#include "rendering/vulkan/shaders/VulkanShaderControl.h"
#include "rendering/vulkan/shaders/VulkanShaderTexture.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "utils/MathUtils.h"
#include "utils/log.h"
#include "windowing/GraphicContext.h"
#include "windowing/WinSystem.h"

#include <cstddef>

#include <glm/gtc/type_ptr.hpp>

using namespace KODI::RENDERING::VULKAN;

namespace KODI::GUILIB::GRAPHICS::VULKAN
{

void CVulkanGUITexture::Register()
{
  CGUITexture::Register(CVulkanGUITexture::CreateTexture, CVulkanGUITexture::DrawQuad);
}

CGUITexture* CVulkanGUITexture::CreateTexture(
    float posX, float posY, float width, float height, const CTextureInfo& texture)
{
  return new CVulkanGUITexture(posX, posY, width, height, texture);
}

CVulkanGUITexture::CVulkanGUITexture(
    float posX, float posY, float width, float height, const CTextureInfo& texture)
  : CGUITexture(posX, posY, width, height, texture)
{
  using KODI::RENDERING::VULKAN::CVulkanRenderSystem;
  m_renderSystem = dynamic_cast<CVulkanRenderSystem*>(CServiceBroker::GetRenderSystem());
  m_shaderTexture = dynamic_cast<CVulkanShaderTexture*>(
      m_renderSystem->ShaderControl()->GetShader(VULKAN_SM_TEXTURE));

  CVulkanDynamicBuffers* dynamicBuffers = m_renderSystem->DynamicBuffers();
  m_uniformBuffer = dynamicBuffers->GetBuffer(BUFFER_TYPE_UNIFORM);
  m_vertexBuffer = dynamicBuffers->GetBuffer(BUFFER_TYPE_VERTEX);
  m_indexBuffer = dynamicBuffers->GetBuffer(BUFFER_TYPE_INDEX);
}

CVulkanGUITexture* CVulkanGUITexture::Clone() const
{
  return new CVulkanGUITexture(*this);
}

void CVulkanGUITexture::Begin(KODI::UTILS::COLOR::Color color)
{
  CVulkanTexture* texture =
      dynamic_cast<CVulkanTexture*>(m_texture.m_textures[m_currentFrame].get());
  texture->LoadToGPU();
  if (m_diffuse.size())
    m_diffuse.m_textures[0]->LoadToGPU();

  // Setup Colors
  // NOTE: Vulkan uses ARGB format, but we need to convert it to RGBA for the shader
  m_color.r = float((color >> 16) & 0xFF) / 255.0f;
  m_color.g = float((color >> 8) & 0xFF) / 255.0f;
  m_color.b = float((color >> 0) & 0xFF) / 255.0f;
  m_color.a = float((color >> 24) & 0xFF) / 255.0f;

  bool hasAlpha = m_texture.m_textures[m_currentFrame]->HasAlpha() || m_color.a < 1.0f;
  TexturePipelineType usedPipelineType;

  if (m_diffuse.size())
  {
    hasAlpha |= m_diffuse.m_textures[0]->HasAlpha();

    if (color == 0xFFFFFFFF)
    {
      fprintf(stderr, "Enabling shader: SM_MULTI\n");
      //m_renderSystem->EnableShader(ShaderMethodGL::SM_MULTI);
    }
    else
    {
      fprintf(stderr, "Enabling shader: SM_MULTI_BLENDCOLOR\n");
      //m_renderSystem->EnableShader(ShaderMethodGL::SM_MULTI_BLENDCOLOR);
    }

    m_diffuse.m_textures[0]->BindToUnit(1);
  }
  else
  {
    if (color == 0xFFFFFFFF)
    {
      m_usePushConst = false;
      if (hasAlpha)
        usedPipelineType = TEXTURE_TYPE_NO_BLEND;
      else
        usedPipelineType = TEXTURE_TYPE_NO_BLEND_NO_ALPHA;
    }
    else
    {
      m_usePushConst = true;
      if (hasAlpha)
        usedPipelineType = TEXTURE_TYPE_BLEND;
      else
        usedPipelineType = TEXTURE_TYPE_BLEND_NO_ALPHA;
    }

    m_usedPipeline = m_shaderTexture->VulkanPipeline(usedPipelineType);
    m_usedPipelineLayout = m_shaderTexture->VulkanPipelineLayout(usedPipelineType);
  }

  m_packedVertices.clear();
  m_idx.clear();
}

void CVulkanGUITexture::End()
{
  CVulkanTexture* vkTexture =
      dynamic_cast<CVulkanTexture*>(m_texture.m_textures[m_currentFrame].get());

  VkCommandBuffer commandBuffer = m_renderSystem->vkCurrentCommandBuffer();

  const uint32_t renderImageIndex = m_renderSystem->vkCurrentRenderImageIndex();

  CVulkanShaderTexture::VulkanUniform uniform{};
  uniform.projectionMatrix = KODI::RENDERING::globalMatrixProject;
  uniform.modelMatrix = KODI::RENDERING::globalMatrixModview;
  uniform.depth = 1.0f;

  VkBuffer buffer;
  VkDeviceSize bufferOffset;

  if (m_usePushConst)
  {
    vkCmdPushConstants(m_renderSystem->vkCurrentCommandBuffer(), m_usedPipelineLayout,
                       VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(glm::vec4), glm::value_ptr(m_color));
  }

  Vertex* vertices = static_cast<Vertex*>(m_vertexBuffer->AllocateOffset(
      sizeof(Vertex) * m_packedVertices.size(), buffer, bufferOffset));
  memcpy(vertices, m_packedVertices.data(), sizeof(Vertex) * m_packedVertices.size());
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, &buffer, &bufferOffset);

  uint32_t* indices = static_cast<uint32_t*>(
      m_indexBuffer->AllocateOffset(sizeof(uint32_t) * m_idx.size(), buffer, bufferOffset));
  memcpy(indices, m_idx.data(), sizeof(uint32_t) * m_idx.size());
  vkCmdBindIndexBuffer(commandBuffer, buffer, bufferOffset, VK_INDEX_TYPE_UINT32);

  m_shaderTexture->UpdateUniformBuffer(renderImageIndex, uniform);
  vkCmdBindDescriptorSets(
      commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_usedPipelineLayout, 0, 1,
      &m_shaderTexture->GetUniformBuffer(renderImageIndex)->descriptorSet, 0,
      nullptr);
  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_usedPipelineLayout, 1,
                          1, vkTexture->vkDescriptorSet(), 0, nullptr);
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_usedPipeline);

  // Draw indexed triangle
  vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_idx.size()), 1, 0, 0, 0);
}

void CVulkanGUITexture::Draw(
    float* x, float* y, float* z, const CRect& texture, const CRect& diffuse, int orientation)
{
  Vertex vertices[4];

  // Setup texture coordinates
  // TopLeft
  vertices[0].in_attrcord0 = {texture.x1, texture.y1};

  // TopRight
  if (orientation & 4)
  {
    vertices[1].in_attrcord0 = {texture.x1, texture.y2};
  }
  else
  {
    vertices[1].in_attrcord0 = {texture.x2, texture.y1};
  }

  // BottomRight
  vertices[2].in_attrcord0 = {texture.x2, texture.y2};

  // BottomLeft
  if (orientation & 4)
  {
    vertices[3].in_attrcord0 = {texture.x2, texture.y1};
  }
  else
  {
    vertices[3].in_attrcord0 = {texture.x1, texture.y2};
  }

  if (m_diffuse.size())
  {
    // TopLeft
    vertices[0].in_attrcord1 = {diffuse.x1, diffuse.y1};

    // TopRight
    if (m_info.orientation & 4)
    {
      vertices[1].in_attrcord1 = {diffuse.x1, diffuse.y2};
    }
    else
    {
      vertices[1].in_attrcord1 = {diffuse.x2, diffuse.y1};
    }

    // BottomRight
    vertices[2].in_attrcord1 = {diffuse.x2, diffuse.y2};

    // BottomLeft
    if (m_info.orientation & 4)
    {
      vertices[3].in_attrcord1 = {diffuse.x2, diffuse.y1};
    }
    else
    {
      vertices[3].in_attrcord1 = {diffuse.x1, diffuse.y2};
    }
  }

  for (int i = 0; i < 4; i++)
  {
    //vertices[i].in_attrpos = {x[i], y[i], z[i]};
    vertices[i].in_attrpos.x = x[i];
    vertices[i].in_attrpos.y = y[i];
    vertices[i].in_attrpos.z = z[i];
    m_packedVertices.push_back(vertices[i]);
  }

  if ((m_packedVertices.size() / 4) > (m_idx.size() / 6))
  {
    size_t i = m_packedVertices.size() - 4;
    m_idx.push_back(i + 0);
    m_idx.push_back(i + 1);
    m_idx.push_back(i + 2);
    m_idx.push_back(i + 2);
    m_idx.push_back(i + 3);
    m_idx.push_back(i + 0);
  }
}

void CVulkanGUITexture::DrawQuad(const CRect& rect,
                                 KODI::UTILS::COLOR::Color color,
                                 CTexture* texture,
                                 const CRect* texCoords,
                                 const float depth,
                                 const bool blending)
{
  fprintf(stderr,
          "CVulkanGUITexture::DrawQuad: rect = (%f, %f, %f, %f), color = 0x%08X, depth = %f, "
          "blending = %d\n",
          double(rect.x1), double(rect.y1), double(rect.x2), double(rect.y2), color, double(depth),
          blending);
  return;
  CVulkanRenderSystem* renderSystem =
      dynamic_cast<CVulkanRenderSystem*>(CServiceBroker::GetRenderSystem());
  CVulkanShaderTexture* shaderTexture = dynamic_cast<CVulkanShaderTexture*>(
      renderSystem->ShaderControl()->GetShader(VULKAN_SM_TEXTURE));
  VkCommandBuffer commandBuffer = renderSystem->vkCurrentCommandBuffer();
  const uint32_t renderImageIndex = renderSystem->vkCurrentRenderImageIndex();

  VkPipeline pipeline = shaderTexture->VulkanPipeline(TEXTURE_TYPE_BLEND);
  VkPipelineLayout pipelineLayout = shaderTexture->VulkanPipelineLayout(TEXTURE_TYPE_BLEND);
  CVulkanDynamicBuffers* dynamicBuffers = renderSystem->DynamicBuffers();
  CVulkanDynamicBuffer* uniformBuffer = dynamicBuffers->GetBuffer(BUFFER_TYPE_UNIFORM);
  CVulkanDynamicBuffer* vertexBuffer = dynamicBuffers->GetBuffer(BUFFER_TYPE_VERTEX);
  CVulkanDynamicBuffer* indexBuffer = dynamicBuffers->GetBuffer(BUFFER_TYPE_INDEX);

  if (texture)
  {
    texture->LoadToGPU();
    texture->BindToUnit(0);
  }

  if (blending)
  {
    //  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //  glEnable(GL_BLEND);
  }
  else
  {
    //  glDisable(GL_BLEND);
  }

  if (texture)
    renderSystem->EnableShader(VULKAN_SM_TEXTURE);
  else
    renderSystem->EnableShader(VULKAN_SM_DEFAULT);

  //glVertexAttribPointer(posLoc, 3, GL_FLOAT, 0, 0, ver);
  //if (texture)
  //  glVertexAttribPointer(tex0Loc, 2, GL_FLOAT, 0, 0, tex);

  //glEnableVertexAttribArray(posLoc);
  //if (texture)
  //  glEnableVertexAttribArray(tex0Loc);

  glm::vec4 col;
  std::array<Vertex, 4> ver;
  std::array<uint32_t, 4> idx = {0, 1, 3, 2}; // Determines order of triangle strip

  // Setup Colors
  // NOTE: Vulkan uses ARGB format, but we need to convert it to RGBA for the shader
  col.r = float((color >> 16) & 0xFF) / 255.0f;
  col.g = float((color >> 8) & 0xFF) / 255.0f;
  col.b = float((color >> 0) & 0xFF) / 255.0f;
  col.a = float((color >> 24) & 0xFF) / 255.0f;

  // bottom left
  ver[0].in_attrpos.x = rect.x1;
  ver[0].in_attrpos.y = rect.y1;
  ver[0].in_attrpos.z = 0;

  // bottom right
  ver[1].in_attrpos.x = rect.x2;
  ver[1].in_attrpos.y = rect.y1;
  ver[1].in_attrpos.z = 0;

  // top right
  ver[2].in_attrpos.x = rect.x2;
  ver[2].in_attrpos.y = rect.y2;
  ver[2].in_attrpos.z = 0;

  // top left
  ver[3].in_attrpos.x = rect.x1;
  ver[3].in_attrpos.y = rect.y2;
  ver[3].in_attrpos.z = 0;

  if (texture)
  {
    // Setup texture coordinates
    CRect coords = texCoords ? *texCoords : CRect(0.0f, 0.0f, 1.0f, 1.0f);
    ver[0].in_attrcord0.x = ver[3].in_attrcord0.x = coords.x1;
    ver[0].in_attrcord0.y = ver[1].in_attrcord0.y = coords.y1;
    ver[1].in_attrcord0.x = ver[2].in_attrcord0.x = coords.x2;
    ver[2].in_attrcord0.y = ver[3].in_attrcord0.y = coords.y2;
  }

  //VulkanUniform uniform{};
  //uniform.projectionMatrix = KODI::RENDERING::globalMatrixProject;
  //uniform.modelMatrix = KODI::RENDERING::globalMatrixModview;
  //uniform.depth = 1.0f;

  //vkCmdPushConstants(renderSystem->vkCurrentCommandBuffer(), shaderTexture->VulkanPipelineLayout(),
  //                   VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(glm::vec4), glm::value_ptr(col));

  //VkBuffer buffer;
  //VkDeviceSize bufferOffset;

  //Vertex* vertices = static_cast<Vertex*>(
  //    vertexBuffer->AllocateOffset(sizeof(Vertex) * ver.size(), buffer, bufferOffset));
  //memcpy(vertices, ver.data(), sizeof(Vertex) * ver.size());
  //vkCmdBindVertexBuffers(commandBuffer, 0, 1, &buffer, &bufferOffset);

  //uint32_t* indices = static_cast<uint32_t*>(
  //    indexBuffer->AllocateOffset(sizeof(uint32_t) * idx.size(), buffer, bufferOffset));
  //memcpy(indices, idx.data(), sizeof(uint32_t) * idx.size());
  //vkCmdBindIndexBuffer(commandBuffer, buffer, bufferOffset, VK_INDEX_TYPE_UINT32);

  //renderSystem->ShaderControl()->UpdateUniformBuffer(renderImageIndex, uniform);
  //vkCmdBindDescriptorSets(
  //    commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
  //    &renderSystem->ShaderControl()->GetUniformBuffer(renderImageIndex)->descriptorSet, 0,
  //    nullptr);
  //vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1,
  //                        vkTexture->vkDescriptorSet(), 0, nullptr);
  //vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  // Draw indexed triangle
  vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(idx.size()), 1, 0, 0, 0);
  CRenderSystemBase::m_GUIElementCount++;
}

} // namespace KODI::GUILIB::GRAPHICS::VULKAN
