#ifndef MDT_SERIAL_PORT_CTL_THREAD_H
#define MDT_SERIAL_PORT_CTL_THREAD_H

#include "mdtPortThread.h"
#include "mdtSerialPort.h"
#include <QObject>

class mdtSerialPortCtlThread : public mdtPortThread
{
 public:

  mdtSerialPortCtlThread(QObject *parent = 0);

 private:

  // Thread implementation
  void run();
};

#endif  // #ifndef MDT_SERIAL_PORT_CTL_THREAD_H
