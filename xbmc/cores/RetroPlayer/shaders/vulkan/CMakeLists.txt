if(TARGET ${APP_NAME_LC}::Vulkan)
  list(APPEND SOURCES ShaderVulkan.cpp
                      ShaderLutVulkan.cpp
                      ShaderPresetVulkan.cpp
                      ShaderTextureVulkan.cpp
                      ShaderTextureVulkanRef.cpp
                      ShaderUtilsVulkan.cpp
  )
  list(APPEND HEADERS ShaderVulkan.h
                      ShaderLutVulkan.h
                      ShaderPresetVulkan.h
                      ShaderTextureVulkan.h
                      ShaderTextureVulkanRef.h
                      ShaderUtilsVulkan.h
  )

  core_add_library(rp-shaders-vulkan)
endif()
