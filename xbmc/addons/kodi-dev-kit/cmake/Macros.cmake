include(../../../../../cmake/scripts/common/Macros.cmake)

macro(devkit_add_object name)
  add_library(${name} OBJECT ${SOURCES} ${HEADERS})
  set_target_properties(${name} PROPERTIES C_VISIBILITY_PRESET hidden
                                           CXX_VISIBILITY_PRESET hidden
                                           VISIBILITY_INLINES_HIDDEN ON
                                           POSITION_INDEPENDENT_CODE ON)
  set(DEVKIT_OBJECTS $<TARGET_OBJECTS:${name}> ${DEVKIT_OBJECTS} PARENT_SCOPE)
endmacro()

#
# Removes the specified compile flag from the specified target.
#   _target     - The target to remove the compile flag from
#   _flag       - The compile flag to remove
#
# Pre: apply_global_cxx_flags_to_all_targets() must be invoked.
#
macro(remove_flag_from_target _target _flag)
  get_target_property(_target_cxx_flags ${_target} COMPILE_OPTIONS)
  if(_target_cxx_flags)
    list(REMOVE_ITEM _target_cxx_flags ${_flag})
    set_target_properties(${_target} PROPERTIES COMPILE_OPTIONS "${_target_cxx_flags}")
  endif()
endmacro()
