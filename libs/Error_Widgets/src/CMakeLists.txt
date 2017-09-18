# Project file for Mdt Error_Widgets

find_package(Qt5 COMPONENTS Widgets)

# set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCE_FILES
    Mdt/ErrorDialog.cpp
)

mdt_add_library(
  NAME Error_Widgets
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Error_Core Qt5::Widgets
)
mdt_set_library_description(
  NAME Error_Widgets
  DESCRIPTION "Error helps for error messages passing, displaying and logging."
)
