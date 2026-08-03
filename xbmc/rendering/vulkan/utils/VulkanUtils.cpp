/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanUtils.h"

#include "filesystem/File.h"
#include "filesystem/SpecialProtocol.h"
#include "rendering/vulkan/VulkanDeviceQueue.h"
#include "rendering/vulkan/VulkanInfo.h"
#include "rendering/vulkan/utils/VulkanInitStructs.h"
#include "utils/StringUtils.h"
#include "utils/log.h"

#include <stdexcept>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{
namespace UTILS
{

VkBool32 vulkanErrorCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                             void* pUserData)
{
  if (!pCallbackData)
    return VK_FALSE;

  int logLevel;
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    logLevel = LOGERROR;
  else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    logLevel = LOGWARNING;
  else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    logLevel = LOGDEBUG;
  else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    logLevel = LOGINFO;
  else // For VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT and unknown flags
    logLevel = LOGINFO;

  /*!
   * @remark pLayerPrefix and pMessage should not be nullptr, but we check them
   * just in case to avoid potential crashes.
   */
  CLog::Log(logLevel, "Vulkan: {}: {}",
            pCallbackData->pMessageIdName ? pCallbackData->pMessageIdName : "Unknown",
            pCallbackData->pMessage ? pCallbackData->pMessage : "No message text");
  return VK_FALSE;
}

VkShaderModule vulkanCreateShaderModule(VkDevice device, const std::string& filename)
{
  XFILE::CFileStream file;

  std::string path = CSpecialProtocol::TranslatePath(
      KODI::UTILS::StringUtils::Format("special://xbmc/system/shaders/Vulkan/{}", filename));
  if (!file.Open(path))
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to open file {} (Function: {})", path, __func__);
    return nullptr;
  }

  std::vector<uint8_t> spirv;
  if (XFILE::CFile().LoadFile(path, spirv) <= 0)
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to load file {} (Function: {})", path, __func__);
    return nullptr;
  }

  VkShaderModuleCreateInfo module_info{
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .codeSize = spirv.size(),
      .pCode = reinterpret_cast<uint32_t*>(spirv.data()),
  };

  VkShaderModule shader_module;
  VK_CHECK_RESULT(vkCreateShaderModule(device, &module_info, nullptr, &shader_module), nullptr);

  return shader_module;
}

void LogGraphicsInfo(const CVulkanInfo& vulkanInfo)
{
  // Log the Vulkan API information
  CLog::Log(LOGINFO, "Vulkan: Logging graphics information...");
  CLog::Log(LOGINFO, "        - Available API Version: {}.{}.{}",
            VK_VERSION_MAJOR(vulkanInfo.availableAPIVersion),
            VK_VERSION_MINOR(vulkanInfo.availableAPIVersion),
            VK_VERSION_PATCH(vulkanInfo.availableAPIVersion));
  CLog::Log(LOGINFO, "        - Used API Version: {0}.{1}.{2}",
            VK_VERSION_MAJOR(vulkanInfo.usedAPIVersion),
            VK_VERSION_MINOR(vulkanInfo.usedAPIVersion),
            VK_VERSION_PATCH(vulkanInfo.usedAPIVersion));
  if (!vulkanInfo.physicalDevices.empty())
  {
    const auto& usedDeviceInfo = vulkanInfo.physicalDevices[vulkanInfo.usedPhysicalDeviceIndex];
    CLog::Log(LOGINFO, "        - Used Physical Device: {0}", usedDeviceInfo.properties.deviceName);
  }
  else
  {
    CLog::Log(LOGINFO, "        - No physical devices found.");
  }
  CLog::Log(LOGINFO, "        - Debug Utils Enabled: {0}",
            vulkanInfo.debugUtilsEnabled ? "Yes" : "No");
  for (const auto& deviceInfo : vulkanInfo.instanceExtensions)
  {
    bool isEnabled = std::find_if(vulkanInfo.requiredInstanceExtensions.begin(),
                                  vulkanInfo.requiredInstanceExtensions.end(),
                                  [&deviceInfo](const char* enabledExtension)
                                  {
                                    return strcmp(enabledExtension, deviceInfo.extensionName) == 0;
                                  }) != vulkanInfo.requiredInstanceExtensions.end();
    CLog::Log(LOGINFO, "        - Instance Extension: {0} (Version {1}), Is Enabled: {2}",
              deviceInfo.extensionName, deviceInfo.specVersion, isEnabled ? "Yes" : "No");
  }
  CLog::Log(LOGINFO,
            "        - Available Physical Devices: {0}:", vulkanInfo.physicalDevices.size());
  for (const auto& deviceInfo : vulkanInfo.physicalDevices)
  {
    CLog::Log(LOGINFO, "          - Physical Device: {0}", deviceInfo.properties.deviceName);
    CLog::Log(LOGINFO, "              - Supported: {0}",
              deviceInfo.properties.apiVersion >= REQUIRED_VK_API_VERSION ? "Yes" : "No");
    CLog::Log(LOGINFO, "              - Vendor ID: {0:#X}", deviceInfo.properties.vendorID);
    CLog::Log(LOGINFO, "              - Device ID: {0:#X}", deviceInfo.properties.deviceID);
    CLog::Log(LOGINFO, "              - Driver Version: {0}.{1}.{2}",
              VK_VERSION_MAJOR(deviceInfo.properties.driverVersion),
              VK_VERSION_MINOR(deviceInfo.properties.driverVersion),
              VK_VERSION_PATCH(deviceInfo.properties.driverVersion));
    CLog::Log(LOGINFO, "              - API Version: {0}.{1}.{2}",
              VK_VERSION_MAJOR(deviceInfo.properties.apiVersion),
              VK_VERSION_MINOR(deviceInfo.properties.apiVersion),
              VK_VERSION_PATCH(deviceInfo.properties.apiVersion));
    CLog::Log(LOGINFO, "              - DRM Device ID: {0:#X}", deviceInfo.drmDeviceId);
    CLog::Log(LOGINFO, "              - Queue Families: {0}", deviceInfo.queueFamilies.size());
    CLog::Log(LOGINFO, "              - Sampler YCbCr Conversion: {0}",
              deviceInfo.featureSamplerYCBCRconversion ? "Supported" : "Not supported");
    CLog::Log(LOGINFO, "              - Protected Memory: {0}",
              deviceInfo.featureProtectedMemory ? "Supported" : "Not supported");
  }
}

std::string ErrorString(VkResult errorCode)
{
  switch (errorCode)
  {
#define STR(r) \
  case VK_##r: \
    return #r
    STR(NOT_READY);
    STR(TIMEOUT);
    STR(EVENT_SET);
    STR(EVENT_RESET);
    STR(INCOMPLETE);
    STR(ERROR_OUT_OF_HOST_MEMORY);
    STR(ERROR_OUT_OF_DEVICE_MEMORY);
    STR(ERROR_INITIALIZATION_FAILED);
    STR(ERROR_DEVICE_LOST);
    STR(ERROR_MEMORY_MAP_FAILED);
    STR(ERROR_LAYER_NOT_PRESENT);
    STR(ERROR_EXTENSION_NOT_PRESENT);
    STR(ERROR_FEATURE_NOT_PRESENT);
    STR(ERROR_INCOMPATIBLE_DRIVER);
    STR(ERROR_TOO_MANY_OBJECTS);
    STR(ERROR_FORMAT_NOT_SUPPORTED);
    STR(ERROR_FRAGMENTED_POOL);
    STR(ERROR_UNKNOWN);
    STR(ERROR_VALIDATION_FAILED);
    STR(ERROR_OUT_OF_POOL_MEMORY);
    STR(ERROR_INVALID_EXTERNAL_HANDLE);
    STR(ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
    STR(ERROR_FRAGMENTATION);
    STR(PIPELINE_COMPILE_REQUIRED);
    STR(ERROR_NOT_PERMITTED);
    STR(ERROR_SURFACE_LOST_KHR);
    STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
    STR(SUBOPTIMAL_KHR);
    STR(ERROR_OUT_OF_DATE_KHR);
    STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
    STR(ERROR_INVALID_SHADER_NV);
    STR(ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR);
    STR(ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR);
    STR(ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR);
    STR(ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR);
    STR(ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR);
    STR(ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR);
    STR(ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
    STR(ERROR_PRESENT_TIMING_QUEUE_FULL_EXT);
    STR(ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
    STR(THREAD_IDLE_KHR);
    STR(THREAD_DONE_KHR);
    STR(OPERATION_DEFERRED_KHR);
    STR(OPERATION_NOT_DEFERRED_KHR);
    STR(ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR);
    STR(ERROR_COMPRESSION_EXHAUSTED_EXT);
    STR(INCOMPATIBLE_SHADER_BINARY_EXT);
    STR(PIPELINE_BINARY_MISSING_KHR);
    STR(ERROR_NOT_ENOUGH_SPACE_KHR);
#undef STR
    default:
      return "UNKNOWN_ERROR";
  }
}

void LogVulkanError(VkResult result,
                    const std::string& functionName,
                    const std::string& fileName,
                    int lineNumber)
{
  if (result == VK_SUCCESS)
    return;
  CLog::Log(LOGERROR, "Vulkan: Error in function \"{}\" at {}:{} - {}", functionName, fileName,
            lineNumber, ErrorString(result));
}

VkPipelineStageFlags GetPipelineStageFlags(const CVulkanDeviceQueue* deviceQueue,
                                           const VkImageLayout layout)
{
  switch (layout)
  {
    case VK_IMAGE_LAYOUT_UNDEFINED:
      return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    case VK_IMAGE_LAYOUT_GENERAL:
      return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    case VK_IMAGE_LAYOUT_PREINITIALIZED:
      return VK_PIPELINE_STAGE_HOST_BIT;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
      return VK_PIPELINE_STAGE_TRANSFER_BIT;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
      return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
    {
      VkPipelineStageFlags flags =
          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
      if (deviceQueue->EnabledDeviceFeatures().tessellationShader)
      {
        flags |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
                 VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
      }
      if (deviceQueue->EnabledDeviceFeatures().geometryShader)
      {
        flags |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
      }
      return flags;
    }
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
      return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    default:
      CLog::Log(LOGERROR, "Unknown layout: {}", layout);
      return 0;
  }
}

VkAccessFlags GetAccessMask(const VkImageLayout layout)
{
  switch (layout)
  {
    case VK_IMAGE_LAYOUT_UNDEFINED:
      return 0;
    case VK_IMAGE_LAYOUT_GENERAL:
      CLog::Log(LOGWARNING, "VK_IMAGE_LAYOUT_GENERAL is used.");
      return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
             VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT |
             VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_HOST_READ_BIT |
             VK_ACCESS_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_MEMORY_READ_BIT |
             VK_ACCESS_MEMORY_WRITE_BIT;
    case VK_IMAGE_LAYOUT_PREINITIALIZED:
      return VK_ACCESS_HOST_WRITE_BIT;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
      return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
      return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
      return VK_ACCESS_TRANSFER_READ_BIT;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
      return VK_ACCESS_TRANSFER_WRITE_BIT;
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
      return 0;
    default:
      CLog::Log(LOGERROR, "Unknown layout: {}", layout);
      return 0;
  }
}

#if VK_HEADER_VERSION >= 135
VkPipelineStageFlags2 GetPipelineStageFlags2(const CVulkanDeviceQueue* deviceQueue,
                                             const VkImageLayout layout)
{
  switch (layout)
  {
    case VK_IMAGE_LAYOUT_UNDEFINED:
      return VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
    case VK_IMAGE_LAYOUT_GENERAL:
      return VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    case VK_IMAGE_LAYOUT_PREINITIALIZED:
      return VK_PIPELINE_STAGE_2_HOST_BIT;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
      return VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
      return VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
    {
      VkPipelineStageFlags2 flags =
          VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
      if (deviceQueue->EnabledDeviceFeatures().tessellationShader)
      {
        flags |= VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT |
                 VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT;
      }
      if (deviceQueue->EnabledDeviceFeatures().geometryShader)
      {
        flags |= VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT;
      }
      return flags;
    }
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
      return VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
    default:
      CLog::Log(LOGERROR, "Unknown layout: {}", layout);
      return 0;
  }
}

VkAccessFlags2 GetAccessMask2(const VkImageLayout layout)
{
  switch (layout)
  {
    case VK_IMAGE_LAYOUT_UNDEFINED:
      return 0;
    case VK_IMAGE_LAYOUT_GENERAL:
      CLog::Log(LOGWARNING, "VK_IMAGE_LAYOUT_GENERAL is used.");
      return VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT |
             VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT |
             VK_ACCESS_2_TRANSFER_READ_BIT | VK_ACCESS_2_SHADER_READ_BIT |
             VK_ACCESS_2_HOST_WRITE_BIT | VK_ACCESS_2_HOST_READ_BIT |
             VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT |
             VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_MEMORY_READ_BIT |
             VK_ACCESS_2_MEMORY_WRITE_BIT;
    case VK_IMAGE_LAYOUT_PREINITIALIZED:
      return VK_ACCESS_2_HOST_WRITE_BIT;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
      return VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
      return VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
      return VK_ACCESS_2_TRANSFER_READ_BIT;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
      return VK_ACCESS_2_TRANSFER_WRITE_BIT;
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
      return 0;
    default:
      CLog::Log(LOGERROR, "Unknown layout: {}", layout);
      return 0;
  }
}
#endif

void SetImageLayout(VkCommandBuffer cmdbuffer,
                    VkImage image,
                    VkImageLayout oldImageLayout,
                    VkImageLayout newImageLayout,
                    VkImageSubresourceRange subresourceRange,
                    VkPipelineStageFlags srcStageMask,
                    VkPipelineStageFlags dstStageMask)
{
  // Create an image barrier object
  VkImageMemoryBarrier imageMemoryBarrier = vkImageMemoryBarrier();
  imageMemoryBarrier.oldLayout = oldImageLayout;
  imageMemoryBarrier.newLayout = newImageLayout;
  imageMemoryBarrier.image = image;
  imageMemoryBarrier.subresourceRange = subresourceRange;

  // Source layouts (old)
  // Source access mask controls actions that have to be finished on the old layout
  // before it will be transitioned to the new layout
  switch (oldImageLayout)
  {
    case VK_IMAGE_LAYOUT_UNDEFINED:
      // Image layout is undefined (or does not matter)
      // Only valid as initial layout
      // No flags required, listed only for completeness
      imageMemoryBarrier.srcAccessMask = 0;
      break;

    case VK_IMAGE_LAYOUT_PREINITIALIZED:
      // Image is preinitialized
      // Only valid as initial layout for linear images, preserves memory contents
      // Make sure host writes have been finished
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
      // Image is a color attachment
      // Make sure any writes to the color buffer have been finished
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
      // Image is a depth/stencil attachment
      // Make sure any writes to the depth/stencil buffer have been finished
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
      // Image is a transfer source
      // Make sure any reads from the image have been finished
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
      break;

    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
      // Image is a transfer destination
      // Make sure any writes to the image have been finished
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
      // Image is read by a shader
      // Make sure any shader reads from the image have been finished
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
      break;
    default:
      // Other source layouts aren't handled (yet)
      break;
  }

  // Target layouts (new)
  // Destination access mask controls the dependency for the new image layout
  switch (newImageLayout)
  {
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
      // Image will be used as a transfer destination
      // Make sure any writes to the image have been finished
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
      // Image will be used as a transfer source
      // Make sure any reads from the image have been finished
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
      break;

    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
      // Image will be used as a color attachment
      // Make sure any writes to the color buffer have been finished
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
      // Image layout will be used as a depth/stencil attachment
      // Make sure any writes to depth/stencil buffer have been finished
      imageMemoryBarrier.dstAccessMask =
          imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
      // Image will be read in a shader (sampler, input attachment)
      // Make sure any writes to the image have been finished
      if (imageMemoryBarrier.srcAccessMask == 0)
      {
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      }
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
      break;
    default:
      // Other source layouts aren't handled (yet)
      break;
  }

  // Put barrier inside setup command buffer
  vkCmdPipelineBarrier(cmdbuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1,
                       &imageMemoryBarrier);
}

} // namespace UTILS
} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
