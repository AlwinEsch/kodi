/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <vulkan/vulkan_core.h>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

class CVulkanInfo;
class CVulkanDeviceQueue;

namespace UTILS
{

constexpr uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;

/*!
 * @brief Error callback function for Vulkan validation layers.
 *
 * Used on @ref vkCreateDebugUtilsMessengerEXT to handle validation layer messages.
 *
 * @param[in] messageSeverity The severity of the message (e.g., warning, error).
 * @param[in] messageType The type of the message (e.g., general, validation, performance
 * @param[in] pCallbackData Pointer to a structure containing the callback data.
 * @param[out] pUserData Pointer to user-defined data passed to the callback.
 * @return VK_TRUE if the application should abort, VK_FALSE otherwise.
 *
 * See https://docs.vulkan.org/refpages/latest/refpages/source/vkCreateDebugUtilsMessengerEXT.html
 *     https://docs.vulkan.org/guide/latest/extensions/VK_EXT_debug_utils.html
 */
VkBool32 vulkanErrorCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                             void* pUserData);

/**
 * @brief Creates a Vulkan shader module from a SPIR-V file.
 *
 * @note This loads only a SPIR-V binary file and creates the @ref VkShaderModule about,
 * all other works must be done outside of this function.
 *
 * @param[in] device The Vulkan device.
 * @param[in] filename The path to the SPIR-V file.
 * @return The created shader module.
 */
VkShaderModule vulkanCreateShaderModule(VkDevice device, const std::string& filename);

/**
 * @brief Logs the Vulkan graphics information.
 *
 * @param[in] vulkanInfo The Vulkan information structure containing details about the Vulkan
 * instance, physical devices, and extensions.
 */
void LogGraphicsInfo(const CVulkanInfo& vulkanInfo);

/**
 * @brief Converts a Vulkan error code to a human-readable string.
 *
 * @param[in] errorCode The Vulkan error code.
 * @return A string representing the Vulkan error code.
 */
std::string ErrorString(VkResult errorCode);

/**
 * @brief Logs a Vulkan error.
 *
 * @param[in] result The Vulkan result code.
 * @param[in] functionName The name of the function where the error occurred.
 * @param[in] fileName The name of the file where the error occurred.
 * @param[in] lineNumber The line number where the error occurred.
 */
void LogVulkanError(VkResult result,
                    const std::string& functionName,
                    const std::string& fileName,
                    int lineNumber);

// clang-format off
/**
 * @brief Checks the result of a Vulkan function call and logs an error if it failed.
 *
 * @param[in] f The Vulkan function call to check.
 * @param[in] ... The return value in case of failure (if required).
 *
 * @warning Macro must be leafed in one line, otherwise `__LINE__` will be wrong and not accurate.
 */
#define VK_CHECK_RESULT(f, ...) do { VkResult res = (f); if (res != VK_SUCCESS) [[unlikely]] { const std::string s = #f; KODI::RENDERING::VULKAN::UTILS::LogVulkanError(res, s.substr(0, s.find('(')), __FILENAME__, __LINE__); return __VA_ARGS__; } } while (0)
// clang-format on

/**
 * @brief Gets the pipeline stage flags for a given image layout.
 *
 * @param[in] deviceQueue The Vulkan device queue.
 * @param[in] layout The image layout.
 * @return The pipeline stage flags.
 */
VkPipelineStageFlags GetPipelineStageFlags(const CVulkanDeviceQueue* deviceQueue,
                                           const VkImageLayout layout);

/**
 * @brief Gets the access mask for a given image layout.
 *
 * @param[in] layout The image layout.
 * @return The access mask.
 */
VkAccessFlags GetAccessMask(const VkImageLayout layout);

#if VK_HEADER_VERSION >= 135
/**
 * @brief Gets the pipeline stage flags for a given image layout (Vulkan 1.3+).
 *
 * @param[in] deviceQueue The Vulkan device queue.
 * @param[in] layout The image layout.
 * @return The pipeline stage flags.
 */
VkPipelineStageFlags2 GetPipelineStageFlags2(const CVulkanDeviceQueue* deviceQueue,
                                             const VkImageLayout layout);

/**
 * @brief Gets the access mask for a given image layout (Vulkan 1.3+).
 *
 * @param[in] layout The image layout.
 * @return The access mask.
 */
VkAccessFlags2 GetAccessMask2(const VkImageLayout layout);
#endif

void SetImageLayout(VkCommandBuffer cmdbuffer,
                    VkImage image,
                    VkImageLayout oldImageLayout,
                    VkImageLayout newImageLayout,
                    VkImageSubresourceRange subresourceRange,
                    VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                    VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

} // namespace UTILS
} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
