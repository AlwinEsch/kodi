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
class CVulkanShaderTest;

class Camera
{
private:
  float fov;
  float znear, zfar;

  void updateViewMatrix()
  {
    glm::mat4 currentMatrix = matrices.view;

    glm::mat4 rotM = glm::mat4(1.0f);
    glm::mat4 transM;

    rotM = glm::rotate(rotM, glm::radians(rotation.x * (flipY ? -1.0f : 1.0f)),
                       glm::vec3(1.0f, 0.0f, 0.0f));
    rotM = glm::rotate(rotM, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotM = glm::rotate(rotM, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::vec3 translation = position;
    if (flipY)
    {
      translation.y *= -1.0f;
    }
    transM = glm::translate(glm::mat4(1.0f), translation);

    if (type == CameraType::firstperson)
    {
      matrices.view = rotM * transM;
    }
    else
    {
      matrices.view = transM * rotM;
    }

    viewPos = glm::vec4(position, 0.0f) * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);

    if (matrices.view != currentMatrix)
    {
      updated = true;
    }
  };

public:
  enum CameraType
  {
    lookat,
    firstperson
  };
  CameraType type = CameraType::lookat;

  glm::vec3 rotation = glm::vec3();
  glm::vec3 position = glm::vec3();
  glm::vec4 viewPos = glm::vec4();

  float rotationSpeed = 1.0f;
  float movementSpeed = 1.0f;

  bool updated = true;
  bool flipY = false;

  struct
  {
    glm::mat4 perspective;
    glm::mat4 view;
  } matrices;

  struct
  {
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
  } keys;

  bool moving() const { return keys.left || keys.right || keys.up || keys.down; }

  float getNearClip() const { return znear; }

  float getFarClip() const { return zfar; }

  void setPerspective(float fov, float aspect, float znear, float zfar)
  {
    glm::mat4 currentMatrix = matrices.perspective;
    this->fov = fov;
    this->znear = znear;
    this->zfar = zfar;
    matrices.perspective = glm::perspective(glm::radians(fov), aspect, znear, zfar);
    if (flipY)
    {
      matrices.perspective[1][1] *= -1.0f;
    }
    if (matrices.view != currentMatrix)
    {
      updated = true;
    }
  };

  void updateAspectRatio(float aspect)
  {
    glm::mat4 currentMatrix = matrices.perspective;
    matrices.perspective = glm::perspective(glm::radians(fov), aspect, znear, zfar);
    if (flipY)
    {
      matrices.perspective[1][1] *= -1.0f;
    }
    if (matrices.view != currentMatrix)
    {
      updated = true;
    }
  }

  void setPosition(glm::vec3 position)
  {
    this->position = position;
    updateViewMatrix();
  }

  void setRotation(glm::vec3 rotation)
  {
    this->rotation = rotation;
    updateViewMatrix();
  }

  void rotate(glm::vec3 delta)
  {
    this->rotation += delta;
    updateViewMatrix();
  }

  void setTranslation(glm::vec3 translation)
  {
    this->position = translation;
    updateViewMatrix();
  };

  void translate(glm::vec3 delta)
  {
    this->position += delta;
    updateViewMatrix();
  }

  void setRotationSpeed(float rotationSpeed) { this->rotationSpeed = rotationSpeed; }

  void setMovementSpeed(float movementSpeed) { this->movementSpeed = movementSpeed; }

  void update(float deltaTime)
  {
    updated = false;
    if (type == CameraType::firstperson)
    {
      if (moving())
      {
        glm::vec3 camFront;
        camFront.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
        camFront.y = sin(glm::radians(rotation.x));
        camFront.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
        camFront = glm::normalize(camFront);

        float moveSpeed = deltaTime * movementSpeed;

        if (keys.up)
          position += camFront * moveSpeed;
        if (keys.down)
          position -= camFront * moveSpeed;
        if (keys.left)
          position -= glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeed;
        if (keys.right)
          position += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeed;
      }
    }
    updateViewMatrix();
  };

  // Update m_camera passing separate axis data (gamepad)
  // Returns true if view or position has been changed
  bool updatePad(glm::vec2 axisLeft, glm::vec2 axisRight, float deltaTime)
  {
    bool retVal = false;

    if (type == CameraType::firstperson)
    {
      // Use the common console thumbstick layout
      // Left = view, right = move

      const float deadZone = 0.0015f;
      const float range = 1.0f - deadZone;

      glm::vec3 camFront;
      camFront.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
      camFront.y = sin(glm::radians(rotation.x));
      camFront.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
      camFront = glm::normalize(camFront);

      float moveSpeed = deltaTime * movementSpeed * 2.0f;
      float rotSpeed = deltaTime * rotationSpeed * 50.0f;

      // Move
      if (fabsf(axisLeft.y) > deadZone)
      {
        float pos = (fabsf(axisLeft.y) - deadZone) / range;
        position -= camFront * pos * ((axisLeft.y < 0.0f) ? -1.0f : 1.0f) * moveSpeed;
        retVal = true;
      }
      if (fabsf(axisLeft.x) > deadZone)
      {
        float pos = (fabsf(axisLeft.x) - deadZone) / range;
        position += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * pos *
                    ((axisLeft.x < 0.0f) ? -1.0f : 1.0f) * moveSpeed;
        retVal = true;
      }

      // Rotate
      if (fabsf(axisRight.x) > deadZone)
      {
        float pos = (fabsf(axisRight.x) - deadZone) / range;
        rotation.y += pos * ((axisRight.x < 0.0f) ? -1.0f : 1.0f) * rotSpeed;
        retVal = true;
      }
      if (fabsf(axisRight.y) > deadZone)
      {
        float pos = (fabsf(axisRight.y) - deadZone) / range;
        rotation.x -= pos * ((axisRight.y < 0.0f) ? -1.0f : 1.0f) * rotSpeed;
        retVal = true;
      }
    }
    else
    {
      // todo: move code from example base class for look-at
    }

    if (retVal)
    {
      updateViewMatrix();
    }

    return retVal;
  }
};

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
  //VkPipeline vkPipeline() const { return m_vkPipeline; }
  VkSwapchainKHR vkSwapchain() const { return m_vkSwapchain; }
  VkRenderPass vkRenderPass() const { return m_vkData.vkRenderPass; }
  VkFormat vkSwapchainFormat() const { return m_vkSwapchainFormat; }
  VkCommandPool vkCommandPool() const { return m_vkData.vkCommandPool; }
  VkQueue vkQueue() const { return m_deviceQueue->vkQueue(); }
  VkCommandBuffer vkCurrentCommandBuffer() const { return m_currentVkCommandBuffer; }
  uint32_t vkIndexBuffer() const { return m_indexBuffer; }
  //@}

  //Camera m_camera;

  //void RenderTriangle(float x, float y);

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

  glm::ivec4 m_viewPort{0, 0, 0, 0};
  VkViewport m_vkViewport{};
  VkRect2D m_vkScissor{};

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
  //VkPipeline m_vkPipeline{VK_NULL_HANDLE}; // Created & destroyed here
  VkSwapchainKHR m_vkSwapchain{VK_NULL_HANDLE}; // Created & destroyed outside
  //VkRenderPass m_vkRenderPass{VK_NULL_HANDLE}; // Created & destroyed here
  //VkCommandPool m_vkCommandPool{VK_NULL_HANDLE}; // Created & destroyed outside
  VkFormat m_vkSwapchainFormat = VK_FORMAT_UNDEFINED;
  /**@}*/

  VkCommandBuffer m_currentVkCommandBuffer{VK_NULL_HANDLE};
  std::optional<CVulkanScopedWrite> m_scopedWrite;
  //std::optional<CVulkanCommandBufferScoped> m_recorder;

  // TEST PURPOSE ONLY
  CVulkanShaderTest* m_testShader;
  CVulkanShaderTexture* m_testShaderTexture;
  uint32_t m_indexBuffer{0};

  VkPipeline m_pipeline{VK_NULL_HANDLE};
  VkPipelineLayout m_pipelineLayout{VK_NULL_HANDLE};
};

} // namespace KODI::RENDERING::VULKAN
