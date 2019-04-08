
# List of source files
set(SOURCE_FILES
  Mdt/Async/WaitDonePredicate.cpp
  Mdt/Async/Wait.cpp
)

find_package(Qt5 COMPONENTS Core)

mdt_add_library(
  NAME Async
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Qt5::Core
)
mdt_set_library_description(
  NAME Async
  DESCRIPTION "Async library."
)
target_compile_definitions(Async PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)
