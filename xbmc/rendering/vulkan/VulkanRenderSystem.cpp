/*
 *  Copyright (C) 2005-2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VulkanRenderSystem.h"

#include "ServiceBroker.h"
#include "URL.h"
#include "VulkanExtensions.h"
#include "VulkanMatrix.h"
#include "guilib/DirtyRegion.h"
#include "guilib/graphics/vulkan/VulkanGUITexture.h"
#include "platform/MessagePrinter.h"
#include "settings/AdvancedSettings.h"
#include "settings/SettingsComponent.h"
#include "utils/FileUtils.h"
#include "utils/MathUtils.h"
#include "utils/SystemInfo.h"
#include "utils/TimeUtils.h"
//#include "utils/VulkanUtils.h"
#include "utils/XTimeUtils.h"
#include "utils/log.h"
#include "windowing/GraphicContext.h"
#include "windowing/WinSystem.h"

#include <cassert>

using namespace std::chrono_literals;
using namespace KODI::GUILIB::GRAPHICS::VULKAN;

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

std::unique_ptr<CVulkanDeviceQueue> CreateVulkanDeviceQueue(CVulkanRenderSystem* vulkanRenderSystem,
                                                            DeviceQueueOptions options,
                                                            uint32_t heapMemoryLimit,
                                                            bool allowProtectedMemory,
                                                            bool isThreadSafe)
{
  assert(vulkanRenderSystem != nullptr);

  std::vector<const char*> requiredExtensions = vulkanRenderSystem->GetRequiredDeviceExtensions();
  std::vector<const char*> optionalExtensions = vulkanRenderSystem->GetOptionalDeviceExtensions();

  uint32_t gpuVendorId{0};
  uint32_t gpuDeviceId{0};

  auto deviceQueue = std::make_unique<CVulkanDeviceQueue>(vulkanRenderSystem);
  if (!deviceQueue->Initialize(options, gpuVendorId, gpuDeviceId, requiredExtensions,
                               optionalExtensions, heapMemoryLimit, allowProtectedMemory,
                               isThreadSafe))
  {
    CLog::Log(LOGERROR, "Vulkan: Failed to initialize device queue");
    return nullptr;
  }

  return deviceQueue;
}

CVulkanRenderSystem::CVulkanRenderSystem() : CRenderSystemBase()
{
}

bool CVulkanRenderSystem::InitRenderSystem()
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);

  m_deviceQueue = CreateVulkanDeviceQueue(this,
                                          DeviceQueueOption::GRAPHICS_QUEUE_FLAG |
                                              DeviceQueueOption::PRESENTATION_SUPPORT_QUEUE_FLAG,
                                          0, false, false);
  if (!m_deviceQueue)
  {
    return false;
  }

  m_bRenderCreated = true;

  CVulkanGUITexture::Register();

  return true;
}

bool CVulkanRenderSystem::ResetRenderSystem(int width, int height)
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  m_width = width;
  m_height = height;

  return true;
}

bool CVulkanRenderSystem::DestroyRenderSystem()
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);

  m_deviceQueue.reset();


  m_bRenderCreated = false;

  return true;
}

bool CVulkanRenderSystem::BeginRender()
{
  //fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return false;

  return true;
}

bool CVulkanRenderSystem::EndRender()
{
  //fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return false;

  return true;
}

void CVulkanRenderSystem::InvalidateColorBuffer()
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return;
}

bool CVulkanRenderSystem::ClearBuffers(KODI::UTILS::COLOR::Color color)
{
  //fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return false;

  return true;
}

bool CVulkanRenderSystem::IsExtSupported(const char* extension) const
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!extension)
    return false;

  for (const auto& [extName, specVersion] : m_vulkanExtensions)
  {
    if (extName == extension)
      return true;
  }

  return false;
}

void CVulkanRenderSystem::PresentRender(bool rendered, bool videoLayer)
{
  //fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);

  if (!m_bRenderCreated)
    return;
}

void CVulkanRenderSystem::CaptureStateBlock()
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return;
}

void CVulkanRenderSystem::ApplyStateBlock()
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return;
}

void CVulkanRenderSystem::SetCameraPosition(const CPoint& camera,
                                            int screenWidth,
                                            int screenHeight,
                                            float stereoFactor)
{
  //fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return;
}

void CVulkanRenderSystem::Project(float& x, float& y, float& z)
{
  //fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
}

void CVulkanRenderSystem::GetViewPort(CRect& viewPort)
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return;
}

void CVulkanRenderSystem::SetViewPort(const CRect& viewPort)
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return;
}

bool CVulkanRenderSystem::ScissorsCanEffectClipping()
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);

  return false;
}

CRect CVulkanRenderSystem::ClipRectToScissorRect(const CRect& rect)
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  //if (!m_pShader[m_method])
  return CRect();
}

void CVulkanRenderSystem::SetScissors(const CRect& rect)
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  if (!m_bRenderCreated)
    return;
}

void CVulkanRenderSystem::ResetScissors()
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
}

void CVulkanRenderSystem::SetDepthCulling(DepthCulling culling)
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
}

bool CVulkanRenderSystem::SupportsStereo(RenderStereoMode mode) const
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  return CRenderSystemBase::SupportsStereo(mode);
}

std::string CVulkanRenderSystem::GetShaderPath(const std::string& filename)
{
  fprintf(stderr, "---> %s\n", __PRETTY_FUNCTION__);
  return "Vulkan/";
}

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
