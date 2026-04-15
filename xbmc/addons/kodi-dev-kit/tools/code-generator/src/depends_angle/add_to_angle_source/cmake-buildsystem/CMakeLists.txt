# CMake build about ANGLE - Almost Native Graphics Layer Engine
#
# Angle, like the Chromium web browser, is based on the GN build system, which
# includes a meta-build system that generates build files for Ninja. However,
# this system is not directly usable outside of Chromium and is therefore
# required as CMake.
#
# Angle code sources:
# - https://chromium.googlesource.com/angle/angle
# - https://github.com/google/angle
#
# Revised CMake components here are based on Microsoft vcpkg and WebKit:
# - https://github.com/microsoft/vcpkg/tree/master/ports/angle
# - https://github.com/WebKit/WebKit/tree/main/Source/ThirdParty/ANGLE
#

cmake_minimum_required(VERSION 3.18)
project(angle CXX C)

set(ANGLE_UNOFFICIAL_NAME "unofficial" CACHE STRING "Name used for the created package (DEFAULT: unofficial")
set(ANGLE_BUILD_SHARED_LIBS OFF CACHE BOOL "Force to create as shared libs" )
set(ANGLE_UNOFFICIAL_INSTALL ON CACHE BOOL "Set the use of include and library, if set to OFF install becomes done as official")
set(USE_METAL OFF CACHE BOOL "Use Metal")

# Overrite request to disable as shared. Background is about parts where need
# mandatory to use as shared and to open with e.g. "dlopen".
#
# This override may be necessary within the Kodi addon dependency build.
if(ANGLE_BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS TRUE)
endif()

# Generate unofficial-angle-config.cmake
set(UNOFFICIAL_ANGLE_CONFIG_CMAKE
  "include(CMakeFindDependencyMacro)\n"
  "find_package(ZLIB PATHS \${CMAKE_CURRENT_LIST_DIR}/../zlib COMPONENTS static)\n"
  "\n"
  "include(\"\${CMAKE_CURRENT_LIST_DIR}/${ANGLE_UNOFFICIAL_NAME}-angle-targets.cmake\")\n")
file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/${ANGLE_UNOFFICIAL_NAME}-angle-config.cmake" ${UNOFFICIAL_ANGLE_CONFIG_CMAKE})

set(ZLIB_BUILD_STATIC TRUE)
set(ZLIB_BUILD_SHARED FALSE)
set(ZLIB_BUILD_BENCHMARK FALSE)
add_subdirectory(third_party/zlib)
# Hack fixes where the from Chromium team added zlib_bench his build can't disabled
# without code changes, as zlib_bench.cc not find zlib.h and created zlib.
target_include_directories(zlib_bench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/zlib)
target_link_directories(zlib_bench PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/third_party/zlib)
add_library(zlib ALIAS zlibstatic)

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(ANGLE_CPU_BITNESS ANGLE_IS_32_BIT_CPU)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ANGLE_CPU_BITNESS ANGLE_IS_64_BIT_CPU)
else()
  message(FATAL_ERROR "Unknown current CPU bit size: ${CMAKE_SIZEOF_VOID_P}")
endif()

set(ANGLE_USE_D3D11_COMPOSITOR_NATIVE_WINDOW "OFF")
if(WIN32 OR WINDOWS_STORE)
  set(ANGLE_BUILDSYSTEM_PORT "Win")
  if(NOT MINGW)
    set(ANGLE_USE_D3D11_COMPOSITOR_NATIVE_WINDOW "ON")
  endif()
elseif(APPLE)
  set(ANGLE_BUILDSYSTEM_PORT "Mac")
elseif(UNIX)
  set(ANGLE_BUILDSYSTEM_PORT "Linux")
else()
  # default other platforms to "Linux" config
  set(ANGLE_BUILDSYSTEM_PORT "Linux")
endif()

add_definitions(-D${ANGLE_CPU_BITNESS}=1
                -DPORT=${ANGLE_BUILDSYSTEM_PORT}
                -DANGLE_USE_D3D11_COMPOSITOR_NATIVE_WINDOW=${ANGLE_USE_D3D11_COMPOSITOR_NATIVE_WINDOW}
                -DVCPKG_TARGET_IS_WINDOWS=${WIN32}
                -DUSE_METAL=${USE_METAL})

#-------------------------------------------------------------------------
# Code below taken from vcpkg CMakeLists.txt

set(PORT ${ANGLE_BUILDSYSTEM_PORT})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(WebKitCommon-minimal)

set(ANGLE_FRAMEWORK_HEADERS_DIR "${CMAKE_BINARY_DIR}/ANGLE/headers")

set(USE_ANGLE_EGL ON)
if (NOT WINDOWS_STORE AND NOT USE_METAL)
    set(USE_OPENGL ON)
endif()

if(MSVC)
    add_compile_options(/d2guard4 /Wv:18 /guard:cf /permissive /bigobj)
    add_link_options(/guard:cf)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS}")
set(CMAKE_STATIC_LIBRARY_PREFIX "")
if(WIN32)
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif()

if (WINDOWS_STORE)
    set(WINRT_DEFINES -DANGLE_ENABLE_WINDOWS_UWP -DWINAPI_FAMILY=WINAPI_FAMILY_PC_APP)
else()
    set(WINRT_DEFINES)
endif()
add_definitions(
    -D_CRT_SECURE_NO_DEPRECATE
    -D_SCL_SECURE_NO_WARNINGS
    -DGL_SILENCE_DEPRECATION
    -D_HAS_EXCEPTIONS=0
    -DNOMINMAX
    -DANGLE_STANDALONE_BUILD
    ${WINRT_DEFINES}
)
add_compile_options("$<$<CONFIG:DEBUG>:-DANGLE_ENABLE_DEBUG_ANNOTATIONS>")
if(WIN32)
    add_definitions(-DANGLE_IS_WIN)
endif()
if(LINUX)
    add_definitions(-DANGLE_IS_LINUX)
endif()
if(ANGLE_IS_64_BIT_CPU)
    add_definitions(-DANGLE_IS_64_BIT_CPU)
elseif(ANGLE_IS_32_BIT_CPU)
    add_definitions(-DANGLE_IS_32_BIT_CPU)
endif()
if(ANGLE_USE_D3D11_COMPOSITOR_NATIVE_WINDOW)
    if(NOT WIN32)
        message(FATAL_ERROR "ANGLE_USE_D3D11_COMPOSITOR_NATIVE_WINDOW is set, but is only supported on Windows")
    endif()
    set(angle_enable_d3d11_compositor_native_window TRUE)
endif()
if(NOT BUILD_SHARED_LIBS)
    add_definitions(
        -DANGLE_EXPORT=
        -DANGLE_STATIC=1
        -DANGLE_UTIL_EXPORT=
        -DEGLAPI=
        -DGL_APICALL=
        -DGL_API=
    )
endif()

##################################################

# Derived from: https://github.com/WebKit/WebKit/blob/92dbcacf4c3e3a8fc6eea68e7022ca59401749e0/Source/ThirdParty/ANGLE/CMakeLists.txt
# With modifications for vcpkg port (marked with "VCPKG EDIT:" comments)

set_property(DIRECTORY . PROPERTY FOLDER "ANGLE")

# VCPKG EDIT: modified block
if (APPLE)
    set(is_apple TRUE)
    if (IOS)
        set(is_ios TRUE)
        if (USE_OPENGL)
            set(angle_enable_eagl TRUE)
        endif()
    else()
        set(is_mac TRUE)
    endif()
    if (USE_OPENGL AND NOT angle_enable_eagl)
        set(angle_enable_cgl TRUE)
    endif()
elseif (WIN32)
    set(is_win TRUE)
    if (NOT WINDOWS_STORE)
        set(angle_is_winuwp FALSE)
    else()
        set(angle_is_winuwp TRUE)
        set(target_os "winuwp")
    endif()
    if (NOT angle_is_winuwp)
        set(angle_enable_d3d9 TRUE)
    endif()
    set(angle_enable_d3d11 TRUE)
elseif (UNIX)
    set(is_linux TRUE)
endif ()
# VCPKG EDIT: end vcpkg modified block

include(cmake/Compiler.cmake)
include(cmake/GLESv2.cmake)

# ANGLE Renderer backends
include(cmake/D3D.cmake)
include(cmake/GL.cmake)
include(cmake/Metal.cmake)

set(no_gl_prototypes
    GL_GLES_PROTOTYPES=0
    EGL_EGL_PROTOTYPES=0
)

set(gl_prototypes
    GL_GLES_PROTOTYPES=1
    EGL_EGL_PROTOTYPES=1
    GL_GLEXT_PROTOTYPES
    EGL_EGLEXT_PROTOTYPES
)

# Default library types for ANGLE
# Override these in Platform*.cmake for your port as needed.
set(ANGLE_LIBRARY_TYPE STATIC) # libANGLE static library (matches expected behavior & prior behavior)
set(GLESv2_LIBRARY_TYPE) # VCPKG EDIT: Default to BUILD_SHARED_LIBS setting
set(EGL_LIBRARY_TYPE) # VCPKG EDIT: Default to BUILD_SHARED_LIBS setting

# ANGLE makes a number of small static libraries that are then joined into a
# bigger library that is built shared. Rather than making the small libraries
# there will be a ANGLE whose sources are dependent on whether the library
# is being used as a compiler or as a GLES implementation.
#
# The corresponding gn targets are described below
#
# ANGLE (Compiler only)
# + angle_common
# + preprocessor
# + translator
#
# ANGLE (GLES)
# + ANGLE (Compiler only)
# + xxhash
# + angle_image_util
# + angle_system_utils (OS specific)
# + angle_(renderer) (Backend and OS specific)

set(ANGLE_PRIVATE_INCLUDE_DIRECTORIES
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/KHR"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/common/third_party/base"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/common/base"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/common/third_party/xxhash"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/zlib/google"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/third_party/khronos" # KODI: Additionally added
    "${CMAKE_CURRENT_BINARY_DIR}/include"
)

set(ANGLE_DEFINITIONS
    ANGLE_ENABLE_ESSL
    ANGLE_ENABLE_GLSL
    HAS_ANGLE
)

# VCPKG EDIT
list(APPEND ANGLE_DEFINITIONS ANGLE_CAPTURE_ENABLED=0)
if (NOT WIN32)
    list(APPEND ANGLE_DEFINITIONS
        "-DGL_API=__attribute__((visibility(\"default\")))"
        "-DGL_APICALL=__attribute__((visibility(\"default\")))"
    )
endif()
# END VCPKG EDIT

set(ANGLE_SOURCES
    ${libangle_common_sources}
    ${libangle_common_shader_state_sources}
    ${angle_preprocessor_sources}
    ${angle_translator_glsl_base_sources}
    ${angle_translator_essl_sources}
    ${angle_translator_essl_symbol_table_sources}
    ${angle_translator_glsl_and_vulkan_base_sources}
    ${angle_translator_glsl_sources}
    ${angle_translator_sources}
    ${angle_system_utils_sources}
    src/common/angle_version_info.cpp
    src/libANGLE/capture/FrameCapture_mock.cpp
    src/libANGLE/capture/serialize_mock.cpp
)

if (WIN32)
    # FIXME: DX11 support will not compile if this preprocessor definition is set
    # DirectX Documentation is setting that version to 0x700 but there is no
    # corresponding value in sdkddkver.h
    remove_definitions(-D_WIN32_WINNT=0x601 -DWINVER=0x601)

    list(APPEND ANGLE_SOURCES
        "src/libANGLE/renderer/dxgi_format_map.h"
        "src/libANGLE/renderer/dxgi_format_map_autogen.cpp"
        "src/libANGLE/renderer/dxgi_support_table.h"
        "src/libANGLE/renderer/dxgi_support_table_autogen.cpp"
    )
    if (NOT angle_is_winuwp)
        list(APPEND ANGLE_SOURCES
            "src/libANGLE/renderer/d3d_format.cpp"
            "src/libANGLE/renderer/d3d_format.h"
        )
    endif()
endif ()

set(ANGLEGLESv2_LIBRARIES
    ANGLE
)

set(zlib_wrapper_sources
    "third_party/zlib/google/compression_utils_portable.h"
    "third_party/zlib/google/compression_utils_portable.cc"
)

set(angle_gl_enum_utils # VCPKG EDIT: Update paths
    "src/common/gl_enum_utils.cpp"
    "src/common/gl_enum_utils.h"
    "src/common/gl_enum_utils_autogen.cpp"
    "src/common/gl_enum_utils_autogen.h"
)

WEBKIT_INCLUDE_CONFIG_FILES_IF_EXISTS()

add_subdirectory(include)

add_library(ANGLEFramework INTERFACE)
add_dependencies(ANGLEFramework GLSLANGHeaders ANGLEHeaders)

if (USE_ANGLE_EGL OR ENABLE_WEBGL)
    add_library(ANGLE ${ANGLE_LIBRARY_TYPE}
        ${ANGLE_SOURCES}
        ${libangle_sources}
        ${libangle_headers}
        ${libangle_image_util_sources}
        ${libangle_image_util_headers}
        ${xxhash_sources}
        ${zlib_wrapper_sources}
        ${angle_gl_enum_utils}
    )
    target_include_directories(ANGLE PRIVATE ${ANGLE_PRIVATE_INCLUDE_DIRECTORIES})
    target_compile_definitions(ANGLE PRIVATE
        ${no_gl_prototypes}
        LIBANGLE_IMPLEMENTATION
    )
    target_compile_definitions(ANGLE PUBLIC ${ANGLE_DEFINITIONS})
    target_link_libraries(ANGLE PRIVATE ZLIB::ZLIBSTATIC ${CMAKE_DL_LIBS})

    target_compile_definitions(ANGLEFramework INTERFACE ${gl_prototypes})

    add_library(GLESv2 ${GLESv2_LIBRARY_TYPE}
        ${libglesv2_entry_point_sources}
        ${libglesv2_sources}
    )
    target_include_directories(GLESv2 PRIVATE ${ANGLE_PRIVATE_INCLUDE_DIRECTORIES})
    target_compile_definitions(GLESv2 PRIVATE
        ${no_gl_prototypes}
        LIBGLESV2_IMPLEMENTATION
    )

    target_compile_definitions(GLESv2 PUBLIC ${gl_prototypes} HAS_ANGLE) 
    target_link_libraries(GLESv2 PRIVATE ${ANGLEGLESv2_LIBRARIES} ${CMAKE_DL_LIBS})

    if(WIN32)
        # Output library name according to the .def
        target_sources(GLESv2 PRIVATE src/libGLESv2/libGLESv2_autogen.def)
        set_target_properties(GLESv2 PROPERTIES OUTPUT_NAME libGLESv2)
    else()
        # Rename libs to avoid conflict with system OpenGL
        set_target_properties(GLESv2 PROPERTIES OUTPUT_NAME libGLESv2_angle)
    endif()

    add_library(GLESv2Framework INTERFACE)
    target_link_libraries(GLESv2Framework INTERFACE GLESv2)
    target_include_directories(GLESv2Framework INTERFACE ${ANGLE_FRAMEWORK_HEADERS_DIR})
    target_compile_definitions(GLESv2Framework INTERFACE USE_SYSTEM_EGL)
    add_library(ANGLE::GLES ALIAS GLESv2Framework)

    if (USE_ANGLE_EGL)
        add_library(EGL ${EGL_LIBRARY_TYPE}
            ${libegl_sources}
        )
        target_include_directories(EGL PRIVATE ${ANGLE_PRIVATE_INCLUDE_DIRECTORIES})
        target_compile_definitions(EGL PRIVATE
            ${ANGLE_DEFINITIONS}
            ${gl_prototypes}
            EGLAPI=
        )

        target_link_libraries(EGL PUBLIC GLESv2 ${CMAKE_DL_LIBS})

        set_target_properties(EGL PROPERTIES LINKER_LANGUAGE CXX)
        if(WIN32)
            # Output library names according to the .def
            target_sources(EGL PRIVATE src/libEGL/libEGL_autogen.def)
            set_target_properties(EGL PROPERTIES OUTPUT_NAME libEGL)
        else()
            # Rename libs to avoid conflict with system OpenGL
            set_target_properties(EGL PROPERTIES OUTPUT_NAME libEGL_angle)
        endif()

        add_library(EGLFramework INTERFACE)
        target_include_directories(EGLFramework INTERFACE ${ANGLE_FRAMEWORK_HEADERS_DIR}/)
        target_compile_definitions(EGLFramework INTERFACE ${gl_prototypes})
        target_link_libraries(EGLFramework INTERFACE EGL)
        add_library(ANGLE::EGL ALIAS EGLFramework)
    endif ()
else ()
    add_library(ANGLE ${ANGLE_LIBRARY_TYPE} ${ANGLE_SOURCES})
    target_include_directories(ANGLE PRIVATE ${ANGLE_PRIVATE_INCLUDE_DIRECTORIES})
    target_link_libraries(ANGLE PRIVATE ${CMAKE_DL_LIBS})
    target_compile_definitions(ANGLE PRIVATE
        ${ANGLE_DEFINITIONS}
        ${no_gl_prototypes}
        LIBANGLE_IMPLEMENTATION
    )
endif ()

if (ENABLE_WEBGL)
    set(libglesv2_entry_points_headers
        src/libGLESv2/entry_points_egl_autogen.h
        src/libGLESv2/entry_points_egl_ext_autogen.h
        src/libGLESv2/entry_points_gles_2_0_autogen.h
        src/libGLESv2/entry_points_gles_3_0_autogen.h
        src/libGLESv2/entry_points_gles_ext_autogen.h
    )

    WEBKIT_COPY_FILES(LibGLESv2EntryPointsHeaders
        DESTINATION ${ANGLE_FRAMEWORK_HEADERS_DIR}/ANGLE
        FILES ${libglesv2_entry_points_headers}
        FLATTENED
    )
    if (WIN32 AND TARGET GLESv2)
        # GLESv2 needs to have a direct or indirect dependency to
        # LibGLESv2EntryPointsHeaders for CMake Visual Studio generator
        # to eliminate duplicated custom commands. Otherwise,
        # entry_points_*.h will be copied twice in both projects.
        add_dependencies(GLESv2 LibGLESv2EntryPointsHeaders)
    endif ()

    add_custom_target(ANGLE-webgl-headers
        DEPENDS LibGLESv2EntryPointsHeaders ANGLEWebGLHeaders
        COMMAND ${CMAKE_COMMAND} -E env
            BUILT_PRODUCTS_DIR=${ANGLE_FRAMEWORK_HEADERS_DIR}
            PUBLIC_HEADERS_FOLDER_PATH=/ANGLE
            ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/adjust-angle-include-paths.py
        VERBATIM
    )
    add_dependencies(GLESv2Framework ANGLE-webgl-headers)
endif ()

if (COMPILER_IS_GCC_OR_CLANG)
    foreach (angle_target ANGLE EGL GLESv2)
        if (TARGET ${angle_target})
            WEBKIT_ADD_TARGET_C_FLAGS(${angle_target} -w)
            WEBKIT_ADD_TARGET_CXX_FLAGS(${angle_target} -w)
        endif ()
    endforeach ()
endif ()

target_link_libraries(ANGLEFramework INTERFACE ANGLE)
target_include_directories(ANGLEFramework INTERFACE ${ANGLE_FRAMEWORK_HEADERS_DIR})

add_library(ANGLE::ANGLE ALIAS ANGLEFramework)


##################################################

#### VCPKG EDIT:
#### various vcpkg additions and install commands

# set export names of some targets to match prior vcpkg port buildsystem
if(TARGET EGL)
    set_target_properties(EGL PROPERTIES EXPORT_NAME libEGL)
endif()
if(TARGET GLESv2)
    set_target_properties(GLESv2 PROPERTIES EXPORT_NAME libGLESv2)
endif()
set_target_properties(ANGLE PROPERTIES EXPORT_NAME libANGLE)

set(_possibleTargets EGL GLESv2 ANGLE)
foreach(_target IN LISTS _possibleTargets)
    if(TARGET ${_target})
        list(APPEND _installableTargets "${_target}")
    endif()
endforeach()

install(TARGETS ${_installableTargets} EXPORT ANGLEExport
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(EXPORT ANGLEExport FILE ${ANGLE_UNOFFICIAL_NAME}-angle-targets.cmake NAMESPACE ${ANGLE_UNOFFICIAL_NAME}::angle:: DESTINATION lib/cmake/${ANGLE_UNOFFICIAL_NAME}-angle)
install(FILES ${ANGLE_UNOFFICIAL_NAME}-angle-config.cmake DESTINATION lib/cmake/${ANGLE_UNOFFICIAL_NAME}-angle)

set(ANGLE_INCLUDE_FOLDER $<IF:$<BOOL:${ANGLE_UNOFFICIAL_INSTALL}>,include/angle,include>)
install(
    DIRECTORY "${ANGLE_FRAMEWORK_HEADERS_DIR}/"
    DESTINATION ${ANGLE_INCLUDE_FOLDER}
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.inc"
    PATTERN "CL" EXCLUDE
    PATTERN "GLSLANG" EXCLUDE
    PATTERN "WGL" EXCLUDE
    PATTERN "export.h" EXCLUDE
)

install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/angle_gl.h
          ${CMAKE_CURRENT_SOURCE_DIR}/include/angle_windowsstore.h
    DESTINATION ${ANGLE_INCLUDE_FOLDER}
)

if(NOT BUILD_SHARED_LIBS)
    foreach(angle_target EGL GLESv2 ANGLE)
        if(TARGET ${angle_target})
            target_compile_definitions(${angle_target} INTERFACE $<INSTALL_INTERFACE:KHRONOS_STATIC>)
        endif()
    endforeach()
endif()
