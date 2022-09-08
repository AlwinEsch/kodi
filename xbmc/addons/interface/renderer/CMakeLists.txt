# Auto generated CMakeLists.txt.
# See xbmc/addons/kodi-dev-kit/tools/code-generator.py.

set(SOURCES
  GLViewRenderer.cpp
)

set(HEADERS
  GLViewRenderer.h
  IRenderHelper.h
)

if(NOT ENABLE_STATIC_LIBS OR SOURCES)
  core_add_library(addons_interface_renderer)
endif()
