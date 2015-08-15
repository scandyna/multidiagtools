# CMake project file for Multi Diag Tools
# * Unit test for: mdtdatabase *
# * Directory: tests/mdtdatabase *

# Project
project (multidiagtools)

# * mdtDatabaseTest executable *
add_executable(mdtdatabasetest "mdtDatabaseTest.cpp")
target_link_libraries(mdtdatabasetest "mdtutils${MDTLIB_VERSION_SUFFIX}")
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
target_link_libraries(mdtdatatabletest "mdttest")

# * mdtSqlCopierTest executable *
add_executable(mdtsqlcopiertest "mdtSqlCopierTest.cpp")
target_link_libraries(mdtsqlcopiertest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtsqlcopiertest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtsqlcopiertest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtsqlcopiertest "mdttest")

# # * mdtDatabaseTest executable *
# QT4_WRAP_CPP(MDT_DATABASE_TEST_MOC "mdtDatabaseTest.h")
# #QT4_WRAP_UI (MDT_DATABASE_TEST_UIC_HEADERS "mdtSqlFormWidgetTestForm.ui")
# add_executable(mdtdatabasetest "mdtDatabaseTest.cpp" ${MDT_DATABASE_TEST_MOC} ${MDT_DATABASE_TEST_UIC_HEADERS})
# target_link_libraries(mdtdatabasetest "mdtutils${MDTLIB_VERSION_SUFFIX}")
# target_link_libraries(mdtdatabasetest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
# #target_link_libraries(mdtdatabasetest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
# target_link_libraries(mdtdatabasetest "mdttest")
# 
# # * mdtDatabaseWidgetTest executable *
# QT4_WRAP_CPP(MDT_DATABASE_WIDGET_TEST_MOC "mdtDatabaseWidgetTest.h")
# add_executable(mdtdatabasewidgettest "mdtDatabaseWidgetTest.cpp" ${MDT_DATABASE_WIDGET_TEST_MOC})
# target_link_libraries(mdtdatabasewidgettest "mdtutils${MDTLIB_VERSION_SUFFIX}")
# target_link_libraries(mdtdatabasewidgettest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
# target_link_libraries(mdtdatabasewidgettest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
# target_link_libraries(mdtdatabasewidgettest "mdttest")
# 
# # * mdtDataTableTest executable *
# QT4_WRAP_CPP(MDT_DATA_TABLE_TEST_MOC "mdtDataTableTest.h")
# add_executable(mdtdatatabletest "mdtDataTableTest.cpp" ${MDT_DATA_TABLE_TEST_MOC})
# target_link_libraries(mdtdatatabletest "mdtutils${MDTLIB_VERSION_SUFFIX}")
# target_link_libraries(mdtdatatabletest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
# target_link_libraries(mdtdatatabletest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
# target_link_libraries(mdtdatatabletest "mdttest")
# 
# # * mdtSqlCopierTest executable *
# QT4_WRAP_CPP(MDT_SQL_COPIER_TEST_MOC "mdtSqlCopierTest.h")
# add_executable(mdtsqlcopiertest "mdtSqlCopierTest.cpp" ${MDT_SQL_COPIER_TEST_MOC})
# target_link_libraries(mdtsqlcopiertest "mdtutils${MDTLIB_VERSION_SUFFIX}")
# target_link_libraries(mdtsqlcopiertest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
# target_link_libraries(mdtsqlcopiertest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
# target_link_libraries(mdtsqlcopiertest "mdttest")
