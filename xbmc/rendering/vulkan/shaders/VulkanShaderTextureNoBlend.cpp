/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanShaderTextureNoBlend.h"

#include "guilib/GUIFontTTF.h"
#include "rendering/vulkan/utils/VulkanInitStructs.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "utils/log.h"

#include <array>
#include <cassert>

namespace KODI::RENDERING::VULKAN
{

namespace
{
//constexpr const char* kVertexShaderFile = "vulkan_shader_gr3_vert_simple.spv";
//constexpr const char* kFragmentShaderFile = "vulkan_shader_gr3_frag_fonts.spv";
constexpr const char* kVertexShaderFile = "text.vert.spv";
constexpr const char* kFragmentShaderFile = "text.frag.spv";
//constexpr const char* kVertexShaderFile = "vulkan_shader_gr0_vert_test_triangle.spv";
//constexpr const char* kFragmentShaderFile = "vulkan_shader_gr0_frag_test_triangle.spv";
} /* namespace */

using namespace KODI::RENDERING::VULKAN::UTILS;

CVulkanShaderTextureNoBlend::CVulkanShaderTextureNoBlend(CVulkanDeviceQueue* deviceQueue,
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
}

bool CVulkanShaderTextureNoBlend::SetupFontTexture(uint32_t width, uint32_t height, uint32_t depth)
{
  m_fontTextureExtent = {width, height, depth};
  return true;
}

bool CVulkanShaderTextureNoBlend::Create(const VkPipelineCache& pipelineCache)
{

  return true;
}

void CVulkanShaderTextureNoBlend::Destroy()
{
  if (m_vkPipeline != VK_NULL_HANDLE)
  {
    // Destroy the Vulkan pipeline
    vkDestroyPipeline(m_vkDevice, m_vkPipeline, nullptr);
    m_vkPipeline = VK_NULL_HANDLE;
  }
  if (m_sampler != VK_NULL_HANDLE)
  {
    // Destroy the Vulkan sampler
    vkDestroySampler(m_vkDevice, m_sampler, nullptr);
    m_sampler = VK_NULL_HANDLE;
  }
  if (m_imageView != VK_NULL_HANDLE)
  {
    // Destroy the Vulkan image view
    vkDestroyImageView(m_vkDevice, m_imageView, nullptr);
    m_imageView = VK_NULL_HANDLE;
  }
  if (m_image != VK_NULL_HANDLE)
  {
    // Destroy the Vulkan image
    vkDestroyImage(m_vkDevice, m_image, nullptr);
    m_image = VK_NULL_HANDLE;
  }
  if (m_imageMemory != VK_NULL_HANDLE)
  {
    // Free the Vulkan image memory
    vkFreeMemory(m_vkDevice, m_imageMemory, nullptr);
    m_imageMemory = VK_NULL_HANDLE;
  }
}

VkPipeline CVulkanShaderTextureNoBlend::VulkanPipeline() const
{
  return m_vkPipeline;
}

} // namespace KODI::RENDERING::VULKAN
