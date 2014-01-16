# CMake project file for Multi Diag Tools
# * Unit test for: mdttesttool *
# * Directory: tests/mdttools/mdttesttool *

# Project
project (multidiagtools)

# * mdtAlgorithmsTest executable *
QT4_WRAP_CPP(MDT_TEST_TOOL_MOC "mdtTestToolTest.h")
add_executable(mdttesttooltest "mdtTestToolTest.cpp" ${MDT_TEST_TOOL_MOC})
target_link_libraries(mdttesttooltest ${QT_LIBRARIES})
target_link_libraries(mdttesttooltest "mdttest")
target_link_libraries(mdttesttooltest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttesttooltest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttesttooltest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttesttooltest "mdttesttool${MDTLIB_VERSION_SUFFIX}")
