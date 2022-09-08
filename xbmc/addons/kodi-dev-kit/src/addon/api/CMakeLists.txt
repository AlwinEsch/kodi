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

if(SOURCES OR HEADERS)
  devkit_add_object(devkit_api)
endif()
