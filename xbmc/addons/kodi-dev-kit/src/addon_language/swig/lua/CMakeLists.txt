find_package(Lua)

if(NOT Lua_FOUND)
  message(WARNING "Lua Support not possible to create!")
  return()
else()
  message(STATUS "Lus support becomes included")
endif()

set_source_files_properties (kodidevkitlua.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties (kodidevkitlua.i PROPERTIES SWIG_FLAGS "-I${CMAKE_BINARY_DIR}/src")

swig_add_library (kodi-lua LANGUAGE lua SOURCES kodidevkitlua.i)
swig_link_libraries (kodi-lua ${LUA_LIBRARIES} kodidevkit)

target_include_directories(${SWIG_MODULE_kodi-lua_REAL_NAME}
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/../.."
  "${LUA_INCLUDE_DIR}"
)
