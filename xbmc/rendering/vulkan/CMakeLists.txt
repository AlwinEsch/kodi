if(TARGET ${APP_NAME_LC}::Vulkan)
  set(SOURCES VulkanCommandBuffer.cpp
              VulkanCommandPool.cpp
              VulkanDeviceQueue.cpp
              VulkanExtensions.cpp
              VulkanFunctionPointers.cpp
              VulkanGuiCompositeShader.cpp
              VulkanImplementation.cpp
              VulkanInstance.cpp
              VulkanMatrix.cpp
              VulkanRenderSystem.cpp
              VulkanScreenshotSurface.cpp
              VulkanShader.cpp
              VulkanSurface.cpp
              VulkanSwapChain.cpp
              VulkanUtils.cpp)

  set(HEADERS VulkanCommandBuffer.h
              VulkanCommandPool.h
              VulkanDeviceQueue.h
              VulkanExtensions.h
              VulkanFunctionPointers.h
              VulkanGuiCompositeShader.h
              VulkanImplementation.h
              VulkanInfo.h
              VulkanInstance.h
              VulkanMatrix.h
              VulkanRenderSystem.h
              VulkanScreenshotSurface.h
              VulkanShader.h
              VulkanSurface.h
              VulkanSwapChain.h
              VulkanUtils.h)

  core_add_library(rendering_vulkan)
endif()
