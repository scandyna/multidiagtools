
# List of source files
set(SOURCE_FILES
  Mdt/TestLib/CompareRowList.cpp
  Mdt/TestLib/CompareColumnList.cpp
)

find_package(Qt5 COMPONENTS Core Test)

# # Get Boost include directory if not allready set
# # We only use header onyl libraries, so we not use COMPONENTS
# # See: http://stackoverflow.com/questions/26749461/how-to-determine-the-boost-library-names-for-cmake
# if(NOT Boost_FOUND)
#   find_package(Boost REQUIRED)
#   if(Boost_FOUND)
#     include_directories(${Boost_INCLUDE_DIRS})
#   endif(Boost_FOUND)
# endif()

mdt_add_library(
  NAME TestLib_Container
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES TestLib_Core Container Qt5::Core Qt5::Test
)
target_compile_definitions(TestLib_Container PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME TestLib_Container
  DESCRIPTION "TestLib provides some additions to QTestLib."
)
