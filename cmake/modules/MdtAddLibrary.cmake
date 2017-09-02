include(MdtCPackComponent)
include(MdtDependenciesUtils)

# Enable multi-arch installation
# For example, install libraries to /usr/lib/x86_64-linux-gnu on a x86_64 Debian
include(GNUInstallDirs)


# Add a library to Mdt project
#
# Input arguments:
# NAME:
#  Name of the library, without any prefix (like project name) or suffix (like version).
#  This library name is also what is named target in CMake.
# SOURCE_FILES:
#  List of relative path to source files to compile for the library.
#  As mentionned by CMake, it is not recommanded to generate source_files using file(GLOB),
#  because CMake will not be able to regenerate.
# HEADERS_DIRECTORY:
#  Relative path to the directory containing header files.
#  All header files present in this directory will also be installed in <install-prefix>/include/Mdt{0|1|..}
# LINK_DEPENDENCIES (optional):
#  List of libraries to link to this library.
#  If the library depends on a other library from Mdt project,
#  its target name should be passed (For example Application, not MdtApplication)
#  If the library depends on a other library that provides CMake Package config files, like Qt5,
#  its target name should be passed (for example Qt5::Widgets).
#
# Several install components will be created:
#  NAME:
#   Contains the library binary.
#   This name can also be passed as LINK_DEPENDENCIES to this function,
#   or as <target> name to target_*() commands.
#  NAME-dev:
#   Contains the public headers.
#  NAME-dbg (Linux only):
#   Contains the debug symbols.
#
# Recommanded usage in Mdt when creating a component
#
# File tree structure:
# libs
#  |_MyLibrary
#        |_src
#           |_CMakeLists.txt
#           |_Mdt
#              |_MyLibrary
#                     |_ClassA.cpp
#                     |_ClassA.h
#                     |_ClassB.cpp
#                     |_ClassB.h
#
# The CMakeLists.txt for src should look like:
#  set(SOURCE_FILES
#    Mdt/MyLibrary/ClassA.cpp
#    Mdt/MyLibrary/ClassB.cpp
#  )
#  mdt_add_library(
#    NAME MyLibrary
#    SOURCE_FILES ${SOURCE_FILES}
#    HEADERS_DIRECTORY .
#    LINK_DEPENDENCIES Application Qt5::Widgets
#  )
function(mdt_add_library)
  # Parse arguments
  set(oneValueArgs NAME HEADERS_DIRECTORY)
  set(multiValueArgs SOURCE_FILES LINK_DEPENDENCIES)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  # Check that all argumenst are passed
  if(NOT VAR_NAME)
    message(FATAL_ERROR "mdt_add_library(): NAME argument is missing.")
  endif()
  if(NOT VAR_SOURCE_FILES)
    message(FATAL_ERROR "mdt_add_library(): No source file provided.")
  endif()
  if(NOT VAR_HEADERS_DIRECTORY)
    message(FATAL_ERROR "mdt_add_library(): HEADERS_DIRECTORY argument is missing.")
  endif()
  # Local variables
  set(target_name ${VAR_NAME})
  set(source_files ${VAR_SOURCE_FILES})
  set(headers_dir ${VAR_HEADERS_DIRECTORY})
  set(link_deps ${VAR_LINK_DEPENDENCIES})
  # Commands to build the library
  add_library(${target_name} SHARED ${source_files})
  target_link_libraries(${target_name} ${link_deps})
  target_include_directories(${target_name} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${headers_dir}>
                            $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  )
  # Set library name
  set(library_name ${PROJECT_NAME}${target_name})
  set(library_version ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
  set_target_properties(${target_name} PROPERTIES
    OUTPUT_NAME ${library_name}
    VERSION ${library_version}
  )
  # For unix, create a separate component with debug symbols
  if(UNIX)
    set(lib_so_name ${CMAKE_SHARED_LIBRARY_PREFIX}${library_name}${CMAKE_SHARED_LIBRARY_SUFFIX}.${library_version})
    set(lib_debug_name ${CMAKE_SHARED_LIBRARY_PREFIX}${library_name}.debug)
    add_custom_command(TARGET ${target_name}
                       POST_BUILD
                       COMMAND objcopy --only-keep-debug ${lib_so_name} ${lib_debug_name}
                       COMMAND strip --strip-debug --strip-unneeded ${lib_so_name}
                       COMMAND objcopy --add-gnu-debuglink=${lib_debug_name} ${lib_so_name}
    )
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${lib_debug_name}"
            DESTINATION "lib/debug/${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}"
            PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
            COMPONENT ${target_name}-dbg
    )
    mdt_cpack_add_library_component(
      COMPONENT ${target_name}-dbg
      EXPLICIT_DEPEND_COMPONENTS ${target_name}
    )
  endif()
  # Commands to install the library
  install(TARGETS ${target_name}
          EXPORT ${library_name}Targets
          LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          RUNTIME DESTINATION bin
          COMPONENT ${target_name}
  )
  install(DIRECTORY ${headers_dir}
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
          COMPONENT ${target_name}-dev
          FILES_MATCHING
            REGEX "/[^.]+$|/.+\\.h|/.+\\.hpp" # Only install files without extension, or with .h or .hpp extension
  )
  # Commands to generate Config.cmake file
  install(EXPORT ${library_name}Targets
          DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${library_name}"
          NAMESPACE ${PROJECT_NAME}::
          COMPONENT ${target_name}
  )
  set(config_in_file "${CMAKE_CURRENT_BINARY_DIR}/${target_name}Config.cmake.in")
  file(WRITE "${config_in_file}" "@PACKAGE_INIT@\n")
  set(mdt_dep_targets)
  mdt_get_library_internal_dependencies(
    mdt_dep_targets
    DEP_TARGETS ${link_deps}
  )
  if(mdt_dep_targets)
    file(APPEND "${config_in_file}" "set(@PROJECT_NAME@${target_name}Dependencies \"${mdt_dep_targets}\")\n")
    file(APPEND "${config_in_file}" "foreach(dependency \${@PROJECT_NAME@${target_name}Dependencies})\n")
    file(APPEND "${config_in_file}" "  find_package(\n")
    file(APPEND "${config_in_file}" "    @PROJECT_NAME@\${dependency}\n")
    file(APPEND "${config_in_file}" "    CONFIG\n")
    file(APPEND "${config_in_file}" "    PATHS \"\${CMAKE_CURRENT_LIST_DIR}/..\" NO_DEFAULT_PATH\n")
    file(APPEND "${config_in_file}" "  )\n")
    file(APPEND "${config_in_file}" "endforeach()\n")
  endif()
  file(APPEND "${config_in_file}" "include(\"\${CMAKE_CURRENT_LIST_DIR}/@PROJECT_NAME@${target_name}Targets.cmake\")\n")
  file(APPEND "${config_in_file}" "check_required_components(@PROJECT_NAME@${target_name})")
  include(CMakePackageConfigHelpers)
  configure_package_config_file(
    "${config_in_file}"
    "${CMAKE_CURRENT_BINARY_DIR}/${library_name}Config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake"
  )
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${library_name}Config.cmake"
          DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${library_name}"
          COMPONENT ${target_name}
  )
  # Commands to generate ConfigVersion.cmake
  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${library_name}ConfigVersion.cmake"
    COMPATIBILITY ExactVersion
  )
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${library_name}ConfigVersion.cmake"
          DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${library_name}"
          COMPONENT ${target_name}
  )
  # Add components to CPack
  mdt_cpack_add_library_component(
    COMPONENT ${target_name}
    DEPEND_TARGETS ${link_deps}
  )
  mdt_cpack_add_library_component(
    COMPONENT ${target_name}-dev
    DEPEND_TARGETS ${link_deps}
    DEPEND_TARGETS_SUFFIX dev
    EXPLICIT_DEPEND_COMPONENTS ${target_name}
  )
  # On Windows, we later will need the external dll's
#   if(WIN32)
#     mdt_register_library_external_direct_dependencies(
#       TARGET ${target_name}
#     )
#   endif()
endfunction()

# Add a qt designer plugin library
#
# Input arguments:
# PLUGIN_NAME:
#  Name of the plugin library, without any prefix (like project name) or suffix (like version).
#  This library name is also what is named target in CMake.
# SOURCE_FILES:
#  List of relative path to source files to compile for the plugin library.
#  As mentionned by CMake, it is not recommanded to generate source_files using file(GLOB),
#  because CMake will not be able to regenerate.
# HEADERS_DIRECTORY:
#  Relative path to the directory containing header files.
# LINK_DEPENDENCIES (optional):
#  List of libraries to link to this plugin library.
#  If the library depends on a other library from Mdt project,
#  its target name should be passed (For example Application, not MdtApplication)
#  If the library depends on a other library that provides CMake Package config files, like Qt5,
#  its target name should be passed (for example Qt5::Widgets).
#  The dependencies to Qt Designer are added by this function internally.
#
# Several install components will be created:
#  PLUGIN_NAME:
#   Contains the plugin library binary.
#
# Recommanded usage in Mdt when creating a component
#
# File tree structure:
# libs
#  |_MyLibrary
#        |_plugins
#            |_QtDesigner
#                 |_CMakeLists.txt
#                     |_ClassAPlugin.cpp
#                     |_ClassAPlugin.h
#                     |_ClassBPlugin.cpp
#                     |_ClassBPlugin.h
#
# The CMakeLists.txt should look like:
#  set(SOURCE_FILES
#    ClassAPlugin.cpp
#    ClassBPlugin.cpp
#  )
#  mdt_add_qt_designer_plugin_library(
#    PLUGIN_NAME MyLibraryQtDesignerPlugin
#    SOURCE_FILES ${SOURCE_FILES}
#    HEADERS_DIRECTORY .
#    LINK_DEPENDENCIES Application Qt5::Widgets
#  )
#
# Note: if Qt5 Designer plugin is not available,
#       the plugin will not be built.
#       (This is for example the case of cross-compilation with MXE)
function(mdt_add_qt_designer_plugin_library)
  # Parse arguments
  set(oneValueArgs PLUGIN_NAME SOURCE_FILES HEADERS_DIRECTORY LINK_DEPENDENCIES)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(plugin_name ${VAR_PLUGIN_NAME})
  if(NOT plugin_name)
    message(FATAL_ERROR "PLUGIN_NAME argument is missing.")
  endif()
  set(source_files ${VAR_SOURCE_FILES})
  if(NOT source_files)
    message(FATAL_ERROR "SOURCE_FILES argument is missing.")
  endif()
  set(headers_directory ${VAR_HEADERS_DIRECTORY})
  if(NOT headers_directory)
    message(FATAL_ERROR "HEADERS_DIRECTORY argument is missing.")
  endif()
  set(link_dependencies ${VAR_LINK_DEPENDENCIES})
  if(NOT link_dependencies)
    message(FATAL_ERROR "LINK_DEPENDENCIES argument is missing.")
  endif()
  # Find external dependencies
  find_package(Qt5 QUIET OPTIONAL_COMPONENTS Designer)
  if(NOT Designer_FOUND)
    message(WARNING "${plugin_name} will not be built because Qt5 Designer is not available.")
    return()
  endif()
  # Commands to build the library
  add_library(${plugin_name} MODULE ${source_files})
  target_compile_definitions(${plugin_name} PRIVATE -DQT_PLUGIN -DQT_NO_DEBUG -DQT_SHARED -DQTDESIGNER_EXPORT_WIDGETS)
  target_link_libraries(${plugin_name} ${link_dependencies} Qt5::Designer)
  target_include_directories(${plugin_name} PRIVATE
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${headers_directory}>
  )
  # Set library name
  set(library_name ${PROJECT_NAME}${plugin_name})
  set_target_properties(${plugin_name} PROPERTIES
    LIBRARY_OUTPUT_NAME ${library_name}
  )
  # Commands to install the library
  install(TARGETS ${plugin_name}
          LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/qt5/plugins/designer"
          COMPONENT ${plugin_name}
  )
  # Add components to CPack
  mdt_cpack_add_library_component(
    COMPONENT ${plugin_name}
    DEPEND_TARGETS ${link_dependencies}
  )
endfunction()

# Set the description of a library
#
# Input arguments:
# NAME:
#  Name of the library, without any prefix (like project name) or suffix (like version).
#  This is th same name that has been passed to mdt_add_library()
# DESCRIPTION:
#  Description of the library.
function(mdt_set_library_description)
  # Parse arguments
  set(oneValueArgs NAME DESCRIPTION)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(name ${VAR_NAME})
  if(NOT name)
    message(FATAL_ERROR "NAME argument is missing.")
  endif()
  set(description ${VAR_DESCRIPTION})
  if(NOT name)
    message(FATAL_ERROR "DESCRIPTION argument is missing.")
  endif()

  # Add component specific descriptions
  set(library_decription "${description}\nThis component contains the library runtime.")
  set(library_dev_decription "${description}\nThis component contains the library developpment headers.")
  set(library_l10n_decription "${description}\nThis component contains the localization files.")
  if(UNIX)
    set(library_dbg_decription "${description}\nThis component contains the library debug symbols.")
  endif()
  # Populate variables
  set_property(
    GLOBAL
    PROPERTY CPACK_COMPONENT_${name}_DESCRIPTION "${library_decription}"
  )
  set_property(
    GLOBAL
    PROPERTY CPACK_COMPONENT_${name}-dev_DESCRIPTION "${library_dev_decription}"
  )
  set_property(
    GLOBAL
    PROPERTY CPACK_COMPONENT_${name}-l10n_DESCRIPTION "${library_l10n_decription}"
  )
  if(UNIX)
    set_property(
      GLOBAL
      PROPERTY CPACK_COMPONENT_${name}-dbg_DESCRIPTION "${library_dbg_decription}"
    )
  endif()
  # Set also Debain component descriptions
  if(UNIX)
    include(MdtCPackDeb)
    mdt_cpack_deb_set_component_description(
      COMPONENT ${name}
    )
    mdt_cpack_deb_set_component_description(
      COMPONENT ${name}-dev
    )
    mdt_cpack_deb_set_component_description(
      COMPONENT ${name}-l10n
    )
    mdt_cpack_deb_set_component_description(
      COMPONENT ${name}-dbg
    )
  endif()
endfunction()
