
find_package(Qt5 COMPONENTS Widgets)

# set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCE_FILES
  qtsingleapplication.cpp
)

mdt_add_library(
  NAME QtSingleApplication_Widgets
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES QtSingleApplication_Core Qt5::Widgets
)
target_compile_definitions(QtSingleApplication_Widgets PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII)

mdt_set_library_description(
  NAME QtSingleApplication_Widgets
  DESCRIPTION "QtSingleApplication helps to run a application only once if needed."
)
