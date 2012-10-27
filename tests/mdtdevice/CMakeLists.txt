# CMake project file for Multi X Diag Tools
# * Unit test for: mdtdevice *
# * Directory: tests/mdtdevice *

# Project
project (multidiagtools)

# mdtDeviceTest
QT4_WRAP_CPP (MDT_DEVICE_TEST_MOC_HEADERS "mdtDeviceTest.h")
add_executable (mdtdevicetest "mdtDeviceTest.cpp" ${MDT_DEVICE_TEST_MOC_HEADERS})
# Libraries to link
target_link_libraries(mdtdevicetest "mdtutils")
target_link_libraries(mdtdevicetest "mdtdevice")
target_link_libraries(mdtdevicetest "mdttest")
target_link_libraries(mdtdevicetest ${QT_LIBRARIES})