#.rst:
# FindVulkanGLSLC
# ----------
# Finds Vulkan glslc executable
#
# This will define the following variables::
#
# Vulkan::glslc - the glslc executable

if(NOT TARGET Vulkan::glslc)
  include(FindPackageHandleStandardArgs)

  find_program(GLSLC_EXECUTABLE glslc
                                HINTS ${NATIVEPREFIX}/bin)

  if(GLSLC_EXECUTABLE)
    execute_process(COMMAND ${GLSLC_EXECUTABLE} -version
                    OUTPUT_VARIABLE GLSLC_VERSION
                    ERROR_QUIET
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
  endif()

  # Provide standardized success/failure messages
  find_package_handle_standard_args(VulkanGLSLC
                                    REQUIRED_VARS GLSLC_EXECUTABLE
                                    VERSION_VAR GLSLC_VERSION)

  if(VulkanGLSLC_FOUND)
    add_executable(Vulkan::glslc IMPORTED GLOBAL)
    set_target_properties(Vulkan::glslc PROPERTIES
                                        IMPORTED_LOCATION "${GLSLC_EXECUTABLE}"
                                        VERSION "${GLSLC_VERSION}")
  else()
    if(VulkanGLSLC_FIND_REQUIRED)
      message(FATAL_ERROR "GLSLC was not found.")
    endif()
  endif()
endif()
