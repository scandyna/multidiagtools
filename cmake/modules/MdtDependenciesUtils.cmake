
# TODO: function to list all targets of current project (probably using components)

# Get a list of all targets for current project
#
# Output arguments:
#  TARGET_LIST:
#   Will contain the list of all targets.
function(mdt_get_target_list TARGET_LIST)
  set(tgt_list)
  get_cmake_property(comp_list COMPONENTS)
  foreach(component ${comp_list})
    foreach(component_item ${component})
      if(TARGET ${component_item})
        list(APPEND tgt_list ${component_item})
      endif()
    endforeach()
  endforeach()
  set(${TARGET_LIST} ${tgt_list} PARENT_SCOPE)
endfunction()


# TODO: function to compare targets

# Get Mdt dependencies for a library
#
# Input arguments:
#  DEP_TARGETS:
#   List of dependent targets, without any prefix (like project name) or suffix (like version).
#   Each target name in this list is the same name that has been passed to mdt_add_library()
# Output arguments:
#  MDT_DEPENDENCIES:
#   Will contain the list of dependencies that are internal to Mdt
#
# For example:
#  mdt_get_library_internal_dependencies(
#    MDT_DEP_TARGETS
#    DEP_TARGETS Application Algoritm Qt5::Widgets
#  )
#  MDT_DEP_TARGETS will be a list that contains Application Algoritm
function(mdt_get_library_internal_dependencies MDT_DEPENDENCIES)
  # Parse arguments
  set(multiValueArgs DEP_TARGETS)
  cmake_parse_arguments(VAR "" "" "${multiValueArgs}" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(dep_targets ${VAR_DEP_TARGETS})
  if(NOT dep_targets)
    return()
  endif()
  # Get the list of currently existing targets
  mdt_get_target_list(existing_mdt_target_list)
  # Add each depend target that is a Mdt target to the list
  set(mdt_dep_targets)
  foreach(target ${dep_targets})
    list(FIND existing_mdt_target_list ${target} list_index)
    if(${list_index} GREATER -1)
      list(APPEND mdt_dep_targets ${target})
    endif()
  endforeach()
  set(${MDT_DEPENDENCIES} ${mdt_dep_targets} PARENT_SCOPE)
endfunction()
