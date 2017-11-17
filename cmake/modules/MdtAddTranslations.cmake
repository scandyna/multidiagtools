
# Add rules to generate translation files
#
# Translation files will be generated (or updated) in the directory speicified by TS_FILES_DIRECTORY argument,
# scanning the sources files in the directory specified by SOURCES_DIRECTORY argument.
# Input arguments:
#  TARGET:
#   Name of the target for which translations must be generated.
#   This is the same name that has been passed to add_executable() or add_library()
#  SOURCES_DIRECTORY:
#   Relative path to the directory containing source files
#  TS_FILES_DIRECTORY:
#   Relative path to the directory into which translations files are generated/updated (TS files)
#  INSTALL_DESTINATION (Optional):
#   Relative path to the installed directory.
# Global variables that are also used:
#  TRANSLATION_LANGUAGES:
#   List of supported languages, in the form en de fr. Should be defined in the project's top level CMakeLists.txt
#
# Recommanded usage
#
# File tree structure:
#   MyApp
#     |_src
#     |_translations
#             |_CMakeLists.txt
#
# The CMakeLists.txt for translations should look like:
# set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)
# mdt_add_translations(
#   TARGET MyApp
#   SOURCES_DIRECTORY ../src
#   TS_FILES_DIRECTORY .
# )
#
# The call of set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1) avoid that TS files to be cleared with make clean
function(mdt_add_translations)
  # Parse arguments
  set(oneValueArgs TARGET SOURCES_DIRECTORY TS_FILES_DIRECTORY INSTALL_DESTINATION)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(target ${VAR_TARGET})
  if(NOT target)
    message(FATAL_ERROR "TARGET argument is missing.")
  endif()
  set(sources_directory ${VAR_SOURCES_DIRECTORY})
  if(NOT sources_directory)
    message(FATAL_ERROR "SOURCES_DIRECTORY argument is missing.")
  endif()
  set(ts_files_directory ${VAR_TS_FILES_DIRECTORY})
  if(NOT ts_files_directory)
    message(FATAL_ERROR "TS_FILES_DIRECTORY argument is missing.")
  endif()
  set(install_destination ${VAR_INSTALL_DESTINATION})
#   if(NOT install_destination)
#     message(FATAL_ERROR "INSTALL_DESTINATION argument is missing.")
#   endif()
  # Find transaltion helpers provided by Qt5
  # Note: without requiering Qt5Core, _qt5_Core_check_file_exists could not be found
  find_package(Qt5
    COMPONENTS Core
    OPTIONAL_COMPONENTS LinguistTools
  )
  if(!${Qt5LinguistTools_FOUND})
    message(WARNING "Qt5::LinguistTools was not found. Transalation files (.ts) will not be generated.")
    return()
  endif()
  # Build paths
  get_filename_component(sources_path "${sources_directory}" ABSOLUTE)
  get_filename_component(ts_files_path "${ts_files_directory}" ABSOLUTE)
  set(ts_files)
  foreach(tl ${TRANSLATION_LANGUAGES})
    list(APPEND ts_files "${ts_files_path}/${target}_${tl}.ts")
  endforeach()
  # Add the rules to generate translations
  mdt_create_translation(
    qm_files
    TARGET ${target}
    SOURCES_DIRECTORY ${sources_directory}
    TS_FILES ${ts_files}
  )
  add_custom_target(${target}Translations ALL DEPENDS ${qm_files})
  mdt_add_translation_qm_files_to_project_qm_list_file(
    QM_FILES ${qm_files}
  )

  # Rules to install translations
  if(install_destination)
    foreach(file ${qm_files})
      # Set language suffix which will be used for component name
      mdt_extract_language_suffix_from_qm_file(language_suffix QM_FILE "${file}")
      install(
        FILES "${file}"
        DESTINATION "${install_destination}"
        COMPONENT ${target}-l10n-${language_suffix}
      )
    endforeach()
  endif()

endfunction()

# Add a list of translations QM files to the project QM files list
#
# This is used by MdtDeployUtils module
#
function(mdt_add_translation_qm_files_to_project_qm_list_file)
  # Parse arguments
  set(oneValueArgs)
  set(multiValueArgs QM_FILES)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(qm_files ${VAR_QM_FILES})
  if(NOT qm_files)
    message(FATAL_ERROR "QM_FILES argument is missing.")
  endif()

  set(project_qm_list_file "${CMAKE_BINARY_DIR}/projectTranslationQmList")

  # Get existing QM files
  if(EXISTS "${project_qm_list_file}")
    file(READ "${project_qm_list_file}" all_qm_files)
  endif()
  # Add our QM files
  list(APPEND all_qm_files ${qm_files})
  list(REMOVE_DUPLICATES all_qm_files)
  # Save the result
  file(WRITE "${project_qm_list_file}" "${all_qm_files}")

endfunction()

# Add rules to create translation files
#
# Makes similar work than qt5_add_translation()
#
# qt5_add_translation() uses add_custom_command(),
#  and passes given sources directory as dependency.
#  This causes the ts files not to be updated when source code was edited.
#  And, this is because translation files and source files are defined
#  in different directory (different CMakeLists.txt files).
#  According to CMake add_custom_command() documentation,
#  passing a target name as dependency will work across different directories,
#  and that is what this function does.
#
# Input arguments:
#  TARGET:
#   Name of the target for which translations must be generated.
#   This is the same name that has been passed to mdt_add_library()
#  SOURCES_DIRECTORY:
#   Relative path to the directory containing source files
#  TS_FILES:
#   A list of ts files to generate or update.
function(mdt_create_translation QM_FILES)
  # Parse arguments
  set(oneValueArgs TARGET SOURCES_DIRECTORY)
  set(multiValueArgs TS_FILES)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(target ${VAR_TARGET})
  if(NOT target)
    message(FATAL_ERROR "TARGET argument is missing.")
  endif()
  set(sources_directory ${VAR_SOURCES_DIRECTORY})
  if(NOT sources_directory)
    message(FATAL_ERROR "SOURCES_DIRECTORY argument is missing.")
  endif()
  set(ts_files ${VAR_TS_FILES})
  if(NOT ts_files)
    message(FATAL_ERROR "TS_FILES argument is missing.")
  endif()

  foreach(ts_file ${ts_files})
    # make a list file to call lupdate on, so we don't make our commands too long for some systems
    get_filename_component(ts_name ${ts_file} NAME_WE)
    set(ts_lst_file "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${ts_name}_lst_file")

    get_filename_component(abs_sources_directory ${sources_directory} ABSOLUTE)
    set(source_directory_list ${abs_sources_directory})

    get_directory_property(inclue_dirs INCLUDE_DIRECTORIES)
    foreach(include_dir ${inclue_dirs})
      set(source_directory_list "-I${include_dir}\n${source_directory_list}")
    endforeach()

    file(WRITE "${ts_lst_file}" "${source_directory_list}")

    add_custom_command(
      OUTPUT ${ts_file}
      COMMAND ${Qt5_LUPDATE_EXECUTABLE}
      ARGS "@${ts_lst_file}" -ts ${ts_file}
      DEPENDS ${abs_sources_directory} ${ts_lst_file} ${target}
      VERBATIM
    )
  endforeach()
  qt5_add_translation(${QM_FILES} ${ts_files})
  set(${QM_FILES} ${${QM_FILES}} PARENT_SCOPE)
endfunction()

# Extract the language suffix from a QM file name
#
# Input arguments:
#  QM_FILE:
#   QM file generated by mdt_create_translation()
# Output:
#  LANGUAGE_SUFFIX:
#   The language suffix extracted from QM_FILE
#
function(mdt_extract_language_suffix_from_qm_file LANGUAGE_SUFFIX)
  # Parse arguments
  set(oneValueArgs QM_FILE)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(qm_file ${VAR_QM_FILE})
  if(NOT qm_file)
    message(FATAL_ERROR "QM_FILE argument is missing.")
  endif()

  get_filename_component(qm_file_name "${qm_file}" NAME_WE)
  string(LENGTH "${qm_file_name}" qm_file_len)
  math(EXPR language_begin "${qm_file_len}-2")
  string(SUBSTRING "${qm_file_name}" ${language_begin} 2 language_suffix)

  set(${LANGUAGE_SUFFIX} ${language_suffix} PARENT_SCOPE)
endfunction()