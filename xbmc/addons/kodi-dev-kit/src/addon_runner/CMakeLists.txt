# Add-on helper executable to launch an add-on library as executable in Kodi
#

cmake_minimum_required(VERSION 3.5)
project(kodi_addon_runner)

cmake_parse_arguments(arg "WRAPPED" "DEVKIT_OUTPUT_DIRECTORY" "" ${ARGN})
if(arg_DEVKIT_OUTPUT_DIRECTORY)
  set(DEVKIT_OUTPUT_DIRECTORY ${arg_DEVKIT_OUTPUT_DIRECTORY})
else()
  set(DEVKIT_OUTPUT_DIRECTORY "addons/kodi.binary.devkit")
endif()

set(SOURCES main.cpp)

set(DEVKIT_LAUNCH_EXE ${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX} CACHE STRING "Launch executable used for dev-kit")

add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES C_VISIBILITY_PRESET hidden
                                      CXX_VISIBILITY_PRESET hidden
                                      VISIBILITY_INLINES_HIDDEN ON
                                      POSITION_INDEPENDENT_CODE ON
                                      LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${DEVKIT_OUTPUT_DIRECTORY}
                                      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${DEVKIT_OUTPUT_DIRECTORY}
                                      OUTPUT_NAME ${DEVKIT_LAUNCH_EXE} PREFIX "")
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
  set_target_properties(${name} PROPERTIES  LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/${DEVKIT_OUTPUT_DIRECTORY}
                                            RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/${DEVKIT_OUTPUT_DIRECTORY})
endforeach()

target_link_libraries(${PROJECT_NAME} PUBLIC dl)
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBDWARF_INCLUDE_DIRS} ../../include)
target_compile_definitions(${PROJECT_NAME} PRIVATE APP_NAME="${PROJECT_NAME}")

set(LIBRARY_FILES ${LIBRARY_FILES} ${CMAKE_BINARY_DIR}/${DEVKIT_OUTPUT_DIRECTORY}/${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX} CACHE STRING "" FORCE)
