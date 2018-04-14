# CMake project file for Mdt::Sql tests

enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= SQL test library ====================

set(SOURCE_FILES
    src/TestBase.cpp
)

add_library(mdtsqlwidgetstest SHARED ${SOURCE_FILES})
target_link_libraries(mdtsqlwidgetstest Sql_Widgets Application_Widgets Qt5::Test)

#=============== Helper functions ===========

# Add a test for this Sql lib
function(addSqlWidgetsTest testName)
  set(complteTestName "MdtSql_Widgets_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtsqlwidgetstest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addSqlWidgetsTest("SQLiteDatabaseDialogTest")
