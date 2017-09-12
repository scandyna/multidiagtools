
set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)

mdt_add_translations(
  NAME Application_Core
  SOURCES_DIRECTORY ../src
  TS_FILES_DIRECTORY .
)
