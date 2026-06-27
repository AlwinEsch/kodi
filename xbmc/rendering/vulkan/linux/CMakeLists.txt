if(TARGET ${APP_NAME_LC}::Vulkan AND (CORE_SYSTEM_NAME STREQUAL linux OR CORE_SYSTEM_NAME STREQUAL freebsd))
  set(SOURCES VulkanImplementationLinux.cpp)

  set(HEADERS VulkanImplementationLinux.h)

  core_add_library(rendering_vulkan_linux)
endif()