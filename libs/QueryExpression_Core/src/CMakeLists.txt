
# List of source files
set(SOURCE_FILES
  Mdt/QueryExpression/LiteralValue.cpp
  Mdt/QueryExpression/FieldName.cpp
  Mdt/QueryExpression/EntityName.cpp
  Mdt/QueryExpression/EntityAndFieldTerminal.cpp
  Mdt/QueryExpression/SelectEntity.cpp
  Mdt/QueryExpression/EntityAndField.cpp
  Mdt/QueryExpression/SelectField.cpp
  Mdt/QueryExpression/SelectFieldList.cpp
  Mdt/QueryExpression/Terminal.cpp
  Mdt/QueryExpression/LikeExpression.cpp
  Mdt/QueryExpression/LikeExpressionRegexTransform.cpp
  Mdt/QueryExpression/SelectQuery.cpp
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
  NAME QueryExpression_Core
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Algorithm Qt5::Core;
)
target_compile_definitions(QueryExpression_Core PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME QueryExpression_Core
  DESCRIPTION "Provide some expression language for queries."
)
