# CMake project file for Multi Diag Tools
# * Unit test for: mdttesttool *
# * Directory: tests/mdttools/mdttesttool *

# Project
project (multidiagtools)

# * mdtClConnectorTest executable *
QT4_WRAP_CPP(MDT_CONNECTOR_TEST_MOC "mdtClConnectorTest.h")
add_executable(mdtclconnectortest mdtClConnectorTest.cpp ${MDT_CONNECTOR_TEST_MOC})
target_link_libraries(mdtclconnectortest ${QT_LIBRARIES})
target_link_libraries(mdtclconnectortest "mdttest")
target_link_libraries(mdtclconnectortest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclconnectortest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdtclconnectortest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclconnectortest "mdttesttool${MDTLIB_VERSION_SUFFIX}")

# * mdtClArticleConnectionTest executable *
QT4_WRAP_CPP(MDT_ARTICLE_CONNECTION_TEST_MOC "mdtClArticleConnectionTest.h")
add_executable(mdtclarticleconnectiontest mdtClArticleConnectionTest.cpp mdtCableListTestScenario.cpp ${MDT_ARTICLE_CONNECTION_TEST_MOC})
target_link_libraries(mdtclarticleconnectiontest ${QT_LIBRARIES})
target_link_libraries(mdtclarticleconnectiontest "mdttest")
target_link_libraries(mdtclarticleconnectiontest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclarticleconnectiontest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdtclarticleconnectiontest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclarticleconnectiontest "mdttesttool${MDTLIB_VERSION_SUFFIX}")

# * mdtTtValueLimitsTest executable *
QT4_WRAP_CPP(MDT_VALUE_LIMITS_MOC "mdtTtValueLimitsTest.h")
add_executable(mdttvaluelimitstest mdtTtValueLimitsTest.cpp ${MDT_VALUE_LIMITS_MOC})
target_link_libraries(mdttvaluelimitstest ${QT_LIBRARIES})
target_link_libraries(mdttvaluelimitstest "mdttest")
target_link_libraries(mdttvaluelimitstest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttvaluelimitstest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttvaluelimitstest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttvaluelimitstest "mdttesttool${MDTLIB_VERSION_SUFFIX}")

# * mdtTtTestSystemComponentTest executable *
QT4_WRAP_CPP(MDT_TEST_SYSTEM_COMPONENT_MOC "mdtTtTestSystemComponentTest.h")
add_executable(mdttestsystemcomponenttest mdtTtTestSystemComponentTest.cpp mdtCableListTestScenario.cpp ${MDT_TEST_SYSTEM_COMPONENT_MOC})
target_link_libraries(mdttestsystemcomponenttest ${QT_LIBRARIES})
target_link_libraries(mdttestsystemcomponenttest "mdttest")
target_link_libraries(mdttestsystemcomponenttest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttestsystemcomponenttest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdttestsystemcomponenttest "mdtdevice${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttestsystemcomponenttest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttestsystemcomponenttest "mdttesttool${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdttestsystemcomponenttest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")

# * mdtTestNodeTest executable *
QT4_WRAP_CPP(MDT_TEST_NODE_MOC "mdtTestNodeTest.h")
add_executable(mdttestnodetest mdtTestNodeTest.cpp mdtCableListTestScenario.cpp mdtTtTestNodeTestData.cpp ${MDT_TEST_NODE_MOC})
target_link_libraries(mdttestnodetest ${QT_LIBRARIES})
target_link_libraries(mdttestnodetest "mdttest")
target_link_libraries(mdttestnodetest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttestnodetest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdttestnodetest "mdtdevice${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttestnodetest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttestnodetest "mdttesttool${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdttestnodetest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")

# * mdtTestCableTest executable *
QT4_WRAP_CPP(MDT_TEST_CABLE_MOC "mdtTestCableTest.h")
add_executable(mdttestcabletest mdtTestCableTest.cpp mdtTtDut01TestData.cpp ${MDT_TEST_CABLE_MOC})
target_link_libraries(mdttestcabletest ${QT_LIBRARIES})
target_link_libraries(mdttestcabletest "mdttest")
target_link_libraries(mdttestcabletest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttestcabletest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttestcabletest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttestcabletest "mdttesttool${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdttestcabletest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")

# * mdtTestToolTest executable *
QT4_WRAP_CPP(MDT_TEST_TOOL_MOC "mdtTestToolTest.h")
add_executable(mdttesttooltest mdtTestToolTest.cpp mdtCableListTestScenario.cpp mdtTtTestNodeTestData.cpp ${MDT_TEST_TOOL_MOC})
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
