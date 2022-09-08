set_source_files_properties (kodidevkit3.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties (kodidevkit3.i PROPERTIES SWIG_FLAGS "-I${CMAKE_BINARY_DIR}/src")

message ("PYTHON3 attempting to build!")

if (CMAKE_VERSION VERSION_LESS "3.8")
  swig_add_module (python3-kodidevkit python kodidevkit3.i)
else ()
  swig_add_library (python3-kodidevkit LANGUAGE python SOURCES kodidevkit3.i)
endif ()
swig_link_libraries (python3-kodidevkit ${PYTHON3_LIBRARIES} kodidevkit)

target_include_directories(${SWIG_MODULE_python3-kodidevkit_REAL_NAME}
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/../.."
  "${PYTHON3_INCLUDE_DIR}"
)

set_target_properties (${SWIG_MODULE_python3-kodidevkit_REAL_NAME} PROPERTIES
  OUTPUT_NAME _kodi
  COMPILE_FLAGS "${CMAKE_C_FLAGS} -DSWIGPYTHON=${SWIG_FOUND} -DSWIGPYTHON3=${SWIG_FOUND}"
)

install (TARGETS ${SWIG_MODULE_python3-kodidevkit_REAL_NAME}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${PYTHON3_PACKAGES_PATH}
)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/kodidevkit.py
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${PYTHON3_PACKAGES_PATH}
)
