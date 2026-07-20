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

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

class CVulkanDeviceQueue;

class CVulkanShaderFontsShaderClip : public IVulkanShader
{
public:
  CVulkanShaderFontsShaderClip(CVulkanDeviceQueue* deviceQueue,
                               VkDevice device,
                               VkPipelineLayout pipelineLayout,
                               VkRenderPass renderPass);
  virtual ~CVulkanShaderFontsShaderClip() = default;

  bool Create(const VkPipelineCache& pipelineCache) override;
  void Destroy() override;

  VkPipeline VulkanPipeline() const override;

private:
  /// Properties of the vertices used in this sample.
  struct Vertex
  {
    glm::vec3 position;
    glm::vec3 color;
  };

  VkDevice m_vkDevice{VK_NULL_HANDLE};
  VkPipeline m_vkPipeline{VK_NULL_HANDLE};
  VkPipelineLayout m_vkPipelineLayout{VK_NULL_HANDLE};
  VkRenderPass m_vkRenderPass{VK_NULL_HANDLE};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI