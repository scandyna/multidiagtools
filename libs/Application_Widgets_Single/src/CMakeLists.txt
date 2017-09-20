
find_package(Qt5 COMPONENTS Widgets)

# set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCE_FILES
  Mdt/SingleApplication.cpp
)

mdt_add_library(
  NAME Application_Widgets_Single
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Application_Core QtSingleApplication_Widgets Qt5::Widgets
)
target_compile_definitions(Application_Widgets_Single PRIVATE QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_BYTEARRAY)

mdt_set_library_description(
  NAME Application_Widgets_Single
  DESCRIPTION "Helper, using QtSingleApplication, to create a single instance application main"
)
