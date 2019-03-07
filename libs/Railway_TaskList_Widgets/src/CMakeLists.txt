
# List of source files
set(SOURCE_FILES
  Mdt/Railway/TaskList/EditVehicleTypeWindow.cpp
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS Widgets)

mdt_add_library(
  NAME Railway_TaskList_Widgets
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES ItemModel ItemEditor_Widgets Qt5::Widgets Railway_TaskList_Core Entity_Widgets Error_Widgets;
)
target_compile_definitions(Railway_TaskList_Widgets PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME Railway_TaskList_Widgets
  DESCRIPTION "Railway task list"
)
