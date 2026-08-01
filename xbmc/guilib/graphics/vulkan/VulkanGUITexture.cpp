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
  m_dynamicBuffers = m_renderSystem->DynamicBuffers();
  m_uniformBuffer = m_dynamicBuffers->GetBuffer(BUFFER_TYPE_UNIFORM);
  m_vertexBuffer = m_dynamicBuffers->GetBuffer(BUFFER_TYPE_VERTEX);
  m_indexBuffer = m_dynamicBuffers->GetBuffer(BUFFER_TYPE_INDEX);
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

  if (m_diffuse.size())
  {
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

    hasAlpha |= m_diffuse.m_textures[0]->HasAlpha();

    m_diffuse.m_textures[0]->BindToUnit(1);
  }
  //else
  //{
  //  if (color == 0xFFFFFFFF)
  //  {
  //    //fprintf(stderr, "Enabling shader: SM_TEXTURE_NOBLEND\n");
  //    //m_renderSystem->EnableShader(ShaderMethodGL::SM_TEXTURE_NOBLEND);
  //  }
  //  //else
  //  //{
  //  //  fprintf(stderr, "Enabling shader: SM_TEXTURE\n");
  //  //  //m_renderSystem->EnableShader(ShaderMethodGL::SM_TEXTURE);
  //  //}
  //}

  m_packedVertices.clear();
  m_idx.clear();
}

void CVulkanGUITexture::End()
{

  //--------------------------------------------------------------

  CVulkanTexture* vkTexture =
      dynamic_cast<CVulkanTexture*>(m_texture.m_textures[m_currentFrame].get());

  VkCommandBuffer commandBuffer = m_renderSystem->vkCurrentCommandBuffer();
  VkPipeline pipeline = m_shaderTexture->VulkanPipeline();
  VkPipelineLayout pipelineLayout = m_shaderTexture->VulkanPipelineLayout();

  const uint32_t renderImageIndex = m_renderSystem->vkCurrentRenderImageIndex();

  VulkanUniform uniform{};
  uniform.projectionMatrix = KODI::RENDERING::globalMatrixProject;
  uniform.modelMatrix = KODI::RENDERING::globalMatrixModview;
  uniform.depth = 1.0f;

  VkBuffer buffer;
  VkDeviceSize bufferOffset;

  Vertex* vertices = static_cast<Vertex*>(m_vertexBuffer->AllocateOffset(
      sizeof(Vertex) * m_packedVertices.size(), buffer, bufferOffset));
  memcpy(vertices, m_packedVertices.data(), sizeof(Vertex) * m_packedVertices.size());
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, &buffer, &bufferOffset);

  uint32_t* indices = static_cast<uint32_t*>(
      m_indexBuffer->AllocateOffset(sizeof(uint32_t) * m_idx.size(), buffer, bufferOffset));
  memcpy(indices, m_idx.data(), sizeof(uint32_t) * m_idx.size());
  vkCmdBindIndexBuffer(commandBuffer, buffer, bufferOffset, VK_INDEX_TYPE_UINT32);

  m_renderSystem->ShaderControl()->UpdateUniformBuffer(renderImageIndex, uniform);
  vkCmdBindDescriptorSets(
      commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
      &m_renderSystem->ShaderControl()->GetUniformBuffer(renderImageIndex)->descriptorSet, 0,
      nullptr);
  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1,
                          vkTexture->vkDescriptorSet(), 0, nullptr);
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

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

  vkCmdPushConstants(m_renderSystem->vkCurrentCommandBuffer(),
                     m_shaderTexture->VulkanPipelineLayout(), VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                     sizeof(glm::vec4), glm::value_ptr(m_color));

  //fprintf(stderr, "CVulkanGUITexture::Begin: color = %f, %f, %f, %f\n", double(m_color.r),
  //        double(m_color.g), double(m_color.b), double(m_color.a));

  for (int i = 0; i < 4; i++)
  {
    //vertices[i].in_attrpos = {x[i], y[i], z[i]};
    vertices[i].in_attrpos.x = x[i];
    vertices[i].in_attrpos.y = y[i];
    vertices[i].in_attrpos.z = z[i];
    m_packedVertices.push_back(vertices[i]);
  }

  //fprintf(stderr, "DrawQuad: x1=%.02f, y1=%.02f, "
  //                          "x2=%.02f, y2=%.02f, "
  //                          "x3=%.02f, y3=%.02f, "
  //                          "x4=%.02f, y4=%.02f\n",
  // double(x[0]), double(y[0]), double(x[1]), double(y[1]),
  // double(x[2]), double(y[2]), double(x[3]), double(y[3]));

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
  CVulkanRenderSystem* renderSystem =
      dynamic_cast<CVulkanRenderSystem*>(CServiceBroker::GetRenderSystem());
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

  //VerifyVulkanState();

  //GLubyte col[4];
  //GLfloat ver[4][3];
  //GLfloat tex[4][2];
  //GLubyte idx[4] = {0, 1, 3, 2}; // Determines order of triangle strip

  if (texture)
    renderSystem->EnableShader(VULKAN_SM_TEXTURE);
  else
    renderSystem->EnableShader(VULKAN_SM_DEFAULT);

  //GLint posLoc = renderSystem->GUIShaderGetPos();
  //GLint tex0Loc = renderSystem->GUIShaderGetCoord0();
  //GLint uniColLoc = renderSystem->GUIShaderGetUniCol();
  //GLint depthLoc = renderSystem->GUIShaderGetDepth();

  //glVertexAttribPointer(posLoc, 3, GL_FLOAT, 0, 0, ver);
  //if (texture)
  //  glVertexAttribPointer(tex0Loc, 2, GL_FLOAT, 0, 0, tex);

  //glEnableVertexAttribArray(posLoc);
  //if (texture)
  //  glEnableVertexAttribArray(tex0Loc);

  //// Setup Colors
  //col[0] = KODI::UTILS::RENDER::GetChannelFromARGB(KODI::UTILS::VULKAN::ColorChannel::R, color);
  //col[1] = KODI::UTILS::RENDER::GetChannelFromARGB(KODI::UTILS::VULKAN::ColorChannel::G, color);
  //col[2] = KODI::UTILS::RENDER::GetChannelFromARGB(KODI::UTILS::VULKAN::ColorChannel::B, color);
  //col[3] = KODI::UTILS::RENDER::GetChannelFromARGB(KODI::UTILS::VULKAN::ColorChannel::A, color);

  //glUniform4f(uniColLoc, col[0] / 255.0f, col[1] / 255.0f, col[2] / 255.0f, col[3] / 255.0f);
  //glUniform1f(depthLoc, depth);

  //ver[0][0] = ver[3][0] = rect.x1;
  //ver[0][1] = ver[1][1] = rect.y1;
  //ver[1][0] = ver[2][0] = rect.x2;
  //ver[2][1] = ver[3][1] = rect.y2;
  //ver[0][2] = ver[1][2] = ver[2][2] = ver[3][2] = 0;

  //if (texture)
  //{
  //  // Setup texture coordinates
  //  CRect coords = texCoords ? *texCoords : CRect(0.0f, 0.0f, 1.0f, 1.0f);
  //  tex[0][0] = tex[3][0] = coords.x1;
  //  tex[0][1] = tex[1][1] = coords.y1;
  //  tex[1][0] = tex[2][0] = coords.x2;
  //  tex[2][1] = tex[3][1] = coords.y2;
  //}

  //glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, idx);
  //CRenderSystemBase::m_GUIElementCount++;

  //glDisableVertexAttribArray(posLoc);
  //if (texture)
  //  glDisableVertexAttribArray(tex0Loc);

  renderSystem->DisableShader();
}

} // namespace KODI::GUILIB::GRAPHICS::VULKAN
