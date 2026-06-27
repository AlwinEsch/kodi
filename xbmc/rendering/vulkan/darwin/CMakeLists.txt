if(TARGET ${APP_NAME_LC}::Vulkan AND (CORE_SYSTEM_NAME STREQUAL osx OR CORE_SYSTEM_NAME STREQUAL darwin_embedded))
  set(SOURCES VulkanImplementationMac.cpp)

  set(HEADERS VulkanImplementationMac.h)

  core_add_library(rendering_vulkan_mac)
endif()