#ifndef MDT_DEVICE_FILE_WRITE_THREAD_H
#define MDT_DEVICE_FILE_WRITE_THREAD_H

#include "mdtDeviceFileThread.h"
#include <QObject>

class mdtDeviceFileWriteThread : public mdtDeviceFileThread
{
 public:

  mdtDeviceFileWriteThread(QObject *parent = 0);

 private:

  // Get a new frame from TX frames
  // If no frame is available, a null pointer is returned
  // Note: this function handle not the serial port mutex
  mdtFrame *getNewFrame();

  // Thread implementation
  void run();
};

#endif  // #ifndef MDT_DEVICE_FILE_WRITE_THREAD_H
