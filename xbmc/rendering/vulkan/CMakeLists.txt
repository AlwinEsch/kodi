if(TARGET ${APP_NAME_LC}::Vulkan)
  set(SOURCES VulkanExtensions.cpp
              VulkanGuiCompositeShader.cpp
              VulkanMatrix.cpp
              VulkanRenderSystem.cpp
              VulkanScreenshotSurface.cpp
              VulkanShader.cpp)

  set(HEADERS VulkanExtensions.h
              VulkanGuiCompositeShader.h
              VulkanMatrix.h
              VulkanRenderSystem.h
              VulkanScreenshotSurface.h
              VulkanShader.h)

  if(ARCH MATCHES arm AND ENABLE_NEON)
    list(APPEND SOURCES VulkanMatrix.neon.cpp)
    if(NOT DEFINED NEON_FLAGS)
      set_source_files_properties(VulkanMatrix.neon.cpp PROPERTIES COMPILE_OPTIONS -mfpu=neon)
    endif()
  endif()

  core_add_library(rendering_vulkan)
endif()
