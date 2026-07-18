/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanShaderTexture.h"

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

CVulkanShaderTexture::CVulkanShaderTexture(VkDevice device,
                                     VkPipelineLayout pipelineLayout,
                                     VkRenderPass renderPass)
  : m_vkDevice(device),
    m_vkPipelineLayout(pipelineLayout),
    m_vkRenderPass(renderPass)
{
  assert(m_vkDevice != VK_NULL_HANDLE);
  assert(m_vkPipelineLayout != VK_NULL_HANDLE);
  assert(m_vkRenderPass != VK_NULL_HANDLE);
}

bool CVulkanShaderTexture::Create()
{

  return true;
}

void CVulkanShaderTexture::Destroy()
{
}

VkPipeline CVulkanShaderTexture::VulkanPipeline() const
{
  return m_vkPipeline;
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
