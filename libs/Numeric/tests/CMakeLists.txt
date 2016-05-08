# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)
enable_testing()


#================= Tests ====================

# Target MdtNumericDoubleTest
add_executable(mdtnumericdoubletest src/DoubleTest.cpp)
# target_link_libraries(mdtnumericdoubletest "mdtnumeric${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtnumericdoubletest Qt5::Test)
target_link_libraries(mdtnumericdoubletest "mdtutils${MDTLIB_VERSION_SUFFIX}")
add_test(MdtNumericDoubleTest ${CMAKE_CURRENT_BINARY_DIR}/mdtnumericdoubletest)


