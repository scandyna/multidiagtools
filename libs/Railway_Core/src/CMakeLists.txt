
# List of source files
set(SOURCE_FILES
  Mdt/Railway/Entity/VehicleTypeClass.cpp
  Mdt/Railway/Entity/VehicleType.cpp
  Mdt/Railway/VehicleTypeClassData.cpp
  Mdt/Railway/VehicleTypeClassDataList.cpp
  Mdt/Railway/VehicleTypeClassTableModel.cpp
  Mdt/Railway/VehicleTypeClassEdition.cpp
  Mdt/Railway/VehicleTypeData.cpp
  Mdt/Railway/VehicleTypeModel.cpp
  Mdt/Railway/VehicleTypeClassRepository.cpp
  Mdt/Railway/VehicleTypeEdition.cpp
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
  LINK_DEPENDENCIES ItemModel Qt5::Core Entity_Core Error_Core;
)
target_compile_definitions(Railway_Core PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME Railway_Core
  DESCRIPTION "Provide some facility for railway vehicle entities."
)
