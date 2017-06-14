# CPack supports component based installation, and handles dependencies between components.
# But, the dependencies passed to target_link_libraries() are not used to deduce component dependencies.
# For some other components, like data, translations, etc.. ,
# CMake has probably no way to deduce dependencies.
# This is why this module was created.

# TODO: should follow CPACK_PACKAGE_DIRECTORY or similar

if(UNIX)
  include(MdtCPackDeb)
endif()

# Add a Mdt library component to CPack
#
# This function will populate following variable:
#  - CPACK_COMPONENT_<compName>_DEPENDS
#
# Input arguments:
#  COMPONENT:
#   Component to add.
#  DEPEND_TARGETS (optionnal):
#   List of targets, on which COMPONENT depends.
#   Will only add dependencies to component of Mdt project.
#  DEPEND_TARGETS_SUFFIX (optional):
#   Suffix to add for each TARGET in DEPEND_TARGETS when searching on which COMPONENT it depends.
#  EXPLICIT_DEPEND_COMPONENTS (optionnal):
#   Explicit list of component on which COMPONENT depends.
#
# Typical usage for a binary library:
#  mdt_cpack_add_library_component(
#    COMPONENT Sql
#    DEPEND_TARGETS Application Error
#  )
#
# Typical usage for a dev library:
#  mdt_cpack_add_library_component(
#    COMPONENT Sql-dev
#    DEPEND_TARGETS Application Error
#    DEPEND_TARGETS_SUFFIX dev
#    EXPLICIT_DEPEND_COMPONENTS Sql
#  )
#
# Note: currently, deducing dependencies only works as long as TARGET and COMPONENT are named equally.
#       For example, the library Sql has target name Sql and will be installed as Sql component:
#       install(TARGETS Sql COMPONENT Sql)
#       It could be fine to use some functions to really find on which components a target depends on,
#       but I did not find any documentation about this.
function(mdt_cpack_add_library_component)
  # Parse arguments
  set(oneValueArgs COMPONENT DEPEND_TARGETS_SUFFIX)
  set(multiValueArgs DEPEND_TARGETS EXPLICIT_DEPEND_COMPONENTS)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(component ${VAR_COMPONENT})
  if(NOT component)
    message(FATAL_ERROR "COMPONENT argument is missing.")
  endif()
  set(depend_targets ${VAR_DEPEND_TARGETS})
  set(depend_targets_suffix ${VAR_DEPEND_TARGETS_SUFFIX})
  set(explicit_depend_components ${VAR_EXPLICIT_DEPEND_COMPONENTS})
  # Set dependencies
  mdt_cpack_target_set_internal_component_dependencies_by_target_names(
    COMPONENT ${component}
    DEPEND_TARGETS ${depend_targets}
    DEPEND_TARGETS_SUFFIX ${depend_targets_suffix}
    EXPLICIT_DEPEND_COMPONENTS ${explicit_depend_components}
  )
  # Set component description
  set(CPACK_COMPONENT_${component}_DESCRIPTION "Component description"  CACHE INTERNAL "")
  # Add to plateform specific CPack modules
  if(UNIX)
    mdt_cpack_deb_add_library_component(
      COMPONENT ${component}
    )
  endif()
endfunction()


# Add a Mdt library component to CPack
#
# This function will populate following variable:
#  - CPACK_COMPONENT_<compName>_DEPENDS
#
# Input arguments:
#  COMPONENT:
#   Component to add.
#  DEPEND_COMPONENTS:
#   List of component on which COMPONENT depends.
# function(mdt_cpack_add_library_component)
#   # Parse arguments
#   set(oneValueArgs COMPONENT)
#   set(multiValueArgs DEPEND_COMPONENTS)
#   cmake_parse_arguments(VAR "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
#   # Set our local variables and check the mandatory ones
#   set(component ${VAR_COMPONENT})
#   if(NOT component)
#     message(FATAL_ERROR "COMPONENT argument is missing.")
#   endif()
#   set(depend_components ${VAR_DEPEND_COMPONENTS})
#   # Set CPack variables
#   mdt_cpack_target_set_internal_component_dependencies(
#     COMPONENT ${component}
#     DEPEND_COMPONENTS ${depend_components}
#   )
# endfunction()

# Set Mdt internal component dependencies for given component name
#
# This function will populate CPACK_COMPONENT_<compName>_DEPENDS
#
# Input arguments:
#  COMPONENT:
#   Component to which dependencies must be set.
#  DEPEND_COMPONENTS:
#   List of component on which COMPONENT depends.
function(mdt_cpack_target_set_internal_component_dependencies)
  # Parse arguments
  set(oneValueArgs COMPONENT)
  set(multiValueArgs DEPEND_COMPONENTS)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(component ${VAR_COMPONENT})
  if(NOT component)
    message(FATAL_ERROR "COMPONENT argument is missing.")
  endif()
  set(depend_components ${VAR_DEPEND_COMPONENTS})
  # Set CPACK's variable
  set(CPACK_COMPONENT_${component}_DEPENDS ${depend_components} CACHE INTERNAL "")
endfunction()


# Set Mdt internal component dependencies for given component name
#
# This function will populate CPACK_COMPONENT_<compName>_DEPENDS
#
# Input arguments:
#  COMPONENT:
#   Component to which dependencies must be set.
#  DEPEND_TARGETS:
#   List of targets, on which COMPONENT depends.
#   Will only add dependencies to component of Mdt project.
#  DEPEND_TARGETS_SUFFIX (optional):
#   Suffix to add for each TARGET in DEPEND_TARGETS when searching on which COMPONENT it deoends.
#  EXPLICIT_DEPEND_COMPONENTS (optionnal):
#   Explicit list of component on which COMPONENT depends.
#
# Typical usage for a binary library:
#  mdt_cpack_target_set_internal_component_dependencies_by_target_names(
#    COMPONENT Sql
#    DEPEND_TARGETS Application Error
#  )
#
# Typical usage for a dev library:
#  mdt_cpack_target_set_internal_component_dependencies_by_target_names(
#    COMPONENT Sql-dev
#    DEPEND_TARGETS Application Error
#    DEPEND_TARGETS_SUFFIX dev
#    EXPLICIT_DEPEND_COMPONENTS Sql
#  )
function(mdt_cpack_target_set_internal_component_dependencies_by_target_names)
  # Parse arguments
  set(oneValueArgs COMPONENT DEPEND_TARGETS_SUFFIX)
  set(multiValueArgs DEPEND_TARGETS EXPLICIT_DEPEND_COMPONENTS)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(component ${VAR_COMPONENT})
  if(NOT component)
    message(FATAL_ERROR "COMPONENT argument is missing.")
  endif()
  set(depend_targets ${VAR_DEPEND_TARGETS})
  set(depend_targets_suffix ${VAR_DEPEND_TARGETS_SUFFIX})
  set(explicit_depend_components ${VAR_EXPLICIT_DEPEND_COMPONENTS})
  # Build list of targets on which component depends
  set(dep_targets)
  if(depend_targets_suffix)
    foreach(target ${depend_targets})
      list(APPEND dep_targets ${target}-${depend_targets_suffix})
    endforeach()
  else()
    set(dep_targets ${depend_targets})
  endif()
  # Find component for each depend target
  set(depend_components)
  list(APPEND depend_components ${explicit_depend_components})
  get_cmake_property(comp_list COMPONENTS)
  foreach(dep_target ${dep_targets})
    # Search the component on which dep_target belongs
    foreach(comp ${comp_list})
      foreach(comp_target ${comp})
        if(${dep_target} STREQUAL ${comp_target})
          list(APPEND depend_components ${comp})
        endif()
      endforeach()
    endforeach()
  endforeach()
  # Set CPack's variable
  mdt_cpack_target_set_internal_component_dependencies(
    COMPONENT ${component}
    DEPEND_COMPONENTS ${depend_components}
  )
endfunction()
