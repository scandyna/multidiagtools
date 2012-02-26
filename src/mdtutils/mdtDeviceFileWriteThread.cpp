
#include "mdtDeviceFileWriteThread.h"
#include <QDebug>
#include <QApplication>

mdtDeviceFileWriteThread::mdtDeviceFileWriteThread(QObject *parent)
 : mdtDeviceFileThread(parent)
{
}

mdtFrame *mdtDeviceFileWriteThread::getNewFrame()
{
  Q_ASSERT(pvDeviceFile != 0);

  mdtFrame *frame;

  // Check if there is a frame to transmit
  if(pvDeviceFile->writeFrames().size() < 1){
    return 0;
  }
  frame = pvDeviceFile->writeFrames().dequeue();
  Q_ASSERT(frame != 0);

  return frame;
}

void mdtDeviceFileWriteThread::run()
{
  Q_ASSERT(pvDeviceFile != 0);

  char *buffer;
  char *bufferCursor = 0;
  mdtFrame *frame = 0;
  int toWrite = 0;
  int written = 0;
 
  // Set the running flag
  pvDeviceFile->lockMutex();
  pvRunning = true;
  pvDeviceFile->unlockMutex();

  // Run...
  while(1){
    // Read thread state
    pvDeviceFile->lockMutex();
    if(!pvRunning){
      pvDeviceFile->unlockMutex();
      break;
    }
    pvDeviceFile->unlockMutex();

    // Wait on write ready event
    if(!pvDeviceFile->waitEventWriteReady()){
      pvDeviceFile->lockMutex();
      pvRunning = false;
      pvDeviceFile->unlockMutex();
      break;
    }
    // Event occured, send the data to port - Check timeout state first
    if(!pvDeviceFile->writeTimeoutOccured()){
      pvDeviceFile->lockMutex();
      // Check if we have something to transmit
      if(frame == 0){
        frame = getNewFrame();
        if(frame != 0){
          // New frame to transmit
          buffer = frame->data();
          bufferCursor = buffer;
          //qDebug() << "TX thd: bufferCursor [NF]: " << bufferCursor;
          toWrite = frame->size();
        }
      }
      if(frame != 0){
        // Write data to port
        written = pvDeviceFile->writeData(bufferCursor, toWrite);
        ///qDebug() << "TX thd: written: " << written;
        frame->take(written);
        ///qDebug() << "TX thd: frame size: " << frame->size();
        // Check if current frame was completly sent
        if(frame->isEmpty()){
          pvDeviceFile->writeFramesPool().enqueue(frame);
          frame = 0;
        }else{
          bufferCursor += written;
          ///qDebug() << "TX thd: bufferCursor [CF]: " << bufferCursor;
          Q_ASSERT(bufferCursor < (buffer + frame->size()));
          toWrite -= written;
          Q_ASSERT(toWrite >= 0);
        }
      }
      pvDeviceFile->unlockMutex();
    }
  }
}
