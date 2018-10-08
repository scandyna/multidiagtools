include(MdtAddSqlPlugin)

# List of source files
set(SOURCE_FILES
  Mdt/Sql/SQLiteErrorDriver.cpp
  Mdt/Sql/SQLiteErrorDriverPlugin.cpp
)

find_package(Qt5 COMPONENTS Sql)

mdt_add_sql_plugin(
  NAME ErrorDriverSQLite
  SOURCE_FILES ${SOURCE_FILES}
)
target_include_directories(ErrorDriverSQLite
  PRIVATE
    "${SQLITE_SOURCE_DIR}"
)
target_compile_definitions(ErrorDriverSQLite
  PRIVATE
    QT_NO_CAST_FROM_ASCII
    QT_NO_CAST_TO_ASCII
    QT_NO_CAST_FROM_BYTEARRAY
)

mdt_set_library_description(
  NAME ErrorDriverSQLite
  DESCRIPTION "SQLite implemetation of Mdt::Sql::ErrorDriver."
)
