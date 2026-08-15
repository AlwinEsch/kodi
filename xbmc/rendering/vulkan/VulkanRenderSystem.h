/*
 *  Copyright (C) 2005-2024 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "VulkanDeviceQueue.h"
#include "rendering/RenderSystem.h"
#include "rendering/vulkan/VulkanCommandBuffer.h"
#include "rendering/vulkan/VulkanData.h"
#include "rendering/vulkan/VulkanScopedWrite.h"
#include "rendering/vulkan/VulkanSurface.h"
#include "rendering/vulkan/VulkanSwapChain.h"
#include "rendering/vulkan/shaders/VulkanShaderControl.h"

#include <deque>
#include <map>
#include <memory>
#include <optional>
#include <vector>

#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vulkan/vulkan_core.h>

namespace KODI::RENDERING::VULKAN
{

class CVulkanShaderTexture;
struct UniformBuffer;
class CVulkanCommandBuffer;
class CVulkanDeviceQueue;
class CVulkanSurface;
class CVulkanInstance;
class CVulkanRenderSystem;
class CVulkanRenderPass;
class CVulkanScopedWrite;
class CVulkanCommandBufferScoped;
class CVulkanShaderControl;
class CVulkanCommandPool;
class CVulkanFramebuffer;
class CVulkanDynamicBuffers;

class CVulkanCommandBufferScoped;

class CVulkanRenderSystem : public CRenderSystemBase
{
public:
  CVulkanRenderSystem();
  ~CVulkanRenderSystem() override;

  bool InitRenderSystem() override;
  bool DestroyRenderSystem() override;
  bool ResetRenderSystem(int width, int height) override;

  bool BeginRender() override;
  bool EndRender() override;
  void PresentRender(bool rendered, bool videoLayer) override;
  void InvalidateColorBuffer() override;
  bool ClearBuffers(KODI::UTILS::COLOR::Color color) override;
  bool IsExtSupported(const char* extension) const override;
  void SetViewPort(const CRect& viewPort) override;
  void GetViewPort(CRect& viewPort) override;

  /**
   * @note @ref CRenderSystemBase::ScissorsCanEffectClipping and @ref CRenderSystemBase::ClipRectToScissorRect
   * are only used by @ref CGUIFontTTF, which done directly on @ref CVulkanGUIFontTTF.
   */
  void SetScissors(const CRect& rect) override;
  void ResetScissors() override;

  void SetDepthCulling(DepthCulling culling) override;

  void CaptureStateBlock() override;
  void ApplyStateBlock() override;

  void SetCameraPosition(const CPoint& camera,
                         int screenWidth,
                         int screenHeight,
                         float stereoFactor = 0.0f) override;

  bool SupportsStereo(RenderStereoMode mode) const override;

  void Project(float& x, float& y, float& z) override;

  std::string GetShaderPath(const std::string& filename) override;

  virtual CVulkanInstance* GetVulkanInstance() = 0;
  virtual VkSurfaceKHR GetVulkanSurface() = 0;
  virtual std::vector<const char*> GetRequiredDeviceExtensions() = 0;
  virtual std::vector<const char*> GetOptionalDeviceExtensions() = 0;
  virtual bool GetPhysicalDevicePresentationSupport(
      VkPhysicalDevice device,
      const std::vector<VkQueueFamilyProperties>& queueFamilyProperties,
      uint32_t queueFamilyIndex)
  {
    return false;
  }

  void InitialiseShaders();
  void ReleaseShaders();
  void EnableShader(ShaderId method);
  void DisableShader();

  CVulkanShaderControl* ShaderControl() { return m_shaderControl.get(); }
  CVulkanDeviceQueue* DeviceQueue() { return m_deviceQueue.get(); }
  CVulkanSurface* Surface() { return m_surface.get(); }
  CVulkanRenderPass* RenderPass() { return m_renderPass.get(); }
  CVulkanDynamicBuffers* DynamicBuffers() { return m_dynamicBuffers.get(); }

  const VkPhysicalDeviceFeatures& EnabledDeviceFeatures() const
  {
    return m_deviceQueue->EnabledDeviceFeatures();
  }

  const VkPhysicalDeviceLimits& DeviceLimits() const { return m_deviceQueue->DeviceLimits(); }

  /**
   * @brief Gets the Vulkan objects used for rendering.
   */
  //@{
  VkSurfaceKHR vkSurface() const { return m_vkData.vkSurface; }
  VkSurfaceFormatKHR vkSurfaceFormat() const { return m_vkData.vkSurfaceFormat; }
  VkInstance vkInstance() const { return m_vkData.vkInstance; }
  VkDevice vkDevice() const { return m_vkData.vkDevice; }
  VkPhysicalDevice vkPhysicalDevice() const { return m_vkData.vkPhysicalDevice; }
  //VkPipeline vkPipeline() const { return m_vkPipeline; }
  VkSwapchainKHR vkSwapchain() const { return m_vkData.vkSwapchain; }
  VkRenderPass vkRenderPass() const { return m_vkData.vkRenderPass; }
  VkFormat vkSwapchainFormat() const { return m_vkData.vkSwapchainFormat; }
  VkCommandPool vkCommandPool() const { return m_vkData.vkCommandPool; }
  VkQueue vkQueue() const { return m_deviceQueue->vkQueue(); }
  VkCommandBuffer vkCurrentCommandBuffer() const { return m_currentVkCommandBuffer; }
  uint32_t vkCurrentRenderImageIndex() const { return m_indexBuffer; }
  //@}

  const VulkanData* vkData() const { return &m_vkData; }

protected:
  /**
   * @brief Gets the width and height of the render system.
   *
   * Can be becomes set by calling @ref ResetRenderSystem() and by child classes
   * during initialization that implement the @ref CVulkanRenderSystem as a parent class.
   */
  /**@{*/
  uint32_t m_width{0};
  uint32_t m_height{0};
  /**@}*/

private:
  bool CreatePipeline();
  void DestroyPipeline();

  bool BeginRender2();
  bool EndRender2();

  std::unique_ptr<CVulkanShaderControl> m_shaderControl;
  std::unique_ptr<CVulkanDeviceQueue> m_deviceQueue;
  std::unique_ptr<CVulkanSurface> m_surface;
  std::unique_ptr<CVulkanRenderPass> m_renderPass;
  std::unique_ptr<CVulkanDynamicBuffers> m_dynamicBuffers;
  std::vector<std::unique_ptr<CVulkanFramebuffer>> m_framebuffers;

  //glm::ivec4 m_viewPort{0, 0, 0, 0};
  VkViewport m_vkViewport{};
  VkRect2D m_vkScissor{};

  glm::vec4 m_clearColor;
  bool m_stereoEnabled{false};
  bool m_rendered{true};

  VulkanData m_vkData;

  VkCommandBuffer m_currentVkCommandBuffer{VK_NULL_HANDLE};
  std::optional<CVulkanScopedWrite> m_scopedWrite;
  //std::optional<CVulkanCommandBufferScoped> m_recorder;

  uint32_t m_indexBuffer{0};
};

} // namespace KODI::RENDERING::VULKAN
