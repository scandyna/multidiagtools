#ifndef MDT_SERIAL_PORT_CTL_THREAD_H
#define MDT_SERIAL_PORT_CTL_THREAD_H

#include "mdtPortThread.h"
#include "mdtSerialPort.h"
#include <QObject>

class mdtSerialPortCtlThread : public mdtPortThread
{
 public:

  mdtSerialPortCtlThread(QObject *parent = 0);

  /*! \brief Re-implemented stop function
   *  Note: this is Posix implementation specific
   *  \pre Serial port instance must be defined. \see mdtSerialPortThread::setDeviceFile()
   */
#ifdef Q_OS_UNIX
  ///void stop();
#endif

 private:

  // Thread implementation
  void run();
};

#endif  // #ifndef MDT_SERIAL_PORT_CTL_THREAD_H
