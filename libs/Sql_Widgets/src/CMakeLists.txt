
# List of source files
set(SOURCE_FILES 
  Mdt/Sql/SQLiteDatabaseFileDialog.cpp
)

find_package(Qt5 COMPONENTS Widgets)

mdt_add_library(
  NAME Sql_Widgets
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Sql_Core Error_Widgets Qt5::Widgets
)
mdt_set_library_description(
  NAME Sql_Widgets
  DESCRIPTION "Sql provides additions for database (schema, ...)."
)
target_compile_definitions(Sql_Widgets PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)
