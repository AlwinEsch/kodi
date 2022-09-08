# Auto generated CMakeLists.txt.
# See xbmc/addons/kodi-dev-kit/tools/code-generator.py.

set(SOURCES
  addon_base.cpp
  audio_engine.cpp
  filesystem.cpp
  general.cpp
  monitor.cpp
  network.cpp
)

set(HEADERS
  addon_base.h
  audio_engine.h
  filesystem.h
  general.h
  monitor.h
  network.h
)

if(NOT ENABLE_STATIC_LIBS OR SOURCES)
  core_add_library(addons_interface_api)
endif()
