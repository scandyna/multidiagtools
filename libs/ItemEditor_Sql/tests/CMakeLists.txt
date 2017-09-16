
enable_testing()

find_package(Qt5 COMPONENTS Test)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../ItemEditor/tests/src")

#================= Item/View SQL test library ====================

set(SOURCES_FILES
  src/Schema/Client.cpp
  src/Schema/Address.cpp
  src/Schema/ClientAddressSchema.cpp
  src/Schema/ClientPopulation.cpp
  src/SqlTestBase.cpp
  src/SqlTableModelControllerTester.cpp
)

add_library(mdtitemeditorsqltest SHARED ${SOURCES_FILES})
target_link_libraries(mdtitemeditorsqltest ItemEditor_Sql mdtitemeditortest Application_Widgets Qt5::Test)

#=============== Helper functions ===========

# Add a test for this ItemEditor_Sql lib
function(addItemEditorSqlTest testName)
  set(complteTestName "MdtItemEditor_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtitemeditorsqltest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addItemEditorSqlTest("SqlControllerTest")
addItemEditorSqlTest("SqlTableModelControllerTest")
addItemEditorSqlTest("SqlTableViewControllerTest")
addItemEditorSqlTest("SqlTableWidgetMapperControllerTest")
