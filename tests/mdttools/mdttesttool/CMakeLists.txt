# CMake project file for Multi Diag Tools
# * Unit test for: mdttesttool *
# * Directory: tests/mdttools/mdttesttool *

# Project
project (multidiagtools)

# * mdtTestToolTest executable *
QT4_WRAP_CPP(MDT_TEST_TOOL_MOC "mdtTestToolTest.h")
add_executable(mdttesttooltest mdtTestToolTest.cpp mdtCableListTestScenario.cpp ${MDT_TEST_TOOL_MOC})
target_link_libraries(mdttesttooltest ${QT_LIBRARIES})
target_link_libraries(mdttesttooltest "mdttest")
target_link_libraries(mdttesttooltest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttesttooltest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdttesttooltest "mdtdevice${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttesttooltest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttesttooltest "mdttesttool${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdttesttooltest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")

# * mdtCableListTest executable *
QT4_WRAP_CPP(MDT_CABLE_LIST_MOC "mdtCableListTest.h")
add_executable(mdtcablelisttest mdtCableListTest.cpp mdtCableListTestScenario.cpp ${MDT_CABLE_LIST_MOC})
target_link_libraries(mdtcablelisttest ${QT_LIBRARIES})
target_link_libraries(mdtcablelisttest "mdttest")
target_link_libraries(mdtcablelisttest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelisttest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelisttest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelisttest "mdttesttool${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdtcablelisttest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
