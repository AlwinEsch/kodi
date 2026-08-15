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

#include <string>

using namespace KODI::RENDERING::VULKAN::UTILS;

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

CVulkanShaderControl::CVulkanShaderControl(VulkanData* vulkanData,
                                           CVulkanDeviceQueue* deviceQueue)
  : m_vkData(vulkanData),
    m_deviceQueue(deviceQueue)
{
}

bool CVulkanShaderControl::Init()
{
  /**
   * Doing below the initialization of all shader parts.
   *
   * They are done in a separete loops to have same type of initialization together, as some
   * shader parts are needed for the next shader part initialization.
   */
  //@{
  if (!CreateDescriptorSetLayouts())
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create descriptor set layout");
    return false;
  }

  if (!CreateDescriptorPool())
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create descriptor pool");
    return false;
  }

  if (!CreateSamplers())
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create samplers");
    return false;
  }

  return true;
}

void CVulkanShaderControl::DeInit()
{
  DestroyDescriptorPool();
  DestroyDescriptorSetLayouts();
  DestroySamplers();
}

bool CVulkanShaderControl::CreateAllShaders()
{
  for (const auto& entry : shaderList)
  {
    std::unique_ptr<IVulkanShader> shader = entry.create(m_vkData, m_deviceQueue);
    if (shader == nullptr)
    {
      CLog::Log(LOGERROR, "Vulkan: Failed to allocate shader: {}", entry.name);
      return false;
    }

    m_shaders[entry.id] = std::move(shader);
  }

  for (const auto& entry : m_shaders)
  {
    if (!entry.second->Create())
    {
      CLog::Log(LOGERROR, "Vulkan: Failed to create shader: {}", entry.first);
      return false;
    }
  }
  //@}

  return true;
}

void CVulkanShaderControl::DestroyAllShaders()
{
  // Implementation of DestroyAllShaders
  m_shaders.clear();
}

bool CVulkanShaderControl::CreateDescriptorSetLayouts()
{
  VkDescriptorSetLayoutCreateInfo layoutInfo = {};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = 1;
  layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_DESCRIPTOR_BUFFER_BIT_EXT;
  VkDescriptorSetLayoutBinding binding = {};
  {
    binding.binding = 0;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    binding.descriptorCount = 1;
    binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    binding.pImmutableSamplers = nullptr;

    layoutInfo.pBindings = &binding;

    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_vkData->vkDevice, &layoutInfo, nullptr,
                                                &m_vkData->vkDescriptorSetLayout_Uniform),
                    false);
  }

  {
    binding.binding = 0;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    binding.descriptorCount = 1;
    binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    binding.pImmutableSamplers = nullptr;

    layoutInfo.pBindings = &binding;

    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_vkData->vkDevice, &layoutInfo, nullptr,
                                                &m_vkData->vkDescriptorSetLayout_UniformDynamic),
                    false);
  }

  {
    binding.binding = 0;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding.descriptorCount = 1;
    binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;
    binding.pImmutableSamplers = nullptr;

    layoutInfo.pBindings = &binding;

    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_vkData->vkDevice, &layoutInfo, nullptr,
                                                &m_vkData->vkDescriptorSetLayout_Texture),
                    false);
  }

  return true;
}

void CVulkanShaderControl::DestroyDescriptorSetLayouts()
{
  if (m_vkData->vkDescriptorSetLayout_Texture != VK_NULL_HANDLE)
  {
    vkDestroyDescriptorSetLayout(m_vkData->vkDevice, m_vkData->vkDescriptorSetLayout_Texture,
                                 nullptr);
    m_vkData->vkDescriptorSetLayout_Texture = VK_NULL_HANDLE;
  }
  if (m_vkData->vkDescriptorSetLayout_Uniform != VK_NULL_HANDLE)
  {
    vkDestroyDescriptorSetLayout(m_vkData->vkDevice, m_vkData->vkDescriptorSetLayout_Uniform,
                                 nullptr);
    m_vkData->vkDescriptorSetLayout_Uniform = VK_NULL_HANDLE;
  }
}

bool CVulkanShaderControl::CreateDescriptorPool()
{
  // clang-format off
  std::array<VkDescriptorPoolSize, 3> poolSizes{};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = MIN_DESCRIPTORS_PER_TYPE;
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  poolSizes[1].descriptorCount = MIN_DESCRIPTORS_PER_TYPE;
  poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[2].descriptorCount = MIN_DESCRIPTORS_PER_TYPE + (MAX_TEXTURES + 1);
  // clang-format on

  VkDescriptorPoolCreateInfo poolCreateInfo = {};
  poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolCreateInfo.pPoolSizes = poolSizes.data();
  poolCreateInfo.maxSets = MAX_TEXTURES + 128;
  poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

  VK_CHECK_RESULT(vkCreateDescriptorPool(m_vkData->vkDevice, &poolCreateInfo, nullptr,
                                         &m_vkData->vkDescriptorPool),
                  false);

  return true;
}

void CVulkanShaderControl::DestroyDescriptorPool()
{
  if (m_vkData->vkDescriptorPool != VK_NULL_HANDLE)
  {
    vkDestroyDescriptorPool(m_vkData->vkDevice, m_vkData->vkDescriptorPool, nullptr);
    m_vkData->vkDescriptorPool = VK_NULL_HANDLE;
  }
}

bool CVulkanShaderControl::CreateSamplers()
{
  VkSamplerCreateInfo samplerInfo = vkSamplerCreateInfo();
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.anisotropyEnable = VK_FALSE;

  samplerInfo.magFilter = VK_FILTER_NEAREST;
  samplerInfo.minFilter = VK_FILTER_NEAREST;
  VK_CHECK_RESULT(
      vkCreateSampler(m_vkData->vkDevice, &samplerInfo, nullptr, &m_vkData->vkPointSampler), false);

  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  VK_CHECK_RESULT(
      vkCreateSampler(m_vkData->vkDevice, &samplerInfo, nullptr, &m_vkData->vkLinearSampler),
      false);

  return true;
}

void CVulkanShaderControl::DestroySamplers()
{
  if (m_vkData->vkPointSampler != VK_NULL_HANDLE)
  {
    vkDestroySampler(m_vkData->vkDevice, m_vkData->vkPointSampler, nullptr);
    m_vkData->vkPointSampler = VK_NULL_HANDLE;
  }
  if (m_vkData->vkLinearSampler != VK_NULL_HANDLE)
  {
    vkDestroySampler(m_vkData->vkDevice, m_vkData->vkLinearSampler, nullptr);
    m_vkData->vkLinearSampler = VK_NULL_HANDLE;
  }
}

//------------------------------------------------------------------------------

IVulkanShader* CVulkanShaderControl::GetShader(ShaderId shaderId) const
{
  auto it = m_shaders.find(shaderId);
  if (it != m_shaders.end())
  {
    return it->second.get();
  }
  return nullptr;
}

ShaderId CVulkanShaderControl::AddOptionalShader(std::unique_ptr<IVulkanShader> shader)
{
  ShaderId shaderId = m_nextShaderId++;
  m_shaders[shaderId] = std::move(shader);
  return shaderId;
}

} // namespace KODI::RENDERING::VULKAN
