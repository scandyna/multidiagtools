# CMake project file for Multi X Diag Tools
# * Unit test for: mdtport *
# * Directory: tests/mdtport *

# Project
project (multidiagtools)

# Target mdtPortLockTest
if(UNIX)
  add_executable (mdtportlocktest mdtPortLockTest.cpp)
  target_link_libraries(mdtportlocktest "mdtutils${MDTLIB_VERSION_SUFFIX}")
  target_link_libraries(mdtportlocktest "mdtport${MDTLIB_VERSION_SUFFIX}")
  target_link_libraries(mdtportlocktest "mdttest")
  target_link_libraries(mdtportlocktest Qt5::Core)
endif(UNIX)

# Target mdtDeviceAddressTest
add_executable (mdtdeviceaddresstest mdtDeviceAddressTest.cpp)
target_link_libraries(mdtdeviceaddresstest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdeviceaddresstest "mdtport${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtdeviceaddresstest "mdttest")
target_link_libraries(mdtdeviceaddresstest Qt5::Core)

# Target mdtPortTest
add_executable (mdtporttest mdtPortTest.cpp)
target_link_libraries(mdtporttest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtporttest "mdtport${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtporttest "mdttest")
target_link_libraries(mdtporttest Qt5::Core)

# Target mdtUsbPortTest

add_executable (mdtusbporttest mdtUsbPortTest.cpp)
target_link_libraries(mdtusbporttest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtusbporttest "mdtport${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtusbporttest "mdttest")
target_link_libraries(mdtusbporttest "usb-1.0")
target_link_libraries(mdtusbporttest Qt5::Core)

# Target usbTmcPortTest
if(UNIX)
  add_executable(mdtusbtmcporttest mdtUsbTmcPortTest.cpp)
  target_link_libraries(mdtusbtmcporttest "mdttest")
  target_link_libraries(mdtusbtmcporttest "mdtutils${MDTLIB_VERSION_SUFFIX}")
  target_link_libraries(mdtusbtmcporttest "mdtport${MDTLIB_VERSION_SUFFIX}")
  target_link_libraries(mdtusbtmcporttest Qt5::Core)
endif(UNIX)

# Target mdtTcpSocketTest
if(UNIX)
  add_executable(mdttcpsockettest mdtTcpSocketTest.cpp mdtTcpServer.cpp mdtTcpServerThread.cpp)
  target_link_libraries(mdttcpsockettest "mdttest")
  target_link_libraries(mdttcpsockettest "mdtutils${MDTLIB_VERSION_SUFFIX}")
  target_link_libraries(mdttcpsockettest "mdtport${MDTLIB_VERSION_SUFFIX}")
  target_link_libraries(mdttcpsockettest Qt5::Core)
endif(UNIX)

# Target mdtPortManagerTest
add_executable (mdtportmanagertest mdtPortManagerTest.cpp)
target_link_libraries(mdtportmanagertest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtportmanagertest "mdtport${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtportmanagertest "mdttest")
target_link_libraries(mdtportmanagertest Qt5::Core)

# Target mdtPortInfoTest
add_executable (mdtportinfotest mdtPortInfoTest.cpp)
target_link_libraries(mdtportinfotest "mdtutils${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtportinfotest "mdtport${MDTLIB_VERSION_SUFFIX}")
target_link_libraries(mdtportinfotest "mdttest")
target_link_libraries(mdtportinfotest Qt5::Core)
