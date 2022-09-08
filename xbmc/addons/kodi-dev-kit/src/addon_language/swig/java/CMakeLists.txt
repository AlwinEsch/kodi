find_package(Java)
find_package(JNI)

if(NOT Java_FOUND)
  message(WARNING "Java package not found, Java Support not possible to create!")
  return()
elseif(NOT JNI_FOUND)
  message(WARNING "JNI package not found, Java Support not possible to create!")
  return()
else()
  message(STATUS "Java support becomes included")
endif()

include(UseJava)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/manifest.txt.in
               ${CMAKE_CURRENT_BINARY_DIR}/manifest.txt)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pom.xml.in
               ${CMAKE_CURRENT_BINARY_DIR}/kodidevkit-${VERSION_SHORT}.pom)

if (PERIPHERALMAN)
  # Override the JNI include directory for Android Things since these
  # will require an Android-friendly jni.h
  set(JAVA_INCLUDE_PATH "${ANDROID_SYSROOT}/usr/include")
endif()

include_directories (
  ${JAVA_INCLUDE_PATH}
  ${JAVA_INCLUDE_PATH2}
  ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# SWIG treats SWIG_FLAGS as a list and not a string so semicolon seperation is required
set_source_files_properties(
    ${PROJECT_SOURCE_DIR}/swig/java/kodi_addon.i
    PROPERTIES SWIG_FLAGS ";-package;kodi;-I${CMAKE_BINARY_DIR}/src"
               CPLUSPLUS ON
)
set_source_files_properties(
    ${PROJECT_SOURCE_DIR}/swig/java/kodi_addon.i
    ${PROJECT_SOURCE_DIR}/swig/kodi_addon.i
    ${PROJECT_SOURCE_DIR}/swig/global.i
    PROPERTIES CPLUSPLUS ON
)
set_source_files_properties(
    ${PROJECT_SOURCE_DIR}/swig/java/kodi_gui_dialogs_TextViewer.i
    ${PROJECT_SOURCE_DIR}/swig/java/kodi_gui_dialogs_YesNo.i
    PROPERTIES SWIG_FLAGS ";-package;kodi.gui.dialogs;-I${CMAKE_BINARY_DIR}/src"
               CPLUSPLUS ON
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -DJAVACALLBACK")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DJAVACALLBACK")

if (NOT DEFINED ENV{JAVA_HOME_NATIVE})
  if (NOT DEFINED ENV{JAVA_HOME})
#    message (WARNING "Neither JAVA_HOME nor JAVA_HOME_NATIVE are set, falling back to '/usr' path")
    set(JAVA_HOME "/usr")

    set(JAVA_HOME_NATIVE $ENV{JAVA_HOME})
    set(JAVAC ${JAVA_HOME}/bin/javac)
    set(JAR ${JAVA_HOME}/bin/jar)
  else()
    set(JAVA_HOME_NATIVE $ENV{JAVA_HOME})
    set(JAVAC $ENV{JAVA_HOME}/bin/javac)
    set(JAR $ENV{JAVA_HOME}/bin/jar)
  endif()
else ()
  set (JAVAC $ENV{JAVA_HOME_NATIVE}/bin/javac)
  set (JAR $ENV{JAVA_HOME_NATIVE}/bin/jar)
endif ()

cmake_parse_arguments(arg "WRAPPED" "DEVKIT_OUTPUT_DIRECTORY" "" ${ARGN})
if(arg_DEVKIT_OUTPUT_DIRECTORY)
  set(DEVKIT_OUTPUT_DIRECTORY ${arg_DEVKIT_OUTPUT_DIRECTORY})
else()
  set(DEVKIT_OUTPUT_DIRECTORY "addons/kodi.binary.devkit")
endif()

#if(CORE_SYSTEM_NAME STREQUAL windows)
  set(OUTPUT_NAME ${CMAKE_SHARED_LIBRARY_PREFIX}kodidevkit-java)
#else()
  #set(OUTPUT_NAME ${CMAKE_SHARED_LIBRARY_PREFIX}kodidevkit-java-${ARCH})
#endif()

swig_add_library(
    kodidevkit-java-base
    TYPE OBJECT
    LANGUAGE java
    NO_PROXY
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
    SOURCES ${PROJECT_SOURCE_DIR}/swig/java/kodi_addon.i
)
set_property(TARGET kodidevkit-java-base PROPERTY POSITION_INDEPENDENT_CODE ON)
swig_add_library(
    kodidevkit-java-gui-dialogs
    TYPE OBJECT
    LANGUAGE java
    NO_PROXY
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/gui/dialogs
    SOURCES ${PROJECT_SOURCE_DIR}/swig/java/kodi_gui_dialogs_TextViewer.i
            ${PROJECT_SOURCE_DIR}/swig/java/kodi_gui_dialogs_YesNo.i
)
set_property(TARGET kodidevkit-java-gui-dialogs PROPERTY POSITION_INDEPENDENT_CODE ON)

add_library(kodidevkit-java SHARED $<TARGET_OBJECTS:kodidevkit-java-base>
                                   $<TARGET_OBJECTS:kodidevkit-java-gui-dialogs>)

#swig_link_libraries(kodidevkit-java kodidevkit dl ${JAVA_LIBRARIES})
set_target_properties(kodidevkit-java PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${DEVKIT_OUTPUT_DIRECTORY}/java
                                                 RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${DEVKIT_OUTPUT_DIRECTORY}/java
                                                 OUTPUT_NAME ${OUTPUT_NAME} PREFIX "")


add_custom_command(TARGET kodidevkit-java
  POST_BUILD
  COMMAND cmake -E echo "Compiling java.."
  COMMAND ${JAVAC} *.java gui/dialogs/*.java -d ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND cmake -E echo "Creating jar"
  COMMAND ${JAR} cvmf manifest.txt kodidevkit.jar kodi/*
  COMMAND cmake -E copy ${CMAKE_CURRENT_BINARY_DIR}/kodidevkit.jar ${CMAKE_BINARY_DIR}/${DEVKIT_OUTPUT_DIRECTORY}/java
  COMMAND cmake -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/kodi ${CMAKE_BINARY_DIR}/${DEVKIT_OUTPUT_DIRECTORY}/java/kodi
)

#kodidevkit_create_install_pkgconfig (kodijava.pc ${LIB_INSTALL_DIR}/pkgconfig)

#install (TARGETS kodijava LIBRARY DESTINATION ${LIB_INSTALL_DIR})
#install (FILES ${CMAKE_CURRENT_BINARY_DIR}/kodidevkit.jar DESTINATION ${LIB_INSTALL_DIR}/../lib/java)
