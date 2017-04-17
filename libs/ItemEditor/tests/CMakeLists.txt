# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)
enable_testing()

#================= Item/View test library ====================

set(SOURCES_FILES
  src/TestBase.cpp
  src/ItemModelControllerTester.cpp
  src/AbstractControllerRelationTestClass.cpp
  src/RowStateChangedSpy.cpp
  src/CurrentRowToBeSetSpy.cpp
#   src/CurrentRowChangedSpy.cpp
  src/ControllerStateChangedSignalSpy.cpp
  src/PrimaryKeyChangedSignalSpy.cpp
  src/ItemViewTestEdit.cpp
)

add_library(mdtitemeditortest SHARED ${SOURCES_FILES})
target_link_libraries(mdtitemeditortest "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtitemeditortest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtitemeditortest Qt5::Test)

#=============== Helper functions ===========

# Add a test for this ItemEditor lib
function(addItemEditorTest testName)
  set(complteTestName "MdtItemEditor${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} "mdtitemeditortest")
  add_test(${complteTestName} ${CMAKE_CURRENT_BINARY_DIR}/${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addItemEditorTest("ItemSelectionModelTest")
addItemEditorTest("RowChangeTest")
addItemEditorTest("ControllerStatePermissionProxyModelTest")
addItemEditorTest("ControllerStateMachineTest")
addItemEditorTest("ActionsTest")
addItemEditorTest("ItemDelegateTest")
addItemEditorTest("ItemViewTest")
addItemEditorTest("WidgetEditablePropertyMapTest")
addItemEditorTest("WidgetStyleSheetTest")
addItemEditorTest("DataWidgetMapperTest")
addItemEditorTest("ControllerTest")
addItemEditorTest("ItemViewPrivateContainerTest")
addItemEditorTest("TableViewControllerImplementationTest")
addItemEditorTest("TableViewControllerTest")
addItemEditorTest("WidgetMapperControllerTest")
addItemEditorTest("ControllerRelationTest")
addItemEditorTest("ControllerRelationListTest")
addItemEditorTest("FilterControllerRelationTest")
addItemEditorTest("NavigationControllerRelationTest")
addItemEditorTest("AbstractEditorLayoutTest")
addItemEditorTest("WindowTest")

addItemEditorTest("StandardWidgetsTest")
