
# List of source files
set(SOURCE_FILES
  Mdt/Sql/SQLiteOpenMode.cpp
  Mdt/Sql/SQLiteConnectionParameters.cpp
  Mdt/Sql/SQLiteDatabase.cpp
  Mdt/Sql/SQLiteAsyncQueryThreadWorker.cpp
  Mdt/Sql/SQLiteAsyncQueryConnection.cpp
)

find_package(Qt5 COMPONENTS Sql)

mdt_add_library(
  NAME Sql_Core_SQLite
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Error_Core Sql_Core Qt5::Sql
)
mdt_set_library_description(
  NAME Sql_Core_SQLite
  DESCRIPTION "SQLite support for the Sql library."
)
target_compile_definitions(Sql_Core_SQLite PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)
