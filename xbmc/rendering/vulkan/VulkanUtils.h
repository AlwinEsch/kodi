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

#include <vulkan/vulkan_core.h>

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

class CVulkanInfo;

/*!
 * @brief Error callback function for Vulkan validation layers.
 *
 * See https://docs.vulkan.org/refpages/latest/refpages/source/PFN_vkDebugReportCallbackEXT.html
 */
VkBool32 vulkanErrorCallback(VkDebugReportFlagsEXT flags,
                             VkDebugReportObjectTypeEXT objectType,
                             uint64_t object,
                             size_t location,
                             int32_t messageCode,
                             const char* pLayerPrefix,
                             const char* pMessage,
                             void* pUserData);

void LogGraphicsInfo(const CVulkanInfo& vulkanInfo);

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
