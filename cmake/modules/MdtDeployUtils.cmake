
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
# Example create a rule to install a app,
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

  # To make all work with CPack,
  # copy dependencies to a temporary directory,
  # then create rules to install dependencies.
  set(app_dependencies_path "${CMAKE_CURRENT_BINARY_DIR}/MdtAppDependnecies")
  file(REMOVE_RECURSE "${app_dependencies_path}")
  set(bin_tmp_path "${app_dependencies_path}/bin")
  set(library_tmp_path "${app_dependencies_path}/${lib_dir}")
  set(plugin_tmp_path "${app_dependencies_path}/plugins")
  set(translation_tmp_path "${app_dependencies_path}/translations")

  # Get projects translations (see MdtAddTranslations module)
  set(project_qm_list_file "${CMAKE_BINARY_DIR}/projectTranslationQmList")
  if(EXISTS "${project_qm_list_file}")
    file(READ "${project_qm_list_file}" project_qm_files)
  endif()

  foreach(f ${project_qm_files})
    message("mdt_install_app(): QM: ${f}")
  endforeach()
  message("mdt_install_app(): LD_LIBRARY_PATH: $ENV{LD_LIBRARY_PATH}")

  mdt_install_copy_targets_dependencies(
    TARGETS ${target}
    TRANSLATIONS ${translations}
    PROJECT_QM_FILES ${project_qm_files}
    PREFIX_PATH ${prefix_path}
    LIBRARY_DESTINATION "${library_tmp_path}"
    PLUGIN_DESTINATION "${plugin_tmp_path}"
    TRANSLATION_DESTINATION "${translation_tmp_path}"
  )

  # Write a qt.conf file
  set(qt_conf_file_path "${bin_tmp_path}/qt.conf")
  file(WRITE "${qt_conf_file_path}" "[Paths]\nPrefix=..\n")

  # Create a install rule for this new target
  # Note: if(EXISTS) is evaluated at generation time, not at install time,
  #       so we better check passed arguments to check if installation of some parts is required.
  set_target_properties(${target} PROPERTIES INSTALL_RPATH "\$ORIGIN/../${lib_dir}")
  install(
    TARGETS ${target}
    DESTINATION "bin"
  )
  install(
    DIRECTORY "${library_tmp_path}"
    DESTINATION "."
  )
  install(
    DIRECTORY "${plugin_tmp_path}"
    DESTINATION "."
  )
  if(translations)
    install(
      DIRECTORY "${translation_tmp_path}"
      DESTINATION "."
    )
  endif()
  install(
    FILES "${qt_conf_file_path}"
    DESTINATION "bin"
  )
endfunction()

# Copy dependencies of a list of targets to a directory at install time
#
# Note that the copy is not done when this function is called.
# Behind the scene, a install script is called at install time.
#
# Note: to prevent failures during the copy, only call this function once per destination directory.
#
# Input arguments:
#  TARGETS:
#   List containing names of the targets for which to copy dependencies
#  TRANSLATIONS (optionnal):
#   A list of translations suffixes.
#   For example, for French translations, pass fr.
#   It is also possible to specifiy a country, in the form fr_ca .
#   For each translation suffix and each target, a file, called <TARGET>_<translation_suffix>.qm,
#   will be installed in <TRANSLATION_DESTINATION>.
#   This file will contain translations for used Qt libraries, for used Mdt libraries and the the target itself.
#  PROJECT_QM_FILES (optional):
#   A list of full path to QM files for the project.
#   Those files will also be included to the final QM file, regarding specified translations.
#  PREFIX_PATH (optional):
#   A list of full paths to directories where to find dependencies,
#   like Qt plugins, Qt and Mdt translations (qm files).
#   On DLL platforms, this is also used to find libraries.
#   For known libraries, like Qt5 and Mdt, passing the root of the installed library is fine.
#   Internally, searching is done in known subdirectories of each specified directory (for example bin, qt5/bin).
#   A good choise is to pass CMAKE_PREFIX_PATH .
#  LIBRARY_DESTINATION:
#   Full path to the destination of libraries.
#  PLUGIN_DESTINATION:
#   Full path to the destination of plugins.
#  TRANSLATION_DESTINATION:
#   Full path to the destination of translations.
#
function(mdt_install_copy_targets_dependencies)
  # Parse arguments
  set(options)
  set(oneValueArgs LIBRARY_DESTINATION PLUGIN_DESTINATION TRANSLATION_DESTINATION)
  set(multiValueArgs TARGETS TRANSLATIONS PROJECT_QM_FILES PREFIX_PATH)
  cmake_parse_arguments(VAR "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  if(VAR_UNPARSED_ARGUMENTS)
      message(FATAL_ERROR "mdt_install_copy_targets_dependencies(): unexpected arguments: ${VAR_UNPARSED_ARGUMENTS}")
  endif()
  # Set our local variables and check the mandatory ones
  set(targets "${VAR_TARGETS}")
  if(NOT target)
    message(FATAL_ERROR "mdt_install_copy_targets_dependencies(): expected at least 1 target.")
  endif()
  set(translations ${VAR_TRANSLATIONS})
  set(project_qm_files ${VAR_PROJECT_QM_FILES})
  set(prefix_path ${VAR_PREFIX_PATH})
  set(library_destination ${VAR_LIBRARY_DESTINATION})
  if(NOT library_destination)
    message(FATAL_ERROR "mdt_install_copy_targets_dependencies(): LIBRARY_DESTINATION argument is missing.")
  endif()
  set(plugin_destination ${VAR_PLUGIN_DESTINATION})
  if(NOT plugin_destination)
    message(FATAL_ERROR "mdt_install_copy_targets_dependencies(): PLUGIN_DESTINATION argument is missing.")
  endif()
  set(translation_destination ${VAR_TRANSLATION_DESTINATION})
  if(NOT translation_destination)
    message(FATAL_ERROR "mdt_install_copy_targets_dependencies(): TRANSLATION_DESTINATION argument is missing.")
  endif()

  # Find mdtcpbindeps tool
  # Maybe we are compiling Mdt, in which case it is a target,
  # else we have to find it as a imported executable
  if(TARGET mdtcpbindeps)
    set(mdtcpbindeps_exe ${mdtcpbindeps})
  else()
    find_package(mdtcpbindeps REQUIRED)
    get_target_property(mdtcpbindeps_exe mdtcpbindeps LOCATION)
  endif()

  if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/modules/MdtInstallCopyTargetsDependenciesScript.cmake.in")
    set(install_script_in "${CMAKE_SOURCE_DIR}/cmake/modules/MdtInstallCopyTargetsDependenciesScript.cmake.in")
  else()
    find_file(install_script_in "MdtInstallCopyTargetsDependenciesScript.cmake.in" PATHS "${MDT_CMAKE_MODULE_PATH}")
  endif()

  # Using add_custom_target() supports generator expressions.
  # So, passing $<TARGET_FILE:${target}> is resolved correctly.
  # But, there is no way I found to trigger it at install time,
  # making dependencies resolutions at each build.
  # One solution is install(CODE execute_process()),
  # but execute_process() do not resolve generator expressions.
  # A solution is to use file(GENERATE), which supports generator expressions,
  # to create a file that contains the full path to the target,
  # then generate a script that reads its content.
  set(target_path_files)
  foreach(target ${targets})
    set(target_path_file "${CMAKE_CURRENT_BINARY_DIR}/${target}_path_file")
    file(GENERATE OUTPUT "${target_path_file}" CONTENT "$<TARGET_FILE:${target}>")
    list(APPEND target_path_files "${target_path_file}")
  endforeach()

  configure_file("${install_script_in}" "${CMAKE_CURRENT_BINARY_DIR}/MdtInstallCopyTargetsDependenciesScript.cmake" @ONLY)
  install(SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/MdtInstallCopyTargetsDependenciesScript.cmake")

endfunction()
