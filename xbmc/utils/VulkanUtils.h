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

#include "system_vulkan.h"

namespace KODI
{
namespace UTILS
{
namespace VULKAN
{

//void vulkanErrorCallback(GLenum source,
//                         GLenum type,
//                         GLuint id,
//                         GLenum severity,
//                         GLsizei length,
//                         const GLchar* message,
//                         const void* userParam);
//
//int vulkanFormatElementByteCount(GLenum format);
//
enum class ColorChannel
{
  A,
  R,
  G,
  B,
};

uint8_t GetChannelFromARGB(const ColorChannel colorChannel, const uint32_t argb);

} // namespace VULKAN
} // namespace UTILS
} // namespace KODI

void _VerifyVulkanState(const char* szfile, const char* szfunction, int lineno);
#if defined(VULKAN_DEBUGGING) && (defined(HAS_VULKAN))
#define VerifyVulkanState() _VerifyVulkanState(__FILE__, __FUNCTION__, __LINE__)
#else
#define VerifyVulkanState()
#endif

void LogGraphicsInfo();
