# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)

# Target MdtCableListPathGraphTest
add_executable(mdtcablelistpathgraphtest src/PathGraphTest.cpp)
target_link_libraries(mdtcablelistpathgraphtest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistpathgraphtest Qt5::Test)

# Target MdtCableListArticleTest
add_executable(mdtcablelistarticletest src/ArticleTest.cpp)
target_link_libraries(mdtcablelistarticletest "mdtcablelist${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtcablelistarticletest Qt5::Test)

# Add executables to tests list
enable_testing()
add_test(MdtCableListPathGraphTest ${CMAKE_CURRENT_BINARY_DIR}/mdtcablelistpathgraphtest)
add_test(MdtCableListArticleTest ${CMAKE_CURRENT_BINARY_DIR}/mdtcablelistarticletest)
