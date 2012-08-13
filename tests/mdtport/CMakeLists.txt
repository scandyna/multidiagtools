# CMake project file for Multi X Diag Tools
# * Unit test for: mdtport *
# * Directory: tests/mdtport *

# Project
project (multidiagtools)

# Target mdtPortLockTest
if(UNIX)
  QT4_WRAP_CPP (MOC_MDT_PORT_LOCK_TEST mdtPortLockTest.h)
  add_executable (mdtportlocktest mdtPortLockTest.cpp ${MOC_MDT_PORT_LOCK_TEST})
  target_link_libraries(mdtportlocktest "mdtutils")
  target_link_libraries(mdtportlocktest "mdtport")
  target_link_libraries(mdtportlocktest "mdttest")
  target_link_libraries(mdtportlocktest ${QT_LIBRARIES})
endif(UNIX)

# Target mdtPortTest
QT4_WRAP_CPP (MOC_MDT_PORT_TEST mdtPortTest.h)
add_executable (mdtporttest mdtPortTest.cpp ${MOC_MDT_PORT_TEST})
target_link_libraries(mdtporttest "mdtutils")
target_link_libraries(mdtporttest "mdtport")
target_link_libraries(mdtporttest "mdttest")
target_link_libraries(mdtporttest ${QT_LIBRARIES})

# Target mdtUsbPortTest
QT4_WRAP_CPP (MOC_MDT_USB_PORT_TEST mdtUsbPortTest.h)
add_executable (mdtusbporttest mdtUsbPortTest.cpp ${MOC_MDT_USB_PORT_TEST})
target_link_libraries(mdtusbporttest "mdtutils")
target_link_libraries(mdtusbporttest "mdtport")
target_link_libraries(mdtusbporttest "mdttest")
target_link_libraries(mdtusbporttest "usb-1.0")
target_link_libraries(mdtusbporttest ${QT_LIBRARIES})

# Target usbTmcPortTest
if(UNIX)
  QT4_WRAP_CPP (MOC_MDT_USB_TMC_PORT_TEST mdtUsbTmcPortTest.h)
  add_executable(mdtusbtmcporttest mdtUsbTmcPortTest.cpp ${MOC_MDT_USB_TMC_PORT_TEST})
  target_link_libraries(mdtusbtmcporttest "mdttest")
  target_link_libraries(mdtusbtmcporttest "mdtutils")
  target_link_libraries(mdtusbtmcporttest "mdtport")
  target_link_libraries(mdtusbtmcporttest ${QT_LIBRARIES})
endif(UNIX)

# Target mdtTcpSocketTest
if(UNIX)
  QT4_WRAP_CPP (MOC_MDT_TMC_SOCKET_TEST mdtTcpSocketTest.h mdtTcpServer.h mdtTcpServerThread.h)
  add_executable(mdttcpsockettest mdtTcpSocketTest.cpp mdtTcpServer.cpp mdtTcpServerThread.cpp ${MOC_MDT_TMC_SOCKET_TEST})
  target_link_libraries(mdttcpsockettest "mdttest")
  target_link_libraries(mdttcpsockettest "mdtutils")
  target_link_libraries(mdttcpsockettest "mdtport")
  target_link_libraries(mdttcpsockettest ${QT_LIBRARIES})
endif(UNIX)

# Target mdtPortManagerTest
QT4_WRAP_CPP (MOC_MDT_PORT_MANAGER_TEST mdtPortManagerTest.h)
add_executable (mdtportmanagertest mdtPortManagerTest.cpp ${MOC_MDT_PORT_MANAGER_TEST})
target_link_libraries(mdtportmanagertest "mdtutils")
target_link_libraries(mdtportmanagertest "mdtport")
target_link_libraries(mdtportmanagertest "mdttest")
target_link_libraries(mdtportmanagertest ${QT_LIBRARIES})
