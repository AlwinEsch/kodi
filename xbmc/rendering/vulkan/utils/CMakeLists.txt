if(TARGET ${APP_NAME_LC}::Vulkan)
  set(SOURCES VulkanUtils.cpp)

  set(HEADERS VulkanInitStructs.h
              VulkanUtils.h)

  core_add_library(rendering_vulkan_utils)
endif()