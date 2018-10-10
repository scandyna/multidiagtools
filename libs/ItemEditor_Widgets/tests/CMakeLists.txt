
enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= Item/View test library ====================

set(SOURCES_FILES
  src/TestBase.cpp
#   src/ItemModelControllerTester.cpp
#   src/AbstractControllerRelationTestClass.cpp
#   src/RowStateChangedSpy.cpp
#   src/CurrentRowToBeSetSpy.cpp
#   src/CurrentRowChangedSpy.cpp
#   src/ControllerStateChangedSignalSpy.cpp
#   src/PrimaryKeyChangedSignalSpy.cpp
  src/ItemViewTestEdit.cpp
)

add_library(mdtitemeditortest SHARED ${SOURCES_FILES})
target_link_libraries(mdtitemeditortest ItemEditor_Widgets Application_Widgets Qt5::Test)

#=============== Helper functions ===========

# Add a test for this ItemEditor lib
function(addItemEditorWidgetsTest testName)
  set(complteTestName "MdtItemEditor_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtitemeditortest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

# addItemEditorTest("ItemSelectionModelTest")
# addItemEditorTest("RowChangeTest")
# addItemEditorTest("ControllerStatePermissionProxyModelTest")
# addItemEditorTest("ControllerStateMachineTest")
# addItemEditorTest("ActionsTest")
# addItemEditorTest("ItemDelegateTest")
# addItemEditorTest("ItemViewTest")
addItemEditorWidgetsTest("WidgetEditablePropertyMapTest")
addItemEditorWidgetsTest("WidgetStyleSheetTest")
addItemEditorWidgetsTest("DataWidgetMapperItemDelegateTest")
addItemEditorWidgetsTest("DataWidgetMapperTest")
# addItemEditorTest("ControllerTest")
# addItemEditorTest("ItemViewPrivateContainerTest")
# addItemEditorTest("ForeignKeyColumnVisibilityMapTest")
# addItemEditorTest("TableViewControllerImplementationTest")
# addItemEditorTest("TableViewControllerTest")
# addItemEditorTest("WidgetMapperControllerTest")
# addItemEditorTest("ControllerRelationTest")
# addItemEditorTest("ControllerRelationListTest")
# addItemEditorTest("FilterControllerRelationTest")
# addItemEditorTest("NavigationControllerRelationTest")
# addItemEditorTest("AbstractEditorLayoutTest")
# addItemEditorTest("WindowTest")
# 
# addItemEditorTest("StandardWidgetsTest")
