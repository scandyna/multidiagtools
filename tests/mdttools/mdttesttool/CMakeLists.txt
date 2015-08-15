# CMake project file for Multi Diag Tools
# * Unit test for: mdttesttool *
# * Directory: tests/mdttools/mdttesttool *

# Project
project (multidiagtools)

# * mdtClModificationTest executable *
add_executable(mdtclmodificationtest mdtClModificationTest.cpp)
target_link_libraries(mdtclmodificationtest Qt5::Widgets)
target_link_libraries(mdtclmodificationtest "mdttest")
target_link_libraries(mdtclmodificationtest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclmodificationtest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdtclmodificationtest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclmodificationtest "mdttesttool${MDTLIB_VERSION_SUFFIX}")

# * mdtClConnectorTest executable *
add_executable(mdtclconnectortest mdtClConnectorTest.cpp)
target_link_libraries(mdtclconnectortest Qt5::Widgets)
target_link_libraries(mdtclconnectortest "mdttest")
target_link_libraries(mdtclconnectortest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclconnectortest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdtclconnectortest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclconnectortest "mdttesttool${MDTLIB_VERSION_SUFFIX}")

# * mdtClArticleConnectionTest executable *
add_executable(mdtclarticleconnectiontest mdtClArticleConnectionTest.cpp mdtCableListTestScenario.cpp)
target_link_libraries(mdtclarticleconnectiontest Qt5::Widgets)
target_link_libraries(mdtclarticleconnectiontest "mdttest")
target_link_libraries(mdtclarticleconnectiontest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclarticleconnectiontest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdtclarticleconnectiontest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclarticleconnectiontest "mdttesttool${MDTLIB_VERSION_SUFFIX}")

# * mdtClUnitConnectionTest executable *
add_executable(mdtclunitconnectiontest mdtClUnitConnectionTest.cpp mdtCableListTestScenario.cpp)
target_link_libraries(mdtclunitconnectiontest Qt5::Widgets)
target_link_libraries(mdtclunitconnectiontest "mdttest")
target_link_libraries(mdtclunitconnectiontest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclunitconnectiontest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdtclunitconnectiontest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclunitconnectiontest "mdttesttool${MDTLIB_VERSION_SUFFIX}")

# * mdtClLinkTest executable *
add_executable(mdtcllinktest mdtClLinkTest.cpp mdtCableListTestScenario.cpp mdtClVehicleTypeLinkTestData.cpp)
target_link_libraries(mdtcllinktest Qt5::Widgets)
target_link_libraries(mdtcllinktest "mdttest")
target_link_libraries(mdtcllinktest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcllinktest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdtcllinktest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcllinktest "mdttesttool${MDTLIB_VERSION_SUFFIX}")

# * mdtClArticleLinkTest executable *
add_executable(mdtclarticlelinktest mdtClArticleLinkTest.cpp mdtCableListTestScenario.cpp)
target_link_libraries(mdtclarticlelinktest Qt5::Widgets)
target_link_libraries(mdtclarticlelinktest "mdttest")
target_link_libraries(mdtclarticlelinktest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclarticlelinktest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdtclarticlelinktest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclarticlelinktest "mdttesttool${MDTLIB_VERSION_SUFFIX}")

# * mdtClAutoConnectionTest executable *
add_executable(mdtclautoconnectiontest mdtClAutoConnectionTest.cpp mdtCableListTestScenario.cpp)
target_link_libraries(mdtclautoconnectiontest Qt5::Widgets)
target_link_libraries(mdtclautoconnectiontest "mdttest")
target_link_libraries(mdtclautoconnectiontest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclautoconnectiontest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdtclautoconnectiontest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtclautoconnectiontest "mdttesttool${MDTLIB_VERSION_SUFFIX}")

# * mdtTtValueLimitsTest executable *
add_executable(mdttvaluelimitstest mdtTtValueLimitsTest.cpp)
target_link_libraries(mdttvaluelimitstest Qt5::Widgets)
target_link_libraries(mdttvaluelimitstest "mdttest")
target_link_libraries(mdttvaluelimitstest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttvaluelimitstest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttvaluelimitstest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttvaluelimitstest "mdttesttool${MDTLIB_VERSION_SUFFIX}")

# * mdtTtTestSystemComponentTest executable *
add_executable(mdttestsystemcomponenttest mdtTtTestSystemComponentTest.cpp mdtCableListTestScenario.cpp)
target_link_libraries(mdttestsystemcomponenttest Qt5::Widgets)
target_link_libraries(mdttestsystemcomponenttest "mdttest")
target_link_libraries(mdttestsystemcomponenttest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttestsystemcomponenttest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdttestsystemcomponenttest "mdtdevice${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttestsystemcomponenttest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttestsystemcomponenttest "mdttesttool${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdttestsystemcomponenttest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")

# * mdtTestNodeTest executable *
add_executable(mdttestnodetest mdtTestNodeTest.cpp mdtCableListTestScenario.cpp mdtTtTestNodeTestData.cpp)
target_link_libraries(mdttestnodetest Qt5::Widgets)
target_link_libraries(mdttestnodetest "mdttest")
target_link_libraries(mdttestnodetest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttestnodetest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdttestnodetest "mdtdevice${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttestnodetest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttestnodetest "mdttesttool${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdttestnodetest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")

# * mdtTestCableTest executable *
add_executable(mdttestcabletest mdtTestCableTest.cpp mdtTtDut01TestData.cpp)
target_link_libraries(mdttestcabletest Qt5::Widgets)
target_link_libraries(mdttestcabletest "mdttest")
target_link_libraries(mdttestcabletest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttestcabletest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttestcabletest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttestcabletest "mdttesttool${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdttestcabletest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")

# * mdtTestToolTest executable *
add_executable(mdttesttooltest mdtTestToolTest.cpp mdtCableListTestScenario.cpp mdtTtTestNodeTestData.cpp)
target_link_libraries(mdttesttooltest Qt5::Widgets)
target_link_libraries(mdttesttooltest "mdttest")
target_link_libraries(mdttesttooltest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttesttooltest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdttesttooltest "mdtdevice${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttesttooltest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdttesttooltest "mdttesttool${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdttesttooltest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")

# * mdtCableListTest executable *
add_executable(mdtcablelisttest mdtCableListTest.cpp mdtCableListTestScenario.cpp)
target_link_libraries(mdtcablelisttest Qt5::Widgets)
target_link_libraries(mdtcablelisttest "mdttest")
target_link_libraries(mdtcablelisttest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelisttest "mdtutilsgui${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelisttest "mdtdatabase${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelisttest "mdttesttool${MDTLIB_VERSION_SUFFIX}")
#target_link_libraries(mdtcablelisttest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
