/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanShaderControl.h"

#include "VulkanShaderDefault.h"
#include "VulkanShaderFonts.h"
#include "VulkanShaderFontsShaderClip.h"
#include "VulkanShaderMulti.h"
#include "VulkanShaderMultiBlendColor.h"
#include "VulkanShaderTest.h"
#include "VulkanShaderTexture.h"
#include "VulkanShaderTextureLim.h"
#include "VulkanShaderTextureNoAlpha.h"
#include "VulkanShaderTextureNoBlend.h"
#include "utils/log.h"

#include <array>
#include <string>
#include <string_view>

namespace KODI::RENDERING::VULKAN
{

// clang-format off
std::array<ShaderListEntry, 10> shaderList = {{
    {VULKAN_SM_TEST, ObjectFactory<CVulkanShaderTest>, "TestShader"},
    {VULKAN_SM_DEFAULT, ObjectFactory<CVulkanShaderDefault>, "Default shader"},
    {VULKAN_SM_TEXTURE, ObjectFactory<CVulkanShaderTexture>, "Texture shader"},
    {VULKAN_SM_TEXTURE_LIM, ObjectFactory<CVulkanShaderTextureLim>, "Texture Lim shader"},
    {VULKAN_SM_TEXTURE_NOALPHA, ObjectFactory<CVulkanShaderTextureNoAlpha>, "Texture no alpha"},
    {VULKAN_SM_TEXTURE_NOBLEND, ObjectFactory<CVulkanShaderTextureNoBlend>, "Texture no blending"},
    {VULKAN_SM_MULTI, ObjectFactory<CVulkanShaderMulti>, "Multi shader"},
    {VULKAN_SM_FONTS, ObjectFactory<CVulkanShaderFonts>, "Fonts shader"},
    {VULKAN_SM_FONTS_SHADER_CLIP, ObjectFactory<CVulkanShaderFontsShaderClip>, "Fonts Shader Clip shader"},
    {VULKAN_SM_MULTI_BLENDCOLOR, ObjectFactory<CVulkanShaderMultiBlendColor>, "Multi Blend Color shader"},
}};
// clang-format on

bool CVulkanShaderControl::CreateAllShaders(VkDevice device,
                                            VkPipelineLayout pipelineLayout,
                                            VkRenderPass renderPass)
{
  // Implementation of CreateAllShaders
  for (const auto& entry : shaderList)
  {
    std::shared_ptr<IVulkanShader> shader = entry.create(device, pipelineLayout, renderPass);
    if (shader == nullptr || !shader->Create())
    {
      CLog::Log(LOGERROR, "Vulkan: Failed to create shader: {}", entry.name);
      return false;
    }

    m_shaders[entry.id] = shader;
  }
  return true;
}

void CVulkanShaderControl::DestroyAllShaders()
{
  // Implementation of DestroyAllShaders
  m_shaders.clear();
}

std::shared_ptr<IVulkanShader> CVulkanShaderControl::GetShader(ShaderId shaderId) const
{
  auto it = m_shaders.find(shaderId);
  if (it != m_shaders.end())
  {
    return it->second;
  }
  return nullptr;
}

VkPipeline CVulkanShaderControl::GetPipeline(ShaderId shaderId) const
{
  auto shader = GetShader(shaderId);
  if (shader)
  {
    return shader->VulkanPipeline();
  }
  return VK_NULL_HANDLE;
}

ShaderId CVulkanShaderControl::AddOptionalShader(const std::shared_ptr<IVulkanShader>& shader)
{
  ShaderId shaderId = m_nextShaderId++;
  m_shaders[shaderId] = shader;
  return shaderId;
}

} // namespace KODI::RENDERING::VULKAN
