# Auto generated CMakeLists.txt.
# See xbmc/addons/kodi-dev-kit/tools/code-generator.py.

set(SOURCES
  context_menu.cpp
  extended_progress.cpp
  filebrowser.cpp
  keyboard.cpp
  numeric.cpp
  ok.cpp
  progress.cpp
  select.cpp
  text_viewer.cpp
  yes_no.cpp
)

set(HEADERS
  context_menu.h
  extended_progress.h
  filebrowser.h
  keyboard.h
  numeric.h
  ok.h
  progress.h
  select.h
  text_viewer.h
  yes_no.h
)

if(NOT ENABLE_STATIC_LIBS OR SOURCES)
  core_add_library(addons_interface_api_gui_dialogs)
endif()
