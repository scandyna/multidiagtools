# CMake project file for Multi X Diag Tools
# * Unit test for: mdtutils *
# * Directory: tests/mdtserialport *

# Project
project (multidiagtools)


# Target mdtSerialPortTest
QT4_WRAP_CPP (MOC_HEADERS mdtSerialPortTest.h)
add_executable (mdtserialporttest mdtSerialPortTest.cpp ${MOC_HEADERS})
target_link_libraries(mdtserialporttest "mdtutils")
target_link_libraries(mdtserialporttest "mdtport")
target_link_libraries(mdtserialporttest "mdtserialport")
target_link_libraries(mdtserialporttest "mdttest")
target_link_libraries(mdtserialporttest ${QT_LIBRARIES})