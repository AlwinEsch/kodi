if(TARGET ${APP_NAME_LC}::Vulkan)
  set(SOURCES VulkanExtensions.cpp
              VulkanFunctionPointers.cpp
              VulkanGuiCompositeShader.cpp
              VulkanImplementation.cpp
              VulkanInstance.cpp
              VulkanMatrix.cpp
              VulkanRenderSystem.cpp
              VulkanScreenshotSurface.cpp
              VulkanShader.cpp
              VulkanUtils.cpp)

  set(HEADERS VulkanExtensions.h
              VulkanFunctionPointers.h
              VulkanGuiCompositeShader.h
              VulkanImplementation.h
              VulkanInfo.h
              VulkanInstance.h
              VulkanMatrix.h
              VulkanRenderSystem.h
              VulkanScreenshotSurface.h
              VulkanShader.h
              VulkanUtils.h)

  core_add_library(rendering_vulkan)
endif()
