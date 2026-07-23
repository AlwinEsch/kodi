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

  bool Create(const VkPipelineCache& pipelineCache) override;
  void Destroy() override;

  VkPipeline VulkanPipeline() const override { return m_vkPipeline; }
  CVulkanMemoryBuffer* VertexBuffer() { return m_vertexBuffer.get(); }

private:
  /// Properties of the vertices used in this sample.
  struct Vertex
  {
    glm::vec3 position;
    glm::vec3 color;
  };

  std::unique_ptr<CVulkanMemoryBuffer> m_vertexBuffer;

  VkPipeline m_vkPipeline{VK_NULL_HANDLE};
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
