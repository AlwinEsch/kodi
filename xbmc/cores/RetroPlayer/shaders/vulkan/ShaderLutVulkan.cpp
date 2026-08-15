/*
 *  Copyright (C) 2019-2025 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "ShaderLutVulkan.h"

#include "ShaderUtilsVulkan.h"
#include "cores/RetroPlayer/rendering/RenderContext.h"
#include "cores/RetroPlayer/shaders/IShaderPreset.h"
#include "guilib/graphics/vulkan/VulkanTexture.h"
#include "rendering/vulkan/VulkanRenderSystem.h"
#include "utils/log.h"

#include <utility>

using namespace KODI::GUILIB::GRAPHICS::VULKAN;
using namespace KODI::SHADER;

CShaderLutVulkan::CShaderLutVulkan(std::string id, std::string path)
  : IShaderLut(std::move(id), std::move(path))
{
}

bool CShaderLutVulkan::Create(const ShaderLut& lut)
{
  std::unique_ptr<CTexture> lutTexture(CreateLUTTexture(lut));
  if (!lutTexture)
  {
    CLog::Log(LOGWARNING, "CShaderLutVulkan::Create: Couldn't create texture for LUT: {}", lut.strId);
    return false;
  }

  m_texture = std::move(lutTexture);
  return true;
}

std::unique_ptr<CTexture> CShaderLutVulkan::CreateLUTTexture(const ShaderLut& lut)
{
  std::unique_ptr<CTexture> texture = CTexture::LoadFromFile(lut.path);
  //auto* textureGL = static_cast<CVulkanTexture*>(texture.get());

  //if (textureGL == nullptr)
  //{
  //  CLog::Log(LOGERROR, "CShaderLutVulkan::CreateLUTTexture: Couldn't open LUT: {}", lut.path);
  //  return std::unique_ptr<CTexture>();
  //}

  //if (lut.mipmap)
  //  textureGL->SetMipmapping();

  //textureGL->SetScalingMethod(lut.filterType == FilterType::LINEAR ? TEXTURE_SCALING::LINEAR
  //                                                                 : TEXTURE_SCALING::NEAREST);
  //textureGL->LoadToGPU();

  //const GLint wrapType = CShaderUtilsVulkan::TranslateWrapType(lut.wrapType);

  //glBindTexture(GL_TEXTURE_2D, textureGL->GetTextureID());
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapType);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapType);

  return texture;
}
