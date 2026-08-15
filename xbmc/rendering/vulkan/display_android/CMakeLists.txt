if(TARGET ${APP_NAME_LC}::Vulkan AND CORE_SYSTEM_NAME STREQUAL android)
  set(SOURCES VulkanRenderSystemAndroid.cpp)

  set(HEADERS VulkanRenderSystemAndroid.h)

  core_add_library(rendering_vulkan_display_android)
endif()