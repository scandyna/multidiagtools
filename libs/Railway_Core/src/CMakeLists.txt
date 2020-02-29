
# List of source files
set(SOURCE_FILES
  Mdt/Railway/AbstractEntityRepository.cpp
  Mdt/Railway/Entity/VehicleTypeClass.cpp
  Mdt/Railway/Entity/VehicleType.cpp
  Mdt/Railway/VehicleTypeClassData.cpp
  Mdt/Railway/VehicleTypeClassDataValidator.cpp
  Mdt/Railway/CreateVehicleTypeClass.cpp
  Mdt/Railway/UpdateVehicleTypeClass.cpp
  Mdt/Railway/RemoveVehicleTypeClass.cpp
  Mdt/Railway/VehicleTypeClassDataList.cpp
  Mdt/Railway/VehicleTypeClass.cpp
  Mdt/Railway/VehicleTypeClassTableModel.cpp
  Mdt/Railway/EditVehicleTypeClassCache.cpp
  Mdt/Railway/EditVehicleTypeClassCacheAsync.cpp
  Mdt/Railway/EditVehicleTypeClassTableModel.cpp
  Mdt/Railway/EditVehicleTypeClass.cpp
  Mdt/Railway/VehicleTypeId.cpp
  Mdt/Railway/VehicleTypeData.cpp
  Mdt/Railway/VehicleTypeDataValidator.cpp
  Mdt/Railway/VehicleTypeModel.cpp
  Mdt/Railway/VehicleTypeClassRepository.cpp
  Mdt/Railway/VehicleTypeEdition.cpp
  Mdt/Railway/EditVehicleTypeTableModel.cpp
  Mdt/Railway/ChooseVehicleTypeClassCache.cpp
  Mdt/Railway/ChooseVehicleTypeCache.cpp
  Mdt/Railway/VehicleTypeRepository.cpp
  Mdt/Railway/CreateVehicleType.cpp
  Mdt/Railway/UpdateVehicleType.cpp
  Mdt/Railway/VehicleTypeClassCommonError.cpp
)

find_package(Qt5 COMPONENTS Core)

# Get Boost include directory if not allready set
# We only use header onyl libraries, so we not use COMPONENTS
# See: http://stackoverflow.com/questions/26749461/how-to-determine-the-boost-library-names-for-cmake
# if(NOT Boost_FOUND)
#   find_package(Boost REQUIRED)
#   if(Boost_FOUND)
#     include_directories(${Boost_INCLUDE_DIRS})
#   endif(Boost_FOUND)
# endif()

mdt_add_library(
  NAME Railway_Core
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES ItemModel Qt5::Core Error_Core Expected QueryExpression_Reflection Entity_Core; # TODO remove Entity_Core when possible
)
target_compile_definitions(Railway_Core PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME Railway_Core
  DESCRIPTION "Provide some facility for railway vehicle entities."
)
