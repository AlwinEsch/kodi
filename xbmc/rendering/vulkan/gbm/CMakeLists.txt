if(TARGET ${APP_NAME_LC}::Vulkan AND (CORE_SYSTEM_NAME STREQUAL linux OR CORE_SYSTEM_NAME STREQUAL freebsd))
  set(SOURCES VulkanRenderSystemGBM.cpp)

  set(HEADERS VulkanRenderSystemGBM.h)

  core_add_library(rendering_vulkan_gbm)
endif()