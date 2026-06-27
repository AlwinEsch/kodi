if(TARGET ${APP_NAME_LC}::Vulkan AND (CORE_SYSTEM_NAME STREQUAL windows OR CORE_SYSTEM_NAME STREQUAL windowsstore))
  set(SOURCES VulkanImplementationWindows.cpp)

  set(HEADERS VulkanImplementationWindows.h)

  core_add_library(rendering_vulkan_windows)
endif()