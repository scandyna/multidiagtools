# CMake project file for Multi X Diag Tools
# * Unit test for: mdtdevice *
# * Directory: tests/mdtdevice *

# Project
project (multidiagtools)

# mdtDeviceTest
add_executable (mdtdevicetest "mdtDeviceTest.cpp")
# Libraries to link
target_link_libraries(mdtdevicetest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdevicetest "mdtdevice${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdevicetest "mdttest")
target_link_libraries(mdtdevicetest Qt5::Core)
