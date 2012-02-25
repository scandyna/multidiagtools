#ifndef MDT_SERIAL_PORT_TX_THREAD_H
#define MDT_SERIAL_PORT_TX_THREAD_H

#include "mdtSerialPortThread.h"
#include <QObject>

class mdtSerialPortTxThread : public mdtSerialPortThread
{
 public:

  mdtSerialPortTxThread(QObject *parent = 0);

 private:

  // Get a new frame from TX frames
  // If no frame is available, a null pointer is returned
  // Note: this function handle not the serial port mutex
  mdtFrame *getNewFrame();

  // Thread implementation
  void run();
};

#endif  // #ifndef MDT_SERIAL_PORT_TX_THREAD_H
