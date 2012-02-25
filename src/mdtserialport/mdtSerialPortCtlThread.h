#ifndef MDT_SERIAL_PORT_CTL_THREAD_H
#define MDT_SERIAL_PORT_CTL_THREAD_H

#include "mdtDeviceFileThread.h"
#include "mdtSerialPort.h"
#include <QObject>

class mdtSerialPortCtlThread : public mdtDeviceFileThread
{
 public:

  mdtSerialPortCtlThread(QObject *parent = 0);

  // Overloaded method, this class need access to mdtSerialPort specific methods
  void setDeviceFile(mdtSerialPort *serialPort);

  /*! \brief Re-implemented stop function
   *  Note: this is Posix implementation specific
   *  \pre Serial port instance must be defined. \see mdtSerialPortThread::setDeviceFile()
   */
#ifdef Q_OS_UNIX
  void stop();
#endif

 private:

  // Thread implementation
  void run();

  // Overloaded pvDeviceFile, type mdtSerialPort
  mdtSerialPort *pvSerialPort;
};

#endif  // #ifndef MDT_SERIAL_PORT_CTL_THREAD_H
