#.rst:
# MdtPostInstall
# --------------
#
# Run some tasks that can be done only once Mdt library is installed.
#
# This script has only sense when using a Qt library that is installed
# on a location not known by the environnment (LD_LIBRARY_PATH, PATH).
#
# On Linux, path to the used Qt installation is set to the RPATH of Mdt libraries.
# On Windows, mdtenv.bat script is generated.
# For all platforms, QtPath.conf is also created.
#

message("Hello !")

if(UNIX)
  message("Unix")
endif(UNIX)

if(WIN32)
  message("Windows")
endif(WIN32)

message("CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
message("CMAKE_CURRENT_LIST_DIR: ${CMAKE_CURRENT_LIST_DIR}")

# Set some CMake variable that are not available in script mode
# TODO should be generated at compile time
set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".so")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

# On Linux, we will need patchelf later
if(UNIX AND NOT APPLE)
  find_program(PATCHELF_EXECUTABLE NAMES patchelf)
  if(NOT EXISTS "${PATCHELF_EXECUTABLE}")
    message(FATAL_ERROR "Could not find patchelf")
  endif()
endif()

# Check that we are in a Mdt installation directory and set Mdt path
# Note: add_library(), which may be used by find_package(), is not scriptable
set(MDT_PATH "${CMAKE_CURRENT_LIST_DIR}")
set(MDT_LIB_PATH "${MDT_PATH}/lib")
if(NOT EXISTS "${MDT_LIB_PATH}")
  message(FATAL_ERROR "This script must be located at the root of the Mdt installation (could not find lib directory)")
endif()
find_library(MDT_TEST_LIBRARY NAMES Mdt0Application_Core PATHS "${MDT_LIB_PATH}" NO_DEFAULT_PATH)
if(NOT EXISTS "${MDT_TEST_LIBRARY}")
  message(FATAL_ERROR "This script must be located at the root of the Mdt installation (could not find Mdt0Application_Core library)")
endif()
set(MDT_BIN_PATH "${MDT_PATH}/bin")
if(NOT EXISTS "${MDT_BIN_PATH}")
  message(FATAL_ERROR "This script must be located at the root of the Mdt installation (could not find bin directory)")
endif()

# Check that QT_PREFIX_PATH was set and that it points to a Qt installation
if(NOT QT_PREFIX_PATH)
  message(FATAL_ERROR "QT_PREFIX_PATH is missing, please specify it with -D QT_PREFIX_PATH=\"/path/to/Qt\" (Example: -D QT_PREFIX_PATH=\"/opt/Qt5/5.9.1/gcc_64\")")
endif()
get_filename_component(QT_PATH "${QT_PREFIX_PATH}" ABSOLUTE)
set(QT_LIB_PATH "${QT_PATH}/lib")
if(NOT EXISTS "${QT_LIB_PATH}")
  message(FATAL_ERROR "Specified QT_PREFIX_PATH is not a Qt installation (could not find lib directory)")
endif()
find_library(QT_TEST_LIBRARY NAMES Qt5Core PATHS "${QT_LIB_PATH}" NO_DEFAULT_PATH)
if(NOT EXISTS "${QT_TEST_LIBRARY}")
  message(FATAL_ERROR "Specified QT_PREFIX_PATH is not a Qt installation (could not find Qt5Core library)")
endif()
set(QT_BIN_PATH "${QT_PATH}/bin")
if(NOT EXISTS "${QT_BIN_PATH}")
  message(FATAL_ERROR "Specified QT_PREFIX_PATH is not a Qt installation (could not find bin directory)")
endif()
find_program(QMAKE_EXECUTABLE NAMES qmake PATHS "${QT_BIN_PATH}" NO_DEFAULT_PATH)
if(NOT EXISTS "${QMAKE_EXECUTABLE}")
  message(FATAL_ERROR "Specified QT_PREFIX_PATH is not a Qt installation (could not find qmake)")
endif()

message("MDT_LIB_PATH: ${MDT_LIB_PATH}")
message("QT_LIB_PATH: ${QT_LIB_PATH}")

# On Linux, patch RPATH of Mdt libraries to find Qt
if(UNIX AND NOT APPLE)
  message(STATUS "Patching Mdt libraries")
  file(GLOB mdt_libraries LIST_DIRECTORIES false "${MDT_LIB_PATH}/libMdt*.so.[0-9].[0-9].[0-9]")
  foreach(mdt_library ${mdt_libraries})
    # Get current RPATH
    execute_process(COMMAND "${PATCHELF_EXECUTABLE}" "--print-rpath" "${mdt_library}" OUTPUT_VARIABLE rpath_raw RESULT_VARIABLE retval ERROR_VARIABLE error_string)
    if( NOT (${retval} EQUAL 0) )
      message(FATAL_ERROR "Running patchelf failed: ${error_string}")
    endif()
    string(STRIP "${rpath_raw}" rpath)
    # Update with Qt location
    set(rpath "${rpath}:${QT_LIB_PATH}")
    execute_process(COMMAND "${PATCHELF_EXECUTABLE}" "--set-rpath" "${rpath}" "${mdt_library}" OUTPUT_VARIABLE rpath_raw RESULT_VARIABLE retval ERROR_VARIABLE error_string)
    if( NOT (${retval} EQUAL 0) )
      message(FATAL_ERROR "Running patchelf failed: ${error_string}")
    endif()
  endforeach()
endif()

# Write the QtPrefixPath file
message(STATUS "Writing QtPrefixPath.txt")
file(WRITE "${MDT_PATH}/QtPrefixPath.txt" "${QT_PATH}")

# On Windows, write mdtenv.bat
if(WIN32)
  # Find the root of the compiler
  get_filename_component(compiler_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
  # Generate env script
  string(REPLACE "/" "\\" compiler_bin_dir_win "${compiler_bin_dir}")
  string(REPLACE "/" "\\" qt_bin_dir_win "${QT_BIN_PATH}")
  string(REPLACE "/" "\\" mdt_bin_dir_win "${MDT_BIN_PATH}")
  set(env_script_content "\@echo off\r\nset PATH=${compiler_bin_dir_win};${qt_bin_dir_win};${mdt_bin_dir_win}")
  # Write env script
  message(STATUS "Writing mdtenv.bat")
  set(env_script_path "${MDT_PATH}/mdtenv.bat")
  file(WRITE "${env_script_path}" "${env_script_content};%PATH%")
endif(WIN32)

