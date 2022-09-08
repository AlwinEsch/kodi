# Auto generated CMakeLists.txt.
# See xbmc/addons/kodi-dev-kit/tools/code-generator.py.

set(SOURCES
  _test_main.cpp
  rectrace.cpp
  select_signals.cpp
  stacktrace.cpp
  suicide.cpp
  test.cpp
)

set(HEADERS
  test.hpp
)

if(SOURCES OR HEADERS)
  devkit_add_object(devkit_third_party_backward-cpp_test)
endif()
