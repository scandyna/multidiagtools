#ifndef MDT_SERIAL_PORT_RX_THREAD_H
#define MDT_SERIAL_PORT_RX_THREAD_H

#include "mdtSerialPortThread.h"
#include <QObject>

class mdtSerialPortRxThread : public mdtSerialPortThread
{
 public:

  mdtSerialPortRxThread(QObject *parent = 0);

 private:

  // Thread implementation
  void run();
};

#endif  // #ifndef MDT_SERIAL_PORT_RX_THREAD_H
