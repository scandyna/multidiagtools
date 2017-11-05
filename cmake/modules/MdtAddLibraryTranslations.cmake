include(MdtAddTranslations)
include(MdtCPackComponent)

# Add rules to generate translation files
#
# Translation files will be generated (or updated) in the directory speicified by TS_FILES_DIRECTORY argument,
# scanning the sources files in the directory specified by SOURCES_DIRECTORY argument.
# Input arguments:
#  TARGET:
#   Name of the target for which translations must be generated.
#   This is the same name that has been passed to mdt_add_library()
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
# mdt_add_library_translations(
#   TARGET MyComponent
#   SOURCES_DIRECTORY ../src
#   TS_FILES_DIRECTORY .
# )
#
# The call of set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1) avoid that TS files to be cleared with make clean
function(mdt_add_library_translations)
  # Parse arguments
  set(oneValueArgs TARGET SOURCES_DIRECTORY TS_FILES_DIRECTORY)
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
  # Add translations
  mdt_add_translations(
    TARGET ${target}
    SOURCES_DIRECTORY "${sources_directory}"
    TS_FILES_DIRECTORY "${ts_files_directory}"
    INSTALL_DESTINATION "share/${PROJECT_NAME}/translations/"
  )
  # Add translations to CPack
  foreach(language ${TRANSLATION_LANGUAGES})
    mdt_cpack_add_library_component(
      COMPONENT ${target}-l10n-${language}
      EXPLICIT_DEPEND_COMPONENTS ${target}
    )
  endforeach()
endfunction()
