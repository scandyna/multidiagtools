# CMake project file for Mdt::Sql tests

enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= SQL test library ====================

set(SOURCE_FILES
    src/TestBase.cpp
    src/Schema/Client.cpp
    src/Schema/Client2.cpp
    src/Schema/Address.cpp
    src/Schema/Address2.cpp
#     src/Schema/ClientPopulation.cpp
#     src/Schema/ClientAddressView.cpp
#     src/Schema/TestSchema.cpp
#     src/Schema/LikeExpressionTestSchema.cpp
)

add_library(mdtsqltest SHARED ${SOURCE_FILES})
target_link_libraries(mdtsqltest Sql Application_Widgets Qt5::Test)

#=============== Helper functions ===========

# Add a test for this Sql lib
function(addSqlTest testName)
  set(complteTestName "MdtSql_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtsqltest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addSqlTest("SimpleTypesTest")
addSqlTest("FieldNameTest")
# addSqlTest("SelectFieldTest")
# addSqlTest("ExpressionTest")
# addSqlTest("LikeExpressionTest")
# addSqlTest("LikeExpressionSqliteTest")
# addSqlTest("WhereExpressionTest")
# addSqlTest("JoinConstraintExpressionTest")
# addSqlTest("JoinConstraintFieldPairTest")
# addSqlTest("JoinConstraintTest")
# addSqlTest("JoinClauseTest")
# addSqlTest("FromClauseTest")
# addSqlTest("SelectStatementTest")
# addSqlTest("QueryTest")
addSqlTest("SchemaFieldTypeMapTest")
addSqlTest("SchemaFieldTest")
# addSqlTest("SchemaFieldModelTest")
addSqlTest("SchemaPrimaryKeyTest")
# addSqlTest("SchemaForeignKeyTest")
addSqlTest("SchemaTableTest")
addSqlTest("SchemaTableBenchmark")
# addSqlTest("SchemaTableModelTest")
# addSqlTest("SchemaTablePopulationTest")
# addSqlTest("SchemaTest")
# addSqlTest("SchemaDriverTest")
# addSqlTest("SchemaDriverSqliteTest")
# addSqlTest("SchemaDriverMySqlTest")
# addSqlTest("SchemaDriverPostgreSqlTest")
# addSqlTest("SchemaViewTest")
