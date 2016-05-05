# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)
enable_testing()

#================= Modification ====================

# Target MdtCableListModificationTest
add_executable(mdtcablelistmodificationtest src/ModificationTest.cpp)
target_link_libraries(mdtcablelistmodificationtest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistmodificationtest Qt5::Test)
add_test(MdtCableListModificationTest ${CMAKE_CURRENT_BINARY_DIR}/mdtcablelistmodificationtest)


#================= Article ====================

# Target MdtCableListArticleTest
add_executable(mdtcablelistarticletest src/ArticleTest.cpp)
target_link_libraries(mdtcablelistarticletest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistarticletest Qt5::Test)
add_test(MdtCableListArticleTest ${CMAKE_CURRENT_BINARY_DIR}/mdtcablelistarticletest)


#================= Unit ====================

# Target MdtCableListUnitConnectionTest
add_executable(mdtcablelistunitconnectiontest src/UnitConnectionTest.cpp)
target_link_libraries(mdtcablelistunitconnectiontest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistunitconnectiontest Qt5::Test)
add_test(MdtCableListUnitConnectionTest ${CMAKE_CURRENT_BINARY_DIR}/mdtcablelistunitconnectiontest)


#================= Link ====================

# Target MdtCableListLinkTest
add_executable(mdtcablelistlinktest src/LinkTest.cpp)
target_link_libraries(mdtcablelistlinktest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistlinktest Qt5::Test)
add_test(MdtCableListLinkTest ${CMAKE_CURRENT_BINARY_DIR}/mdtcablelistlinktest)


#================= Path Graph ====================

# MdtCableListPathGraphPrivateTest
add_executable(mdtcablelistpathgraphprivatetest src/PathGraphPrivateTest.cpp)
target_link_libraries(mdtcablelistpathgraphprivatetest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistpathgraphprivatetest Qt5::Test)
add_test(MdtCableListPathGraphPrivateTest ${CMAKE_CURRENT_BINARY_DIR}/mdtcablelistpathgraphprivatetest)

# MdtCableListPathGraphTest
add_executable(mdtcablelistpathgraphtest src/PathGraphTest.cpp)
target_link_libraries(mdtcablelistpathgraphtest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistpathgraphtest Qt5::Test)
add_test(MdtCableListPathGraphTest ${CMAKE_CURRENT_BINARY_DIR}/mdtcablelistpathgraphtest)
