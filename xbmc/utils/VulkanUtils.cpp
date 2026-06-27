/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanUtils.h"

#include "ServiceBroker.h"
#include "log.h"
#include "rendering/RenderSystem.h"
#include "rendering/vulkan/VulkanExtensions.h"
#include "rendering/vulkan/VulkanMatrix.h"
#include "settings/AdvancedSettings.h"
#include "settings/SettingsComponent.h"
#include "utils/Map.h"
#include "utils/StringUtils.h"

#include <stdexcept>
#include <string_view>
#include <utility>

//namespace
//{
//
//// clang-format off
//#define X(VAL) std::make_pair(VAL, #VAL)
//constexpr auto vulkanErrors = make_map<GLenum, std::string_view>({
//  // please keep attributes in accordance to:
//  // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetError.xhtml
//  X(GL_NO_ERROR),
//  X(GL_INVALID_ENUM),
//  X(GL_INVALID_VALUE),
//  X(GL_INVALID_OPERATION),
//  X(GL_INVALID_FRAMEBUFFER_OPERATION),
//  X(GL_OUT_OF_MEMORY),
//});
//
//#ifdef TARGET_LINUX
//constexpr auto vulkanErrorSource = make_map<GLenum, std::string_view>({
//    X(GL_DEBUG_SOURCE_API_KHR),
//    X(GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR),
//    X(GL_DEBUG_SOURCE_SHADER_COMPILER_KHR),
//    X(GL_DEBUG_SOURCE_THIRD_PARTY_KHR),
//    X(GL_DEBUG_SOURCE_APPLICATION_KHR),
//    X(GL_DEBUG_SOURCE_OTHER_KHR),
//});
//
//constexpr auto vulkanErrorType = make_map<GLenum, std::string_view>({
//    X(GL_DEBUG_TYPE_ERROR_KHR),
//    X(GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR),
//    X(GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR),
//    X(GL_DEBUG_TYPE_PORTABILITY_KHR),
//    X(GL_DEBUG_TYPE_PERFORMANCE_KHR),
//    X(GL_DEBUG_TYPE_OTHER_KHR),
//    X(GL_DEBUG_TYPE_MARKER_KHR),
//});
//
//constexpr auto vulkanErrorSeverity = make_map<GLenum, std::string_view>({
//    X(GL_DEBUG_SEVERITY_HIGH_KHR),
//    X(GL_DEBUG_SEVERITY_MEDIUM_KHR),
//    X(GL_DEBUG_SEVERITY_LOW_KHR),
//    X(GL_DEBUG_SEVERITY_NOTIFICATION_KHR),
//});
//#endif
//#undef X
//// clang-format on
//
//} // namespace

//void KODI::UTILS::VULKAN::vulkanErrorCallback(GLenum source,
//                                              GLenum type,
//                                              GLuint id,
//                                              GLenum severity,
//                                              GLsizei length,
//                                              const GLchar* message,
//                                              const void* userParam)
//{
//#ifdef TARGET_LINUX
//  const std::string_view sourceStr = vulkanErrorSource.get(source).value_or("");
//  const std::string_view typeStr = vulkanErrorType.get(type).value_or("");
//  const std::string_view severityStr = vulkanErrorSeverity.get(severity).value_or("");
//
//  CLog::Log(LOGDEBUG, "Vulkan Debugging:\nSource: {}\nType: {}\nSeverity: {}\nID: {}\nMessage: {}",
//            sourceStr, typeStr, severityStr, id, message);
//#else
//  CLog::Log(LOGDEBUG, "Vulkan Debugging:\nID: {}\nMessage: {}", id, message);
//#endif
//}
//
//static void PrintMatrix(const GLfloat* matrix, const std::string& matrixName)
//{
//  CLog::Log(LOGDEBUG,
//            "{}:\n{:> 10.3f} {:> 10.3f} {:> 10.3f} {:> 10.3f}\n{:> 10.3f} {:> 10.3f} {:> 10.3f} "
//            "{:> 10.3f}\n{:> 10.3f} {:> 10.3f} {:> 10.3f} {:> 10.3f}\n{:> 10.3f} {:> 10.3f} {:> "
//            "10.3f} {:> 10.3f}",
//            matrixName, matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5], matrix[6],
//            matrix[7], matrix[8], matrix[9], matrix[10], matrix[11], matrix[12], matrix[13],
//            matrix[14], matrix[15]);
//}

void _VerifyVulkanState(const char* szfile, const char* szfunction, int lineno)
{
//  GLenum err = glGetError();
//  if (err == GL_NO_ERROR)
//  {
//    return;
//  }
//
//  auto error = vulkanErrors.find(err);
//  if (error != vulkanErrors.end())
//  {
//    CLog::Log(LOGERROR, "Vulkan ERROR: {}", error->second);
//  }
//
//  if (szfile && szfunction)
//  {
//    CLog::Log(LOGERROR, "In file: {} function: {} line: {}", szfile, szfunction, lineno);
//  }
//
//  GLboolean scissors;
//  glGetBooleanv(GL_SCISSOR_TEST, &scissors);
//  CLog::Log(LOGDEBUG, "Scissor test enabled: {}", scissors == GL_TRUE ? "True" : "False");
//
//  GLfloat matrix[16];
//  glGetFloatv(GL_SCISSOR_BOX, matrix);
//  CLog::Log(LOGDEBUG, "Scissor box: {}, {}, {}, {}", matrix[0], matrix[1], matrix[2], matrix[3]);
//
//  glGetFloatv(GL_VIEWPORT, matrix);
//  CLog::Log(LOGDEBUG, "Viewport: {}, {}, {}, {}", matrix[0], matrix[1], matrix[2], matrix[3]);
//
//  PrintMatrix(vulkanMatrixProject.Get(), "Projection Matrix");
//  PrintMatrix(vulkanMatrixModview.Get(), "Modelview Matrix");
}

void LogGraphicsInfo()
{
//  const char* s;
//
//  s = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
//  if (s)
//    CLog::Log(LOGINFO, "GL_VENDOR = {}", s);
//  else
//    CLog::Log(LOGINFO, "GL_VENDOR = NULL");
//
//  s = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
//  if (s)
//    CLog::Log(LOGINFO, "GL_RENDERER = {}", s);
//  else
//    CLog::Log(LOGINFO, "GL_RENDERER = NULL");
//
//  s = reinterpret_cast<const char*>(glGetString(GL_VERSION));
//  if (s)
//    CLog::Log(LOGINFO, "GL_VERSION = {}", s);
//  else
//    CLog::Log(LOGINFO, "GL_VERSION = NULL");
//
//  s = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
//  if (s)
//    CLog::Log(LOGINFO, "GL_SHADING_LANGUAGE_VERSION = {}", s);
//  else
//    CLog::Log(LOGINFO, "GL_SHADING_LANGUAGE_VERSION = NULL");
//
//  //GL_NVX_gpu_memory_info extension
//#define GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX 0x9047
//#define GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX 0x9048
//#define GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX 0x9049
//#define GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX 0x904A
//#define GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX 0x904B
//
//  if (CVulkanExtensions::IsExtensionSupported(CVulkanExtensions::NVX_gpu_memory_info))
//  {
//    GLint mem = 0;
//
//    glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &mem);
//    CLog::Log(LOGINFO, "GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX = {}", mem);
//
//    //this seems to be the amount of ram on the videocard
//    glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &mem);
//    CLog::Log(LOGINFO, "GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX = {}", mem);
//  }
//
//  std::string extensions = "";
//  const char* extension = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
//  if (extension)
//    extensions += extension;
//
//  if (!extensions.empty())
//    CLog::Log(LOGINFO, "GL_EXTENSIONS = {}", extensions);
//  else
//    CLog::Log(LOGINFO, "GL_EXTENSIONS = NULL");
}

//int KODI::UTILS::VULKAN::vulkanFormatElementByteCount(GLenum format)
//{
//  switch (format)
//  {
//    case GL_RED:
//      return 1;
//    case GL_RG:
//      return 2;
//    case GL_RGBA:
//      return 4;
//    case GL_RGB:
//      return 3;
//    case GL_LUMINANCE_ALPHA:
//      return 2;
//    case GL_LUMINANCE:
//    case GL_ALPHA:
//      return 1;
//    case GL_BGRA_EXT:
//      return 4;
//    default:
//      CLog::Log(LOGERROR, "vulkanFormatElementByteCount - Unknown format {}", format);
//      return 1;
//  }
//}

uint8_t KODI::UTILS::VULKAN::GetChannelFromARGB(
    const KODI::UTILS::VULKAN::ColorChannel colorChannel, const uint32_t argb)
{
  switch (colorChannel)
  {
    case KODI::UTILS::VULKAN::ColorChannel::A:
      return (argb >> 24) & 0xFF;
    case KODI::UTILS::VULKAN::ColorChannel::R:
      return (argb >> 16) & 0xFF;
    case KODI::UTILS::VULKAN::ColorChannel::G:
      return (argb >> 8) & 0xFF;
    case KODI::UTILS::VULKAN::ColorChannel::B:
      return (argb >> 0) & 0xFF;
    default:
      throw std::runtime_error("KODI::UTILS::VULKAN::GetChannelFromARGB: ColorChannel not handled");
  };
}
