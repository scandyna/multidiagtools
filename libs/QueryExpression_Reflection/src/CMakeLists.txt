
# List of source files
set(SOURCE_FILES
  Mdt/QueryExpression/ReflectionQueryField.cpp
  Mdt/QueryExpression/ReflectionSelectStatement.cpp
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
  NAME QueryExpression_Reflection
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES QueryExpression_Core Reflection_Core Qt5::Core;
)
target_compile_definitions(QueryExpression_Reflection PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME QueryExpression_Reflection
  DESCRIPTION "Provide some expression language for queries. This lib provides compatibility with Reflection lib."
)
