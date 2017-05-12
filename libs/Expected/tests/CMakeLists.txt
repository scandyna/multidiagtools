# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)
enable_testing()

# Target MdtExpectedTest
add_executable(mdtexpectedtest src/ExpectedTest.cpp)
add_test(MdtExpectedTest ${CMAKE_CURRENT_BINARY_DIR}/mdtexpectedtest)
target_link_libraries(mdtexpectedtest "mdtexpected${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtexpectedtest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtexpectedtest Qt5::Test)
