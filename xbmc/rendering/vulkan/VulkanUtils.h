/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

// GL Error checking macro
// this function is useful for tracking down GL errors, which otherwise
// just result in undefined behavior and can be difficult to track down.
//
// Just call it 'VerifyGLState()' after a sequence of GL calls
//
// if GL_DEBUGGING and HAS_GL are defined, the function checks
// for GL errors and prints the current state of the various matrices;
// if not it's just an empty inline stub, and thus won't affect performance
// and will be optimized out.

#include <string>
#include <vulkan/vulkan_core.h>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

class CVulkanInfo;

namespace UTILS
{

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
 * @brief Creates a Vulkan buffer.
 *
 * @param[in] vkInstance The Vulkan instance.
 * @param[in] device The Vulkan device.
 * @param[in] physicalDevice The physical Vulkan device.
 * @param[in] size The size of the buffer.
 * @param[in] usage The usage flags for the buffer.
 * @param[in] properties The memory property flags for the buffer.
 * @param[out] buffer The created Vulkan buffer.
 * @param[out] bufferMemory The allocated memory for the buffer.
 * @return True if the buffer was created successfully, false otherwise.
 */
bool vulkanCreateBuffer(VkInstance vkInstance,
                        VkDevice device,
                        VkPhysicalDevice physicalDevice,
                        VkDeviceSize size,
                        VkBufferUsageFlags usage,
                        VkMemoryPropertyFlags properties,
                        VkBuffer& buffer,
                        VkDeviceMemory& bufferMemory);

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

} // namespace UTILS
} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
