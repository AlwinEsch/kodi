if(TARGET ${APP_NAME_LC}::Vulkan)
  set(SOURCES VulkanExtensions.cpp
              VulkanGuiCompositeShader.cpp
              VulkanImplementation.cpp
              VulkanInstance.cpp
              VulkanMatrix.cpp
              VulkanRenderSystem.cpp
              VulkanScreenshotSurface.cpp
              VulkanShader.cpp)

  set(HEADERS VulkanExtensions.h
              VulkanGuiCompositeShader.h
              VulkanImplementation.h
              VulkanInstance.h
              VulkanMatrix.h
              VulkanRenderSystem.h
              VulkanScreenshotSurface.h
              VulkanShader.h)

  core_add_library(rendering_vulkan)
endif()
