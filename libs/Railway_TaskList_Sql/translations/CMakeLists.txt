
set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)

mdt_add_library_translations(
  TARGET Railway_TaskList_Sql
  SOURCES_DIRECTORY ../src
  TS_FILES_DIRECTORY .
)
