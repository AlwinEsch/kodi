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

bool CVulkanShaderControl::CreateAllShaders(VkDevice device, VkRenderPass renderPass)
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
  if (!CreateUniformBuffers())
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create uniform buffers");
    return false;
  }

  if (!CreateSamplers())
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to create samplers");
    return false;
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

  DestroyUniformBuffers();
  DestroyDescriptorPool();
  DestroyDescriptorSetLayouts();
  DestroySamplers();
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
  std::array<VkDescriptorPoolSize, 2> poolSizes{};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = MIN_DESCRIPTORS_PER_TYPE;
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = MIN_DESCRIPTORS_PER_TYPE + (MAX_TEXTURES + 1);
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

bool CVulkanShaderControl::CreateUniformBuffers()
{
  assert(m_vkData->vkDescriptorPool != VK_NULL_HANDLE);
  assert(m_vkData->vkDescriptorSetLayout_Uniform != VK_NULL_HANDLE);

  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = m_vkData->vkDescriptorPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = &m_vkData->vkDescriptorSetLayout_Uniform;

  for (auto& entry : m_uniformBuffers)
  {
    VK_CHECK_RESULT(m_deviceQueue->CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                &entry, sizeof(VulkanUniform), nullptr),
                    false);

    VK_CHECK_RESULT(vkBindBufferMemory(m_vkData->vkDevice, entry.buffer, entry.memory, 0), false);
    VK_CHECK_RESULT(vkMapMemory(m_vkData->vkDevice, entry.memory, 0, sizeof(VulkanUniform), 0,
                                (void**)&entry.mapped),
                    false);

    VK_CHECK_RESULT(vkAllocateDescriptorSets(m_vkData->vkDevice, &allocInfo, &entry.descriptorSet),
                    false);

    // The m_buffer's information is passed using a m_descriptor info structure
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = entry.buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(VulkanUniform);

    VkWriteDescriptorSet uboWrite{};
    uboWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    uboWrite.dstSet = entry.descriptorSet;
    uboWrite.dstBinding = 0;
    uboWrite.dstArrayElement = 0;
    uboWrite.descriptorCount = 1;
    uboWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(m_vkData->vkDevice, 1, &uboWrite, 0, nullptr);
  }

  return true;
}

void CVulkanShaderControl::DestroyUniformBuffers()
{
  for (auto& entry : m_uniformBuffers)
  {
    if (entry.mapped)
    {
      vkUnmapMemory(m_vkData->vkDevice, entry.memory);
      entry.mapped = nullptr;
    }
    m_deviceQueue->DestroyBuffer(&entry);
    entry = {};
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

void CVulkanShaderControl::UpdateUniformBuffer(uint32_t index, const VulkanUniform& uniformData)
{
  memcpy(m_uniformBuffers[index].mapped, &uniformData, sizeof(VulkanUniform));
}

} // namespace KODI::RENDERING::VULKAN
