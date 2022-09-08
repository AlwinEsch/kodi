# Auto generated CMakeLists.txt.
# See xbmc/addons/kodi-dev-kit/tools/code-generator.py.

set(SOURCES
  GUITranslator.cpp
)

set(HEADERS
  GUITranslator.h
)

if(NOT ENABLE_STATIC_LIBS OR SOURCES)
  core_add_library(addons_interface_tools)
endif()
