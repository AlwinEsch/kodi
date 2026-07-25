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
  m_testShaderTexture = dynamic_cast<CVulkanShaderTexture*>(
      m_renderSystem->ShaderControl()->GetShader(VULKAN_SM_TEXTURE));
  m_camera = new Camera;
  m_camera->type = Camera::CameraType::lookat;
  m_camera->setPosition(glm::vec3(0.0f, 0.0f, -2.5f));
  m_camera->setRotation(glm::vec3(0.0f));
  m_camera->setPerspective(60.0f, (float)1920 / (float)1080, 1.0f, 256.0f);
}

CVulkanGUITexture* CVulkanGUITexture::Clone() const
{
  return new CVulkanGUITexture(*this);
}

void CVulkanGUITexture::Begin(KODI::UTILS::COLOR::Color color)
{
  CTexture* texture = m_texture.m_textures[m_currentFrame].get();
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

  //m_packedVertices.clear();
}

void CVulkanGUITexture::End()
{
}

void CVulkanGUITexture::Draw(
    float* x, float* y, float* z, const CRect& texture, const CRect& diffuse, int orientation)
{
  Vertex verts[4];
  verts[0].in_attrpos = {x[0], y[0], z[0]};
  verts[0].in_attrcord0 = {texture.x1, texture.y1};
  verts[0].in_attrcord1 = {diffuse.x1, diffuse.y1};
  verts[0].in_attrcol = m_color;

  verts[1].in_attrpos = {x[1], y[1], z[1]};
  if (orientation & 4)
  {
    verts[1].in_attrcord0 = {texture.x1, texture.y2};
  }
  else
  {
    verts[1].in_attrcord0 = {texture.x2, texture.y1};
  }
  if (m_info.orientation & 4)
  {
    verts[1].in_attrcord1 = {diffuse.x1, diffuse.y2};
  }
  else
  {
    verts[1].in_attrcord1 = {diffuse.x2, diffuse.y1};
  }
  verts[1].in_attrcol = m_color;

  verts[2].in_attrpos = {x[2], y[2], z[2]};
  verts[2].in_attrcord0 = {texture.x2, texture.y2};
  verts[2].in_attrcord1 = {diffuse.x2, diffuse.y2};
  verts[2].in_attrcol = m_color;

  verts[3].in_attrpos = {x[3], y[3], z[3]};
  if (orientation & 4)
  {
    verts[3].in_attrcord0 = {texture.x2, texture.y1};
  }
  else
  {
    verts[3].in_attrcord0 = {texture.x1, texture.y2};
  }
  if (m_info.orientation & 4)
  {
    verts[3].in_attrcord1 = {diffuse.x2, diffuse.y1};
  }
  else
  {
    verts[3].in_attrcord1 = {diffuse.x1, diffuse.y2};
  }
  verts[3].in_attrcol = m_color;

  uint32_t indexBuffer = m_renderSystem->vkIndexBuffer();
  m_testShaderTexture->UpdateVerticesBuffer(indexBuffer, verts[0]);

  //--------------------------------------------------------------

  VkCommandBuffer commandBuffer = m_renderSystem->vkCurrentCommandBuffer();
  VkPipeline pipeline = m_testShaderTexture->VulkanPipeline();
  VkPipelineLayout pipelineLayout = m_testShaderTexture->VulkanPipelineLayout();

  // Update the uniform m_buffer for the next frame
  ShaderData shaderData{};
  //const float* projMatrix = vulkanMatrixProject.Get();
  //const float* modelMatrix = vulkanMatrixModview.Get();

  //memcpy(glm::value_ptr(shaderData.projectionMatrix), projMatrix, 16 * sizeof(float));
  //memcpy(glm::value_ptr(shaderData.modelMatrix), modelMatrix, 16 * sizeof(float));

  shaderData.projectionMatrix = m_camera->matrices.perspective;
  shaderData.viewMatrix = m_camera->matrices.view;
  shaderData.modelMatrix = glm::mat4(1.0f);

  m_testShaderTexture->UpdateUniformBuffer(indexBuffer, shaderData);

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
        .extent = {.width = static_cast<uint32_t>(x[2] - x[0]), .height = static_cast<uint32_t>(y[2] - y[0])},
    };
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  // Bind m_descriptor set for the current frame's uniform m_buffer, so the shader uses the data from that m_buffer for this draw
  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                          &m_testShaderTexture->GetUniformBuffer(indexBuffer)->descriptorSet, 0,
                          nullptr);
  // Bind the rendering m_pipeline
  // The m_pipeline (state object) contains all states of the rendering m_pipeline, binding it will set all the states specified at m_pipeline creation time
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
  // Bind triangle vertex m_buffer (contains position and colors)
  VkDeviceSize offsets[1]{0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_testShaderTexture->GetVertexBuffer()->buffer,
                         offsets);
  // Bind triangle index m_buffer
  vkCmdBindIndexBuffer(commandBuffer, m_testShaderTexture->GetIndexBuffer()->buffer, 0,
                       VK_INDEX_TYPE_UINT32);
  // Draw indexed triangle
  vkCmdDrawIndexed(commandBuffer, m_testShaderTexture->GetIndexBuffer()->count, 1, 0, 0, 0);
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
//
//void CVulkanGUITexture::RenderTriangle(float x, float y, float width, float height)
//{
//  VkCommandBuffer commandBuffer = m_renderSystem->vkCurrentCommandBuffer();
//  VkPipeline pipeline = m_testShaderTexture->VulkanPipeline();
//  VkPipelineLayout pipelineLayout = m_testShaderTexture->VulkanPipelineLayout();
//
//  // Update the uniform m_buffer for the next frame
//  ShaderData shaderData{};
//  const float* projMatrix = vulkanMatrixProject.Get();
//  const float* modelMatrix = vulkanMatrixModview.Get();
//
//  memcpy(glm::value_ptr(shaderData.projectionMatrix), projMatrix, 16 * sizeof(float));
//  memcpy(glm::value_ptr(shaderData.modelMatrix), modelMatrix, 16 * sizeof(float));
//
//  //shaderData.projectionMatrix = m_camera->matrices.perspective;
//  //shaderData.viewMatrix = m_camera->matrices.view;
//  //shaderData.modelMatrix = glm::mat4(1.0f);
//
//
//  uint32_t indexBuffer = m_renderSystem->vkIndexBuffer();
//
//  m_testShaderTexture->UpdateUniformBuffer(indexBuffer, shaderData);
//
//  //VkViewport viewport{
//  //    .x = x, .y = y, .width = width, .height = height, .minDepth = 0.0f, .maxDepth = 1.0f};
//  //vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
//  //// Update dynamic scissor state
//  //VkRect2D scissor{
//  //    .offset = {.x = static_cast<int32_t>(x), .y = static_cast<int32_t>(y)},
//  //    .extent = {.width = static_cast<uint32_t>(width), .height = static_cast<uint32_t>(height)},
//  //};
//  //vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
//  // Bind m_descriptor set for the current frame's uniform m_buffer, so the shader uses the data from that m_buffer for this draw
//  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
//                          &m_testShaderTexture->GetUniformBuffer(indexBuffer)->descriptorSet, 0,
//                          nullptr);
//  // Bind the rendering m_pipeline
//  // The m_pipeline (state object) contains all states of the rendering m_pipeline, binding it will set all the states specified at m_pipeline creation time
//  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
//  // Bind triangle vertex m_buffer (contains position and colors)
//  VkDeviceSize offsets[1]{0};
//  vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_testShaderTexture->GetVertexBuffer()->buffer,
//                         offsets);
//  // Bind triangle index m_buffer
//  vkCmdBindIndexBuffer(commandBuffer, m_testShaderTexture->GetIndexBuffer()->buffer, 0,
//                       VK_INDEX_TYPE_UINT32);
//  // Draw indexed triangle
//  vkCmdDrawIndexed(commandBuffer, m_testShaderTexture->GetIndexBuffer()->count, 1, 0, 0, 0);
//}

} // namespace KODI::GUILIB::GRAPHICS::VULKAN
