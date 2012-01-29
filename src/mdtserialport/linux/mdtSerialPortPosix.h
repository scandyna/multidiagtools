#ifndef MDT_SERIAL_PORT_POSIX_H
#define MDT_SERIAL_PORT_POSIX_H

#include "mdtAbstractSerialPort.h"
#include "mdtSerialPortConfig.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <pthread.h>
#include <QObject>

/*
 * Ressources: http://www.easysw.com/~mike/serial/serial.html
 * 
 */

class mdtSerialPortPosix : public mdtAbstractSerialPort
{
 public:

  mdtSerialPortPosix(QObject *parent = 0);
  ~mdtSerialPortPosix();

  // Open a port with a given configuration
  bool openPort(mdtSerialPortConfig &cfg);

  // Set the RX timeout [ms]
  void setRxTimeout(int timeout);

  // Set the TX timeout [ms]
  void setTxTimeout(int timeout);

  // Wait on RX event
  bool waitEventRx();
  
  int readData(char *data, int maxLen);
  
  // Wait until data TX is possible
  bool waitEventTxReady();
    
  int writeData(const char *data, int len);

  // Wait until a control signal (modem line state) changes
  bool waitEventCtl();
  
  // Get the control signal (modem line) states and update member flags
  bool getCtlStates();

  void setRts(bool on);
  
  void setDtr(bool on);
  
  // Must be called from signal control thread (see mdtSerialPortCtlThread)
  void defineCtlThread(pthread_t ctlThread);
  
  // Abort the waitEventCtl() function
  // Note: the thread in witch the waitEventCtl() function
  //       is called must be defined with defineCtlThread() before
  void abortWaitEventCtl();

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
  
  // Check if configuration could be done on system
  bool checkConfig(mdtSerialPortConfig cfg);
  
  // Used to catch the SIGALRM signal (doese nothing else)
  static void sigactionHandle(int signum);
  
  int pvSerialPortFd;               // Serial port file descriptor
  struct termios pvTermios;         // Termios configuration structure
  struct termios pvOriginalTermios; // Original termios configuration structure to restore
  // Timeouts
  struct timeval pvRxTimeout;
  struct timeval pvTxTimeout;
  // Ctl signals states memory
  int pvPreviousCarState;
  int pvPreviousDsrState;
  int pvPreviousCtsState;
  int pvPreviousRngState;
  // Members used for control signals thread kill
  pthread_t pvCtlThread;
  struct sigaction pvSigaction;
  bool pvAbortingWaitEventCtl;
};

#endif  // #ifndef MDT_SERIAL_PORT_POSIX_H
