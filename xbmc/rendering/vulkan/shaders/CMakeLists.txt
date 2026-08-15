if(TARGET ${APP_NAME_LC}::Vulkan)
  set(SOURCES IVulkanShader.cpp
              VulkanShaderControl.cpp
              VulkanShaderDefault.cpp
              VulkanShaderFonts.cpp
              VulkanShaderFontsShaderClip.cpp
              VulkanShaderMulti.cpp
              VulkanShaderMultiBlendColor.cpp
              VulkanShaderTexture.cpp
              VulkanShaderTextureLim.cpp
              VulkanShaderTextureNoAlpha.cpp
              VulkanShaderTextureNoBlend.cpp)

  set(HEADERS IVulkanShader.h
              VulkanShaderControl.h
              VulkanShaderDefault.h
              VulkanShaderFonts.h
              VulkanShaderFontsShaderClip.h
              VulkanShaderMulti.h
              VulkanShaderMultiBlendColor.h
              VulkanShaderTexture.h
              VulkanShaderTextureLim.h
              VulkanShaderTextureNoAlpha.h
              VulkanShaderTextureNoBlend.h)

  core_add_library(rendering_vulkan_shaders)
endif()