#ifndef MDT_SERIAL_PORT_INTERFACE_MANAGER_H
#define MDT_SERIAL_PORT_INTERFACE_MANAGER_H

#include <QtGlobal>

#ifdef Q_OS_UNIX
 #include "mdtSerialPortInterfaceManagerPosix.h"
#endif

#include "mdtSerialPortConfig.h"

class mdtSerialPortInterfaceManager : public mdtSerialPortInterfaceManagerPosix
{
 public:

  mdtSerialPortInterfaceManager();
  ~mdtSerialPortInterfaceManager();

 private:

  mdtSerialPortConfig *pvConfig;
};

#endif  // #ifndef MDT_SERIAL_PORT_INTERFACE_MANAGER_H
