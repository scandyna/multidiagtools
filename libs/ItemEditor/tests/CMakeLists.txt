# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)
enable_testing()

#================= Item/View test library ====================

set(SOURCES_FILES
  src/ItemViewTestEdit.cpp
  src/TestTableEditionWidget.cpp
  src/TestFormEditionWidget.cpp
  src/TestWindowEditor.cpp
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
addItemEditorTest("ActionsTest")
addItemEditorTest("StandardWidgetsTest")
addItemEditorTest("ItemDelegateTest")
addItemEditorTest("ItemViewTest")
addItemEditorTest("DataWidgetMapperTest")
addItemEditorTest("ControllerTest")
addItemEditorTest("TestEditorTest")
