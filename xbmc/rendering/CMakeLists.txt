set(SOURCES MatrixStack.cpp
            RenderSystem.cpp)

set(HEADERS MatrixStack.h
            RenderSystem.h
            RenderSystemTypes.h)

# TODO: Remove this to use the new MatrixStack implementation in the future
if(TARGET ${APP_NAME_LC}::OpenGl OR TARGET ${APP_NAME_LC}::OpenGLES)
  list(APPEND SOURCES GLExtensions.cpp
                      MatrixGL.cpp)
  list(APPEND HEADERS GLExtensions.h
                      MatrixGL.h)

  if(ARCH MATCHES arm AND ENABLE_NEON)
    list(APPEND SOURCES MatrixGL.neon.cpp)
    if(NOT DEFINED NEON_FLAGS)
      set_source_files_properties(MatrixGL.neon.cpp PROPERTIES COMPILE_OPTIONS -mfpu=neon)
    endif()
  endif()
endif()

core_add_library(rendering)
if(NOT CORE_SYSTEM_NAME STREQUAL windows AND NOT CORE_SYSTEM_NAME STREQUAL windowsstore)
  if(HAVE_SSE)
    target_compile_options(${CORE_LIBRARY} PRIVATE -msse)
  endif()
  if(HAVE_SSE2)
    target_compile_options(${CORE_LIBRARY} PRIVATE -msse2)
  endif()
endif()

