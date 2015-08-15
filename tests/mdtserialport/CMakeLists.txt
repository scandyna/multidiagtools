# CMake project file for Multi X Diag Tools
# * Unit test for: mdtutils *
# * Directory: tests/mdtserialport *

# Project
project (multidiagtools)

# Target mdtSerialPortTest
add_executable (mdtserialporttest mdtSerialPortTest.cpp)
target_link_libraries(mdtserialporttest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtserialporttest "mdtport${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtserialporttest "mdtserialport${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtserialporttest "mdttest")
target_link_libraries(mdtserialporttest Qt5::Core)

# Target mdtSerialPortConfigTest
add_executable (mdtserialportconfigtest mdtSerialPortConfigTest.cpp)
target_link_libraries(mdtserialportconfigtest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtserialportconfigtest "mdtport${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtserialportconfigtest "mdtserialport${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtserialportconfigtest "mdttest")
target_link_libraries(mdtserialportconfigtest Qt5::Core)

# Target mdtSerialPortManagerTest
add_executable (mdtserialportmanagertest mdtSerialPortManagerTest.cpp)
target_link_libraries(mdtserialportmanagertest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtserialportmanagertest "mdtport${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtserialportmanagertest "mdtserialport${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtserialportmanagertest "mdttest")
target_link_libraries(mdtserialportmanagertest Qt5::Core)
