include(MdtAddSqlDriverPlugin)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# List of source files
set(SOURCE_FILES
  "${SQLITE_SOURCE_DIR}/sqlite3.c"
  qsql_sqlite.cpp
  smain.cpp
)

find_package(Qt5 COMPONENTS Sql)

mdt_add_sql_driver_plugin(
  NAME Mdt_QSQLite_Driver
  SOURCE_FILES ${SOURCE_FILES}
  DEPENDENCIES Qt5::Sql Qt5::SqlPrivate
)
target_include_directories(Mdt_QSQLite_Driver
  PRIVATE
    "${SQLITE_SOURCE_DIR}"
)
target_compile_definitions(Mdt_QSQLite_Driver
  PRIVATE
    SQLITE_OMIT_LOAD_EXTENSION
    SQLITE_OMIT_COMPLETE
    SQLITE_ENABLE_FTS3
    SQLITE_ENABLE_FTS3_PARENTHESIS
    SQLITE_ENABLE_FTS5
    SQLITE_ENABLE_RTREE
)
target_compile_definitions(Mdt_QSQLite_Driver
  PRIVATE
    QT_NO_CAST_FROM_ASCII
    QT_NO_CAST_TO_ASCII
    QT_NO_CAST_FROM_BYTEARRAY
)

mdt_set_library_description(
  NAME Mdt_QSQLite_Driver
  DESCRIPTION "Variant of the QSQLite driver."
)
