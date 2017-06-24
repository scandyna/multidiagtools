
set(SOURCE_FILES
  Mdt/PlainText/FileInputIterator.cpp
  Mdt/PlainText/FileInputIteratorSharedData.cpp
  Mdt/PlainText/FileMultiPassIterator.cpp
  Mdt/PlainText/StringConstIterator.cpp
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
  NAME PlainText_Core
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Error Qt5::Core
)
mdt_set_library_description(
  NAME PlainText_Core
  DESCRIPTION "PlainText_Core provides common stuff for plain text processing."
)
