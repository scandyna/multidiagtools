
# Generate a mdtenv.bat script
function(mdt_generate_env_script)
  # Parse arguments
  set(oneValueArgs DESTINATION)
  set(multiValueArgs PREFIX_PATH)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  # Check mandatory arguments and set our local variables
  if(VAR_DESTINATION)
    set(destination "${VAR_DESTINATION}")
  else()
    set(destination "${CMAKE_BINARY_DIR}")
  endif()
  if(NOT VAR_PREFIX_PATH)
    message(FATAL_ERROR "mdt_generate_env_script(): PREFIX_PATH argument is missing.")
  endif()
  set(prefix_path ${VAR_PREFIX_PATH})
  if(NOT MDT_PREFIX_PATH)
    message(FATAL_ERROR "mdt_generate_env_script(): MDT_PREFIX_PATH variable is missing.")
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
  string(REPLACE "/" "\\" mdt_bin_dir_win "${MDT_PREFIX_PATH}/bin")
  set(env_script_content "\@echo off\r\nset PATH=${compiler_bin_dir_win};${qt_bin_dir_win};${mdt_bin_dir_win};%PATH%")
  file(WRITE "${env_script_path}" "${env_script_content}")
endif(WIN32)

endfunction()
