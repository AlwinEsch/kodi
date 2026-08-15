#.rst:
# FindGLM
# ------------
# Finds the OpenGL Mathematics (GLM) as a header only C++ mathematics library.
#
# This will define the following variables:
#
# GLM_FOUND - system has GLM
# GLM_INCLUDE_DIR - the GLM include directory

if(NOT TARGET ${APP_NAME_LC}::${CMAKE_FIND_PACKAGE_NAME})
  find_path(GLM_INCLUDE_DIR NAMES glm/glm.hpp
                            HINTS ${DEPENDS_PATH}/include)
  if(GLM_INCLUDE_DIR)
    set(GLM_FOUND TRUE)
  else()
    set(GLM_FOUND FALSE)
  endif()
  # Create imported target ${APP_NAME_LC}::${CMAKE_FIND_PACKAGE_NAME}
  add_library(${APP_NAME_LC}::${CMAKE_FIND_PACKAGE_NAME} INTERFACE IMPORTED)
  set_target_properties(${APP_NAME_LC}::${CMAKE_FIND_PACKAGE_NAME} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${GLM_INCLUDE_DIR}"
    INTERFACE_COMPILE_DEFINITIONS GLM_FORCE_DEPTH_ZERO_TO_ONE
  )
endif()
