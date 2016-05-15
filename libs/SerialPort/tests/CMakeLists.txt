# CMake project file for Multi Diag Tools

# Project
project(multidiagtools)

# Target mdtSerialPortTest
add_executable(mdtserialporttest src/mdtSerialPortTest.cpp)
target_link_libraries(mdtserialporttest "mdtserialport${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtserialporttest "mdtapplication${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtserialporttest Qt5::Test)

# Add executables to tests list
enable_testing()
add_test(mdtSerialPortTest ${CMAKE_CURRENT_BINARY_DIR}/mdtserialporttest)
