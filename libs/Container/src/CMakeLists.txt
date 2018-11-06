
# List of source files
set(SOURCE_FILES
  Mdt/Container/StlContainer.cpp
  Mdt/Container/StlContainerIteratorAdapter.cpp
  Mdt/Container/Vector.cpp
  Mdt/Container/RowColumnListBase.cpp
  Mdt/Container/ColumnList.cpp
  Mdt/Container/RowList.cpp
  Mdt/Container/TableCacheTransaction.cpp
  Mdt/Container/TableCacheRowTransaction.cpp
  Mdt/Container/TableCacheRowTransactionList.cpp
  Mdt/Container/TableCacheOperationIndex.cpp
  Mdt/Container/TableCacheOperationMap.cpp
  Mdt/Container/TableCache.cpp
  Mdt/Container/VariantRecord.cpp
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
  NAME Container
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES IndexRange Qt5::Core
)
target_compile_definitions(Container PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME Container
  DESCRIPTION "Basic container library."
)
