#.rst:
# FindVulkanMemoryAllocator
# --------------------------
# Finds the Vulkan Memory Allocator library
#
# This will define the following target:
#
#   ${APP_NAME_LC}::VulkanMemoryAllocator - The Vulkan Memory Allocator IMPORTED library

if(NOT TARGET ${APP_NAME_LC}::${CMAKE_FIND_PACKAGE_NAME})
  find_path(VULKAN_MEMORY_ALLOCATOR_INCLUDE_DIR NAMES vk_mem_alloc.h
                                                HINTS ${DEPENDS_PATH}/include)

  # Create imported target ${APP_NAME_LC}::${CMAKE_FIND_PACKAGE_NAME}
  add_library(${APP_NAME_LC}::${CMAKE_FIND_PACKAGE_NAME} INTERFACE IMPORTED)
  set_target_properties(${APP_NAME_LC}::${CMAKE_FIND_PACKAGE_NAME} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${VULKAN_MEMORY_ALLOCATOR_INCLUDE_DIR}"
  )
endif()
