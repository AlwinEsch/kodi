if(TARGET ${APP_NAME_LC}::Vulkan AND (CORE_SYSTEM_NAME STREQUAL linux OR CORE_SYSTEM_NAME STREQUAL freebsd))
  set(SOURCES VulkanRenderSystemXLib.cpp)

  set(HEADERS VulkanRenderSystemXLib.h)

  core_add_library(rendering_vulkan_display_xlib)
endif()