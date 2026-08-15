/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "rendering/vulkan/VulkanData.h"
#include "rendering/vulkan/shaders/IVulkanShader.h"

#include <array>
#include <cstring>

#include <glm/glm.hpp>

namespace KODI::RENDERING::VULKAN
{

typedef enum DefaultPipelineType
{
  DEFAULT_TYPE_BLEND = 0,
  DEFAULT_TYPE_NO_BLEND = 1,
  DEFAULT_TYPE_SIZE = 2
} DefaultPipelineType;

class CVulkanDeviceQueue;

class CVulkanShaderDefault : public IVulkanShader
{
public:
  CVulkanShaderDefault(const VulkanData* vulkanData, CVulkanDeviceQueue* deviceQueue);
  virtual ~CVulkanShaderDefault() = default;

  struct Vertex
  {
    glm::vec3 in_attrpos;
  };

  struct Uniform
  {
    glm::mat4 projectionMatrix;
    glm::mat4 modelMatrix;
  };

  void vkPipeline(VkPipeline& pipeline, VkPipelineLayout& layout, int type) override
  {
    pipeline = m_vkPipeline[type];
    layout = m_vkPipelineLayout[type];
  }

  VkPipelineLayout VulkanPipelineLayout(DefaultPipelineType type) const
  {
    return m_vkPipelineLayout[type];
  }
  VkPipeline VulkanPipeline(DefaultPipelineType type) const { return m_vkPipeline[type]; }

  VulkanMemoryData* GetUniformBuffer(uint32_t index) override { return &m_uniformBuffers[index]; }
  void UpdateUniformBuffer(uint32_t index, const Uniform& uniform);

protected:
  bool CreatePipelineLayout() override;
  void DestroyPipelineLayout() override;

  bool CreatePipeline() override;
  void DestroyPipeline() override;

  bool CreateUniformBuffers() override;
  void DestroyUniformBuffers() override;

private:
  std::array<VkPipelineLayout, DEFAULT_TYPE_SIZE> m_vkPipelineLayout{};
  std::array<VkPipeline, DEFAULT_TYPE_SIZE> m_vkPipeline{};
  std::array<VulkanMemoryData, MAX_CONCURRENT_FRAMES> m_uniformBuffers{};
};

} // namespace KODI::RENDERING::VULKAN


///*
// *  Copyright (C) 2005-2026 Team Kodi
// *  This file is part of Kodi - https://kodi.tv
// *
// *  SPDX-License-Identifier: GPL-2.0-or-later
// *  See LICENSES/README.md for more information.
// */
//
//#pragma once
//
//#include "rendering/vulkan/shaders/IVulkanShader.h"
//
//#include <glm/glm.hpp>
//
//namespace KODI::RENDERING::VULKAN
//{
//
//class CVulkanDeviceQueue;
//
//class CVulkanShaderDefault : public IVulkanShader
//{
//public:
//  CVulkanShaderDefault(const VulkanData* vulkanData, CVulkanDeviceQueue* deviceQueue);
//  virtual ~CVulkanShaderDefault() = default;
//
//  typedef enum PipelineType
//  {
//    TYPE_BLEND = 0,
//    TYPE_NO_BLEND = 1,
//    TYPE_SIZE = 2
//  } PipelineType;
//
//  struct Vertex
//  {
//    glm::vec3 in_attrpos;
//    //glm::vec2 in_attrcord0;
//    //glm::vec2 in_attrcord1;
//  };
//
//  struct Uniform
//  {
//    glm::mat4 projectionMatrix;
//    glm::mat4 modelMatrix;
//  };
//
//  void VulkanPipeline(VkPipelineLayout& layout, VkPipeline& pipeline, int type) override
//  {
//    layout = m_vkPipelineLayout[type];
//    pipeline = m_vkPipeline[type];
//  }
//
//  VkPipelineLayout VulkanPipelineLayout(PipelineType type) const { return m_vkPipelineLayout[type]; }
//  VkPipeline VulkanPipeline(PipelineType type) const { return m_vkPipeline[type]; }
//
//  VulkanMemoryData* GetUniformBuffer(uint32_t index) override { return &m_uniformBuffers[index]; }
//  void UpdateUniformBuffer(uint32_t index, const Uniform& uniform);
//
//protected:
//  bool CreatePipelineLayout() override;
//  void DestroyPipelineLayout() override;
//
//  bool CreatePipeline() override;
//  void DestroyPipeline() override;
//
//  bool CreateUniformBuffers() override;
//  void DestroyUniformBuffers() override;
//
//private:
//  std::array<VkPipelineLayout, TYPE_SIZE> m_vkPipelineLayout{};
//  std::array<VkPipeline, TYPE_SIZE> m_vkPipeline{};
//  std::array<VulkanMemoryData, MAX_CONCURRENT_FRAMES> m_uniformBuffers{};
//};
//
//} // namespace KODI::RENDERING::VULKAN
