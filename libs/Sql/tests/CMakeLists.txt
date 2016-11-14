# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)
enable_testing()

#================= SQL test library ====================

set(SOURCES_FILES
    src/Schema/Client_tbl.cpp
    src/Schema/Address_tbl.cpp
    src/Schema/ClientAddressView.cpp
    src/Schema/TestSchema.cpp
)

add_library(mdtsqltest SHARED ${SOURCES_FILES})
target_link_libraries(mdtsqltest "mdtsql${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtsqltest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtsqltest Qt5::Test)

#=============== Helper functions ===========

# Add a test for this Sql lib
function(addSqlTest testName)
  set(complteTestName "MdtSql${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} "mdtsqltest")
  add_test(${complteTestName} ${CMAKE_CURRENT_BINARY_DIR}/${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addSqlTest("SimpleTypesTest")
addSqlTest("FieldNameTest")
addSqlTest("SelectFieldTest")
addSqlTest("ExpressionTest")
addSqlTest("WhereExpressionTest")
addSqlTest("JoinConstraintExpressionTest")
addSqlTest("JoinConstraintFieldPairTest")
addSqlTest("JoinConstraintTest")
addSqlTest("JoinClauseTest")
addSqlTest("FromClauseTest")
addSqlTest("SelectStatementTest")
addSqlTest("QueryTest")
addSqlTest("SchemaPrimaryKeyTest")
addSqlTest("SchemaForeignKeyTest")
addSqlTest("SchemaTableTest")
addSqlTest("SchemaTest")
addSqlTest("SchemaDriverTest")
addSqlTest("SchemaDriverSqliteTest")
addSqlTest("SchemaDriverMySqlTest")
addSqlTest("SchemaDriverPostgreSqlTest")
addSqlTest("SchemaViewTest")
