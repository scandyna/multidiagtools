# CMake project file for Multi Diag Tools
# * Unit test for: mdtdatabase *
# * Directory: tests/mdtdatabase *

# Project
project (multidiagtools)

# * mdtDatabaseTest executable *
add_executable(mdtdatabasetest "mdtDatabaseTest.cpp")
target_link_libraries(mdtdatabasetest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdatabasetest "mdterrorgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdatabasetest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdtdatabasetest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdatabasetest "mdttest")

# * mdtDatabaseWidgetTest executable *
add_executable(mdtdatabasewidgettest "mdtDatabaseWidgetTest.cpp")
target_link_libraries(mdtdatabasewidgettest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdatabasewidgettest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdatabasewidgettest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdatabasewidgettest "mdttest")

# * mdtDataTableTest executable *
add_executable(mdtdatatabletest "mdtDataTableTest.cpp")
target_link_libraries(mdtdatatabletest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdatatabletest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdatatabletest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdtdatabasetest "mdterrorgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdatatabletest "mdttest")

# * mdtSqlCopierTest executable *
add_executable(mdtsqlcopiertest "mdtSqlCopierTest.cpp")
target_link_libraries(mdtsqlcopiertest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtsqlcopiertest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtsqlcopiertest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtsqlcopiertest "mdttest")

# * mdtSqlApplicationWidgetsTest executable *
add_executable(mdtsqlapplicationwidgetstest "mdtSqlApplicationWidgetsTest.cpp")
target_link_libraries(mdtsqlapplicationwidgetstest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtsqlapplicationwidgetstest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtsqlapplicationwidgetstest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtsqlapplicationwidgetstest "mdttest")

# * mdtSqlCsvTest executable *
add_executable(mdtsqlcsvtest "mdtSqlCsvTest.cpp")
target_link_libraries(mdtsqlcsvtest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtsqlcsvtest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtsqlcsvtest "mdttest")
