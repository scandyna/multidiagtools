
# List of source files
set(SOURCE_FILES
    Mdt/ItemEditor/AbstractSqlController.cpp
    Mdt/ItemEditor/AbstractSqlTableModelController.cpp
    Mdt/ItemEditor/SqlTableViewController.cpp
    Mdt/ItemEditor/SqlTableViewWidget.cpp
    Mdt/ItemEditor/SqlTableWidgetMapperController.cpp
    Mdt/ItemEditor/AbstractSqlTableFormWidget.cpp
)

# set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Widgets)

mdt_add_library(
  NAME ItemEditor_Sql
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Error_Core Error_Widgets FilterExpression ItemModel ItemEditor Sql Qt5::Widgets
)
mdt_set_library_description(
  NAME ItemEditor_Sql
  DESCRIPTION "Provides Sql support to ItemEditor."
)
