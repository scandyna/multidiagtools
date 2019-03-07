
# List of source files
set(SOURCE_FILES
  Mdt/Railway/TaskList/VehicleTypeDataStruct.cpp
  Mdt/Railway/TaskList/VehicleType.cpp
  Mdt/Railway/TaskList/VehicleType_p.cpp
  Mdt/Railway/TaskList/Place.cpp
  Mdt/Railway/TaskList/GetAllVehicleTypes.cpp
  Mdt/Railway/TaskList/CreateVehicleType.cpp
  Mdt/Railway/TaskList/VehicleTypeRepository.cpp
  Mdt/Railway/TaskList/EditVehicleTypeTableModel.cpp
)

find_package(Qt5 COMPONENTS Core)

mdt_add_library(
  NAME Railway_TaskList_Core
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Numeric Error_Core Expected Reflection_Core Railway_Core ItemModel Qt5::Core;
)
target_compile_definitions(Railway_TaskList_Core PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME Railway_TaskList_Core
  DESCRIPTION "Railway task list."
)
