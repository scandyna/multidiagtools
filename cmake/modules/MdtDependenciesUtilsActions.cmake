include(GetPrerequisites)

# Copy dependencies of a binary file to a directory
#
# Input arguments:
#  BINARY_FILE:
#   Full path to a library or a executable
#  SEARCH_DIRECTORIES (optionnal):
#   List of full path to directories where to find dependencies
#  RESOLVE_RECURSIVE:
#   Tell if resolving must be done recursively.
#   When do cross-compilation, should be 0
#  DEP_TOOL (optionnal):
#   Tool to get dependencies.
#   Only use when cross-compiling
#  DESTINATION_DIRECTORY:
#   Full path to the destination directory
#
function(mdt_copy_binary_dependencies)
  # Parse arguments
  set(oneValueArgs BINARY_FILE RESOLVE_RECURSIVE DEP_TOOL DESTINATION_DIRECTORY)
  set(multiValueArgs SEARCH_DIRECTORIES)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(binary_file "${VAR_BINARY_FILE}")
  if(NOT binary_file)
    message(FATAL_ERROR "mdt_copy_binary_dependencies(): BINARY_FILE argument is missing.")
  endif()
  set(resolve_recursive "${VAR_RESOLVE_RECURSIVE}")
  set(search_directories "${VAR_SEARCH_DIRECTORIES}")
#   if(NOT search_directories)
#     message(FATAL_ERROR "mdt_copy_binary_dependencies(): SEARCH_DIRECTORIES argument is missing.")
#   endif()
  set(destination_directory "${VAR_DESTINATION_DIRECTORY}")
  if(NOT destination_directory)
    message(FATAL_ERROR "mdt_copy_binary_dependencies(): DESTINATION_DIRECTORY argument is missing.")
  endif()
  message(STATUS "Searching dependencies for ${binary_file}")
  # Get dependencies
  get_filename_component(binary_file_directory "${binary_file}" DIRECTORY)
  if(VAR_DEP_TOOL)
    set(gp_tool ${VAR_DEP_TOOL})
  endif()
  set(dependencies "")
  get_prerequisites("${binary_file}" dependencies 0 ${resolve_recursive} "${binary_file_directory}" "${search_directories}")
  # Remove dependencies that are system libraries
  list(REMOVE_ITEM dependencies "KERNEL32.dll" "msvcrt.dll")
  # Copy dependencies
  set(dependencies_files)
  foreach(file ${dependencies})
    gp_resolve_item("${binary_file}" "${file}" "" "${search_directories}" file_path)
    list(APPEND dependencies_files "${file_path}")
  endforeach()
  file(COPY ${dependencies_files} DESTINATION "${destination_directory}")
endfunction()

# Main
#
# Input arguments:
#  ACTION:
#   The action to perform.
#   If ACTION is copy_binary_dependencies, mdt_copy_binary_dependencies() is called.
#  BINARY_FILE:
#   Full path to a library or a executable
#   This argument is required when ACTION is copy_binary_dependencies
#  DEP_TOOL (optionnal):
#   Tool to get dependencies.
#   Only use when cross-compiling
#  RESOLVE_RECURSIVE:
#   Tell if resolving must be done recursively.
#   When do cross-compilation, should be 0
#  SEARCH_DIRECTORIES_FILE:
#   File that contains a list of full path to directories where to find dependencies
#  DESTINATION_DIRECTORY:
#   Full path to the destination directory
#   This argument is required when ACTION is copy_binary_dependencies
if(NOT ACTION)
  message(FATAL_ERROR "ACTION argument is missing")
endif()
if(${ACTION} STREQUAL mdt_copy_binary_dependencies)
  if(NOT BINARY_FILE)
    message(FATAL_ERROR "For '${ACTION}' action, BINARY_FILE argument is required.")
  endif()
  if(NOT DESTINATION_DIRECTORY)
    message(FATAL_ERROR "For '${ACTION}' action, DESTINATION_DIRECTORY argument is required.")
  endif()
  if(NOT SEARCH_DIRECTORIES_FILE)
    message(FATAL_ERROR "For '${ACTION}' action, SEARCH_DIRECTORIES_FILE argument is required.")
  endif()
  # Remove quotes in arguments
  string(REGEX REPLACE "^\"|\"$" "" binary_file "${BINARY_FILE}")
  string(REGEX REPLACE "^\"|\"$" "" destination_directory "${DESTINATION_DIRECTORY}")
  string(REGEX REPLACE "^\"|\"$" "" search_directories_file "${SEARCH_DIRECTORIES_FILE}")
  # Build the list of search directories
  file(READ "${search_directories_file}" search_directories)

  mdt_copy_binary_dependencies(
    BINARY_FILE "${binary_file}"
    SEARCH_DIRECTORIES ${search_directories}
    RESOLVE_RECURSIVE ${RESOLVE_RECURSIVE}
    DEP_TOOL ${DEP_TOOL}
    DESTINATION_DIRECTORY "${destination_directory}"
  )
else()
  message(FATAL_ERROR "Unknown action: ${ACTION}")
endif()
