
set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)

mdt_add_library_translations(
  TARGET QueryExpression_Core
  SOURCES_DIRECTORY ../src
  TS_FILES_DIRECTORY .
)
