set(PLATFORM_REQUIRED_DEPS LibAndroidJNI LibZip)
set(PLATFORM_OPTIONAL_DEPS_EXCLUDE CEC)
if(NOT APP_RENDER_SYSTEM)
  set(APP_RENDER_SYSTEM vulkan)
endif()
list(APPEND PLATFORM_OPTIONAL_DEPS LibDovi)

if(APP_RENDER_SYSTEM STREQUAL "vulkan")
  list(APPEND PLATFORM_REQUIRED_DEPS Vulkan VulkanMemoryAllocator GLM)
elseif(APP_RENDER_SYSTEM STREQUAL "gles")
  list(APPEND PLATFORM_REQUIRED_DEPS OpenGLES EGL)
else()
  message(SEND_ERROR "Only \"vulkan\" or \"gles\" are available as render systems values")
endif()

# Store SDK compile version
set(TARGET_SDK 37)
# Minimum supported SDK version
set(TARGET_MINSDK 24)

set(${CORE_SYSTEM_NAME}_SEARCH_CONFIG NO_DEFAULT_PATH CACHE STRING "")
