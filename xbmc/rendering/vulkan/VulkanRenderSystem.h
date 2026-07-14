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

enum class ShaderMethodVulkan
{
  SM_DEFAULT = 0,
  SM_TEXTURE,
  SM_TEXTURE_LIM,
  SM_MULTI,
  SM_FONTS,
  SM_FONTS_SHADER_CLIP,
  SM_TEXTURE_NOBLEND,
  SM_TEXTURE_NOALPHA,
  SM_MULTI_BLENDCOLOR,
  SM_MAX
};

template<>
struct fmt::formatter<ShaderMethodVulkan> : fmt::formatter<std::string_view>
{
  template<typename FormatContext>
  constexpr auto format(const ShaderMethodVulkan& shaderMethod, FormatContext& ctx)
  {
    const auto it = ShaderMethodVulkanMap.find(shaderMethod);
    if (it == ShaderMethodVulkanMap.cend())
      throw std::range_error("no string mapping found for shader method");

    return fmt::formatter<string_view>::format(it->second, ctx);
  }

private:
  static constexpr auto ShaderMethodVulkanMap = make_map<ShaderMethodVulkan, std::string_view>({
      {ShaderMethodVulkan::SM_DEFAULT, "default"},
      {ShaderMethodVulkan::SM_TEXTURE, "texture"},
      {ShaderMethodVulkan::SM_TEXTURE_LIM, "texture limited"},
      {ShaderMethodVulkan::SM_MULTI, "multi"},
      {ShaderMethodVulkan::SM_FONTS, "fonts"},
      {ShaderMethodVulkan::SM_FONTS_SHADER_CLIP, "fonts with vertex shader based clipping"},
      {ShaderMethodVulkan::SM_TEXTURE_NOBLEND, "texture no blending"},
      {ShaderMethodVulkan::SM_TEXTURE_NOALPHA, "texture no alpha"},
      {ShaderMethodVulkan::SM_MULTI_BLENDCOLOR, "multi blend colour"},
  });

  static_assert(
      static_cast<size_t>(ShaderMethodVulkan::SM_MAX) == ShaderMethodVulkanMap.size(),
      "ShaderMethodVulkanMap doesn't match the size of ShaderMethodVulkan, did you forget to "
      "add/remove a mapping?");
};

namespace KODI
{
namespace RENDERING
{
namespace VULKAN
{

class CVulkanCommandBuffer;
class CVulkanDeviceQueue;
class CVulkanSurface;
class CVulkanInstance;
class CVulkanRenderSystem;
class CVulkanRenderPass;
class CVulkanShaderControl;
class CVulkanCommandPool;
class CVulkanFramebuffer;

std::unique_ptr<CVulkanDeviceQueue> CreateVulkanDeviceQueue(CVulkanRenderSystem* vulkanRenderSystem,
                                                            DeviceQueueOptions options,
                                                            uint32_t heapMemoryLimit,
                                                            bool allowProtectedMemory = false,
                                                            bool isThreadSafe = false);

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
  void EnableShader(ShaderMethodVulkan method);
  void DisableShader();

  CVulkanShaderControl* ShaderControl() { return m_shaderControl.get(); }

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
  bool CreateFramebuffers();
  void DestroyFramebuffers();

  /**
   * @brief Creates a Vulkan pipeline layout.
   *
   * The pipeline layout is used to define the interface between shader stages and shader resources.
   *
   * @param[in] layout [optional] The descriptor set layout to use for the pipeline layout.
   * @return The created pipeline layout, or VK_NULL_HANDLE on failure.
   *
   * @note Descruction is inside the @ref Destroy() function, which is called in the destructor of CVulkanRenderSystem.
   *
   * Documentation about @ref vkCreatePipelineLayout and @ref vkDestroyPipelineLayout is available at:
   * - https://docs.vulkan.org/refpages/latest/refpages/source/vkCreatePipelineLayout.html
   * - https://docs.vulkan.org/spec/latest/chapters/descriptorsets.html
   */
  VkPipelineLayout CreatePipelineLayout(VkDescriptorSetLayout layout = VK_NULL_HANDLE);

  std::unique_ptr<CVulkanShaderControl> m_shaderControl;
  std::unique_ptr<CVulkanCommandPool> m_commandPool;
  std::unique_ptr<CVulkanDeviceQueue> m_deviceQueue;
  std::unique_ptr<CVulkanSurface> m_surface;
  std::unique_ptr<CVulkanSwapChain> m_swapChain;
  std::unique_ptr<CVulkanRenderPass> m_renderPass;
  std::vector<std::unique_ptr<CVulkanFramebuffer>> m_framebuffers;

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
  VkSurfaceKHR m_vkSurface{VK_NULL_HANDLE}; // Created & destroyed outside
  VkSurfaceFormatKHR m_vkSurfaceFormat{}; // Created & destroyed outside
  VkInstance m_vkInstance{VK_NULL_HANDLE}; // Created & destroyed outside
  VkDevice m_vkDevice{VK_NULL_HANDLE}; // Created & destroyed outside
  VkPipeline m_vkPipeline{VK_NULL_HANDLE}; // Created & destroyed outside
  VkPipelineLayout m_vkPipelineLayout{VK_NULL_HANDLE}; // Created & destroyed here
  VkSwapchainKHR m_vkSwapchain{VK_NULL_HANDLE}; // Created & destroyed outside
  VkRenderPass m_vkRenderPass{VK_NULL_HANDLE};
  VkFormat m_vkSwapchainFormat = VK_FORMAT_UNDEFINED;
  /**@}*/


  /*


  */

  /*


















  */

  struct PerFrame
  {
    VkFence queue_submit_fence = VK_NULL_HANDLE;
    std::unique_ptr<CVulkanCommandPool> primary_command_pool;
    CVulkanCommandBuffer* primary_command_buffer = nullptr;
    VkSemaphore swapchain_acquire_semaphore = VK_NULL_HANDLE;
    VkSemaphore swapchain_release_semaphore = VK_NULL_HANDLE;
    VkImageView swapchain_image_view = VK_NULL_HANDLE;
    VkFramebuffer swapchain_framebuffer = VK_NULL_HANDLE;
  };

  struct Vertex
  {
    glm::vec3 position;
    glm::vec3 color;
  };

  void Destroy();
  bool resize(const uint32_t width, const uint32_t height);
  void init_vertex_buffer();
  VkResult acquire_next_image(uint32_t* image);
  void render_triangle(uint32_t swapchain_index);
  VkResult present_image(uint32_t index);

  std::vector<VkSemaphore> m_cycled_semaphores;
  std::vector<PerFrame> m_per_frame;
  VkBuffer m_vertex_buffer = VK_NULL_HANDLE;
  //VkDeviceMemory m_vertex_buffer_memory = VK_NULL_HANDLE;
  VmaAllocation m_vertex_buffer_allocation = VK_NULL_HANDLE;
};

} // namespace VULKAN
} // namespace RENDERING
} // namespace KODI
