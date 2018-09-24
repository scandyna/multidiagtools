include(GenerateExportHeader)

# Problemes to solve:
# - Build the QSQLITE-mdt plugin:
#  -- 
# - Use/load the QSQLITE-mdt:
#  -- A) when developping Mdt:
#   --> Put all plugins in a common dir (${CMAKE_BINARY_DIR}/plugins/sqldrivers)
#   --> Set QT_PLUGIN_PATH to ${CMAKE_BINARY_DIR}/plugins Nok
#   --> note: build flags Nok
#  -- B) when using Mdt to developp app:
#   --> Install plugins in ${MDT_PREFIX_PATH}/plugins/sqldrivers
#   --> Set QT_PLUGIN_PATH to ${MDT_PREFIX_PATH}/plugins
#  -- C) for a distributed app:
#   --> Install plugins in $App/plugins/sqldrivers
#   --> Qt should also find them out of the box

# Note: no headers to install

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
# The plugin will be placed ${CMAKE_BINARY_DIR}/plugins/sqldrivers
# so that all components of the Mdt library can find it in a common way.
# The tests that needs to load the plugin should built with a flag that tells where to find the plugins:
# target_compile_definitions(MyTest PRIVATE MDT_PLUGIN_PATH="${CMAKE_BINARY_DIR}/plugins")
# In the main of the test, this path should also be added:
# QCoreApplication::addLibraryPath( QString::fromLocal8Bit(MDT_PLUGIN_PATH) );
# TODO See if this can be unified with mdt_add_test(TARGET, ...)
#
# When the Mdt library is installed, the plugin will be placed in ${MDT_PREFIX_PATH}/plugins/sqldrivers
# While developping a application using the Mdt library,
# Mdt::LibraryInfo can be used to query the location of the Mdt plugins path:
# const auto mdtPluginsPath = Mdt::LibraryInfo::getMdtPluginsPath();
#
# While developping a application using the Mdt library, the above rule can be applied to unit tests.
# ...............
# For the finaly application (and libraries) that will be distributed, no build tree specific paths should be compiled in ....
# TODO see how to solve...

# NOTE: how does Qt do to set the path to the installed library ??

# NOTE: Mdt::LibraryInfo::getPluginsPath(); could be used for each case