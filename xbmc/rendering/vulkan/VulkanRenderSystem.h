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
#include "rendering/vulkan/VulkanSurface.h"
#include "rendering/vulkan/VulkanSwapChain.h"

#include <deque>
#include <map>
#include <memory>
#include <vector>

#include <fmt/format.h>
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

//#include "system_vulkan.h"

enum class VulkanShaderMethod
{
  SM_DEFAULT,
  SM_TEXTURE,
  SM_TEXTURE_111R,
  SM_MULTI,
  SM_MULTI_RGBA_111R,
  SM_FONTS,
  SM_FONTS_SHADER_CLIP,
  SM_TEXTURE_NOBLEND,
  SM_MULTI_BLENDCOLOR,
  SM_MULTI_RGBA_111R_BLENDCOLOR,
  SM_MULTI_111R_111R_BLENDCOLOR,
  SM_TEXTURE_RGBA,
  SM_TEXTURE_RGBA_OES,
  SM_TEXTURE_RGBA_BLENDCOLOR,
  SM_TEXTURE_RGBA_BOB,
  SM_TEXTURE_RGBA_BOB_OES,
  SM_TEXTURE_NOALPHA,
  SM_MAX
};

template<>
struct fmt::formatter<VulkanShaderMethod> : fmt::formatter<std::string_view>
{
  template<typename FormatContext>
  constexpr auto format(const VulkanShaderMethod& shaderMethod, FormatContext& ctx)
  {
    const auto it = VulkanShaderMethodMap.find(shaderMethod);
    if (it == VulkanShaderMethodMap.cend())
      throw std::range_error("no string mapping found for shader method");

    return fmt::formatter<string_view>::format(it->second, ctx);
  }

private:
  static constexpr auto VulkanShaderMethodMap = make_map<VulkanShaderMethod, std::string_view>({
      {VulkanShaderMethod::SM_DEFAULT, "default"},
      {VulkanShaderMethod::SM_TEXTURE, "texture"},
      {VulkanShaderMethod::SM_TEXTURE_111R, "alpha texture with diffuse color"},
      {VulkanShaderMethod::SM_MULTI, "multi"},
      {VulkanShaderMethod::SM_MULTI_RGBA_111R, "multi with color/alpha texture"},
      {VulkanShaderMethod::SM_FONTS, "fonts"},
      {VulkanShaderMethod::SM_FONTS_SHADER_CLIP, "fonts with vertex shader based clipping"},
      {VulkanShaderMethod::SM_TEXTURE_NOBLEND, "texture no blending"},
      {VulkanShaderMethod::SM_MULTI_BLENDCOLOR, "multi blend colour"},
      {VulkanShaderMethod::SM_MULTI_RGBA_111R_BLENDCOLOR,
       "multi with color/alpha texture and blend color"},
      {VulkanShaderMethod::SM_MULTI_111R_111R_BLENDCOLOR,
       "multi with alpha/alpha texture and blend color"},
      {VulkanShaderMethod::SM_TEXTURE_RGBA, "texture rgba"},
      {VulkanShaderMethod::SM_TEXTURE_RGBA_OES, "texture rgba OES"},
      {VulkanShaderMethod::SM_TEXTURE_RGBA_BLENDCOLOR, "texture rgba blend colour"},
      {VulkanShaderMethod::SM_TEXTURE_RGBA_BOB, "texture rgba bob"},
      {VulkanShaderMethod::SM_TEXTURE_RGBA_BOB_OES, "texture rgba bob OES"},
      {VulkanShaderMethod::SM_TEXTURE_NOALPHA, "texture no alpha"},
  });

  static_assert(
      static_cast<size_t>(VulkanShaderMethod::SM_MAX) == VulkanShaderMethodMap.size(),
      "VulkanShaderMethodMap doesn't match the size of VulkanShaderMethod, did you forget to "
      "add/remove a mapping?");
};

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

class CVulkanDeviceQueue;
class CVulkanSurface;
class CVulkanInstance;
class CVulkanRenderSystem;

std::unique_ptr<CVulkanDeviceQueue> CreateVulkanDeviceQueue(CVulkanRenderSystem* vulkanRenderSystem,
                                                            DeviceQueueOptions options,
                                                            uint32_t heapMemoryLimit,
                                                            bool allowProtectedMemory = false,
                                                            bool isThreadSafe = false);

class CVulkanRenderSystem : public CRenderSystemBase
{
public:
  CVulkanRenderSystem();
  ~CVulkanRenderSystem() override = default;

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
  std::unique_ptr<CVulkanDeviceQueue> m_deviceQueue;
  std::unique_ptr<CVulkanSurface> m_surface;

  std::vector<std::pair<std::string, uint32_t>> m_vulkanExtensions;

  VkFormat m_SwapchainFormat = VK_FORMAT_UNDEFINED;

  /*


















  */
  struct Vertex
  {
    glm::vec2 position;
    glm::vec3 color;
  };

  // Define the vertex data
  const std::vector<Vertex> TEST___vertices = {
      {{0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, // Vertex 1: Red
      {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}, // Vertex 2: Green
      {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}} // Vertex 3: Blue
  };

  struct PerFrame
  {
    VkFence queue_submit_fence = VK_NULL_HANDLE;
    VkCommandPool primary_command_pool = VK_NULL_HANDLE;
    VkCommandBuffer primary_command_buffer = VK_NULL_HANDLE;
  };

  void TEST___Deinit();
  void TEST___init_swapchain();
  void TEST___init_pipeline();
  void TEST___update(float delta_time);
  void TEST___render_triangle(uint32_t swapchain_index);
  bool TEST___resize(const uint32_t, const uint32_t);
  void TEST___teardown_per_frame(PerFrame& per_frame);
  void TEST___init_per_frame(PerFrame& per_frame);
  void TEST___transition_image_layout(VkCommandBuffer cmd,
                                      VkImage image,
                                      VkImageLayout oldLayout,
                                      VkImageLayout newLayout,
                                      VkAccessFlags2 srcAccessMask,
                                      VkAccessFlags2 dstAccessMask,
                                      VkPipelineStageFlags2 srcStage,
                                      VkPipelineStageFlags2 dstStage);

  std::vector<VkImageView> TEST___swapchain_image_views;
  std::vector<PerFrame> TEST___per_frame;
  VkBuffer TEST___vertex_buffer = VK_NULL_HANDLE;
  VkDeviceMemory TEST___vertex_buffer_memory = VK_NULL_HANDLE;
  VkPipelineLayout TEST___pipeline_layout = VK_NULL_HANDLE;
  VkPipeline TEST___pipeline = VK_NULL_HANDLE;
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
