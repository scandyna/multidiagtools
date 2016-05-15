# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)
enable_testing()

# Target MdtApplicationTest
add_executable(mdtapplicationtest src/ApplicationTest.cpp)
add_test(MdtApplicationTest ${CMAKE_CURRENT_BINARY_DIR}/mdtapplicationtest)
target_link_libraries(mdtapplicationtest "${LIBRARY_BIN_NAME}${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtapplicationtest Qt5::Test)
