include(MdtCPackComponent)

# Add rules to generate translation files
# Translation files will be generated (or updated) in the directory speicified by TS_FILES_DIRECTORY argument,
# scanning the sources files in the directory specified by SOURCES_DIRECTORY argument.
# Input arguments:
#  NAME:
#   Name of the library, without any prefix (like project name) or suffix (like version).
#   This is th same name that has been passed to mdt_add_library()
#  SOURCES_DIRECTORY:
#   Relative path to the directory containing source files
#  TS_FILES_DIRECTORY:
#   Relative path to the directory into which translations files are generated/updated (TS files)
# Global variables that are also used:
#  TRANSLATION_LANGUAGES:
#   List of supported languages, in the form en de fr. Should be defined in the project's top level CMakeLists.txt
# PROJECT_NAME: used to define installation path
#
# Recommanded usage in Mdt when creating a component
#
# File tree structure:
# libs
#  |_MyComponent
#        |_src
#        |_translations
#                |_CMakeLists.txt
#
# The CMakeLists.txt for translations should look like:
# set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)
# mdt_add_translations(
#   NAME MyComponent
#   SOURCES_DIRECTORY ../src
#   TS_FILES_DIRECTORY .
# )
#
# The call of set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1) avoid that TS files to be cleared with make clean
function(mdt_add_translations)
  # Parse arguments
  set(oneValueArgs NAME SOURCES_DIRECTORY TS_FILES_DIRECTORY)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "" ${ARGN})
  # Set our local variables and check the mandatory ones
  set(name ${VAR_NAME})
  if(NOT name)
    message(FATAL_ERROR "NAME argument is missing.")
  endif()
  set(sources_directory ${VAR_SOURCES_DIRECTORY})
  if(NOT sources_directory)
    message(FATAL_ERROR "SOURCES_DIRECTORY argument is missing.")
  endif()
  set(ts_files_directory ${VAR_TS_FILES_DIRECTORY})
  if(NOT ts_files_directory)
    message(FATAL_ERROR "TS_FILES_DIRECTORY argument is missing.")
  endif()
  # Find transaltion helpers provided by Qt5
  find_package(Qt5
    OPTIONAL_COMPONENTS LinguistTools
  )
  if(!${Qt5LinguistTools_FOUND})
    return()
  endif()
  # Build pathes
  get_filename_component(sources_path "${sources_directory}" ABSOLUTE)
  get_filename_component(ts_files_path "${ts_files_directory}" ABSOLUTE)
  set(ts_files)
  foreach(tl ${TRANSLATION_LANGUAGES})
    list(APPEND ts_files "${ts_files_path}/${name}_${tl}.ts")
  endforeach()
  # Add the rules to generate translations
  qt5_create_translation(qm_files "${sources_path}" ${ts_files})
  add_custom_target(${name}Translations ALL DEPENDS ${qm_files})
  # Add the rule to install translations
  install(
    FILES ${qm_files}
    DESTINATION share/${PROJECT_NAME}/translations/
    COMPONENT ${name}-l10n
  )
  mdt_cpack_add_library_component(
    COMPONENT ${name}-l10n
    EXPLICIT_DEPEND_COMPONENTS ${name}
  )
endfunction()
