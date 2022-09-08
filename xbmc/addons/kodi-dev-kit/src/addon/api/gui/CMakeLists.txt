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

if(SOURCES OR HEADERS)
  devkit_add_object(devkit_api_gui)
endif()
