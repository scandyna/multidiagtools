# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)
enable_testing()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../ItemEditor/tests/src")

#================= Item/View SQL test library ====================

set(SOURCES_FILES
  src/Schema/Client.cpp
)

add_library(mdtitemeditorsqltest SHARED ${SOURCES_FILES})
target_link_libraries(mdtitemeditorsqltest "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtitemeditorsqltest "mdtitemeditortest")
target_link_libraries(mdtitemeditorsqltest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtitemeditorsqltest Qt5::Test)

#=============== Helper functions ===========

# Add a test for this ItemEditor_Sql lib
function(addItemEditorSqlTest testName)
  set(complteTestName "MdtItemEditor${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} "mdtitemeditorsqltest")
  add_test(${complteTestName} ${CMAKE_CURRENT_BINARY_DIR}/${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addItemEditorSqlTest("SqlTableViewControllerTest")
