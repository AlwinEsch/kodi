if(TARGET ${APP_NAME_LC}::Vulkan)
  set(SOURCES VulkanGUIFontTTF.cpp
              VulkanGUITexture.cpp
              VulkanShader.cpp
              VulkanTexture.cpp)

  set(HEADERS VulkanGUIFontTTF.h
              VulkanGUITexture.h
              VulkanShader.h
              VulkanTexture.h)

  core_add_library(guilib_graphics_vulkan)
endif()
