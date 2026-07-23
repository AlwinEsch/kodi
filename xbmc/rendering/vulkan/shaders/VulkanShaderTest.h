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

namespace KODI::RENDERING::VULKAN
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

} // namespace KODI::RENDERING::VULKAN
