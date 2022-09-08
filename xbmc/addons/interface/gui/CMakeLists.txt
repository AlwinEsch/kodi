# Auto generated CMakeLists.txt.
# See xbmc/addons/kodi-dev-kit/tools/code-generator.py.

set(SOURCES
  GUIDialogCrashReporter.cpp
)

set(HEADERS
  GUIDialogCrashReporter.h
)

if(NOT ENABLE_STATIC_LIBS OR SOURCES)
  core_add_library(addons_interface_gui)
endif()
