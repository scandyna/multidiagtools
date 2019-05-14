# CMake project file for Mdt::Sql tests

enable_testing()

find_package(Qt5 COMPONENTS Test Sql)

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
target_link_libraries(mdtsqlcoretest Sql_Core Sql_Core_SQLite Entity_Core Application_Core Qt5::Test)

#================= SQL test plugins ====================

function(mdt_add_sql_test_plugin)
  # Parse arguments
  set(oneValueArgs NAME)
  set(multiValueArgs SOURCE_FILES DEPENDENCIES)
  cmake_parse_arguments(VAR "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  # Check that all argumenst are passed
  if(NOT VAR_NAME)
    message(FATAL_ERROR "mdt_add_sql_test_plugin(): NAME argument is missing.")
  endif()
  if(NOT VAR_SOURCE_FILES)
    message(FATAL_ERROR "mdt_add_sql_test_plugin(): No source file provided.")
  endif()
  # Local variables
  set(target_name ${VAR_NAME})
  set(source_files ${VAR_SOURCE_FILES})
  set(dependencies ${VAR_DEPENDENCIES})

  add_library(${target_name} MODULE ${source_files})
  target_link_libraries(${target_name} Sql_Core Qt5::Sql ${dependencies})
  set(library_name ${PROJECT_NAME}${target_name})
  set_target_properties(${target_name} PROPERTIES
    OUTPUT_NAME ${library_name}
  )
  set_target_properties(${target_name} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/Mdt/Sql"
  )
endfunction()

mdt_add_sql_test_plugin(
  NAME ErrorDriverTestPlugin
  SOURCE_FILES src/ErrorDriverTestPlugin.cpp
)
mdt_add_sql_test_plugin(
  NAME ErrorDriverTestPlugin2
  SOURCE_FILES src/ErrorDriverTestPlugin2.cpp
)

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

addSqlCoreTest("LibraryInfoTest")
addSqlCoreTest("PluginLoaderTest")
addSqlCoreTest("PluginLoaderBenchmark")
addSqlCoreTest("ErrorDriverTest")
addSqlCoreTest("ErrorDriverBenchmark")
addSqlCoreTest("SimpleTypesTest")
addSqlCoreTest("FieldNameTest")
addSqlCoreTest("FieldNameValueMapTest")
addSqlCoreTest("FieldNameValueMapBenchmark")
addSqlCoreTest("RecordUtilsTest")
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
addSqlCoreTest("StatementAlgorithmTest")
addSqlCoreTest("SelectStatementTest")
addSqlCoreTest("ConnectionParametersTest")
addSqlCoreTest("ConnectionTest")
addSqlCoreTest("QueryExpressionSqlTransformTest")
addSqlCoreTest("QueryExpressionSqlTransformBenchmark")
addSqlCoreTest("QueryTest")
addSqlCoreTest("QueryBenchmark")
addSqlCoreTest("AsyncQueryTest")
addSqlCoreTest("AsyncQueryBenchmark")
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
addSqlCoreTest("SchemaDriverMySqlTest")
addSqlCoreTest("SchemaDriverPostgreSqlTest")
# addSqlCoreTest("SchemaViewTest")
