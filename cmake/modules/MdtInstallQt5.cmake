# Install Qt5 library to ship it with Mdt

if( QT_PREFIX_PATH AND CMAKE_INSTALL_PREFIX AND NOT ("${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr") )
  install(
    DIRECTORY "${QT_PREFIX_PATH}"
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
    USE_SOURCE_PERMISSIONS
  )
endif()

# On Windows, we mostly setup a environment with PATH
# containing the compiler and the libraries.
# Typically calling qtenv2.bat distributed by Qt
if(WIN32)
  # Find the root of the compiler
  get_filename_component(compiler_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
  # Find the root of Qt5
  find_program(qmake_bin_dir qmake)
  get_filename_component(qt_bin_dir "${qmake_bin_dir}" DIRECTORY)
  # Generate env script
  string(REPLACE "/" "\\" compiler_bin_dir_win "${compiler_bin_dir}")
  string(REPLACE "/" "\\" qt_bin_dir_win "${qt_bin_dir}")
  string(REPLACE "/" "\\" mdt_bin_dir_win "${CMAKE_INSTALL_PREFIX}/bin")
  set(env_script "\@echo off\r\nset PATH=${compiler_bin_dir_win};${qt_bin_dir_win};${mdt_bin_dir_win};%PATH%")
  file(WRITE "${CMAKE_BINARY_DIR}/mdtenv.bat" "${env_script}")
  install(
    PROGRAMS "${CMAKE_BINARY_DIR}/mdtenv.bat"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
  )
endif()
