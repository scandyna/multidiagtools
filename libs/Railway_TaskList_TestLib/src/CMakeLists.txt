
# List of source files
set(SOURCE_FILES
  Mdt/Railway/TaskList/TestLib/ResponseHandler.cpp
  Mdt/Railway/TaskList/TestLib/VehicleTypeRepositoryTestBase.cpp
  Mdt/Railway/TaskList/TestLib/VehicleTypeRepositoryResponseHandler.cpp
)

find_package(Qt5 COMPONENTS Core Test)

mdt_add_library(
  NAME Railway_TaskList_TestLib
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Railway_TaskList_Core Qt5::Test;
)
# target_compile_definitions(Railway_TaskList_TestLib PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME Railway_TaskList_TestLib
  DESCRIPTION "Railway task list test lib."
)
