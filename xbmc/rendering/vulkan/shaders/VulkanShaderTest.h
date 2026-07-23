/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once
#include "rendering/vulkan/shaders/IVulkanShader.h"

#include <memory>

#include <glm/glm.hpp>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

class CVulkanDeviceQueue;
class CVulkanMemoryBuffer;

class CVulkanShaderTest : public IVulkanShader
{
public:
  CVulkanShaderTest(const VulkanData* vkData, CVulkanDeviceQueue* deviceQueue);
  virtual ~CVulkanShaderTest() = default;

  bool Create() override;
  void Destroy() override;

  VkPipeline VulkanPipeline() const override { return m_vkPipeline; }
  VulkanMemoryData* VertexBuffer() { return &m_vertexBuffer; }

private:
  /**
   * @brief Creates a Vulkan pipeline layout.
   *
   * The pipeline layout is used to define the interface between shader stages and shader resources.
   *
   * @param[in] layout [optional] The descriptor set layout to use for the pipeline layout.
   * @return The created pipeline layout, or VK_NULL_HANDLE on failure.
   *
   * @note Descruction is inside the @ref Destroy() function, which is called in the destructor of CVulkanRenderSystem.
   *
   * Documentation about @ref vkCreatePipelineLayout and @ref vkDestroyPipelineLayout is available at:
   * - https://docs.vulkan.org/refpages/latest/refpages/source/vkCreatePipelineLayout.html
   * - https://docs.vulkan.org/spec/latest/chapters/descriptorsets.html
   */
  VkPipelineLayout CreatePipelineLayout(VkDescriptorSetLayout layout = VK_NULL_HANDLE);

  /// Properties of the vertices used in this sample.
  struct Vertex
  {
    glm::vec3 position;
    glm::vec3 color;
  };

  VulkanMemoryData m_vertexBuffer{};

  VkPipeline m_vkPipeline{VK_NULL_HANDLE};
  VkPipelineLayout m_vkPipelineLayout{VK_NULL_HANDLE};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
