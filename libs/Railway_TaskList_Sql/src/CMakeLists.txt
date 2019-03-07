
# List of source files
set(SOURCE_FILES
  Mdt/Railway/TaskList/SqlSchema.cpp
  Mdt/Railway/TaskList/SqlVehicleTypeRepository.cpp
)

find_package(Qt5 COMPONENTS Core Sql)

mdt_add_library(
  NAME Railway_TaskList_Sql
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Numeric Sql_Core Sql_Reflection Error_Core Expected Railway_TaskList_Core Qt5::Core Qt5::Sql;
)
target_compile_definitions(Railway_TaskList_Sql PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME Railway_TaskList_Sql
  DESCRIPTION "Railway task list."
)
