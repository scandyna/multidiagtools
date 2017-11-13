
# Create a rule to install a application and its dependencies
#
# This can be used to create a package that contains a target and its dependencies.
#
# Input arguments:
#  TARGET:
#   Name of the target for which to install dependencies
#  INCLUDE_ICONS:  <--- TODO: should be a list of icons themes ? See KDE CMkae modules for example
#   If set, icons are included.
#   Note: current version simply copies all icons from Oxygen theme.
#  TRANSLATIONS (optionnal):
#   A list of translations suffixes.
#   For example, for French translations, pass fr.
#   It is also possible to specifiy a country, in the form fr_ca .
#   For each translation suffix, a file, called <TARGET>_<translation_suffix>.qm,
#   will be installed in translations subdirectory of the application.
#   This file will contains translations for used Qt libraries, for used Mdt libraries and the the application itself.
#  PREFIX_PATH (optional):
#   A list of full paths to directories where to find dependencies,
#   like Qt plugins, Qt and Mdt translations (qm files).
#   On DLL platforms, this is also used to find libraries.
#   For known libraries, like Qt5 and Mdt, passing the root of the installed library is fine.
#   Internally, searching is done in known subdirectories of each specified directory (for example bin, qt5/bin).
#   A good choise is to pass CMAKE_PREFIX_PATH .
#
# Example create a rule to install deppendencies for app,
# including icons and translations:
#  mdt_install_app(
#    TARGET app
#    INCLUDE_ICONS
#    TRANSLATIONS de fr_ca
#    PREFIX_PATH ${CMAKE_PREFIX_PATH}
#  )
#
# To only include libraries and Qt5 plugins:
#  mdt_install_app(
#    TARGET app
#    PREFIX_PATH ${CMAKE_PREFIX_PATH}
#  )
function(mdt_install_app)
  # Parse arguments
  set(options INCLUDE_ICONS)
  set(oneValueArgs TARGET)
  set(multiValueArgs TRANSLATIONS PREFIX_PATH)
  cmake_parse_arguments(VAR "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  if(VAR_UNPARSED_ARGUMENTS)
      message(FATAL_ERROR "mdt_install_app(): unexpected arguments: ${VAR_UNPARSED_ARGUMENTS}")
  endif()
  # Set our local variables and check the mandatory ones
  set(target "${VAR_TARGET}")
  if(NOT target)
    message(FATAL_ERROR "mdt_install_app(): TARGET argument is missing.")
  endif()
  set(translations ${VAR_TRANSLATIONS})
  set(prefix_path ${VAR_PREFIX_PATH})
  # Set install directories
  if(WIN32)
    set(lib_dir "bin")
  else()
    set(lib_dir "lib")
  endif()
  # Setup a clean temporary directory for dependencies
  set(app_dependencies_path "${CMAKE_CURRENT_BINARY_DIR}/MdtAppDependnecies")
  file(REMOVE_RECURSE "${app_dependencies_path}")
  
#   list(APPEND target_file_list "$<TARGET_FILE:${target}>")
  
  
  message("translations: ${translations}")
  message("prefix_path: ${prefix_path}")
  message("lib_dir: ${lib_dir}")

  message("mdtcpbindeps path: $<TARGET_FILE:mdtcpbindeps>")
  
#   install(CODE
#     "execute_process(
#       COMMAND \"$<TARGET_FILE:mdtcpbindeps>\"
#               -p \"${prefix_path}\"
#               --library-destination \"${app_dependencies_path}/${lib_dir}\"
#               --plugin-destination \"${app_dependencies_path}/plugins\"
#               --verbose 1
#               \"$<TARGET_FILE:${target}>\"
#       WORKING_DIRECTORY \"${CMAKE_BINARY_DIR}\"
#       RESULT_VARIABLE rv
#      )
#      message(\"RV: ${rv}\")
#     "
#   )
#   install(CODE "message(\"Sample install message.\")")


#   set(mdtcpbindeps_exe "")

#   get_property(target_file_name TARGET ${target} PROPERTY LOCATION)
#   get_target_property(target_file_name ${target} OUTPUT_NAME)
#   message("-> For ${target},  target_file_name: ${target_file_name}")

#   string(GENEX_STRIP $<TARGET_FILE:${target}> target_file_path)
#   message("-> For ${target},  target_file_path: ${target_file_path}")

  # Hmmmmmm
  # Maybe we have to create some helper function to extract file path of a target:
  # - Uses add_custom_command() to generate a file
  # - Read this file and extract..
  
#   set(tmp_file_path "${CMAKE_CURRENT_BINARY_DIR}/${target}_file_path")
#   file(GENERATE OUTPUT "${tmp_file_path}" CONTENT "$<TARGET_FILE:${target}>")
#   file(READ "${tmp_file_path}" target_file_path)

  

#   message("-> For ${target},  target_file_path: ${target_file_path}")
  
  # Using add_custom_target() supports generator expressions.
  # So, passing $<TARGET_FILE:${target}> is resolved correctly.
  # But, when using execute_process() this is not the case.
  # It seems that no easy and clean solution exists to get target's file path here.
#   set(target_file_path "${CMAKE_CURRENT_BINARY_DIR}/")
#   message(" -> target_file_path: ${target_file_path}")
  
#   add_custom_command(
#     OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${target}_file_path"
#     COMMAND cmake -E echo "TGT file: $<TARGET_FILE:${target}"
#     COMMAND cmake -E remove "${CMAKE_CURRENT_BINARY_DIR}/${target}_file_path"
#   )
  
  set(tmp_file_path "${CMAKE_CURRENT_BINARY_DIR}/${target}_file_path")
  file(GENERATE OUTPUT "${tmp_file_path}" CONTENT "$<TARGET_FILE:${target}>")
  configure_file("${CMAKE_SOURCE_DIR}/cmake/modules/SandboxScript.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/SandboxScript.cmake" @ONLY)
  install(SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/SandboxScript.cmake")

#   set(install_script_in "${CMAKE_CURRENT_BINARY_DIR}/install_22_23.camke.in")
#   set(install_script "${CMAKE_CURRENT_BINARY_DIR}/install_22_23.camke")
#   file(WRITE "${install_script_in}" "message(\"Target file: @$<TARGET_FILE:${target}@\")")
#   configure_file("${install_script_in}" "${install_script}")
#   install(SCRIPT "${install_script}")
  
  install(CODE
    "message(\" I tmp file: ${tmp_file_path}\")
     file(READ \"${tmp_file_path}\" target_file_path)
     message(\"-> For ${target},  target_file_path: ${target_file_path}\")
     execute_process(
      COMMAND \"${mdtcpbindeps_exe}\"
              \"-p\" \"${prefix_path}\"
              \"--library-destination\" \"${app_dependencies_path}/${lib_dir}\"
              \"--plugin-destination\" \"${app_dependencies_path}/plugins\"
              \"--verbose\" \"1\"
              \"${target_file_path}>\"
      WORKING_DIRECTORY \"${CMAKE_BINARY_DIR}\"
      RESULT_VARIABLE cpresult
    )"
#     "COMMAND message(\"cpresult: ${cpresult}\")"
  )
  install(CODE "message(\"Sample install message.\")")

  message("Cur dir: ${CMAKE_CURRENT_BINARY_DIR}")
  
#   find_program(mdtcpbindeps_exe NAME mdtcpbindeps PATHS "${CMAKE_CURRENT_BINARY_DIR}" NO_DEFAULT_PATH NO_CMAKE_PATH)
#   message("mdtcpbindeps_exe: ${mdtcpbindeps_exe}")
  
#   execute_process(
#     COMMAND "${mdtcpbindeps_exe}"
#             "-p" "${prefix_path} "
#             "--library-destination" "${app_dependencies_path}/${lib_dir}"
#             "--plugin-destination" "${app_dependencies_path}/plugins"
#             "--verbose" "1"
#             "$<TARGET_FILE:${target}>"
#     WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
#     RESULT_VARIABLE cpresult
#   )
#   message("cpresult: ${cpresult}")

#   execute_process(
#     COMMAND "/bin/ls" "-l"
#     RESULT_VARIABLE lsresult
#   )
#   message("lsresult: ${lsresult}")
  
#   execute_process(
#     COMMAND "/bin/ls" "-l"
#     RESULT_VARIABLE lsresult
#   )
#   message("lsresult: ${lsresult}")
  
  install(CODE
    "execute_process(
      COMMAND \"ls -l\"
     )
    "
  )
  
  # Create a new target that depends install
#   get_filename_component(destination_name "${library_destination}" NAME)
#   set(bin_deps_target "${destination_name}_bin_deps")
  
#   set(target_dependencies ${target}_dependencies)
#   message("target_dependencies: ${target_dependencies}")
#   add_custom_target(
#     "${target_dependencies}"
#     COMMAND mdtcpbindeps
#             -p "${prefix_path}"
#             --library-destination "${app_dependencies_path}/${lib_dir}"
#             --plugin-destination "${app_dependencies_path}/plugins"
#             --verbose 1
#             "$<TARGET_FILE:${target}>"
#     WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
#     VERBATIM
#   )
#   add_dependencies(install "${target_dependencies}")

endfunction()

# TODO try to make depend on install, and check with CPack
