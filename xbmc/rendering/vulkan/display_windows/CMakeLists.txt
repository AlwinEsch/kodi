if(TARGET ${APP_NAME_LC}::Vulkan AND (CORE_SYSTEM_NAME STREQUAL windows OR CORE_SYSTEM_NAME STREQUAL windowsstore))
  set(SOURCES VulkanRenderSystemWindows.cpp)

  set(HEADERS VulkanRenderSystemWindows.h)

  core_add_library(rendering_vulkan_display_windows)
endif()