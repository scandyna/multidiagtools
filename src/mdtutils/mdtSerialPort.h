#ifndef MDT_SERIAL_PORT_H
#define MDT_SERIAL_PORT_H

#include <QtGlobal>

#ifdef Q_OS_UNIX
 #include "linux/mdtSerialPortPosix.h"
#endif

#ifdef Q_OS_UNIX
class mdtSerialPort : public mdtSerialPortPosix
#endif
{
 public:

  mdtSerialPort();
  ~mdtSerialPort();
};

#endif  // #ifndef MDT_SERIAL_PORT_H
