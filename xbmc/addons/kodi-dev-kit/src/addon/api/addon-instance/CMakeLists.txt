# Auto generated CMakeLists.txt.
# See xbmc/addons/kodi-dev-kit/tools/code-generator.py.

set(SOURCES
  audiodecoder.cpp
  audioencoder.cpp
  game.cpp
  imagedecoder.cpp
  inputstream.cpp
  peripheral.cpp
  pvr.cpp
  screensaver.cpp
  vfs.cpp
  videocodec.cpp
  visualization.cpp
  web.cpp
)

set(HEADERS
  audiodecoder.h
  audioencoder.h
  game.h
  imagedecoder.h
  inputstream.h
  peripheral.h
  pvr.h
  screensaver.h
  vfs.h
  videocodec.h
  visualization.h
  web.h
)

if(SOURCES OR HEADERS)
  devkit_add_object(devkit_api_addon-instance)
endif()
