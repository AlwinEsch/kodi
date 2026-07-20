/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanShaderTexture.h"

#include "rendering/vulkan/VulkanMemoryBuffer.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "utils/log.h"

#include <cassert>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

using KODI::RENDERING::VULKAN::UTILS::ErrorString;

namespace
{
constexpr const char* kVertexShaderFile = "vulkan_shader_gr1_vert.spv";
constexpr const char* kFragmentShaderFile = "vulkan_shader_gr1_fraq_texture.spv";

const std::array<uint32_t, CVulkanShaderTexture::IndexCount()> kIndices = {0, 1, 2, 2, 3, 0};
} // namespace

CVulkanShaderTexture::CVulkanShaderTexture(CVulkanDeviceQueue* deviceQueue,
                                           VkDevice device,
                                           VkPipelineLayout pipelineLayout,
                                           VkRenderPass renderPass)
  : IVulkanShader(deviceQueue),
    m_vkDevice(device),
    m_vkPipelineLayout(pipelineLayout),
    m_vkRenderPass(renderPass)
{
  assert(m_vkDevice != VK_NULL_HANDLE);
  assert(m_vkPipelineLayout != VK_NULL_HANDLE);
  assert(m_vkRenderPass != VK_NULL_HANDLE);

  m_vertexBuffer = std::make_unique<CVulkanMemoryBuffer>(deviceQueue);
  m_indexBuffer = std::make_unique<CVulkanMemoryBuffer>(deviceQueue);
}

bool CVulkanShaderTexture::Create(const VkPipelineCache& pipelineCache)
{

  return true;
}

void CVulkanShaderTexture::Destroy()
{
  if (m_vkDevice)
  {
    vkDestroyPipeline(m_vkDevice, m_vkPipeline, nullptr);
    vkDestroyPipelineLayout(m_vkDevice, m_vkPipelineLayout, nullptr);
    m_vertexBuffer->DestroyBuffer();
    m_indexBuffer->DestroyBuffer();
    for (auto& buffer : m_uniformBuffers)
    {
      buffer.DestroyBuffer();
    }
  }
}

VkPipeline CVulkanShaderTexture::VulkanPipeline() const
{
  return m_vkPipeline;
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
