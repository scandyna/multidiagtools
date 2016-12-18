# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)
enable_testing()

#=============== Helper functions ===========

# Add a test for this ItemModel lib
function(addItemModelTest testName)
  set(complteTestName "MdtItemModel${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}")
  target_link_libraries(${executableName} "mdtapplication${MDTLIB_VERSION_SUFFIX}")
  target_link_libraries(${executableName} Qt5::Test)
  add_test(${complteTestName} ${CMAKE_CURRENT_BINARY_DIR}/${executableName})
endfunction()

#================= Tests ====================

addItemModelTest("VariantTableModelTest")
addItemModelTest("FilterExpressionTest")
addItemModelTest("FilterProxyModelTest")
addItemModelTest("RelationFilterProxyModelTest")
addItemModelTest("SortProxyModelTest")
