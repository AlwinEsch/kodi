# Auto generated CMakeLists.txt.
# See xbmc/addons/kodi-dev-kit/tools/code-generator.py.

set(SOURCES
  general.cpp
  list_item.cpp
  window.cpp
)

set(HEADERS
  general.h
  list_item.h
  window.h
)

if(NOT ENABLE_STATIC_LIBS OR SOURCES)
  core_add_library(addons_interface_api_gui)
endif()
