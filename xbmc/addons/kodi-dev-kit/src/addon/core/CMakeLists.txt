# Auto generated CMakeLists.txt.
# See xbmc/addons/kodi-dev-kit/tools/code-generator.py.

set(SOURCES
  addon_control.cpp
  addon_offscreen_render_base_linux.cpp
  addon_offscreen_render_control.cpp
  addon_offscreen_render_control_linux.cpp
  addon_thread_control.cpp
)

set(HEADERS
  IMsgHdl.h
  addon_control.h
  addon_offscreen_render_base.h
  addon_offscreen_render_base_linux.h
  addon_offscreen_render_control.h
  addon_offscreen_render_control_linux.h
  addon_thread_control.h
)

if(SOURCES OR HEADERS)
  devkit_add_object(devkit_core)
endif()
