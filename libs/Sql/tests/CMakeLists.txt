# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)
enable_testing()

#================= SQL test library ====================

set(SOURCES_FILES 
    src/TestDatabaseSchema.cpp
)

add_library(mdtsqltest SHARED ${SOURCES_FILES})
target_link_libraries(mdtsqltest "mdtsql${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtsqltest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtsqltest Qt5::Test)

#================= Tests ====================

# Target MdtSqlQueryTest
add_executable(mdtsqlquerytest src/QueryTest.cpp)
target_link_libraries(mdtsqlquerytest "mdtsqltest")
add_test(MdtSqlQueryTest ${CMAKE_CURRENT_BINARY_DIR}/mdtsqlquerytest)


