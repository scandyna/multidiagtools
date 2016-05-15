# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)
enable_testing()

#================= Modification ====================

# Target MdtCableListModificationTest
add_executable(mdtcablelistmodificationtest src/ModificationTest.cpp)
target_link_libraries(mdtcablelistmodificationtest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistmodificationtest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistmodificationtest Qt5::Test)
add_test(MdtCableListModificationTest ${CMAKE_CURRENT_BINARY_DIR}/mdtcablelistmodificationtest)


#================= Article ====================

# Target MdtCableListArticleTest
add_executable(mdtcablelistarticletest src/ArticleTest.cpp)
target_link_libraries(mdtcablelistarticletest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistarticletest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistarticletest Qt5::Test)
add_test(MdtCableListArticleTest ${CMAKE_CURRENT_BINARY_DIR}/mdtcablelistarticletest)

# Target MdtCableListArticleConnectionTest
add_executable(mdtcablelistarticleconnectiontest src/ArticleConnectionTest.cpp)
target_link_libraries(mdtcablelistarticleconnectiontest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistarticleconnectiontest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistarticleconnectiontest Qt5::Test)
add_test(MdtCableListArticleConnectionTest ${CMAKE_CURRENT_BINARY_DIR}/mdtcablelistarticleconnectiontest)

# Target MdtCableListArticleLinkTest
add_executable(mdtcablelistarticlelinktest src/ArticleLinkTest.cpp)
target_link_libraries(mdtcablelistarticlelinktest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistarticlelinktest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistarticlelinktest Qt5::Test)
add_test(MdtCableListArticleLinkTest ${CMAKE_CURRENT_BINARY_DIR}/mdtcablelistarticlelinktest)


#================= Unit ====================

# Target MdtCableListUnitConnectionTest
add_executable(mdtcablelistunitconnectiontest src/UnitConnectionTest.cpp)
target_link_libraries(mdtcablelistunitconnectiontest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistunitconnectiontest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistunitconnectiontest Qt5::Test)
add_test(MdtCableListUnitConnectionTest ${CMAKE_CURRENT_BINARY_DIR}/mdtcablelistunitconnectiontest)


#================= Wire ====================

# Target MdtCableListWireTest
add_executable(mdtcablelistwiretest src/WireTest.cpp)
target_link_libraries(mdtcablelistwiretest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistwiretest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistwiretest Qt5::Test)
add_test(MdtCableListWireTest ${CMAKE_CURRENT_BINARY_DIR}/mdtcablelistwiretest)


#================= LinkBeam ====================

# Target MdtCableListLinkBeamTest
add_executable(mdtcablelistlinkbeamtest src/LinkBeamTest.cpp)
target_link_libraries(mdtcablelistlinkbeamtest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistlinkbeamtest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistlinkbeamtest Qt5::Test)
add_test(MdtCableListLinkBeamTest ${CMAKE_CURRENT_BINARY_DIR}/mdtcablelistlinkbeamtest)


#================= Link ====================

# Target MdtCableListLinkTest
add_executable(mdtcablelistlinktest src/LinkTest.cpp)
target_link_libraries(mdtcablelistlinktest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistlinktest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistlinktest Qt5::Test)
add_test(MdtCableListLinkTest ${CMAKE_CURRENT_BINARY_DIR}/mdtcablelistlinktest)

# Target MdtCableListLinkStorageTest
add_executable(mdtcablelistlinkstoragetest src/LinkStorageTest.cpp)
target_link_libraries(mdtcablelistlinkstoragetest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistlinkstoragetest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistlinkstoragetest Qt5::Test)
add_test(MdtCableListLinkStorageTest ${CMAKE_CURRENT_BINARY_DIR}/mdtcablelistlinkstoragetest)


#================= Path Graph ====================

# MdtCableListPathGraphPrivateTest
add_executable(mdtcablelistpathgraphprivatetest src/PathGraphPrivateTest.cpp)
target_link_libraries(mdtcablelistpathgraphprivatetest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistpathgraphprivatetest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistpathgraphprivatetest Qt5::Test)
add_test(MdtCableListPathGraphPrivateTest ${CMAKE_CURRENT_BINARY_DIR}/mdtcablelistpathgraphprivatetest)

# MdtCableListPathGraphTest
add_executable(mdtcablelistpathgraphtest src/PathGraphTest.cpp)
target_link_libraries(mdtcablelistpathgraphtest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistpathgraphtest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistpathgraphtest Qt5::Test)
add_test(MdtCableListPathGraphTest ${CMAKE_CURRENT_BINARY_DIR}/mdtcablelistpathgraphtest)
