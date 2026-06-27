if(TARGET ${APP_NAME_LC}::Vulkan AND CORE_SYSTEM_NAME STREQUAL android)
  set(SOURCES VulkanImplementationAndroid.cpp)

  set(HEADERS VulkanImplementationAndroid.h)

  core_add_library(rendering_vulkan_android)
endif()