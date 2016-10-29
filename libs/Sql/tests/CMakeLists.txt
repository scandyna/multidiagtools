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

#================= Tests ====================

# TODO Create a function that takes a test name and generates all the rest
#      Example for MdtSqlExpressionTest:
#       - Name to give as input: ExpressionTest
#       - Test name to generate: MdtSqlExpressionTest
#       - Test file to compile: src/ExpressionTest.cpp
#       - Test executable to generate: mdtsqlexpressiontest

# Target MdtSqlSimpleTypesTest
add_executable(mdtsqlsimpletypestest src/SimpleTypesTest.cpp)
target_link_libraries(mdtsqlsimpletypestest "mdtsqltest")
add_test(MdtSqlSimpleTypesTest ${CMAKE_CURRENT_BINARY_DIR}/mdtsqlsimpletypestest)

# Target MdtSqlExpressionTest
add_executable(mdtsqlexpressiontest src/ExpressionTest.cpp)
target_link_libraries(mdtsqlexpressiontest "mdtsqltest")
add_test(MdtSqlExpressionTest ${CMAKE_CURRENT_BINARY_DIR}/mdtsqlexpressiontest)

# Target MdtSqlWhereExpressionTest
add_executable(mdtsqlwhereexpressiontest src/WhereExpressionTest.cpp)
target_link_libraries(mdtsqlwhereexpressiontest "mdtsqltest")
add_test(MdtSqlWhereExpressionTest ${CMAKE_CURRENT_BINARY_DIR}/mdtsqlwhereexpressiontest)

# Target MdtSqlJoinConstraintExpressionTest
add_executable(mdtsqljoinconstraintexpressiontest src/JoinConstraintExpressionTest.cpp)
target_link_libraries(mdtsqljoinconstraintexpressiontest "mdtsqltest")
add_test(MdtSqlJoinConstraintExpressionTest ${CMAKE_CURRENT_BINARY_DIR}/mdtsqljoinconstraintexpressiontest)

# Target MdtSqlJoinClauseTest
add_executable(mdtsqljoinclausetest src/JoinClauseTest.cpp)
target_link_libraries(mdtsqljoinclausetest "mdtsqltest")
add_test(MdtSqlJoinClauseTest ${CMAKE_CURRENT_BINARY_DIR}/mdtsqljoinclausetest)

# Target MdtSqlQueryTest
add_executable(mdtsqlquerytest src/QueryTest.cpp)
target_link_libraries(mdtsqlquerytest "mdtsqltest")
add_test(MdtSqlQueryTest ${CMAKE_CURRENT_BINARY_DIR}/mdtsqlquerytest)

# Target MdtSqlSchemaTest
add_executable(mdtsqlschematest src/SchemaTest.cpp)
target_link_libraries(mdtsqlschematest "mdtsqltest")
add_test(MdtSqlSchemaTest ${CMAKE_CURRENT_BINARY_DIR}/mdtsqlschematest)

# Target MdtSqlSchemaDriverTest
add_executable(mdtsqlschemadrivertest src/SchemaDriverTest.cpp)
target_link_libraries(mdtsqlschemadrivertest "mdtsqltest")
add_test(MdtSqlSchemaDriverTest ${CMAKE_CURRENT_BINARY_DIR}/mdtsqlschemadrivertest)

# Target MdtSqlSchemaDriverSqliteTest
add_executable(mdtsqlschemadriversqlitetest src/SchemaDriverSqliteTest.cpp)
target_link_libraries(mdtsqlschemadriversqlitetest "mdtsqltest")
add_test(MdtSqlSchemaDriverSqliteTest ${CMAKE_CURRENT_BINARY_DIR}/mdtsqlschemadriversqlitetest)

# Target MdtSqlSchemaDriverMySqlTest
add_executable(mdtsqlschemadrivermysqltest src/SchemaDriverMySqlTest.cpp)
target_link_libraries(mdtsqlschemadrivermysqltest "mdtsqltest")
add_test(MdtSqlSchemaDriverMySqlTest ${CMAKE_CURRENT_BINARY_DIR}/mdtsqlschemadrivermysqltest)

# Target MdtSqlSchemaDriverPostgreSqlTest
add_executable(mdtsqlschemadriverpostgresqltest src/SchemaDriverPostgreSqlTest.cpp)
target_link_libraries(mdtsqlschemadriverpostgresqltest "mdtsqltest")
add_test(MdtSqlSchemaDriverPostgreSqlTest ${CMAKE_CURRENT_BINARY_DIR}/mdtsqlschemadriverpostgresqltest)

# Target MdtSqlSchemaViewTest
add_executable(mdtsqlschemaviewtest src/SchemaViewTest.cpp)
target_link_libraries(mdtsqlschemaviewtest "mdtsqltest")
add_test(MdtSqlSchemaViewTest ${CMAKE_CURRENT_BINARY_DIR}/mdtsqlschemaviewtest)
