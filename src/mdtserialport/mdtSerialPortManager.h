#ifndef MDT_SERIAL_PORT_MANAGER_H
#define MDT_SERIAL_PORT_MANAGER_H

#include <QtGlobal>

#ifdef Q_OS_UNIX
 #include "linux/mdtSerialPortManagerPosix.h"
 #define mdtSerialPortManagerSys mdtSerialPortManagerPosix
#endif

#include "mdtSerialPortConfig.h"
#include "mdtSerialPortInterface.h"

class mdtSerialPortManager : public mdtSerialPortManagerSys
{
 public:

  mdtSerialPortManager();
  ~mdtSerialPortManager();
  
  /*! \brief Get interface inatance for given port number
   *  Note that port numbers are generated during scan()
   *  For example, if a Unix system has /dev/ttyS0 , ttyS1 and ttyUSB0 ,
   *  port 1 will refer to ttyS0, port 2 to to ttyS1 and port 3 to ttyUSB0
   *  \param portNumber Port number
   *  \return The rquested interface, or a Null pointer if not available
   */
  mdtSerialPortInterface *interface(int portNumber);

 private:

  mdtSerialPortConfig *pvConfig;
};

#endif  // #ifndef MDT_SERIAL_PORT_MANAGER_H
