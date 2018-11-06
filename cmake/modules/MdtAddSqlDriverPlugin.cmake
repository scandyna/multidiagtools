include(GenerateExportHeader)

# TODO This distributes to a Qt specific way
#      The Qt MDTSQLITE module is a particular case to not generalise
#      Here, adopt a Mdt specific standard
#      Maybe rename mdt_add_sql_plugin() ?

# Add a SQL driver plugin
#
# This function can be used to build and install a plugin
# based on the Qt SQL Database Drivers model.
#
# While building the Mdt library, the plugin will be placed ${CMAKE_BINARY_DIR}/plugins/sqldrivers
# so that all components of the Mdt library can find it in a common way.
#
# When the Mdt library is installed, the plugin will be placed in ${MDT_PREFIX_PATH}/plugins/sqldrivers
#
# When distributing the final application, the plugin should be placed in ${APP_PREFIX_PATH}/plugins/sqldrivers
# (which will be the same location as Qt SQL drivers plugins).
#
# To load the plugin at runtime, the Qt mechanism is used, which simply requires that the plugins path is known.
# For this, Mdt::LibraryInfo can be used:
# const auto pluginsPath = Mdt::LibraryInfo::getPluginsPath();
# if(!pluginsPath){
#   // error handling
# }
# QCoreApplication::addLibraryPath(*pluginsPath);
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
#  If the plugin depends on a other library from Mdt project,
#  its target name should be passed (For example Application, not MdtApplication)
#  If the plugin depends on a other library that provides CMake Package config files, like Qt5,
#  its target name should be passed (for example Qt5::Sql).
#
function(mdt_add_sql_driver_plugin)
  # Parse arguments
  set(oneValueArgs NAME)
  set(multiValueArgs SOURCE_FILES DEPENDENCIES)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  # Check that all argumenst are passed
  if(NOT VAR_NAME)
    message(FATAL_ERROR "mdt_add_sql_driver_plugin(): NAME argument is missing.")
  endif()
  if(NOT VAR_SOURCE_FILES)
    message(FATAL_ERROR "mdt_add_sql_driver_plugin(): No source file provided.")
  endif()
  # Local variables
  set(target_name ${VAR_NAME})
  set(source_files ${VAR_SOURCE_FILES})
  set(dependencies ${VAR_DEPENDENCIES})

  add_library(${target_name} MODULE ${source_files})
  # TODO Should use PRIVATE
  target_link_libraries(${target_name} ${dependencies})
  set_target_properties(${target_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/sqldrivers")

endfunction()
