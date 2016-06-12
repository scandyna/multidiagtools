# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)
enable_testing()

# Target MdtAlgorithTestTest
add_executable(mdtalgorithmtest src/AlgorithmTest.cpp)
target_link_libraries(mdtalgorithmtest "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtalgorithmtest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtalgorithmtest Qt5::Test)
add_test(MdtAlgorithTestTest ${CMAKE_CURRENT_BINARY_DIR}/mdtalgorithmtest)
