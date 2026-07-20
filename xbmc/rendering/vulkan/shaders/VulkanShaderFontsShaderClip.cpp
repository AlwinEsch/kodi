/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanShaderFontsShaderClip.h"

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

CVulkanShaderFontsShaderClip::CVulkanShaderFontsShaderClip(CVulkanDeviceQueue* deviceQueue,
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

bool CVulkanShaderFontsShaderClip::Create(const VkPipelineCache& pipelineCache)
{


  return true;
}

void CVulkanShaderFontsShaderClip::Destroy()
{

}

VkPipeline CVulkanShaderFontsShaderClip::VulkanPipeline() const
{
  return m_vkPipeline;
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
