# CMake project file for Mdt::Sql tests

enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= SQL test library ====================

set(SOURCE_FILES
    src/TestBase.cpp
)

add_library(mdtSql_Core_SQLiteTest SHARED ${SOURCE_FILES})
target_link_libraries(mdtSql_Core_SQLiteTest Sql_Core Sql_Core_SQLite Application_Core LibraryInfo_Core Qt5::Test)

#=============== Helper functions ===========

# Add a test for this Sql lib
function(addSqlCoreSQLiteTest testName)
  set(complteTestName "mdtSql_Core_SQLite_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  add_executable(${complteTestName} ${sourceFilePath})
  target_link_libraries(${complteTestName} mdtSql_Core_SQLiteTest)
  add_test(NAME ${complteTestName} COMMAND ${complteTestName})
endfunction()

#================= Tests ====================

addSqlCoreSQLiteTest("ErrorDriverSQLiteTest")
target_include_directories(mdtSql_Core_SQLite_ErrorDriverSQLiteTest PRIVATE "${SQLITE_SOURCE_DIR}")

addSqlCoreSQLiteTest("ConnectionParametersTest")
addSqlCoreSQLiteTest("SQLiteDatabaseTest")
