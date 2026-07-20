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

#include <memory>

#include <glm/glm.hpp>
#include <vector>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

class CVulkanDeviceQueue;
class CVulkanMemoryBuffer;

class CVulkanShaderTexture : public IVulkanShader
{
public:
  CVulkanShaderTexture(CVulkanDeviceQueue* deviceQueue,
                       VkDevice device,
                       VkPipelineLayout pipelineLayout,
                       VkRenderPass renderPass);
  virtual ~CVulkanShaderTexture() = default;

  bool Create(const VkPipelineCache& pipelineCache) override;
  void Destroy() override;

  VkPipeline VulkanPipeline() const override;

  struct Vertex
  {
    glm::vec3 in_attrpos;
    glm::vec4 in_attrcol;
    glm::vec2 in_attrcord0;
    glm::vec2 in_attrcord1;
  } m_vertexData[4];

  struct UniformData
  {
    glm::mat4 projection;
    glm::mat4 modelView;
    glm::vec4 viewPos;
    // This is used to change the bias for the level-of-detail (mips) in the fragment shader
    float lodBias = 0.0f;
  } m_uniformData;

  constexpr static size_t VertexSize() { return sizeof(Vertex); }
  constexpr static size_t VertexCount() { return 4; }
  constexpr static size_t IndexCount() { return 6; }

private:
  std::unique_ptr<CVulkanMemoryBuffer> m_vertexBuffer;
  std::unique_ptr<CVulkanMemoryBuffer> m_indexBuffer;
  std::vector<CVulkanMemoryBuffer> m_uniformBuffers;

  VkDevice m_vkDevice{VK_NULL_HANDLE};
  VkPipeline m_vkPipeline{VK_NULL_HANDLE};
  VkPipelineLayout m_vkPipelineLayout{VK_NULL_HANDLE};
  VkRenderPass m_vkRenderPass{VK_NULL_HANDLE};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
