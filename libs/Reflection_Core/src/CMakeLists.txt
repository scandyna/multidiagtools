
# List of source files
set(SOURCE_FILES
  Mdt/Reflection/TypeTraits/IsStructDef.cpp
  Mdt/Reflection/TypeTraits/IsField.cpp
  Mdt/Reflection/TypeTraits/IsFieldAssociatedWithReflectedStruct.cpp
  Mdt/Reflection/TypeTraits/IsPrimaryKey.cpp
  Mdt/Reflection/TypeTraits/IsAutoIncrementIdPrimaryKey.cpp
  Mdt/Reflection/TypeTraits/IsIdPrimaryKey.cpp
  Mdt/Reflection/TypeTraits/IsPrimaryKeyClass.cpp
  Mdt/Reflection/TypeTraits/IsUniqueConstraint.cpp
  Mdt/Reflection/Impl/AddFieldNameToList.cpp
  Mdt/Reflection/ReflectField.cpp
  Mdt/Reflection/ReflectStruct.cpp
  Mdt/Reflection/FieldFlags.cpp
  Mdt/Reflection/FieldMaxLength.cpp
  Mdt/Reflection/FieldAttributes.cpp
  Mdt/Reflection/FieldAlgorithm.cpp
  Mdt/Reflection/StructAlgorithm.cpp
  Mdt/Reflection/PrimaryKey.cpp
  Mdt/Reflection/IdPrimaryKey.cpp
  Mdt/Reflection/AutoIncrementIdPrimaryKey.cpp
  Mdt/Reflection/PrimaryKeyAlgorithm.cpp
  Mdt/Reflection/UniqueConstraint.cpp
  Mdt/Reflection/UniqueConstraintAlgorithm.cpp
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
  NAME Reflection_Core
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Assert Qt5::Core
)
mdt_set_library_description(
  NAME Reflection_Core
  DESCRIPTION "Reflection library."
)
