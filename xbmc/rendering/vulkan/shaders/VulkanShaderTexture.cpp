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

namespace
{
constexpr const char* kVertexShaderFile = "vulkan_shader_gr1_vert.spv";
constexpr const char* kFragmentShaderFile = "vulkan_shader_gr1_fraq_texture.spv";

} // namespace

CVulkanShaderTexture::CVulkanShaderTexture(const VulkanData* vkData,
                                           CVulkanDeviceQueue* deviceQueue)
  : IVulkanShader(vkData, deviceQueue){

}

bool CVulkanShaderTexture::Create()
{

  return true;
}

void CVulkanShaderTexture::Destroy()
{
  if (m_vkPipeline != VK_NULL_HANDLE)
  {
    // Destroy the Vulkan pipeline
    vkDestroyPipeline(m_vkData->vkDevice, m_vkPipeline, nullptr);
    m_vkPipeline = VK_NULL_HANDLE;
  }
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
