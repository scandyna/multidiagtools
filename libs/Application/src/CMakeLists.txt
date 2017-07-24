
# config.h is generated in the root of the build tree
include_directories("${CMAKE_BINARY_DIR}")

# set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5
  COMPONENTS Widgets
)

set(SOURCE_FILES
  Mdt/StandardPaths.cpp
  Mdt/Application.cpp
)

mdt_add_library(
  NAME Application
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES QtSingleApplication Error
)
mdt_set_library_description(
  NAME Application
  DESCRIPTION "Helper to create main (init Error subsystem, load translations, etc..)"
)
