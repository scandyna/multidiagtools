#ifndef MDT_SERIAL_PORT_TX_THREAD_H
#define MDT_SERIAL_PORT_TX_THREAD_H

#include "mdtSerialPortThread.h"
#include <QObject>

class mdtSerialPortTxThread : public mdtSerialPortThread
{
 public:

  mdtSerialPortTxThread(QObject *parent = 0);

 private:

  // Thread implementation
  void run();
};

#endif  // #ifndef MDT_SERIAL_PORT_TX_THREAD_H
