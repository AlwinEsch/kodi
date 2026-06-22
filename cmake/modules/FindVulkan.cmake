#.rst:
# FindVulkan
# ------------
# Finds the Vulkan library
#
# This will define the following target:
#
#   ${APP_NAME_LC}::Vulkan - The Vulkan IMPORTED library

if(NOT TARGET ${APP_NAME_LC}::${CMAKE_FIND_PACKAGE_NAME})
  find_package(PkgConfig ${SEARCH_QUIET})
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_VULKAN vulkan ${SEARCH_QUIET})
  endif()

  find_library(VULKAN_LIBRARY NAMES vulkan
                              HINTS ${PC_VULKAN_LIBDIR} ${CMAKE_OSX_SYSROOT}/System/Library
                              PATH_SUFFIXES Frameworks)
  find_path(VULKAN_INCLUDE_DIR NAMES vulkan/vulkan.h
                               HINTS ${PC_VULKAN_INCLUDEDIR} ${VULKAN_LIBRARY}/Headers)

  #-------------------------------------------
  # TEMPORARY
  find_library(GLES_LIBRARY NAMES GLESv2 OpenGLES
                                   HINTS ${PC_VULKAN_LIBDIR} ${CMAKE_OSX_SYSROOT}/System/Library
                                   PATH_SUFFIXES Frameworks)
  find_path(OPENGLES_INCLUDE_DIR NAMES GLES2/gl2.h ES2/gl.h
                                 HINTS ${PC_VULKAN_INCLUDEDIR} ${VULKAN_LIBRARY}/Headers)
  find_path(OPENGLES3_INCLUDE_DIR NAMES GLES3/gl3.h ES3/gl.h
                                  HINTS ${PC_VULKAN_INCLUDEDIR} ${VULKAN_LIBRARY}/Headers)
  #-------------------------------------------

  if(NOT VERBOSE_FIND)
     set(${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY TRUE)
   endif()

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(Vulkan
                                    REQUIRED_VARS VULKAN_LIBRARY VULKAN_INCLUDE_DIR)

  if(VULKAN_FOUND)
    add_library(${APP_NAME_LC}::${CMAKE_FIND_PACKAGE_NAME} UNKNOWN IMPORTED)
    set_target_properties(${APP_NAME_LC}::${CMAKE_FIND_PACKAGE_NAME} PROPERTIES
                                                                     IMPORTED_LOCATION "${VULKAN_LIBRARY}"
                                                                     IMPORTED_LOCATION "${GLES_LIBRARY}"
                                                                     INTERFACE_INCLUDE_DIRECTORIES "${VULKAN_INCLUDE_DIR}"
                                                                     INTERFACE_COMPILE_DEFINITIONS HAS_VULKAN)

    #-------------------------------------------
    # TEMPORARY
    set_property(TARGET ${APP_NAME_LC}::${CMAKE_FIND_PACKAGE_NAME} APPEND PROPERTY
#                                                                   IMPORTED_LOCATION "${GLES_LIBRARY}"
                                                                   INTERFACE_INCLUDE_DIRECTORIES "${OPENGLES_INCLUDE_DIR}"
                                                                                                 "${OPENGLES3_INCLUDE_DIR}")
    #-------------------------------------------
  endif()
endif()