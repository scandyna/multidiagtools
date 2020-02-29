include(GenerateExportHeader)

# Add a SQL plugin
#
# While building the Mdt library, the plugin will be placed ${CMAKE_BINARY_DIR}/plugins/Mdt/Sql
# so that all components of the Mdt library can find it in a common way.
#
# When the Mdt library is installed, the plugin will be placed in ${MDT_PREFIX_PATH}/plugins/Mdt/Sql
#
# When distributing the final application, the plugin should be placed in ${APP_PREFIX_PATH}/plugins/Mdt/Sql
#
# Internally, the components that loads the plugin will use Mdt::Sql::LibraryInfo.
#
# Input arguments:
# NAME:
#  Name of the plugin, without any prefix (like project name) or suffix (like version).
#  This plugin name is also what is named target in CMake.
# SOURCE_FILES:
#  List of relative path to source files to compile for the plugin.
#  As mentionned by CMake, it is not recommanded to generate source_files using file(GLOB),
#  because CMake will not be able to regenerate when a file was added.
# DEPENDENCIES (optional):
#  List of libraries to link to this plugin.
#  Note: the internally used dependencies (like Sql_Core, Qt5::Sql) will be added automatically,
#        and should not be specified as input arguments.
#  If the plugin depends on a other library from Mdt project,
#  its target name should be passed (For example Application, not MdtApplication)
#  If the plugin depends on a other library that provides CMake Package config files, like Qt5,
#  its target name should be passed (for example Qt5::Sql).
#
function(mdt_add_sql_plugin)
  # Parse arguments
  set(oneValueArgs NAME)
  set(multiValueArgs SOURCE_FILES DEPENDENCIES)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  # Check that all argumenst are passed
  if(NOT VAR_NAME)
    message(FATAL_ERROR "mdt_add_sql_plugin(): NAME argument is missing.")
  endif()
  if(NOT VAR_SOURCE_FILES)
    message(FATAL_ERROR "mdt_add_sql_plugin(): No source file provided.")
  endif()
  # Local variables
  set(target_name ${VAR_NAME})
  set(source_files ${VAR_SOURCE_FILES})
  set(dependencies ${VAR_DEPENDENCIES})

  add_library(${target_name} MODULE ${source_files})
  # TODO Should use PRIVATE
  target_link_libraries(${target_name} Sql_Core Qt5::Sql ${dependencies})
  # Set library name and output directory
  set(library_name ${PROJECT_NAME}${target_name})
  set_target_properties(${target_name} PROPERTIES
    OUTPUT_NAME ${library_name}
  )
  set_target_properties(${target_name} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/Mdt/Sql"
  )

endfunction()
