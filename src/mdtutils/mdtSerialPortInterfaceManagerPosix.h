#ifndef MDT_SERIAL_PORT_INTERFACE_MANAGER_POSIX_H
#define MDT_SERIAL_PORT_INTERFACE_MANAGER_POSIX_H

#include "mdtAbstractSerialPortInterfaceManager.h"

#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

/* Ressources
 * Serial Programming Guide for POSIX Operating Systems
 *  Link: http://www.easysw.com/~mike/serial/serial
 */

#include <QString>

class mdtSerialPortInterfaceManagerPosix : public mdtAbstractSerialPortInterfaceManager
{
 public:

  mdtSerialPortInterfaceManagerPosix();
  ~mdtSerialPortInterfaceManagerPosix();
  
  bool scan();

 private:
  
  bool openPort(const QString &path);
  
  mdtSerialPortInterface::sp_uart_type_t uartType();
   
  int pvFd;     // Interface file descriptor
  
};

#endif  // #ifndef MDT_SERIAL_PORT_INTERFACE_MANAGER_POSIX_H
