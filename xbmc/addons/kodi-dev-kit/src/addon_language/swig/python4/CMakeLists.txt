set_source_files_properties (kodidevkit4.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties (kodidevkit4.i PROPERTIES SWIG_FLAGS "-I${CMAKE_BINARY_DIR}/src")

message ("PYTHON4 attempting to build!")

if (CMAKE_VERSION VERSION_LESS "3.8")
  swig_add_module (python4-kodidevkit python kodidevkit4.i)
else ()
  swig_add_library (python4-kodidevkit LANGUAGE python SOURCES kodidevkit4.i)
endif ()
swig_link_libraries (python4-kodidevkit ${PYTHON4_LIBRARIES} kodidevkit)

target_include_directories(${SWIG_MODULE_python4-kodidevkit_REAL_NAME}
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/../.."
  "${PYTHON4_INCLUDE_DIR}"
)

set_target_properties (${SWIG_MODULE_python4-kodidevkit_REAL_NAME} PROPERTIES
  OUTPUT_NAME _kodi
  COMPILE_FLAGS "${CMAKE_C_FLAGS} -DSWIGPYTHON=${SWIG_FOUND} -DSWIGPYTHON4=${SWIG_FOUND}"
)

install (TARGETS ${SWIG_MODULE_python4-kodidevkit_REAL_NAME}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${PYTHON4_PACKAGES_PATH}
)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/kodidevkit.py
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${PYTHON4_PACKAGES_PATH}
)
