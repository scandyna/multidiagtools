include(GetPrerequisites)

# Copy dependencies of a binary file to a directory
#
# Input arguments:
#  BINARY_FILE:
#   Full path to a library or a executable
#  SEARCH_DIRECTORIES:
#   List of full path to directories where to find dependencies
#  DEP_TOOL (optionnal):
#   Tool to get dependencies.
#   Only use when cross-compiling
#  DESTINATION_DIRECTORY:
#   Full path to the destination directory
#
function(mdt_copy_binary_dependencies)
  # Parse arguments
  set(oneValueArgs BINARY_FILE DEP_TOOL DESTINATION_DIRECTORY)
  set(multiValueArgs SEARCH_DIRECTORIES)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  # Set our local variables and check the mandatory ones
#   set(binary_file "${VAR_BINARY_FILE}")
  string(REGEX REPLACE "^\"|\"$" "" binary_file "${VAR_BINARY_FILE}")
  if(NOT binary_file)
    message(FATAL_ERROR "mdt_copy_binary_dependencies(): BINARY_FILE argument is missing.")
  endif()
  set(search_directories "${VAR_SEARCH_DIRECTORIES}")
  if(NOT search_directories)
    message(FATAL_ERROR "mdt_copy_binary_dependencies(): SEARCH_DIRECTORIES argument is missing.")
  endif()
  set(destination_directory "${VAR_DESTINATION_DIRECTORY}")
  if(NOT destination_directory)
    message(FATAL_ERROR "mdt_copy_binary_dependencies(): DESTINATION_DIRECTORY argument is missing.")
  endif()
  

  # Get dependencies
  message(STATUS "Searching dependencies for ${binary_file}")
  get_filename_component(binary_file_directory "${binary_file}" DIRECTORY)
  message("binary_file_directory: ${binary_file_directory}")
  if(VAR_DEP_TOOL)
    set(gp_tool ${VAR_DEP_TOOL})
  endif()
  message("== ACT gp_tool: ${gp_tool}")
  message("== ACT search_directories: ${search_directories}")
  set(dependencies "")
  get_prerequisites("${binary_file}" dependencies 0 0 "${binary_file_directory}" ${search_directories})
  
  message("dependencies: ${dependencies}")
  
  # Remove dependencies that are system libraries
  list(REMOVE_ITEM dependencies "KERNEL32.dll" "msvcrt.dll")
  # Copy dependencies
  set(dependencies_files)
  foreach(file ${dependencies})
    message("  file: ${file}")
    gp_resolve_item("${binary_file}" "${file}" "" "${search_directories}" file_path)
    message("   file_path: ${file_path}")
    list(APPEND dependencies_files "${file_path}")
  endforeach()
  file(COPY ${dependencies_files} DESTINATION "${destination_directory}")

#   list_prerequisites("${binary_file}" 1)
  
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

  message("== ACT Main BINARY_FILE: ${binary_file}")
  message("== ACT Main DESTINATION_DIRECTORY: ${destination_directory}")
  message("== ACT Main SEARCH_DIRECTORIES_FILE: ${search_directories_file}")
  message("== ACT Main DEP_TOOL: ${DEP_TOOL}")
  
  # Build the list of search directories
  file(READ "${search_directories_file}" search_directories)
  
  mdt_copy_binary_dependencies(
    BINARY_FILE "${binary_file}"
    SEARCH_DIRECTORIES ${search_directories}
    DEP_TOOL ${DEP_TOOL}
    DESTINATION_DIRECTORY "${destination_directory}"
  )
else()
  message(FATAL_ERROR "Unknown action: ${ACTION}")
endif()

message("------ BINARY_FILE: ${BINARY_FILE}")
