#################################################################################################################################################
# Mdt top level configuration file
#################################################################################################################################################
cmake_minimum_required(VERSION 3.2)

project(Mdt0 VERSION 0.3.7)

# Additionnal Cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

#################################################################################################################################################
# Common setup
#################################################################################################################################################
# Those must be set before including Mdt modules
set(CPACK_PACKAGE_CONTACT "Philippe Steinmann <philippe.steinmann@gmail.com>") # TODO Check if set outside (see cpack options)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Mdt is a utilities library")
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_NSIS_COMPONENT_INSTALL ON)
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# Specify where to find Qt
# If Qt5 is not installed on a system known location,
# or you want to use a specific version,
# or you are on Windows, QT_PREFIX_PATH should be specified
if(QT_PREFIX_PATH)
  list(APPEND CMAKE_PREFIX_PATH "${QT_PREFIX_PATH}")
endif()

find_package(Qt5 COMPONENTS Core)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

message("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message("CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
message("CMAKE_HOST_SYSTEM_NAME: ${CMAKE_HOST_SYSTEM_NAME}")
message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message("CMAKE_HOST_SYSTEM_PROCESSOR: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message("CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")

# On Windows, RPATH do not exist
# To be able to run tests, we have to put all binaries in the same directory
# We also tell CMake to use Wine to execute tests if we cross-compiled
if(WIN32)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
  if(CMAKE_CROSSCOMPILING)
    set(CMAKE_CROSSCOMPILING_EMULATOR wine)
  endif()
endif()

# Enable testing
# Must be placed before any add_subdirectory() command, else tests that are defined in sub-directories will be ignored
enable_testing()

#################################################################################################################################################
# Compiler options
#################################################################################################################################################

# TODO Should look at PIE (Position Independ Code). Security and other..

# C++ standard
set(CMAKE_CXX_STANDARD 14)
# Build C++ flags adding those set in cache file
set(CMAKE_CXX_FLAGS "${MDT_CXX_FLAGS_TO_PREPEND} ${CMAKE_CXX_FLAGS} ${MDT_CXX_FLAGS_TO_APPEND}")

# On UNIX, we allways add debug symbols (will be stripped and put into separate .dbg files)
if(UNIX)
  if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_compile_options(-g)
  endif()
endif(UNIX)

# Thread support
find_package(Threads REQUIRED)

#################################################################################################################################################
# Additionnal CMake modules
#################################################################################################################################################

# List of targeted translations
# This is used by MdtAddTranslations.cmake module
set(TRANSLATION_LANGUAGES en fr de)
include(MdtAddLibraryTranslations)

include(MdtAddLibrary)

#################################################################################################################################################
# Install options          ( Note: some options needs to be set before add_subdirectory() )
#################################################################################################################################################

get_filename_component(install_prefix "${CMAKE_INSTALL_PREFIX}" ABSOLUTE)

# Generate the post-install script
# This will change the RPATH on the user's target platform
if( NOT ("${install_prefix}" STREQUAL "/usr") )
  configure_file(
    "${CMAKE_SOURCE_DIR}/MdtPostInstall.cmake.in"
    "${CMAKE_BINARY_DIR}/MdtPostInstall.cmake"
    @ONLY
  )
  install(
    FILES "${CMAKE_BINARY_DIR}/MdtPostInstall.cmake"
    DESTINATION "." COMPONENT dev
  )
endif()

#################################################################################################################################################
# Licenses
#################################################################################################################################################

# For system wide installation,
# installing licenses files is specific to the Linux distribution .
if( NOT ("${install_prefix}" STREQUAL "/usr") )
  install(
    FILES "${CMAKE_SOURCE_DIR}/LICENSE"
    DESTINATION "."
  )
endif()

#################################################################################################################################################
# Libraries
#################################################################################################################################################

add_subdirectory(libs/QtSolutions)
add_subdirectory(libs/Assert)
add_subdirectory(libs/Error_Core)
add_subdirectory(libs/Error_Widgets)
add_subdirectory(libs/LibraryInfo_Core)
add_subdirectory(libs/Application_Core)
add_subdirectory(libs/Application_Core_Single)
add_subdirectory(libs/Application_Widgets)
add_subdirectory(libs/Application_Widgets_Single)
add_subdirectory(libs/Algorithm)
add_subdirectory(libs/IndexRange)
add_subdirectory(libs/Container)
add_subdirectory(libs/Expected)
add_subdirectory(libs/FilterExpression)
add_subdirectory(libs/Numeric)
add_subdirectory(libs/PlainText_Core)
add_subdirectory(libs/ItemModel)
# # add_subdirectory(libs/SerialPort)
add_subdirectory(libs/ItemEditor_Widgets)
# add_subdirectory(libs/ItemEditor_Sql)
add_subdirectory(libs/TestLib_Core)
add_subdirectory(libs/TestLib_Container)
add_subdirectory(libs/TestLib_Widgets)
add_subdirectory(libs/FileSystem_Core)
add_subdirectory(libs/DeployUtils_Core)
add_subdirectory(libs/Translation_Core)
add_subdirectory(libs/Translation_Widgets)
add_subdirectory(libs/QueryExpression_Core)
add_subdirectory(libs/QueryExpression_Reflection)
add_subdirectory(libs/QueryExpression_Sql)
add_subdirectory(libs/Sql_Core)
add_subdirectory(libs/Sql_Core_SQLite)
add_subdirectory(libs/Sql_Widgets)
add_subdirectory(libs/Entity_Core)
add_subdirectory(libs/Entity_Sql)
add_subdirectory(libs/Entity_Widgets)
add_subdirectory(libs/Railway_Core)
add_subdirectory(libs/Railway_Sql)
add_subdirectory(libs/Railway_Widgets)
add_subdirectory(libs/Railway_QtQuick)
add_subdirectory(libs/Railway_TaskList_Core)
add_subdirectory(libs/Railway_TaskList_Sql)
add_subdirectory(libs/Railway_TaskList_Widgets)
add_subdirectory(libs/MemoryStore)
add_subdirectory(libs/Reflection_Core)
add_subdirectory(libs/Sql_Reflection)

# include(MdtInstallQt5)

#################################################################################################################################################
# Tools and Helpers
#################################################################################################################################################

add_subdirectory(tools)
add_subdirectory(cmake)

#################################################################################################################################################
# Icons
#################################################################################################################################################

# add_subdirectory(icons)

#################################################################################################################################################
# Unit tests
#################################################################################################################################################

# On Windows, we have to copy external libraries to the test runtime directory

# On Windows we have the following situation
#
# We are on a Windows machine, using Qt provided build suite,
# and the related script that setups PATH accordingly.
# In this case, nothing special to do.
#
# We are cross-compiling for Windows from a Linux machine using MXE.
# In this case, Qt libraries and mingw libraries are not in the PATH.
# We have to find depepndencies of the unit tests and copy them into the bin directory.
# Note: it is probably possible to create a temporary environnment,
#       but doing the current way has the positive side effect
#       to check if dependencies tools works.
#       Those tools will be used later for binary distrubutions.
if(WIN32 AND CMAKE_RUNTIME_OUTPUT_DIRECTORY AND CMAKE_CROSSCOMPILING)
  include(MdtDependenciesUtils)
  # Because I not found a proper way to get a list of all existing tests,
  # just put some of them that have the most dependencies.
  mdt_copy_binary_dependencies(
    TARGETS mdtitemmodel_filterproxymodeltest mdtdeployutils_librarytest mdtplaintext_csvparsertest mdterror_errortest
    LIBRARY_DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    PLUGIN_DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    SEARCH_FIRST_PATH_PREFIX_LIST ${CMAKE_PREFIX_PATH}
  )

#   mdt_copy_binary_dependencies(
#     TARGET QtSingleApplication
#     DESTINATION_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
#     NO_WARNINGS
#   )
#   mdt_copy_binary_dependencies(
#     TARGET Application
#     DESTINATION_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
#     NO_WARNINGS
#   )
#   mdt_copy_binary_dependencies(
#     TARGET mdterrortest
#     DESTINATION_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
#     NO_WARNINGS
#   )
#   mdt_copy_binary_dependencies(
#     TARGET mdterror_widgetstest
#     DESTINATION_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
#     NO_WARNINGS
#   )
#   mdt_copy_binary_dependencies(
#     TARGET mdtitemeditor_sqlcontrollertest
#     DESTINATION_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
#     NO_WARNINGS
#   )

#   mdt_copy_binary_dependencies_experimental(
#     TARGET QtSingleApplication
#     DESTINATION_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
#     SEARCH_FIRST_PATH_PREFIX_LIST ${CMAKE_PREFIX_PATH}
#   )
#   mdt_copy_binary_dependencies_experimental(
#     TARGET Application
#     DESTINATION_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
#     SEARCH_FIRST_PATH_PREFIX_LIST ${CMAKE_PREFIX_PATH}
#   )
#   mdt_copy_binary_dependencies_experimental(
#     TARGET mdterrortest
#     DESTINATION_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
#     SEARCH_FIRST_PATH_PREFIX_LIST ${CMAKE_PREFIX_PATH}
#   )

  # If we are doing cross-compilation,
  # recusrsive resolving do not work
  # We have to add manually libraries that are missing
#   if(CMAKE_CROSSCOMPILING AND MINGW)
#     mdt_find_and_copy_libraries(
#       LIBRARIES
#         libpcre2-16-0 zlib1 libeay32 ssleay32 libharfbuzz-0 libpng16-16 libfreetype-6 libglib-2.0-0
#         libbz2 libintl-8 libpcre-1 libiconv-2
#       DESTINATION_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
#     )
#   endif()
endif()

#################################################################################################################################################
# Top level package configuration files
#################################################################################################################################################

# Top level Mdt<major-version>Config.cmake
# This permits the user to call, for example, find_package(Mdt<major-version> COMPONENTS ItemModel Sql)
# Will generate Mdt<major-version>Config.cmake at install time
# and install it to <install-prefix>/lib/cmake/Mdt<major-version>/
include(CMakePackageConfigHelpers)
configure_package_config_file(
  MdtConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake"
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
        COMPONENT ${PROJECT_NAME}
)

# Top level Mdt<major-version>ConfigVersion.cmake
# This permits the user to call, for example, find_package(Mdt<major-version> 0.1 COMPONENTS ItemModel Sql)
# Will generate Mdt<major-version>ConfigVersion.cmake at install time
# and install it to <install-prefix>/lib/cmake/Mdt<major-version>/
# NOTE: Mdt0 is experimental and will allways break between version
#       Major version compatibility should be provided starting Mdt1
#       (When true, pass COMPATIBILITY SameMajorVersion as argument)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  COMPATIBILITY ExactVersion
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
        COMPONENT ${PROJECT_NAME}
)

#################################################################################################################################################
#     CPack
#################################################################################################################################################

set(CPACK_SOURCE_IGNORE_FILES
  "${CMAKE_BINARY_DIR}"
  "build"
  "\.#"
  ".DS_Store*"
  "ehthumbs.db"
  "Icon?"
  "Thumbs.db"
  ".directory"
  "$.log^"
  "$.tmp^"
  "CMakeFiles"
  "CTest."
  "Makefile"
  "cmake_install.cmake"
)

if(UNIX)
  set(CPACK_GENERATOR TBZ2)
  set(CPACK_SOURCE_GENERATOR TBZ2)
endif()
if(WIN32)
#   set(CPACK_GENERATOR NSIS)
  set(CPACK_SOURCE_GENERATOR ZIP)
endif()


# Make properties defined for CPack availabe as variables in current scope
get_cmake_property(components COMPONENTS)
foreach(component ${components})
  get_property(
    CPACK_COMPONENT_${component}_DESCRIPTION
    GLOBAL
    PROPERTY CPACK_COMPONENT_${component}_DESCRIPTION
  )
  if(UNIX)
    string(TOUPPER ${component} component_upper)
    get_property(
      CPACK_COMPONENT_${component_upper}_DESCRIPTION
      GLOBAL
      PROPERTY CPACK_COMPONENT_${component_upper}_DESCRIPTION
    )
  endif()
endforeach()

# Detect compile architecture
# See: https://stackoverflow.com/questions/11944060/how-to-detect-target-architecture-using-cmake
# Thanks to: https://github.com/axr/solar-cmake
include(TargetArch)
target_architecture(COMPILE_ARCHITECTURE)
message("COMPILE_ARCHITECTURE: ${COMPILE_ARCHITECTURE}")

# Get Qt5 version
# After success, find_package() wil define Qt5_VERSION, Qt5_VERSION_MAJOR, etc..
find_package(Qt5 COMPONENTS Core)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
  message("DEBUG build")
  set(debug_file_name_postfix "-debug")
else()
  set(debug_file_name_postfix "")
endif()
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-Qt-${Qt5_VERSION_MAJOR}.${Qt5_VERSION_MINOR}-${CMAKE_SYSTEM_NAME}-${CMAKE_CXX_COMPILER_ID}-${COMPILE_ARCHITECTURE}${debug_file_name_postfix}")

include(CPack)
