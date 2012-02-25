#ifndef MDT_DEVICE_FILE_READ_THREAD_H
#define MDT_DEVICE_FILE_READ_THREAD_H

#include "mdtDeviceFileThread.h"
#include "mdtFrame.h"
#include <QObject>

class mdtDeviceFileReadThread : public mdtDeviceFileThread
{
 public:

  mdtDeviceFileReadThread(QObject *parent = 0);

 private:

  // Get a new frame from RX frames pool
  // If no frame is available, a null pointer is returned
  // Note: this function handle not the serial port mutex
  mdtFrame *getNewFrame();

  // Thread implementation
  void run();
};

#endif  // #ifndef MDT_DEVICE_FILE_READ_THREAD_H
