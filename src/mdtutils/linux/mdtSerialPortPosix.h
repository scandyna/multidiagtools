#ifndef MDT_SERIAL_PORT_POSIX_H
#define MDT_SERIAL_PORT_POSIX_H

#include "mdtAbstractSerialPort.h"
#include "mdtSerialPortConfig.h"

#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

/*
 * Ressources: http://www.easysw.com/~mike/serial/serial.html
 * 
 */

class mdtSerialPortPosix : public mdtAbstractSerialPort
{
 public:

  mdtSerialPortPosix();
  ~mdtSerialPortPosix();

  bool setConfig(mdtSerialPortConfig &cfg);

 private:

  // Set the baud rate. Returns false if baud rate is not supported
  bool setBaudRate(int rate);

  // Get configured baud rate
  int baudRate();

  // Set number of data bits. Returns false on un unsupported number
  bool setDataBits(int n);

  // Get configured number of data bits
  int dataBits();

  // Set number of stop bits. Returns false on un unsupported number
  bool setStopBits(int n);

  // Get configured number of stop bits
  int stopBits();

  //  Set parity check
  void setParity(mdtSerialPortConfig::parity_t p);

  // Get configured parity
  mdtSerialPortConfig::parity_t parity();

  // Enable/diseable RTS/CTS flow control
  void setFlowCtlRtsCts(bool on);

  // Returns true if RTS/CTS flow control is enabled
  bool flowCtlRtsCtsOn();

  // Enable/diseable Xon/Xoff flow control
  void setFlowCtlXonXoff(bool on, char xonChar, char xoffChar);

  // Returns true if Xon/Xoff flow control is enabled
  bool flowCtlXonXoffOn();

  // Restore original setup and close the port
  void closePort();
  
  int pvSerialPortFd;               // Serial port file descriptor
  struct termios pvTermios;         // Termios configuration structure
  struct termios pvOriginalTermios; // Original termios configuration structure to restore
};

#endif  // #ifndef MDT_SERIAL_PORT_POSIX_H
