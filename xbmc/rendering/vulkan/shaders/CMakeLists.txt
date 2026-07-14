if(TARGET ${APP_NAME_LC}::Vulkan)
  set(SOURCES VulkanShaderControl.cpp
              VulkanShaderTest.cpp)

  set(HEADERS IVulkanShader.h
              VulkanShaderControl.h
              VulkanShaderTest.h)

  core_add_library(rendering_vulkan_shaders)
endif()