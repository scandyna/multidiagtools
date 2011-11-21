#ifndef MDT_SERIAL_PORT_MANAGER_POSIX_H
#define MDT_SERIAL_PORT_MANAGER_POSIX_H

#include "mdtAbstractSerialPortManager.h"

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
#include <QList>

class mdtSerialPortManagerPosix : public mdtAbstractSerialPortManager
{
 public:

  mdtSerialPortManagerPosix();
  ~mdtSerialPortManagerPosix();

  // Scan system (/dev) for devices
  // and construct the list of found interfaces
  bool scan();

 private:

  // Try to open port. Return the file descriptor
  // that the open() function returns, or <0 on error
  int openPort(const QString &path);

  // Get the UART type for given file descriptor
  mdtSerialPortInterface::sp_uart_type_t uartType(int fd);
  
  // Get the list of available baud rates
  // At the base, the standard rates are added to list
  // When a device is rocognized, additionnal baud rates are added NOTE: to implement
  QList<int> availableBaudRates(mdtSerialPortInterface::sp_uart_type_t _uartType);
};

#endif  // #ifndef MDT_SERIAL_PORT_MANAGER_POSIX_H
