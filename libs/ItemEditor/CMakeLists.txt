# Project file for Multi Diag Tools

# Project
project(multidiagtools)

# List of source files
set(SOURCES_FILES
    src/Mdt/ItemEditor/Debug.cpp
    src/Mdt/ItemEditor/RowState.cpp
    src/Mdt/ItemEditor/ItemDelegateProxy.cpp
    src/Mdt/ItemEditor/EventCatchItemDelegate.cpp
    src/Mdt/ItemEditor/ItemSelectionModel.cpp
    src/Mdt/ItemEditor/ControllerState.cpp
    src/Mdt/ItemEditor/ControllerEvent.cpp
    src/Mdt/ItemEditor/RowChangeEventDispatcher.cpp
    src/Mdt/ItemEditor/AbstractActionContainer.cpp
    src/Mdt/ItemEditor/NavigationActions.cpp
    src/Mdt/ItemEditor/InsertAction.cpp
    src/Mdt/ItemEditor/RemoveAction.cpp
    src/Mdt/ItemEditor/EditionActions.cpp
    src/Mdt/ItemEditor/ResizeToContentsAction.cpp
    src/Mdt/ItemEditor/AbstractControllerStatePermission.cpp
    src/Mdt/ItemEditor/ControllerStatePermissionProxyModel.cpp
    src/Mdt/ItemEditor/ControllerStateTableRow.cpp
    src/Mdt/ItemEditor/AbstractControllerStateTable.cpp
    src/Mdt/ItemEditor/ControllerStateMachine.cpp
    src/Mdt/ItemEditor/AbstractController.cpp
    src/Mdt/ItemEditor/ItemViewPrivateContainer.cpp
    src/Mdt/ItemEditor/TableViewControllerImplementation.cpp
    src/Mdt/ItemEditor/TableViewControllerStatePermission.cpp
    src/Mdt/ItemEditor/TableViewControllerStateTable.cpp
    src/Mdt/ItemEditor/TableViewController.cpp
    src/Mdt/ItemEditor/MappedWidgetList.cpp
    src/Mdt/ItemEditor/WidgetEditablePropertyMap.cpp
    src/Mdt/ItemEditor/WidgetStyleSheet.cpp
    src/Mdt/ItemEditor/DataWidgetMapperItemDelegate.cpp
    src/Mdt/ItemEditor/DataWidgetMapper.cpp
    src/Mdt/ItemEditor/WidgetMapperControllerStatePermission.cpp
    src/Mdt/ItemEditor/WidgetMapperControllerStateTable.cpp
    src/Mdt/ItemEditor/WidgetMapperController.cpp
    src/Mdt/ItemEditor/AbstractControllerRelation.cpp
    src/Mdt/ItemEditor/FilterControllerRelation.cpp
    src/Mdt/ItemEditor/NavigationControllerRelation.cpp
    src/Mdt/ItemEditor/ControllerRelationList.cpp
    src/Mdt/ItemEditor/StandardEditorLayoutWidget.cpp
    
    src/Mdt/ItemEditor/AbstractEditorLayout.cpp
    src/Mdt/ItemEditor/AbstractEditorWidget.cpp
    
    src/Mdt/ItemEditor/AbstractFormWidget.cpp
    src/Mdt/ItemEditor/AbstractTableViewWidget.cpp
    src/Mdt/ItemEditor/TableViewWidget.cpp
    src/Mdt/ItemEditor/StandardWindow.cpp
    src/Mdt/ItemEditor/AbstractWindow.cpp
)

# Headers directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../FilterExpression/src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../ItemModel/src")
# Because some headers are generated during building (config.h, Qt's UIC), we also build path
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Output library name
set(LIBRARY_NAME "ItemEditor")

# Output library technical names
string(TOLOWER ${LIBRARY_NAME} LIBRARY_NAME_LC)
set(LIBRARY_BIN_NAME "mdt${LIBRARY_NAME_LC}")
# Add output library
add_library(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} SHARED ${SOURCES_FILES})
target_link_libraries(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} "mdterror${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} "mdterrorgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} "mdtfilterexpression${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} "mdtitemmodel${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX} Qt5::Widgets)

# Qt Designer plugins
add_subdirectory("qttools")

# Translation files
set(TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/${LIBRARY_BIN_NAME}_fr.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/${LIBRARY_BIN_NAME}_de.ts
)
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src ${TS_FILES} OPTIONS -I ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_custom_target("Mdt${LIBRARY_NAME}Ts" DEPENDS ${QM_FILES} ${TS_FILES})

# Headers to install
file(GLOB INSTALL_HEADERS *.h)
install(FILES ${INSTALL_HEADERS} DESTINATION "${HEADERS_DEST_DIR}/Mdt/${LIBRARY_NAME}" COMPONENT dev)

# Install targets
install(TARGETS "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}" DESTINATION "${LIB_DEST_DIR}" COMPONENT lib)

# Strip library and put debug symbols into separate file
if(UNIX)
  mdtSeparateDebugSymbols(${LIBRARY_BIN_NAME})
endif(UNIX)

# Unit tests
add_subdirectory("tests")
# Examples
add_subdirectory("examples")
