
set(SOURCE_FILES
  Mdt/MemoryStore/Table.cpp
  Mdt/MemoryStore/TableWithAutoId.cpp
)

find_package(Qt5 COMPONENTS Core)

mdt_add_library(
  NAME MemoryStore
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Error_Core Expected Qt5::Core
)
mdt_set_library_description(
  NAME MemoryStore
  DESCRIPTION "Helpers to implement a memory repository."
)
