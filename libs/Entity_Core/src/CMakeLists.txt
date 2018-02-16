
# List of source files
set(SOURCE_FILES
  Mdt/Entity/TypeTraits/IsEntityDef.cpp
  Mdt/Entity/FieldFlags.cpp
  Mdt/Entity/FieldMaxLength.cpp
  Mdt/Entity/FieldAttributes.cpp
  Mdt/Entity/FieldDef.cpp
  Mdt/Entity/Def.cpp
  Mdt/Entity/DataTemplate.cpp
  Mdt/Entity/Debug.cpp
  Mdt/Entity/ValueAt.cpp
  Mdt/Entity/FieldAt.cpp
  Mdt/Entity/DataList.cpp
  Mdt/Entity/PrimaryKeyField.cpp
  Mdt/Entity/PrimaryKey.cpp
  Mdt/Entity/ItemModelPrimaryKey.cpp
  Mdt/Entity/TableModelRecordAdapter.cpp
  Mdt/Entity/ReadOnlyTableModel.cpp
  Mdt/Entity/EditableTableModel.cpp
  Mdt/Entity/RowResizableTableModel.cpp
)

find_package(Qt5 COMPONENTS Core)

# Get Boost include directory if not allready set
# We only use header onyl libraries, so we not use COMPONENTS
# See: http://stackoverflow.com/questions/26749461/how-to-determine-the-boost-library-names-for-cmake
if(NOT Boost_FOUND)
  find_package(Boost REQUIRED)
  if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
  endif(Boost_FOUND)
endif()

mdt_add_library(
  NAME Entity_Core
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES ItemModel Qt5::Core Error_Core; # Error_Core currently only for MDT_ASSERT()
)
target_compile_definitions(Entity_Core PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME Entity_Core
  DESCRIPTION "Possible introspection of a entity."
)
