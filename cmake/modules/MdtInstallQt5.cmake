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
  get_filename_component(cxx_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
  message("Cxx bin dir: ${cxx_bin_dir}")
  # Rule to install the compiler
  message("Install compiler to ${CMAKE_INSTALL_PREFIX}/compiler")
  install(
    DIRECTORY "${cxx_bin_dir}/.."
    DESTINATION "${CMAKE_INSTALL_PREFIX}/compiler"
    USE_SOURCE_PERMISSIONS
  )
  # Find the root of Qt5
  find_program(qmake_bin_dir qmake)
  message("Qmake bin dir: ${qmake_bin_dir}")
  # Rule to install Qt5
  message("Install Qt5 to ${CMAKE_INSTALL_PREFIX}")
  install(
    DIRECTORY "${qmake_bin_dir}/.."
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
    USE_SOURCE_PERMISSIONS
  )
  # Generate env script
  message("mdtenv.bat : ${CMAKE_INSTALL_PREFIX}/bin/mdtenv.bat")
  string(REPLACE "/" "\\" cxx_installed_bin_dir "${CMAKE_INSTALL_PREFIX}/compiler/bin")
  string(REPLACE "/" "\\" qt_installed_bin_dir "${CMAKE_INSTALL_PREFIX}/bin")
  set(env_script "\@echo off\r\nset PATH=${cxx_installed_bin_dir};${qt_installed_bin_dir};%PATH%")
  message(" -> ${env_script}")
  file(WRITE "mdtenv.bat" "${env_script}")
  install(
    PROGRAMS "mdtenv.bat"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
  )
endif()
