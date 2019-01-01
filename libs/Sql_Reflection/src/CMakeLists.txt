
# List of source files
set(SOURCE_FILES
  Mdt/Sql/Schema/Reflection.cpp
)

find_package(Qt5 COMPONENTS Sql)

mdt_add_library(
  NAME Sql_Reflection
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Sql_Core Qt5::Sql
)
mdt_set_library_description(
  NAME Sql_Reflection
  DESCRIPTION "Reflection support for the Sql library."
)
target_compile_definitions(Sql_Reflection PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)
