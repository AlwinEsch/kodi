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
#include <vulkan/vulkan_core.h>

namespace KODI::RENDERING::VULKAN
{

class CVulkanCommandBuffer;
class CVulkanDeviceQueue;
class CVulkanSurface;
class CVulkanInstance;
class CVulkanRenderSystem;
class CVulkanRenderPass;
class CVulkanScopedWrite;
class CVulkanShaderControl;
class CVulkanCommandPool;
class CVulkanFramebuffer;
class CVulkanShaderTest;

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

  bool ScissorsCanEffectClipping() override;
  CRect ClipRectToScissorRect(const CRect& rect) override;
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
  VkInstance vkInstance() const { return m_vkInstance; }
  VkDevice vkDevice() const { return m_vkData.vkDevice; }
  VkPhysicalDevice vkPhysicalDevice() const { return m_vkPhysicalDevice; }
  VkPipeline vkPipeline() const { return m_vkPipeline; }
  VkSwapchainKHR vkSwapchain() const { return m_vkSwapchain; }
  VkRenderPass vkRenderPass() const { return m_vkData.vkRenderPass; }
  VkFormat vkSwapchainFormat() const { return m_vkSwapchainFormat; }
  VkCommandPool vkCommandPool() const { return m_vkCommandPool; }
  VkQueue vkQueue() const { return m_deviceQueue->vkQueue(); }
  VkCommandBuffer vkCurrentCommandBuffer() const { return m_currentVkCommandBuffer; }
  //@}

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

  std::unique_ptr<CVulkanShaderControl> m_shaderControl;
  std::unique_ptr<CVulkanDeviceQueue> m_deviceQueue;
  std::unique_ptr<CVulkanSurface> m_surface;
  std::unique_ptr<CVulkanRenderPass> m_renderPass;
  std::vector<std::unique_ptr<CVulkanFramebuffer>> m_framebuffers;

  VulkanData m_vkData;
  /**
   * @brief Values in group of Vulkan objects that are used for rendering and are initialized in
   * @ref InitRenderSystem() and taken from other Vulkan objects.
   *
   * Done to avoid having to pass them around in function calls and to have them available for the
   * entire lifetime of the render system. They are no more changed after initialization, so they
   * can be used as a reference to the Vulkan objects they are taken from.
   *
   * @warning About destruction of this values, some are not instantiated in this class, but are
   * taken from other Vulkan objects, so they are destroyed in the destructor of the other Vulkan objects.
   */
  /**@{*/
  //VkSurfaceKHR m_vkSurface{VK_NULL_HANDLE}; // Created & destroyed outside
  /*VkSurfaceFormatKHR m_vkSurfaceFormat{};*/ // Created & destroyed outside
  VkInstance m_vkInstance{VK_NULL_HANDLE}; // Created & destroyed outside
  //VkDevice m_vkDevice{VK_NULL_HANDLE}; // Created & destroyed outside
  VkPhysicalDevice m_vkPhysicalDevice{VK_NULL_HANDLE}; // Created & destroyed outside
  VkPipeline m_vkPipeline{VK_NULL_HANDLE}; // Created & destroyed here
  VkSwapchainKHR m_vkSwapchain{VK_NULL_HANDLE}; // Created & destroyed outside
  //VkRenderPass m_vkRenderPass{VK_NULL_HANDLE}; // Created & destroyed here
  VkCommandPool m_vkCommandPool{VK_NULL_HANDLE}; // Created & destroyed outside
  VkFormat m_vkSwapchainFormat = VK_FORMAT_UNDEFINED;
  /**@}*/

  VkCommandBuffer m_currentVkCommandBuffer{VK_NULL_HANDLE};
  std::optional<CVulkanScopedWrite> m_scopedWrite;

  // TEST PURPOSE ONLY
  CVulkanShaderTest* m_testShader;
};

} // namespace KODI::RENDERING::VULKAN
