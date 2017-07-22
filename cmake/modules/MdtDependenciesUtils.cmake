
# Enable multi-arch installation
# For example, install libraries to /usr/lib/x86_64-linux-gnu on a x86_64 Debian
include(GNUInstallDirs)

# When getting a imported target LOCATION
# CMake fails on some libraries, like Threads,
# with a obscure message about whitelist and not allowed property LOCATION
set_property(
  GLOBAL
  PROPERTY MDT_GET_LOCATION_PROPERTY_NOT_ALLOWED_EXTERNAL_TARGETS
           Threads
           Threads::Threads
)

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

# TODO Add a QUIET, or W_QUIET, or NO_WARNINGS option !
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

# Register a external library file
#
# This will add LIBRARY_FILE to the global MDT_EXTERNAL_LIBRARIES_FILES variable
#
# Input arguments:
#  LIBRARY_FILE:
#   Full path to a external library file
#
function(mdt_register_external_library_file)
  # Parse arguments
  set(oneValueArgs LIBRARY_FILE)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(library_file "${VAR_LIBRARY_FILE}")
  if(NOT library_file)
    message(FATAL_ERROR "mdt_register_external_library_file(): LIBRARY_FILE argument is missing.")
  endif()
  # Create a local copy of the global variable to work on
  get_property(
    external_libraries_files
    GLOBAL
    PROPERTY MDT_EXTERNAL_LIBRARIES_FILES
  )
  # Add the file if not allready registered
  list(FIND external_libraries_files "${library_file}" list_index)
  if(${list_index} LESS 0)
    list(APPEND external_libraries_files "${library_file}")
  endif()
  # Update global variable
  set_property(
    GLOBAL
    PROPERTY MDT_EXTERNAL_LIBRARIES_FILES ${external_libraries_files}
  )
endfunction()

# Copy external libraries files to a directory
#
# Will copy files registered in MDT_EXTERNAL_LIBRARIES_FILES
# to the directory DESTINATION_DIRECTORY
#
# This is specially needed for Windows,
# to copy the libraries to the test runtime directory.
# On Linux, CMake take care of setting RPATH on compiled binaires
#
# Input arguments:
#  DESTINATION_DIRECTORY:
#   Full path to the destination directory
#
function(mdt_copy_external_libraries_files)
  # Parse arguments
  set(oneValueArgs DESTINATION_DIRECTORY)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(destination_directory ${VAR_DESTINATION_DIRECTORY})
  if(NOT destination_directory)
    message(FATAL_ERROR "mdt_copy_external_libraries_files(): DESTINATION_DIRECTORY argument is missing.")
  endif()
  # Create a local copy of the global variable to work with
  get_property(
    external_libraries_files
    GLOBAL
    PROPERTY MDT_EXTERNAL_LIBRARIES_FILES
  )
  # Copy each file
  foreach(file ${external_libraries_files})
    message("-- Copy ${file} -> ${destination_directory}")
  endforeach()
  file(COPY ${external_libraries_files} DESTINATION "${destination_directory}")
endfunction()


# TODO custom target to install dlls - Must depend on target (see translations and MdtCPackComponent.cmake for examples)
#      See GetPrerequisites CMake module
# TODO custem target to copy dlls into test directory

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
  message("search_directories: ${search_directories}")
  # Find libraries
  set(libraries_files "")
  message("*  libraries_files: ${libraries_files} *")
  
  foreach(library ${libraries})
    message("-- Searching ${library}")
#     set(${library}_file "")
    find_library(${library}_file NAMES ${library} PATHS ${search_directories} NO_DEFAULT_PATH)
    if(NOT ${library}_file)
      message(WARNING "Could not find library ${library}")
    else()
      message("-- Found: ${${library}_file}")
      list(APPEND libraries_files "${${library}_file}")
    endif()
  endforeach()
#   message("3) CMAKE_FIND_LIBRARY_SUFFIXES: ${CMAKE_FIND_LIBRARY_SUFFIXES}")
#   set(CMAKE_FIND_LIBRARY_SUFFIXES ${ORIGINAL_CMAKE_FIND_LIBRARY_SUFFIXES})
  # Copy the libraries
  message("** libraries_files: ${libraries_files} **")
  if(libraries_files)
    file(COPY ${libraries_files} DESTINATION "${destination_directory}")
  endif()
endfunction()


# Register external direct dependencies for a library
#
# Input arguments:
#  TARGET:
#   Name of the target, without any prefix (like project name) or suffix (like version).
#   Target name is the same name that has been passed to mdt_add_library()
#
# The external dependecies library files will be added with mdt_register_external_library_file()
#
# For example:
#  mdt_get_library_external_direct_dependencies(
#    TARGET Error
#  )
#
function(mdt_register_library_external_direct_dependencies)
  # Parse arguments
  set(oneValueArgs TARGET)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(target ${VAR_TARGET})
  if(NOT target)
    message(FATAL_ERROR "mdt_register_library_external_direct_dependencies(): TARGET argument is missing.")
  endif()
  # Get the list of currently existing targets
  mdt_get_target_list(existing_mdt_target_list)
  # Find the link dependencies fot TARGET and add those that are not
  set(external_dependencies)
  get_target_property(target_link_dep_list ${target} LINK_LIBRARIES)
  foreach(target_link_dep ${target_link_dep_list})
    # Add this link dependency if it is a external one
    list(FIND existing_mdt_target_list ${target_link_dep} list_index)
    if(${list_index} LESS 0)
      mdt_get_external_library_path(
        external_library_path
        IMPORTED_TARGET ${target_link_dep}
      )
      message("external_library_path: ${external_library_path}")
      if(external_library_path)
        mdt_register_external_library_file(
          LIBRARY_FILE "${external_library_path}"
        )
      endif()
    endif()
#     # Check if this external dependency is allready in the list
#     list(FIND external_dependencies ${target_link_dep} list_index)
#     if(${list_index} LESS 0)
#       message(" Try ${target_link_dep}")
#       # Add this link dependency if it is a external one
#       list(FIND existing_mdt_target_list ${target_link_dep} list_index_2)
#       if(${list_index_2} LESS 0)
#         message("  Add ${target_link_dep}")
#         list(APPEND external_dependencies ${target_link_dep})
#         
#         mdt_get_external_library_path(
#           libpath
#           IMPORTED_TARGET ${target_link_dep}
#         )
#         
#       endif()
#     endif()
  endforeach()
endfunction()

# Get the file path for a external library
#
# Input arguments:
#  IMPORTED_TARGET:
#   Name of the target, without any prefix (like project name) or suffix (like version).
# Output arguments:
#  LIBRARY_PATH:
#   Path to the library, or empty on failure
#
function(mdt_get_external_library_path LIBRARY_PATH)
  # Parse arguments
  set(oneValueArgs IMPORTED_TARGET)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(imported_target ${VAR_IMPORTED_TARGET})
  if(NOT imported_target)
    message(FATAL_ERROR "mdt_get_external_library_path(): IMPORTED_TARGET argument is missing.")
  endif()
  # Set library path
  set(library_path)
  get_property(
    not_allowed_targets
    GLOBAL
    PROPERTY MDT_GET_LOCATION_PROPERTY_NOT_ALLOWED_EXTERNAL_TARGETS
  )
  message("Not allowd: ${not_allowed_targets}")
  list(FIND not_allowed_targets ${imported_target} list_index)
  if(${list_index} LESS 0)
    get_target_property(library_path ${imported_target} LOCATION)
  endif()

  message("library_path: ${library_path}")
  
  set(${LIBRARY_PATH} ${library_path} PARENT_SCOPE)
endfunction()

# get_cmake_property(all_cmp COMPONENTS)
# foreach(cmp ${all_cmp})
#   message("Component: ${cmp}")
#   foreach(cmp_item ${cmp})
#     if(TARGET ${cmp_item})
#       message(" Target: ${cmp_item}")
#       get_target_property(dep_tgt_list ${cmp_item} LINK_LIBRARIES)
#       message("  Deps: ${dep_tgt_list}")
#       foreach(dep_tgt ${dep_tgt_list})
#         get_target_property(dep_dll ${dep_tgt} LOCATION)
#         message("   Dll: ${dep_dll}")
#       endforeach()
#     endif()
#   endforeach()
# endforeach()

# get_target_property(myp Qt5::Core LOCATION)
# message("Lib: ${myp}")

# Get all external direct dependencies for the current project
#
# Will get a list of all existing targets in current project.
# For each target, it will look its dependcies.
# Each dependenciy (that is not a target of this project) will be added to the list.
#
# Output arguments:
#  EXTERNAL_DEPENDENCIES:
#   Will contain the list of dependencies that external internal to Mdt
#
function(mdt_get_all_libraries_external_direct_dependencies EXTERNAL_DEPENDENCIES)
  # Get the list of currently existing targets
  mdt_get_target_list(existing_mdt_target_list)
  # For each target, find link dependencies, and add those that are not Mdt targets
  set(external_dependencies)
  foreach(mdt_target ${existing_mdt_target_list})
    message("Target: ${mdt_target}")
    get_target_property(target_link_dep_list ${mdt_target} LINK_LIBRARIES)
    foreach(target_link_dep ${target_link_dep_list})
      # Check if this external dependency is allready in the list
      list(FIND external_dependencies ${target_link_dep} list_index)
      if(${list_index} LESS 0)
        message(" Try ${target_link_dep}")
        # Add this link dependency if it is a external one
        list(FIND existing_mdt_target_list ${target_link_dep} list_index_2)
        if(${list_index_2} LESS 0)
          message("  Add ${target_link_dep}")
          list(APPEND external_dependencies ${target_link_dep})
        endif()
      endif()
    endforeach()
#     message(" Link deps: ${target_link_deps}")
  endforeach()
  set(${EXTERNAL_DEPENDENCIES} ${external_dependencies} PARENT_SCOPE)
endfunction()

# Get a list containing the full path of each directly used external library
#
# Output arguments:
#  EXTERNAL_DEPENDENCIES_LIBRARIES:
#   Will contain the list of path to directly dependend external libraries
#
function(mdt_get_all_libraries_external_direct_dependencies_files EXTERNAL_DEPENDENCIES_LIBRARIES)
  # Get all direct dependencies
  mdt_get_all_libraries_external_direct_dependencies(external_dependencies)
  # For each external dependency, add the file location to the list
  set(external_dependencies_libraries)
  foreach(external_dependency ${external_dependencies})
#     find_package(${external_dependency})
    find_library(ext_dep_lib ${external_dependency})
    get_target_property(external_dependency_file ${ext_dep_lib} LOCATION)
    list(APPEND external_dependencies_libraries external_dependency_file)
  endforeach()
  set(${EXTERNAL_DEPENDENCIES_LIBRARIES} ${external_dependencies_libraries} PARENT_SCOPE)
endfunction()

# Get a list of directories where direct library dependencies lives
#
# Output arguments:
#  EXTERNAL_DEPENDENCIES_DIRECTORIES:
#   Will contain the list of directories where direct external libraries are installed
#
function(mdt_get_directory_list_of_all_libraries_external_direct_dependencies EXTERNAL_DEPENDENCIES_DIRECTORIES)
  # Get all direct dependencies
  mdt_get_all_libraries_external_direct_dependencies(external_dependencies)
  # For each external dependency, add its directory to the list
  set(external_dependencies_directories)
  foreach(external_dependency ${external_dependencies})
#     get_target_property(external_dependency ${external_dependency_directory} LINK_LIBRARIES)
  endforeach()
endfunction()






