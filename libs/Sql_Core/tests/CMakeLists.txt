# CMake project file for Mdt::Sql tests

enable_testing()

find_package(Qt5 COMPONENTS Test)

#================= SQL test library ====================

set(SOURCE_FILES
    src/TestBase.cpp
    src/AsyncTestQueryReceiver.cpp
    src/Schema/Client.cpp
    src/Schema/Client2.cpp
    src/Schema/Address.cpp
    src/Schema/Address2.cpp
    src/Schema/Link.cpp
    src/Schema/ClientPopulation.cpp
    src/Schema/ClientAddressView.cpp
    src/Schema/TestSchema.cpp
#     src/Schema/LikeExpressionTestSchema.cpp
)

add_library(mdtsqlcoretest SHARED ${SOURCE_FILES})
target_link_libraries(mdtsqlcoretest Sql_Core Entity_Core Entity_Sql Application_Core Qt5::Test)

#=============== Helper functions ===========

# Add a test for this Sql lib
function(addSqlCoreTest testName)
  set(complteTestName "MdtSql_Core_${testName}")
  set(sourceFilePath "src/${testName}.cpp")
  string(TOLOWER ${complteTestName} executableName)
  add_executable(${executableName} ${sourceFilePath})
  target_link_libraries(${executableName} mdtsqlcoretest)
  add_test(NAME ${complteTestName} COMMAND ${executableName})
#  message("Test name: ${complteTestName} , src: ${sourceFilePath} , exec: ${executableName}")
endfunction()

#================= Tests ====================

addSqlCoreTest("SimpleTypesTest")
addSqlCoreTest("FieldNameTest")
addSqlCoreTest("FieldNameValueMapTest")
addSqlCoreTest("FieldNameValueMapBenchmark")
addSqlCoreTest("PrimaryKeyRecordTest")
# addSqlCoreTest("SelectFieldTest")
addSqlCoreTest("ExpressionTest")
# addSqlCoreTest("LikeExpressionTest")
# addSqlCoreTest("LikeExpressionSqliteTest")
# addSqlCoreTest("WhereExpressionTest")
# addSqlCoreTest("JoinConstraintExpressionTest")
# addSqlCoreTest("JoinConstraintFieldPairTest")
# addSqlCoreTest("JoinConstraintTest")
# addSqlCoreTest("JoinClauseTest")
# addSqlCoreTest("FromClauseTest")
addSqlCoreTest("SelectStatementTest")
addSqlCoreTest("QueryTest")
addSqlCoreTest("AsyncQueryTest")
addSqlCoreTest("AsyncQueryBenchmark")
addSqlCoreTest("SQLiteDatabaseTest")
addSqlCoreTest("SchemaFieldTypeMapTest")
addSqlCoreTest("SchemaFieldTest")
addSqlCoreTest("SchemaFieldBenchmark")
# addSqlCoreTest("SchemaFieldModelTest")
addSqlCoreTest("SchemaPrimaryKeyTest")
addSqlCoreTest("SchemaForeignKeyTest")
addSqlCoreTest("SchemaTableTest")
addSqlCoreTest("SchemaTableBenchmark")
# addSqlCoreTest("SchemaTableModelTest")
# addSqlCoreTest("SchemaTablePopulationTest")
# addSqlCoreTest("SchemaTest")
addSqlCoreTest("SchemaDriverTest")
addSqlCoreTest("SchemaDriverSqliteTest")
# addSqlCoreTest("SchemaDriverMySqlTest")
# addSqlCoreTest("SchemaDriverPostgreSqlTest")
# addSqlCoreTest("SchemaViewTest")
