
set(SOURCE_FILES
  qtlocalpeer.cpp
  qtsingleapplication.cpp
)

find_package(Qt5
  COMPONENTS Widgets Network
)

mdt_add_library(
  NAME QtSingleApplication
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Qt5::Widgets Qt5::Network
)

mdt_set_library_description(
  NAME QtSingleApplication
  DESCRIPTION "QtSingleApplication helps to run a application only once if needed."
)
