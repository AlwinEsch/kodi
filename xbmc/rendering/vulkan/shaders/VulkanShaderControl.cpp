/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanShaderControl.h"

#include "rendering/vulkan/VulkanDeviceQueue.h"
#include "rendering/vulkan/shaders/VulkanShaderDefault.h"
#include "rendering/vulkan/shaders/VulkanShaderFonts.h"
#include "rendering/vulkan/shaders/VulkanShaderFontsShaderClip.h"
#include "rendering/vulkan/shaders/VulkanShaderMulti.h"
#include "rendering/vulkan/shaders/VulkanShaderMultiBlendColor.h"
#include "rendering/vulkan/shaders/VulkanShaderTexture.h"
#include "rendering/vulkan/shaders/VulkanShaderTextureLim.h"
#include "rendering/vulkan/shaders/VulkanShaderTextureNoAlpha.h"
#include "rendering/vulkan/shaders/VulkanShaderTextureNoBlend.h"
#include "rendering/vulkan/utils/VulkanInitStructs.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "utils/log.h"

#include <array>
#include <string>
#include <string_view>

namespace KODI::RENDERING::VULKAN
{

// clang-format off
std::array<ShaderListEntry, 9> shaderList = {{
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

CVulkanShaderControl::CVulkanShaderControl(const VulkanData* vulkanData,
                                           CVulkanDeviceQueue* deviceQueue)
  : m_vulkanData(vulkanData),
    m_deviceQueue(deviceQueue)
{
}

bool CVulkanShaderControl::CreateAllShaders(VkDevice device, VkRenderPass renderPass)
{
  // Implementation of CreateAllShaders
  for (const auto& entry : shaderList)
  {
    std::unique_ptr<IVulkanShader> shader = entry.create(m_vulkanData, m_deviceQueue);
    if (shader == nullptr || !shader->Create())
    {
      CLog::Log(LOGERROR, "Vulkan: Failed to create shader: {}", entry.name);
      return false;
    }

    m_shaders[entry.id] = std::move(shader);
  }
  return true;
}

void CVulkanShaderControl::DestroyAllShaders()
{
  // Implementation of DestroyAllShaders
  m_shaders.clear();
}

IVulkanShader* CVulkanShaderControl::GetShader(ShaderId shaderId) const
{
  auto it = m_shaders.find(shaderId);
  if (it != m_shaders.end())
  {
    return it->second.get();
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

ShaderId CVulkanShaderControl::AddOptionalShader(std::unique_ptr<IVulkanShader> shader)
{
  ShaderId shaderId = m_nextShaderId++;
  m_shaders[shaderId] = std::move(shader);
  return shaderId;
}

} // namespace KODI::RENDERING::VULKAN
