
# List of source files
set(SOURCE_FILES
  Mdt/ItemEditor/Debug.cpp
  Mdt/ItemEditor/RowState.cpp
  Mdt/ItemEditor/ItemDelegateProxy.cpp
  Mdt/ItemEditor/EventCatchItemDelegate.cpp
  Mdt/ItemEditor/ItemSelectionModel.cpp
  Mdt/ItemEditor/ControllerState.cpp
  Mdt/ItemEditor/ControllerEvent.cpp
  Mdt/ItemEditor/RowChangeEventDispatcher.cpp
  Mdt/ItemEditor/AbstractActionContainer.cpp
  Mdt/ItemEditor/NavigationActions.cpp
  Mdt/ItemEditor/InsertAction.cpp
  Mdt/ItemEditor/RemoveAction.cpp
  Mdt/ItemEditor/EditionActions.cpp
  Mdt/ItemEditor/ResizeToContentsAction.cpp
  Mdt/ItemEditor/AbstractControllerStatePermission.cpp
  Mdt/ItemEditor/ControllerStatePermissionProxyModel.cpp
  Mdt/ItemEditor/ControllerStateTableRow.cpp
  Mdt/ItemEditor/AbstractControllerStateTable.cpp
  Mdt/ItemEditor/ControllerStateMachine.cpp
  Mdt/ItemEditor/AbstractController.cpp
  Mdt/ItemEditor/ItemViewPrivateContainer.cpp
  Mdt/ItemEditor/ForeignKeyColumnVisibilityMap.cpp
  Mdt/ItemEditor/TableViewControllerImplementation.cpp
  Mdt/ItemEditor/TableViewControllerStatePermission.cpp
  Mdt/ItemEditor/TableViewControllerStateTable.cpp
  Mdt/ItemEditor/TableViewController.cpp
  Mdt/ItemEditor/MappedWidgetList.cpp
  Mdt/ItemEditor/WidgetEditablePropertyMap.cpp
  Mdt/ItemEditor/WidgetStyleSheet.cpp
  Mdt/ItemEditor/DataWidgetMapperItemDelegate.cpp
  Mdt/ItemEditor/DataWidgetMapper.cpp
  Mdt/ItemEditor/WidgetMapperControllerStatePermission.cpp
  Mdt/ItemEditor/WidgetMapperControllerStateTable.cpp
  Mdt/ItemEditor/WidgetMapperController.cpp
  Mdt/ItemEditor/AbstractControllerRelation.cpp
  Mdt/ItemEditor/FilterControllerRelation.cpp
  Mdt/ItemEditor/NavigationControllerRelation.cpp
  Mdt/ItemEditor/ControllerRelationList.cpp
  Mdt/ItemEditor/StandardEditorLayoutWidget.cpp
    
  Mdt/ItemEditor/AbstractEditorLayout.cpp
  Mdt/ItemEditor/AbstractEditorWidget.cpp
    
  Mdt/ItemEditor/AbstractFormWidget.cpp
  Mdt/ItemEditor/AbstractTableViewWidget.cpp
  Mdt/ItemEditor/TableViewWidget.cpp
  Mdt/ItemEditor/StandardWindow.cpp
  Mdt/ItemEditor/AbstractWindow.cpp
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Widgets)

mdt_add_library(
  NAME ItemEditor
  SOURCE_FILES ${SOURCE_FILES}
  HEADERS_DIRECTORY .
  LINK_DEPENDENCIES Error Error_Widgets FilterExpression ItemModel Qt5::Widgets
)
mdt_set_library_description(
  NAME ItemEditor
  DESCRIPTION "ItemEditor provides some additions to Qt model/view."
)
