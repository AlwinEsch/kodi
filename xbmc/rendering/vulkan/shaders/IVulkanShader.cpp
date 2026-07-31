/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "IVulkanShader.h"

#include "rendering/vulkan/VulkanDeviceQueue.h"
#include "rendering/vulkan/utils/VulkanInitStructs.h"
#include "rendering/vulkan/utils/VulkanUtils.h"
#include "utils/log.h"

namespace KODI::RENDERING::VULKAN
{

IVulkanShader::IVulkanShader(const VulkanData* vkData, CVulkanDeviceQueue* deviceQueue)
  : m_vkData(vkData),
    m_deviceQueue(deviceQueue)
{
}

IVulkanShader::~IVulkanShader()
{
  Destroy();
}

bool IVulkanShader::Create()
{
  if (!CreatePipelineLayout())
  {
    CLog::Log(LOGERROR, "IVulkanShader::Create - failed to create pipeline layout");
    return false;
  }
  //if (!CreateVertexBuffer())
  //{
  //  CLog::Log(LOGERROR, "IVulkanShader::Create - failed to create vertex buffer");
  //  return false;
  //}
  if (!CreatePipeline())
  {
    CLog::Log(LOGERROR, "IVulkanShader::Create - failed to create pipeline");
    return false;
  }
  return true;
}

void IVulkanShader::Destroy()
{
  DestroyPipeline();
  //DestroyVertexBuffer();
  DestroyPipelineLayout();
}

void IVulkanShader::DestroyPipelineLayout()
{
  if (m_vkPipelineLayout != VK_NULL_HANDLE)
  {
    vkDestroyPipelineLayout(m_vkData->vkDevice, m_vkPipelineLayout, nullptr);
    m_vkPipelineLayout = VK_NULL_HANDLE;
  }
}

//void IVulkanShader::DestroyVertexBuffer()
//{
//  //for (auto& buffer : m_vertexBuffers)
//  //{
//  //  m_deviceQueue->DestroyBuffer(&buffer);
//  //  buffer = {};
//  //}
//  for (auto& buffer : m_indexBuffers)
//  {
//    m_deviceQueue->DestroyBuffer(&buffer);
//    buffer = {};
//  }
//}

void IVulkanShader::DestroyPipeline()
{
  if (m_vkPipeline != VK_NULL_HANDLE)
  {
    vkDestroyPipeline(m_vkData->vkDevice, m_vkPipeline, nullptr);
    m_vkPipeline = VK_NULL_HANDLE;
  }
}

VkPipelineShaderStageCreateInfo IVulkanShader::LoadShader(std::string fileName,
                                                          VkShaderStageFlagBits stage)
{
  VkPipelineShaderStageCreateInfo shaderStage{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = stage,
      .module = UTILS::vulkanCreateShaderModule(m_vkData->vkDevice, fileName),
      .pName = "main",
      .pSpecializationInfo = nullptr,
  };

  return shaderStage;
}

void IVulkanShader::UnloadShader(VkPipelineShaderStageCreateInfo shaderStage)
{
  VkShaderModule shaderModule = shaderStage.module;
  vkDestroyShaderModule(m_vkData->vkDevice, shaderModule, nullptr);
}

} // namespace KODI::RENDERING::VULKAN
