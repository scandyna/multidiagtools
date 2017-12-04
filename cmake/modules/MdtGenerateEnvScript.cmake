
# TODO  should be obselete

# Generate a mdtenv.bat script
function(mdt_generate_env_script)
  # Parse arguments
  set(oneValueArgs DESTINATION)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "" ${ARGN})
  # Check mandatory arguments and set our local variables
  if(VAR_DESTINATION)
    set(destination "${VAR_DESTINATION}")
  else()
    set(destination "${CMAKE_BINARY_DIR}")
  endif()

if(WIN32)
  set(env_script_path "${destination}/mdtenv.bat")
  # Find the root of the compiler
  get_filename_component(compiler_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
  # Find the root of Qt5
  find_program(qmake_bin_dir qmake)
  get_filename_component(qt_bin_dir "${qmake_bin_dir}" DIRECTORY)
  # Generate env script
  string(REPLACE "/" "\\" compiler_bin_dir_win "${compiler_bin_dir}")
  string(REPLACE "/" "\\" qt_bin_dir_win "${qt_bin_dir}")
  set(env_script_content "\@echo off\r\nset PATH=${compiler_bin_dir_win};${qt_bin_dir_win}")
  if(MDT_PREFIX_PATH)
    string(REPLACE "/" "\\" mdt_bin_dir_win "${MDT_PREFIX_PATH}/bin")
    set(env_script_content "${env_script_content};${mdt_bin_dir_win}")
  endif()
  file(WRITE "${env_script_path}" "${env_script_content};%PATH%")
endif(WIN32)

endfunction()
