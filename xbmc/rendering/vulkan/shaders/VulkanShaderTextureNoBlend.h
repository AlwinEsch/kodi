/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "rendering/vulkan/VulkanMemoryBuffer.h"
#include "rendering/vulkan/shaders/IVulkanShader.h"

#include <glm/glm.hpp>

namespace KODI::RENDERING::VULKAN
{

class CVulkanDeviceQueue;

class CVulkanShaderTextureNoBlend : public IVulkanShader
{
public:
  CVulkanShaderTextureNoBlend(CVulkanDeviceQueue* deviceQueue,
                              VkDevice device,
                              VkPipelineLayout pipelineLayout,
                              VkRenderPass renderPass);
  virtual ~CVulkanShaderTextureNoBlend() = default;

  bool Create(const VkPipelineCache& pipelineCache) override;
  void Destroy() override;

  VkPipeline VulkanPipeline() const override;

  bool SetupFontTexture(uint32_t width, uint32_t height, uint32_t depth = 1);

private:
  VkExtent3D m_fontTextureExtent{0, 0, 1};

  struct Vertex
  {
    glm::vec3 position;
    glm::vec3 color;
  };

  VkSampler m_sampler{VK_NULL_HANDLE};
  VkImage m_image{VK_NULL_HANDLE};
  VkImageView m_imageView{VK_NULL_HANDLE};
  VkDeviceMemory m_imageMemory{VK_NULL_HANDLE};
  VkDescriptorPool m_descriptorPool{VK_NULL_HANDLE};
  VkDescriptorSet m_descriptorSet{VK_NULL_HANDLE};
  VkDescriptorSetLayout m_descriptorSetLayout{VK_NULL_HANDLE};

  VkDevice m_vkDevice{VK_NULL_HANDLE};
  VkPipeline m_vkPipeline{VK_NULL_HANDLE};
  VkPipelineLayout m_vkPipelineLayout{VK_NULL_HANDLE};
  VkRenderPass m_vkRenderPass{VK_NULL_HANDLE};
};

} // namespace KODI::RENDERING::VULKAN
