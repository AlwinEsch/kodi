# Auto generated CMakeLists.txt.
# See xbmc/addons/kodi-dev-kit/tools/code-generator.py.

set(SOURCES
  LauncherDirectDll.cpp
  $<$<STREQUAL:${CORE_SYSTEM_NAME},linux>:LauncherPosix.cpp>
  $<$<STREQUAL:${CORE_SYSTEM_NAME},freebsd>:LauncherPosix.cpp>
  $<$<STREQUAL:${CORE_SYSTEM_NAME},android>:LauncherPosix.cpp>
  $<$<STREQUAL:${CORE_SYSTEM_NAME},osx>:LauncherPosix.cpp>
  $<$<STREQUAL:${CORE_SYSTEM_NAME},darwin_embedded>:LauncherPosix.cpp>
)

set(HEADERS
  ILauncher.h
  LauncherDirectDll.h
  $<$<STREQUAL:${CORE_SYSTEM_NAME},linux>:LauncherPosix.h>
  $<$<STREQUAL:${CORE_SYSTEM_NAME},freebsd>:LauncherPosix.h>
  $<$<STREQUAL:${CORE_SYSTEM_NAME},android>:LauncherPosix.h>
  $<$<STREQUAL:${CORE_SYSTEM_NAME},osx>:LauncherPosix.h>
  $<$<STREQUAL:${CORE_SYSTEM_NAME},darwin_embedded>:LauncherPosix.h>
)

if(NOT ENABLE_STATIC_LIBS OR SOURCES)
  core_add_library(addons_interface_launcher)
endif()
