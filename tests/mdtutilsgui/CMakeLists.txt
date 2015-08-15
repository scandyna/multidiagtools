# CMake project file for Multi X Diag Tools
# * Unit test for: mdtutilsgui *
# * Directory: tests/mdutilsgui *

# Project
project (multidiagtools)

# * mdtIoWidgetTest executable *
add_executable(mdtiowidgettest "mdtIoWidgetTest.cpp")
target_link_libraries(mdtiowidgettest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtiowidgettest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtiowidgettest "mdttest")

# * mdtDatabaseTest executable *
add_executable(mdtdatabasetestold "mdtDatabaseTest.cpp")
target_link_libraries(mdtdatabasetestold "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdatabasetestold "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdatabasetestold "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdatabasetestold "mdttest")

# * mdtWidgetsTest executable *
add_executable(mdtwidgetstest "mdtWidgetsTest.cpp" "mdtWidgetsTestData.cpp")
target_link_libraries(mdtwidgetstest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtwidgetstest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtwidgetstest "mdttest")
