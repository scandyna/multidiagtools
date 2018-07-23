
# List of source files
set(SOURCE_FILES
  Mdt/QueryExpression/SqlTransform.cpp
  Mdt/QueryExpression/SqlSelectQuery.cpp
  Mdt/QueryExpression/SqlSelectQueryFactory.cpp
)

find_package(Qt5 COMPONENTS Core Sql)

mdt_add_library(
  NAME QueryExpression_Sql
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES QueryExpression_Core Algorithm Sql_Core Qt5::Core Qt5::Sql;
)
target_compile_definitions(QueryExpression_Sql PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME QueryExpression_Sql
  DESCRIPTION "Provide some expression language for queries."
)
