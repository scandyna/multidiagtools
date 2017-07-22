
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


# Create a rule to install dependencies of a binary target
#
# This can be usefull to create packages which also contains
# libraries given target depends on.
#
# Input arguments:
#  TARGET:
#   Name of the target for which to install dependencies
#
function(mdt_install_binary_dependencies)
  # Parse arguments
  set(oneValueArgs TARGET)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(target "${VAR_TARGET}")
  if(NOT target)
    message(FATAL_ERROR "mdt_install_binary_dependencies(): TARGET argument is missing.")
  endif()
  # Create a custom target that depends on target and that will put dependencies in a directory
  mdt_copy_binary_dependencies(
    TARGET ${target}
    DESTINATION_DIRECTORY "${CMAKE_BINARY_DIR}/MdtBinaryDependencies"
  )
  # Create a install rule for this new target
  install(
    DIRECTORY "${CMAKE_BINARY_DIR}/MdtBinaryDependencies"
    DESTINATION bin
  )
endfunction()

# Copy dependencies of a binary target to a directory
#
# This can be used to copy needed dependencies to run unit test
# on systems that not support RPATH (f.ex. Windows).
#
# Note that the copy is not done when this function is called.
# Behind the scene, a target that depend on TARGET is created,
# so the dependnecies are only evaluated after TARGET was built.
#
# Input arguments:
#  TARGET:
#   Name of the target for which to copy dependencies
#  DESTINATION_DIRECTORY:
#   Full path to the destination directory
#  NO_WARNINGS:
#   If this option is passed, no warnings will be emitted.
#   (Most of the warnings are actually about cross-compiling)
#
function(mdt_copy_binary_dependencies)
  # Parse arguments
  set(oneValueArgs TARGET DESTINATION_DIRECTORY)
  set(options NO_WARNINGS)
  cmake_parse_arguments(VAR "${options}" "${oneValueArgs}" "" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(target "${VAR_TARGET}")
  if(NOT target)
    message(FATAL_ERROR "mdt_copy_binary_dependencies(): TARGET argument is missing.")
  endif()
  set(destination_directory "${VAR_DESTINATION_DIRECTORY}")
  if(NOT destination_directory)
    message(FATAL_ERROR "mdt_copy_binary_dependencies(): DESTINATION_DIRECTORY argument is missing.")
  endif()
  set(no_warnings "${VAR_NO_WARNINGS}")
  # Check if we can resolve recusrively
  # If we are cross-compiling, typically on a Linux system -> Windows,
  # some system libraries, like KERNEL32.dll, are not available.
  set(resolve_recursive 1)
  if(CMAKE_CROSSCOMPILING)
    if(NOT no_warnings)
      message(WARNING "It seems that you are do cross-compilation. It will not be possible to resolve all dependencies. Please use a other tool to add missing ones.")
    endif()
    set(resolve_recursive 0)
  endif()
  # Build the list of directories into which to serach
  # If we have a CMAKE_PREFIX_PATH we do some guesses on how it could be organized
  set(search_directories)
  foreach(path ${CMAKE_PREFIX_PATH})
    list(APPEND search_directories "${path}")
    list(APPEND search_directories "${path}/bin")
    # Cross-compilation with MXE
    list(APPEND search_directories "${path}/qt5/bin")
  endforeach()
  # Directories to resolve libraries of current project
  if(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    list(APPEND search_directories "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
  endif()
  # Passing a list of elements to a COMMAND argument is a problem.
  # So, lets write our search directories to a file, and tell the path to this file
  set(search_directories_file "${CMAKE_BINARY_DIR}/MdtBinDepsSearchDirectories")
  file(WRITE "${search_directories_file}" "${search_directories}")
  # If we are cross-compiling from Linux to Windows, we must sepcify the tool to obtain dependencies
  if(CMAKE_CROSSCOMPILING AND MINGW)
    set(dep_tool objdump)
  endif()
  # Find our actions file
  # For some misterious reason, find_file() and find_path() did never find the file.
  # So, lets do it by hand
  set(actions_file "")
  if(NOT CMAKE_MODULE_PATH)
    if(NOT no_warnings)
      message(WARNING "CMAKE_MODULE_PATH was not set, and is required to find MdtDependenciesUtilsActions.cmake. Resolving dependencies will not be done.")
    endif()
    return()
  endif()
  foreach(path ${CMAKE_MODULE_PATH})
    if(EXISTS "${path}/MdtDependenciesUtilsActions.cmake")
      set(actions_file "${path}/MdtDependenciesUtilsActions.cmake")
      break()
    endif()
  endforeach()
  if(NOT actions_file)
    if(NOT no_warnings)
      message(WARNING "Could not find MdtDependenciesUtilsActions.cmake. Resolving dependencies will not be done.")
    endif()
    return()
  endif()
  # Create a new target that depends on TARGET
  add_custom_target(
    ${target}_bin_deps
    ALL
    COMMAND "${CMAKE_COMMAND}"
            -D ACTION=mdt_copy_binary_dependencies -D BINARY_FILE="$<TARGET_FILE:${target}>"
            -D DESTINATION_DIRECTORY="${destination_directory}"
            -D SEARCH_DIRECTORIES_FILE="${search_directories_file}"
            -D RESOLVE_RECURSIVE=${resolve_recursive}
            -D DEP_TOOL=${dep_tool}
            -P "${actions_file}"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    VERBATIM
  )
  add_dependencies(${target}_bin_deps ${target})
endfunction()

# Find and copy a library
#
# Use this function to copy files that have not be detected
#
# This is specially needed for Windows,
# to copy the libraries to the test runtime directory.
#
# Input arguments:
#  LIBRARIES:
#   A list of libraries
#  DESTINATION_DIRECTORY:
#   Full path to the destination directory
#
# Example:
#  mdt_find_and_copy_libraries(
#    LIBRARIES libstdc++-6.dll libwinpthread-1.dll
#    DESTINATION_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
#  )
#
function(mdt_find_and_copy_libraries)
  # Parse arguments
  set(oneValueArgs DESTINATION_DIRECTORY)
  set(multiValueArgs LIBRARIES)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(libraries ${VAR_LIBRARIES})
  if(NOT libraries)
    message(FATAL_ERROR "mdt_find_and_copy_libraries(): LIBRARIES argument is missing.")
  endif()
  set(destination_directory ${VAR_DESTINATION_DIRECTORY})
  if(NOT destination_directory)
    message(FATAL_ERROR "mdt_find_and_copy_libraries(): DESTINATION_DIRECTORY argument is missing.")
  endif()
  # On Windows, we need to find dll libraries, not .a or .dll.a
  # Setting CMAKE_FIND_LIBRARY_SUFFIXES does not fix the problem.
  # So, we tell CMake to only search in bin directories, not lib
  set(search_directories "")
  if(WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .dll)
    foreach(directory ${CMAKE_FIND_ROOT_PATH})
      list(APPEND search_directories "${directory}/bin")
      list(APPEND search_directories "${directory}/qt5/bin")
    endforeach()
    foreach(directory ${CMAKE_PREFIX_PATH})
      list(APPEND search_directories "${directory}/bin")
      list(APPEND search_directories "${directory}/qt5/bin")
    endforeach()
  else()
    list(APPEND search_directories ${CMAKE_FIND_ROOT_PATH})
    list(APPEND search_directories ${CMAKE_PREFIX_PATH})
  endif()
  # Find libraries
  set(libraries_files "")
  foreach(library ${libraries})
    message(STATUS "Searching ${library}")
    find_library(${library}_file NAMES ${library} PATHS ${search_directories} NO_DEFAULT_PATH)
    if(NOT ${library}_file)
      message(WARNING "Could not find library ${library}")
    else()
      list(APPEND libraries_files "${${library}_file}")
    endif()
  endforeach()
  if(libraries_files)
    file(COPY ${libraries_files} DESTINATION "${destination_directory}")
  endif()
endfunction()
