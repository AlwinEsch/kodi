/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "guilib/Texture.h"

#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

namespace KODI::RENDERING::VULKAN
{

class CVulkanRenderSystem;
class CVulkanShaderTexture;

} // namespace KODI::RENDERING::VULKAN

namespace KODI::GUILIB::GRAPHICS::VULKAN
{

class CVulkanTexture : public CTexture
{
public:
  CVulkanTexture(unsigned int width = 0, unsigned int height = 0, XB_FMT format = XB_FMT_A8R8G8B8);
  ~CVulkanTexture() override;

  // Implementation of CTexture
  void CreateTextureObject() override;
  void DestroyTextureObject() override;
  void LoadToGPU() override;
  void SyncGPU() override;
  void BindToUnit(unsigned int unit) override;
  bool SupportsFormat(KD_TEX_FMT textureFormat, KD_TEX_SWIZ textureSwizzle) override;

  VkImage vkImage() const { return m_image; }
  VkDeviceMemory vkImageMemory() const { return m_imageMemory; }
  VkImageView vkImageView() const { return m_imageView; }
  VkSampler vkSampler() const { return m_sampler; }

private:
  void createDescriptorSetLayout();
  void createDescriptorPool();
  void createDescriptorSets();

  VkDescriptorSetLayout m_descriptorSetLayout{VK_NULL_HANDLE};
  VkDescriptorPool m_descriptorPool{VK_NULL_HANDLE};

  KODI::RENDERING::VULKAN::CVulkanRenderSystem* m_renderSystem;
  KODI::RENDERING::VULKAN::CVulkanShaderTexture* m_shaderTexture;
  VkDevice m_vkDevice{VK_NULL_HANDLE};
  VkPhysicalDevice m_vkPhysicalDevice{VK_NULL_HANDLE};

  VkImage m_image{VK_NULL_HANDLE};
  VkImageView m_imageView{VK_NULL_HANDLE};
  VkDeviceMemory m_imageMemory{VK_NULL_HANDLE};
  VkSampler m_sampler{VK_NULL_HANDLE};
};

} // namespace KODI::GUILIB::GRAPHICS::VULKAN
