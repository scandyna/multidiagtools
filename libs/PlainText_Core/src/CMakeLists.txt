
set(SOURCE_FILES
  Mdt/PlainText/FileInputIterator.cpp
  Mdt/PlainText/FileInputIteratorSharedData.cpp
)

find_package(Qt5 COMPONENTS Core)

mdt_add_library(
  NAME PlainText_Core
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Error Qt5::Core
)
mdt_set_library_description(
  NAME PlainText_Core
  DESCRIPTION "PlainText_Core provides common stuff plain text processing."
)
