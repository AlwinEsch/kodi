# Auto generated CMakeLists.txt.
# See xbmc/addons/kodi-dev-kit/tools/code-generator.py.

set(SOURCES
  button.cpp
  edit.cpp
  fade_label.cpp
  image.cpp
  label.cpp
  progress.cpp
  radio_button.cpp
  rendering.cpp
  settings_slider.cpp
  slider.cpp
  spin.cpp
  text_box.cpp
)

set(HEADERS
  button.h
  edit.h
  fade_label.h
  image.h
  label.h
  progress.h
  radio_button.h
  rendering.h
  settings_slider.h
  slider.h
  spin.h
  text_box.h
)

if(SOURCES OR HEADERS)
  devkit_add_object(devkit_api_gui_controls)
endif()
