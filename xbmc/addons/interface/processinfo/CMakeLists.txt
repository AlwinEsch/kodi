# Auto generated CMakeLists.txt.
# See xbmc/addons/kodi-dev-kit/tools/code-generator.py.

set(SOURCES
  $<$<STREQUAL:${CORE_SYSTEM_NAME},android>:ProcessInfoAndroid.cpp>
  $<$<STREQUAL:${CORE_SYSTEM_NAME},darwin_embedded>:ProcessInfoDarwinEmbedded.cpp>
  $<$<STREQUAL:${CORE_SYSTEM_NAME},freebsd>:ProcessInfoFreeBSD.cpp>
  $<$<STREQUAL:${CORE_SYSTEM_NAME},linux>:ProcessInfoLinux.cpp>
  $<$<STREQUAL:${CORE_SYSTEM_NAME},osx>:ProcessInfoOSX.cpp>
  $<$<STREQUAL:${CORE_SYSTEM_NAME},windows>:ProcessInfoWindows.cpp>
  $<$<STREQUAL:${CORE_SYSTEM_NAME},windowsstore>:ProcessInfoWindowsStore.cpp>
)

set(HEADERS
  ProcessInfo.h
  $<$<STREQUAL:${CORE_SYSTEM_NAME},android>:ProcessInfoAndroid.h>
  $<$<STREQUAL:${CORE_SYSTEM_NAME},darwin_embedded>:ProcessInfoDarwinEmbedded.h>
  $<$<STREQUAL:${CORE_SYSTEM_NAME},freebsd>:ProcessInfoFreeBSD.h>
  $<$<STREQUAL:${CORE_SYSTEM_NAME},linux>:ProcessInfoLinux.h>
  $<$<STREQUAL:${CORE_SYSTEM_NAME},osx>:ProcessInfoOSX.h>
  $<$<STREQUAL:${CORE_SYSTEM_NAME},windows>:ProcessInfoWindows.h>
  $<$<STREQUAL:${CORE_SYSTEM_NAME},windowsstore>:ProcessInfoWindowsStore.h>
)

if(NOT ENABLE_STATIC_LIBS OR SOURCES)
  core_add_library(addons_interface_processinfo)
endif()
