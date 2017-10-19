# CPack offers the CPackDeb module that generate Debian packages
# Mdt needs a bit more control to generate those packages, like:
# - Generate Debian dependencies for Mdt internal package,
#   before they are installed (CPACK_DEBIAN_<COMPONENT>_PACKAGE_SHLIBDEPS does not work for that)
# - Generate Debian dependencies for non binary packages (for example Sql-dev)
# - Reuse CPACK_COMPONENT_<compName>_DEPENDS to generate Debian dependencies
#   for Mdt components
# - Add Debian dependencies like CPackDeb does (CPACK_DEBIAN_<COMPONENT>_PACKAGE_SHLIBDEPS),
#   but only for external libraries (like Qt5)
# - Set CPACK_DEBIAN_<COMPONENT>_PACKAGE_NAME that matches Debian policy
#
# TODO This should be under BSD licence, because it takes some code from CPackDeb
# TODO: should follow CPACK_PACKAGE_DIRECTORY or similar
# TODO: could be nice to find -dev external dependencies

if(NOT UNIX)
  message(FATAL_ERROR "MdtCPackDeb may only be used under UNIX.")
endif()

list(FIND CPACK_GENERATOR "DEB" _gen_deb_set)
if(_gen_deb_set GREATER -1)
  message(WARNING "CPACK_GENERATOR contains DEB, which permit to generate Debian packages using package target. It is recommended to use package_debian target instead.")
endif()

include(GNUInstallDirs)

if(NOT TARGET package_debian)
  find_program(CPACK_COMMAND cpack)
  add_custom_target(
    package_debian
    COMMAND "${CMAKE_COMMAND}" -E remove "${CMAKE_BINARY_DIR}/CPackConfig.cmake"
    COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
    COMMAND "${CMAKE_COMMAND}" -D ACTION=add_dependencies_to_config_file -D DEPENDENCIES_DIRECTORY="${CMAKE_BINARY_DIR}/MdtDebDep" -P "${CMAKE_SOURCE_DIR}/cmake/modules/MdtCPackDebActions.cmake"
    COMMAND "${CPACK_COMMAND}" -G DEB "${CMAKE_BINARY_DIR}"
    COMMAND "${CMAKE_COMMAND}" -D ACTION=generate_packages_file -P "${CMAKE_SOURCE_DIR}/cmake/modules/MdtCPackDebActions.cmake"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    VERBATIM
  )
endif()

# Add a Mdt library component to CPackDeb
#
# This function will populate following variables:
#  - CPACK_DEBIAN_<COMPONENT>_PACKAGE_NAME
#
# Input arguments:
#  COMPONENT:
#   Component to add.
function(mdt_cpack_deb_add_library_component)
  # Parse arguments
  set(oneValueArgs COMPONENT)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(component ${VAR_COMPONENT})
  if(NOT component)
    message(FATAL_ERROR "COMPONENT argument is missing.")
  endif()
  string(TOUPPER ${component} component_upper)

  # Set package name
  mdt_cpack_deb_set_library_component_package_name(
    COMPONENT ${component}
  )
  # Set package section and priority
  if(${component} MATCHES "-dev$")
    set(package_section "libdevel")
    set(package_priority "optional")
  elseif(${component} MATCHES "-dbg$")
    set(package_section "debug")
    set(package_priority "extra")
  elseif(${component} MATCHES "-l10n$")
    set(package_section "localization")
    set(package_priority "optional")
  else()
    set(package_section "libs")
    set(package_priority "optional")
  endif()
  set(CPACK_DEBIAN_${component_upper}_PACKAGE_SECTION ${package_section} CACHE INTERNAL "")
  set(CPACK_DEBIAN_${component_upper}_PACKAGE_PRIORITY ${package_priority} CACHE INTERNAL "")
  # Set Debian dependencies
  mdt_cpack_deb_set_library_component_package_dependencies(
    COMPONENT ${component}
  )
endfunction()

# Set Debian package name for given library component
#
# This function will populate following variables:
#  - CPACK_DEBIAN_<COMPONENT>_PACKAGE_NAME
#
# Input arguments:
#  COMPONENT:
#   Name of the component.
function(mdt_cpack_deb_set_library_component_package_name)
  # Parse arguments
  set(oneValueArgs COMPONENT)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(component ${VAR_COMPONENT})
  if(NOT component)
    message(FATAL_ERROR "COMPONENT argument is missing.")
  endif()
  # Build package name and set variable
  string(REGEX REPLACE "(-dev|-dbg|-l10n)$" "" package_base_name ${component})
  string(REGEX MATCH "(-dev|-dbg|-l10n)$" package_suffix ${component})
#   string(TOLOWER lib${PROJECT_NAME}${package_base_name}${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}${package_suffix} package_name)
  string(TOLOWER lib${PROJECT_NAME}${package_base_name}${package_suffix} package_name)
  string(TOUPPER ${component} component_upper)
  set(CPACK_DEBIAN_${component_upper}_PACKAGE_NAME ${package_name} CACHE INTERNAL "")
endfunction()

# TODO: use  CPACK_PACKAGE_DESCRIPTION_SUMMARY and CPACK_COMPONENT_<compName>_DESCRIPTION to set CPACK_COMPONENT_<COMPONENT>_DESCRIPTION
#       See debain Description format:
#       Description: summary
#        Long description line 1
#        .
#        Long description line 2

# Set the description of a component
#
# Input arguments:
#  COMPONENT:
#   Component to which description must be set.
function(mdt_cpack_deb_set_component_description)
  # Parse arguments
  set(oneValueArgs COMPONENT)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(component ${VAR_COMPONENT})
  if(NOT component)
    message(FATAL_ERROR "COMPONENT argument is missing.")
  endif()
  string(TOUPPER ${component} component_upper)

  # Get CPack varaibles
  get_property(
    comp_description
    GLOBAL
    PROPERTY CPACK_COMPONENT_${component}_DESCRIPTION
  )
  # Build Debian description
  string(REPLACE "\n" "\n .\n " deb_comp_description "${comp_description}")
  set(deb_description "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}\n ${deb_comp_description}")
  set_property(
    GLOBAL
    PROPERTY CPACK_COMPONENT_${component_upper}_DESCRIPTION "${deb_description}"
  )
  # Populate CPack Debian variable
#   set(CPACK_COMPONENT_${component_upper}_DESCRIPTION "${deb_description}" CACHE INTERNAL "")
#   set_property(
#     GLOBAL
#     PROPERTY CPACK_COMPONENT_${component}_DESCRIPTION "${deb_description}"
#   )

  # TODO: don't use cache variables, thei do not support \n
  #       keep global properties.
  #       see what must change in top level cmakelists.txt
#   message("deb_comp_description: ${deb_comp_description}")
#   message("CPACK_PACKAGE_DESCRIPTION_SUMMARY: ${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
#   message("deb_description: ${deb_description}")

#   message("library_description_var: ${library_description_var}")
#   message("library_description: ${library_description}")
#   
#   message("CPACK_COMPONENT_ERROR_DESCRIPTION: ${CPACK_COMPONENT_ERROR_DESCRIPTION}")
endfunction()

# Set Debian package dependencies
#
# This function will populate following variables:
#  - MDT_CPACK_DEBIAN_<COMPONENT>_PACKAGE_DEPENDS
#
# Input arguments:
#  COMPONENT:
#   Name of the component.
function(mdt_cpack_deb_set_library_component_package_dependencies)
  # Parse arguments
  set(oneValueArgs COMPONENT)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(component ${VAR_COMPONENT})
  if(NOT component)
    message(FATAL_ERROR "COMPONENT argument is missing.")
  endif()
  string(TOUPPER ${component} component_upper)
  # Check if this component is a binary library
  unset(target_is_binary_library)
  foreach(target ${component})
    if(TARGET ${target})
      get_target_property(lib_name ${target} LIBRARY_OUTPUT_NAME)
      if(lib_name)
        set(target_is_binary_library TRUE)
      endif()
    endif()
  endforeach()
  # Get Mdt internal dependencies
  # NOTE: for Mdt0, we depend on exact version
  set(cpack_component "CPACK_COMPONENT_${component}_DEPENDS")
  set(mdt_version ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
  set(mdt_dependencies)
  foreach(dep_comp ${${cpack_component}})
    string(TOUPPER ${dep_comp} dep_comp_upper)
    set(dep_deb_pkg "CPACK_DEBIAN_${dep_comp_upper}_PACKAGE_NAME")
#     list(APPEND mdt_dependencies "${${dep_deb_pkg}} (=${mdt_version})")
    if(mdt_dependencies)
      set(mdt_dependencies "${mdt_dependencies}, ${${dep_deb_pkg}} (=${mdt_version})")
    else()
      set(mdt_dependencies "${${dep_deb_pkg}} (=${mdt_version})")
    endif()
  endforeach()
  # If component is a binary library, we must write dependnecies to a file.
  # Later on, after compilation, this file will be used to generate complete dependencies.
  # For other components, we can directly populate CPACK_DEBIAN_<component>_DEPENDS variable.
  if(mdt_dependencies)
    if(target_is_binary_library)
      set(dep_file "${CMAKE_BINARY_DIR}/MdtDebDep/${component}.mdtdebdep")
      file(WRITE "${dep_file}" "${mdt_dependencies}")
    else()
      set(CPACK_DEBIAN_${component_upper}_PACKAGE_DEPENDS "${mdt_dependencies}" CACHE INTERNAL "")
    endif()
  endif()
  mdt_cpack_deb_add_target_to_find_library_component_external_dependencies(
    COMPONENT ${component}
  )
endfunction()

# Add a target to discover external dependencies of a library
#
# External library dependencies are discovered using informations on the compiled library binary.
# This requires that the library is compiled before finding those dependnecies.
# For this, this function will add a target that depends on target in given component.
#
# Input arguments:
#  COMPONENT:
#   Name of the component.
function(mdt_cpack_deb_add_target_to_find_library_component_external_dependencies)
  # Parse arguments
  set(oneValueArgs COMPONENT)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(component ${VAR_COMPONENT})
  if(NOT component)
    message(FATAL_ERROR "COMPONENT argument is missing.")
  endif()
  # Add a dependencies find target for each library of this component
  foreach(target ${component})
    if(TARGET ${target})
      get_target_property(lib_name ${target} LIBRARY_OUTPUT_NAME)
      if(lib_name)
        # Build library file path
        get_target_property(lib_dir ${target} BINARY_DIR)
        get_target_property(lib_version ${target} VERSION)
        if(lib_version)
          set(library_file "${lib_dir}/lib${lib_name}.so.${lib_version}")
        else()
          set(library_file "${lib_dir}/lib${lib_name}.so")
        endif()
        # Add target
        set(target_dep ${target}_deb_dep)
        add_custom_target(
          ${target_dep}
          COMMAND "${CMAKE_COMMAND}" -D ACTION=find_library_dependencies -D COMPONENT=${component} -D LIBRARY_FILE=${library_file} -D DEPENDENCIES_DIRECTORY="${CMAKE_BINARY_DIR}/MdtDebDep" -P "${CMAKE_SOURCE_DIR}/cmake/modules/MdtCPackDebActions.cmake"
          DEPENDS ${target}
          WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
          VERBATIM
        )
        add_dependencies(package_debian ${target_dep})
      endif()
    endif()
  endforeach()
endfunction()

# # Generate Packages.gz file
# function(mdt_cpack_deb_generate_package_index_file)
#   # Check that dpkg-scanpackages works
#   find_program(scanpackages_executable dpkg-scanpackages)
#   if(NOT scanpackages_executable)
#     message(WARNING "dpkg-scanpackages seems not to be installed. The Packages.gz will not be generated.")
#     return()
#   endif()
#   # Check that gzip is installed
#   find_program(gzip_executable gzip)
#   if(NOT gzip_executable)
#     message(WARNING "gzip seems not to be installed. The Packages.gz will not be generated.")
#     return()
#   endif()
#   message(STATUS "Creating Packages file")
#   # Generate Packages
#   execute_process(
#     COMMAND ${scanpackages_executable} "." "${CMAKE_BINARY_DIR}"
#     RESULT_VARIABLE retval
#     ERROR_VARIABLE scanpackages_error
#     OUTPUT_VARIABLE packages
#     OUTPUT_STRIP_TRAILING_WHITESPACE
#   )
#   if(NOT retval EQUAL 0)
#     message(FATAL_ERROR "MdtCPackDeb: dpkg-scanpackages failed: '${scanpackages_error}'")
#   endif()
#   file(WRITE "${CMAKE_BINARY_DIR}/Packages" "${packages}")
#   # Compress Packages -> Packages.gz
#   # gzip will automatically compress and rename the file
#   # -f : else, gzip will query about confirmation to overwrite the file
#   execute_process(
#     COMMAND ${gzip_executable} -f "${CMAKE_BINARY_DIR}/Packages"
#     RESULT_VARIABLE retval
#     ERROR_VARIABLE gzip_error
#   )
#   if(NOT retval EQUAL 0)
#     message(FATAL_ERROR
#       "MdtCPackDeb: gzip failed: '${gzip_error}'"
#     )
#   endif()
# endfunction()
