# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)
enable_testing()

# Target MdtErrorTest
add_executable(mdterrortest src/ErrorTest.cpp)
target_link_libraries(mdterrortest "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdterrortest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdterrortest Qt5::Test)
add_test(MdtErrorTest ${CMAKE_CURRENT_BINARY_DIR}/mdterrortest)
