#ifndef MDT_SERIAL_PORT_CTL_THREAD_H
#define MDT_SERIAL_PORT_CTL_THREAD_H

#include "mdtSerialPortThread.h"
#include <QObject>

class mdtSerialPortCtlThread : public mdtSerialPortThread
{
 public:

  mdtSerialPortCtlThread(QObject *parent = 0);
  
  /*! \brief Re-implemented stop function
   *  Note: this is Posix implementation specific
   *  \pre Serial port instance must be defined. \see mdtSerialPortThread::setSerialPort()
   */
#ifdef Q_OS_UNIX
  void stop();
#endif

 private:

  // Thread implementation
  void run();
};

#endif  // #ifndef MDT_SERIAL_PORT_CTL_THREAD_H
