
# List of source files
set(SOURCE_FILES
  Mdt/Railway/VehicleTypeClassSqlRepository.cpp
  Mdt/Railway/SqlSchema.cpp
)

find_package(Qt5 COMPONENTS Sql)

mdt_add_library(
  NAME Railway_Sql
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES ItemModel Sql_Core Entity_Sql Qt5::Sql Railway_Core;
)
target_compile_definitions(Railway_Sql PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME Railway_Sql
  DESCRIPTION "Provide some facility for railway vehicle entities (SQL module)."
)
