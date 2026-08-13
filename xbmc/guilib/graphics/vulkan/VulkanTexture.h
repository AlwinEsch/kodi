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

#include <vulkan/vulkan_core.h>

namespace KODI::RENDERING::VULKAN
{

class CVulkanDeviceQueue;
class CVulkanRenderSystem;
struct VulkanData;

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
  bool SupportsFormat(KD_TEX_FMT textureFormat, KD_TEX_SWIZ textureSwizzle) override;

  VkImageView vkImageView() const { return m_imageView; }
  const VkDescriptorSet* vkDescriptorSet() const { return &m_descriptorSet; }

  // Unused functions for Vulkan, but required to implement CTexture interface
  void BindToUnit(unsigned int unit) override {}
  void SyncGPU() override {}

private:
  // Vulkan resources, becomes set during constuction.
  KODI::RENDERING::VULKAN::CVulkanRenderSystem* m_renderSystem;
  KODI::RENDERING::VULKAN::CVulkanDeviceQueue* m_deviceQueue;
  const KODI::RENDERING::VULKAN::VulkanData* m_vkData;

  // Vulkan resources, becomes set during CreateTextureObject()
  VkImage m_image{VK_NULL_HANDLE};
  VkImageView m_imageView{VK_NULL_HANDLE};
  VkDeviceMemory m_imageMemory{VK_NULL_HANDLE};
  VkSampler m_sampler{VK_NULL_HANDLE};
  VkDescriptorSet m_descriptorSet{VK_NULL_HANDLE};
};

} // namespace KODI::GUILIB::GRAPHICS::VULKAN
