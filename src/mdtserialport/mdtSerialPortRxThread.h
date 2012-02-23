#ifndef MDT_SERIAL_PORT_RX_THREAD_H
#define MDT_SERIAL_PORT_RX_THREAD_H

#include "mdtSerialPortThread.h"
#include "mdtFrame.h"
#include <QObject>

class mdtSerialPortRxThread : public mdtSerialPortThread
{
 public:

  mdtSerialPortRxThread(QObject *parent = 0);

 private:

  // Get a new frame from RX frames pool
  // If no frame is available, a null pointer is returned
  // Note: this function handle not the serial port mutex
  mdtFrame *getNewFrame();

  // Thread implementation
  void run();
};

#endif  // #ifndef MDT_SERIAL_PORT_RX_THREAD_H
