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
#include "rendering/vulkan/VulkanMatrix.h"
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
  m_color = glm::unpackUnorm4x8(color);

  //bool hasAlpha = m_texture.m_textures[m_currentFrame]->HasAlpha() || m_color.a < 1.0f;
  //const bool hasBlendColor = m_color != glm::vec4(1.0f);

  //if (m_diffuse.size())
  //{
  //  if (hasBlendColor)
  //  {
  //    m_renderSystem->EnableShader(VULKAN_SM_MULTI_BLENDCOLOR);
  //  }
  //  else
  //  {
  //    m_renderSystem->EnableShader(VULKAN_SM_MULTI);
  //  }

  //  hasAlpha |= m_diffuse.m_textures[0]->HasAlpha();

  //  // We don't need a 111R_RGBA version of the GLES 2.0 shaders, so in the
  //  // unlikely event of having an alpha-only texture, switch with the
  //  // diffuse.
  //  if (texture->GetSwizzle() == KD_TEX_SWIZ_111R)
  //  {
  //    texture->BindToUnit(1);
  //    m_diffuse.m_textures[0]->BindToUnit(0);
  //  }
  //  else
  //  {
  //    texture->BindToUnit(0);
  //    m_diffuse.m_textures[0]->BindToUnit(1);
  //  }
  //}
  //else
  //{
  //  if (hasBlendColor)
  //  {
  //    m_renderSystem->EnableShader(VULKAN_SM_TEXTURE);
  //  }
  //  else
  //  {
  //    m_renderSystem->EnableShader(VULKAN_SM_TEXTURE_NOBLEND);
  //  }

  //  texture->BindToUnit(0);
  //}

  //if (hasAlpha)
  //{
  ////  // See CGUIFontTTFGLES::FirstBegin for rationale. SDR uses accumulator
  ////  // coverage alpha; HDR FBO composite uses a compensated squared-alpha
  ////  // blend because the FBO is color-transformed to PQ/HLG before composite,
  ////  // and alpha blending in non-linear space is mathematically wrong.
  ////  if (CServiceBroker::GetWinSystem()->IsHdrComposite())
  ////    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA,
  ////                        GL_ONE_MINUS_SRC_ALPHA);
  ////  else
  ////    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
  ////  glEnable(GL_BLEND);
  //}
  //else
  //{
  ////  glDisable(GL_BLEND);
  //}

  m_packedVertices.clear();
  m_idx.clear();
}

void CVulkanGUITexture::End()
{
}

void CVulkanGUITexture::Draw(
    float* x, float* y, float* z, const CRect& texture, const CRect& diffuse, int orientation)
{
#define USE_BATCHING // Temporary define for test code to draw a single quad

#if USE_PARTICLES == 1 && defined(USE_BATCHING)
#if 0
  // TODO: Currently not used as we are using currently a single quad for all GUI textures.
  // This will be used when we implement batching of GUI textures.

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

  vertices[0].in_attrcol = {1.0f, 1.0f, 1.0f, 1.0f};
  vertices[1].in_attrcol = {0.0f, 1.0f, 0.0f, 1.0f};
  vertices[2].in_attrcol = {0.0f, 0.0f, 1.0f, 1.0f};
  vertices[3].in_attrcol = {1.0f, 0.0f, 1.0f, 1.0f};

  for (int i = 0; i < 4; i++)
  {
    vertices[i].in_attrpos = {x[i], y[i], z[i]};
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
#else
  // TODO: Temporary test code to draw a single quad. This will be replaced with the above code when we implement batching of GUI textures.
  const std::vector<KODI::RENDERING::VULKAN::Vertex> testVertexBuffer{
      {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
      {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
      {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}},
      {{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}}};
  m_packedVertices = testVertexBuffer;

  const std::vector<uint32_t> testIndexBuffer{0, 1, 2, 2, 3, 0};
  m_idx = testIndexBuffer;
#endif
#endif

  //--------------------------------------------------------------

  CVulkanTexture* vkTexture =
      dynamic_cast<CVulkanTexture*>(m_texture.m_textures[m_currentFrame].get());

  VkCommandBuffer commandBuffer = m_renderSystem->vkCurrentCommandBuffer();
  VkPipeline pipeline = m_shaderTexture->VulkanPipeline();
  VkPipelineLayout pipelineLayout = m_shaderTexture->VulkanPipelineLayout();

  const uint32_t renderImageIndex = m_renderSystem->vkCurrentRenderImageIndex();

  VulkanUniform uniform{};
  uniform.projectionMatrix = m_renderSystem->m_projectionMatrix;
  uniform.modelMatrix = m_renderSystem->m_modelMatrix;
  uniform.depth = 1.0f;

  m_renderSystem->ShaderControl()->UpdateUniformBuffer(renderImageIndex, uniform);
  m_shaderTexture->UpdateVerticesBuffer(renderImageIndex, m_packedVertices.data());
  m_shaderTexture->UpdateIndeciesBuffer(renderImageIndex, m_idx.data(), m_idx.size());

  VkViewport viewport{.x = x[0],
                      .y = y[0],
                      .width = x[2] - x[0],
                      .height = y[2] - y[0],
                      .minDepth = 0.0f,
                      .maxDepth = 1.0f};
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  // Update dynamic scissor state
  VkRect2D scissor{
      .offset = {.x = static_cast<int32_t>(x[0]), .y = static_cast<int32_t>(y[0])},
      .extent = {.width = static_cast<uint32_t>(x[2] - x[0]),
                 .height = static_cast<uint32_t>(y[2] - y[0])},
  };
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  // Environment
  vkCmdBindDescriptorSets(
      commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
      &m_renderSystem->ShaderControl()->GetUniformBuffer(renderImageIndex)->descriptorSet, 0,
      nullptr);
  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1,
                          vkTexture->vkDescriptorSet(), 0, nullptr);
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  // Bind triangle vertex m_buffer (contains position and colors)
  VkDeviceSize offsets[1]{0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1,
                         &m_shaderTexture->GetVertexBuffer(renderImageIndex)->buffer, offsets);
  vkCmdBindIndexBuffer(commandBuffer, m_shaderTexture->GetIndexBuffer(renderImageIndex)->buffer, 0,
                       VK_INDEX_TYPE_UINT32);
  // Draw indexed triangle
  vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_idx.size()), 1, 0, 0, 0);
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
