if(TARGET ${APP_NAME_LC}::Vulkan)
  set(SOURCES DynamicBuffers.cpp
              VulkanCommandBuffer.cpp
              VulkanCommandPool.cpp
              VulkanDeviceQueue.cpp
              VulkanFenceHelper.cpp
              VulkanFramebuffer.cpp
              VulkanFunctionPointers.cpp
              VulkanGuiCompositeShader.cpp
              VulkanImplementation.cpp
              VulkanInstance.cpp
              VulkanRenderPass.cpp
              VulkanRenderSystem.cpp
              VulkanScopedWrite.cpp
              VulkanScreenshotSurface.cpp
              VulkanSurface.cpp
              VulkanSwapChain.cpp)

  set(HEADERS DynamicBuffers.h
              VulkanCommandBuffer.h
              VulkanCommandPool.h
              VulkanData.h
              VulkanDeviceQueue.h
              VulkanFenceHelper.h
              VulkanFramebuffer.h
              VulkanFunctionPointers.h
              VulkanGuiCompositeShader.h
              VulkanImplementation.h
              VulkanInfo.h
              VulkanInstance.h
              VulkanRenderPass.h
              VulkanRenderSystem.h
              VulkanScopedWrite.h
              VulkanScreenshotSurface.h
              VulkanSurface.h
              VulkanSwapChain.h)

  core_add_library(rendering_vulkan)

  find_package(Vulkan COMPONENTS glslc)
  find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

  if(POLICY CMP0116)
    set(CMAKE_POLICY_DEFAULT_CMP0116 NEW)
    cmake_policy(SET CMP0116 NEW)
  endif()

  function(compile_shader target)
      cmake_parse_arguments(PARSE_ARGV 1 arg "" "ENV;FORMAT;STAGE;SRCPATH;OUTPATH" "SOURCES")

      if(arg_FORMAT STREQUAL "bin")
          set(EXTENSION "spv")
      else()
          set(EXTENSION ${arg_FORMAT})
      endif()

      foreach(source ${arg_SOURCES})
        string(REGEX REPLACE "^(.*)(.glsl|.hlsl|.slang)" "\\1.${EXTENSION}" OUTNAME ${source})
          add_custom_command(
              OUTPUT ${arg_OUTPATH}/${OUTNAME}
              DEPENDS ${arg_SRCPATH}/${source}
              DEPFILE ${CMAKE_CURRENT_BINARY_DIR}/${OUTNAME}.d
              COMMAND
                  ${glslc_executable}
                  $<$<BOOL:${arg_ENV}>:--target-env=${arg_ENV}>
                  $<$<BOOL:${arg_FORMAT}>:-mfmt=${arg_FORMAT}>
                  $<$<BOOL:${arg_STAGE}>:-fshader-stage=${arg_STAGE}>
                  -MD -MF ${CMAKE_CURRENT_BINARY_DIR}/${OUTNAME}.d
                  -o ${arg_OUTPATH}/${OUTNAME}
                  ${arg_SRCPATH}/${source}
          )
          target_sources(${target} PRIVATE ${arg_OUTPATH}/${OUTNAME})
      endforeach()
  endfunction()

  set(SHADERS_VERTEX
    fonts_vert.glsl
    fonts_vert_clip.glsl
    vulkan_shader_gr0_vert_test_triangle.glsl
    vulkan_shader_gr1_vert.glsl
    vulkan_shader_gr2_vert_default.glsl
    vulkan_shader_gr3_vert_clip.glsl
    vulkan_shader_gr3_vert_simple.glsl
    vulkan_shader_gr4_vert_gui_composite.glsl
    vulkan_shader_gr5_vert_videofilter.glsl
  )

  set(SHADERS_PIXEL
    fonts_frag.glsl
    vulkan_shader_gr0_frag_test_triangle.glsl
    vulkan_shader_gr1_frag_multi.glsl
    vulkan_shader_gr1_frag_multi_blendcolor.glsl
    vulkan_shader_gr1_frag_texture.glsl
    vulkan_shader_gr1_frag_texture_lim.glsl
    vulkan_shader_gr1_frag_texture_noblend.glsl
    vulkan_shader_gr2_frag_default.glsl
    vulkan_shader_gr3_frag_fonts.glsl
    vulkan_shader_gr4_frag_gui_composite.glsl
    vulkan_shader_gr5_frag_stretch.glsl
    vulkan_shader_gr5_frag_videofilter.glsl
  )

  compile_shader(rendering_vulkan
      ENV vulkan1.2
      FORMAT bin
      STAGE vert
      SRCPATH ${CMAKE_SOURCE_DIR}/system/shaders/Vulkan
      OUTPATH ${CMAKE_BINARY_DIR}/system/shaders/Vulkan
      SOURCES
          ${SHADERS_VERTEX}
  )

  compile_shader(rendering_vulkan
      ENV vulkan1.2
      FORMAT bin
      STAGE frag
      SRCPATH ${CMAKE_SOURCE_DIR}/system/shaders/Vulkan
      OUTPATH ${CMAKE_BINARY_DIR}/system/shaders/Vulkan
      SOURCES
          ${SHADERS_PIXEL}
  )

endif()
