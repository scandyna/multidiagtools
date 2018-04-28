

find_package(Qt5 COMPONENTS Core)

# set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCE_FILES
  Mdt/Algorithm.cpp
)

mdt_add_library(
  NAME Algorithm
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Qt5::Core
)
mdt_set_library_description(
  NAME Algorithm
  DESCRIPTION "Algorithm provides some common helpers."
)
