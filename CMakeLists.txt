#################################################################################################################################################
# Mdt top level configuration file
#################################################################################################################################################
cmake_minimum_required(VERSION 3.3)

project(Mdt0 VERSION 0.3.5)

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

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# On Windows, RPATH do not exist
# To be able to run tests, we have to put all binaries in the same directory
# We also tell CMake to use Wine to execute tests if we cross-compiled
if(WIN32)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
  set(CMAKE_CROSSCOMPILING_EMULATOR wine)
endif()

# Enable testing
# Must be placed before any add_subdirectory() command, else tests that are defined in sub-directories will be ignored
enable_testing()

# Config file
# TODO should be moved somwhere (like Globals library) ?
configure_file(
  "${CMAKE_SOURCE_DIR}/config.h.in"
  "${CMAKE_BINARY_DIR}/config.h"
)

#################################################################################################################################################
# Compiler options
#################################################################################################################################################

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
include(MdtAddTranslations)

include(MdtAddLibrary)

#################################################################################################################################################
# Libraries
#################################################################################################################################################

add_subdirectory(libs/QtSolutions)
add_subdirectory(libs/Error)
add_subdirectory(libs/Error_Widgets)
add_subdirectory(libs/Application)
add_subdirectory(libs/Algorithm)
add_subdirectory(libs/Expected)
add_subdirectory(libs/FilterExpression)
add_subdirectory(libs/Numeric)
add_subdirectory(libs/PlainText_Core)
# add_subdirectory(libs/SerialPort)
add_subdirectory(libs/Sql)
add_subdirectory(libs/ItemModel)
add_subdirectory(libs/ItemEditor)
add_subdirectory(libs/ItemEditor_Sql)

# On Windows, we also install external libraries
# if(WIN32)
#   mdt_install_external_libraries_files()
# endif()

#################################################################################################################################################
# Unit tests
#################################################################################################################################################


include(MdtDependenciesUtils)
mdt_copy_binary_dependencies(
  TARGET mdterrortest
  DESTINATION_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
mdt_install_binary_dependencies(
  TARGET Error
)


# On Windows, we have to copy external libraries to the test runtime directory
# if(WIN32)
#   include(MdtDependenciesUtils)
#   mdt_copy_binary_dependencies(
#     TARGET mdterrortest
#     DESTINATION_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
#   )
#   mdt_install_binary_dependencies(
#     TARGET Error
#   )
# 
# endif()

# TODO should try to not specify the complete dll name
# if(WIN32)
#   mdt_copy_external_libraries_files(DESTINATION_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
#   set(CMAKE_FIND_LIBRARY_SUFFIXES .dll)
#   mdt_find_and_copy_libraries(
#     LIBRARIES
#       libstdc++-6.dll libwinpthread-1.dll libgcc_s_sjlj-1.dll libeay32.dll zlib1.dll
#       ssleay32.dll libharfbuzz-0.dll libpng16-16.dll libpcre2-16-0.dll libfreetype-6.dll libglib-2.0-0.dll 
#       libbz2.dll libintl-8.dll libpcre-1.dll libiconv-2.dll
#       Qt5Test.dll
#     DESTINATION_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
#   )
# endif()

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
# Intall rules to ship Mdt CMake helper modules
#################################################################################################################################################

install(
  DIRECTORY "${PROJECT_SOURCE_DIR}/cmake/modules"
  DESTINATION share/cmake
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

# On Windows, we add external libraries to the distribution


if(UNIX)
  set(CPACK_GENERATOR TBZ2)
  set(CPACK_SOURCE_GENERATOR TBZ2)
endif()
if(WIN32)
#   set(CPACK_GENERATOR NSIS)
  set(CPACK_SOURCE_GENERATOR ZIP)
endif()

# Make proprties defined for CPack availabe as variables in current scope
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

include(CPack)
