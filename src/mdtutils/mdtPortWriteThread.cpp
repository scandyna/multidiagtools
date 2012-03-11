
#include "mdtPortWriteThread.h"
#include <QDebug>
#include <QApplication>

mdtPortWriteThread::mdtPortWriteThread(QObject *parent)
 : mdtPortThread(parent)
{
}

mdtFrame *mdtPortWriteThread::getNewFrame()
{
  Q_ASSERT(pvPort != 0);

  mdtFrame *frame;

  // Check if there is a frame to transmit
  if(pvPort->writeFrames().size() < 1){
    return 0;
  }
  frame = pvPort->writeFrames().dequeue();
  Q_ASSERT(frame != 0);

  return frame;
}

void mdtPortWriteThread::run()
{
  Q_ASSERT(pvPort != 0);

  char *buffer;
  char *bufferCursor = 0;
  mdtFrame *frame = 0;
  int toWrite = 0;
  int written = 0;
 
  // Set the running flag
  pvPort->lockMutex();
  pvRunning = true;
  pvPort->unlockMutex();

  // Run...
  while(1){
    // Read thread state
    pvPort->lockMutex();
    if(!pvRunning){
      pvPort->unlockMutex();
      break;
    }
    pvPort->unlockMutex();

    // Wait the minimal time if requierd
    if(pvWriteMinWaitTime > 0){
      msleep(pvWriteMinWaitTime);
    }
    // Wait on write ready event
    if(!pvPort->waitEventWriteReady()){
      pvPort->lockMutex();
      pvRunning = false;
      pvPort->unlockMutex();
      break;
    }
    // Event occured, send the data to port - Check timeout state first
    if(!pvPort->writeTimeoutOccured()){
      pvPort->lockMutex();
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
        ///qDebug() << "WR THD, have data to send: " << *frame;
        written = pvPort->writeData(bufferCursor, toWrite);
        ///qDebug() << "TX thd: written: " << written;
        frame->take(written);
        ///qDebug() << "TX thd: frame size: " << frame->size();
        // Check if current frame was completly sent
        if(frame->isEmpty()){
          pvPort->writeFramesPool().enqueue(frame);
          ///qDebug() << "WTHD: frame written, emit signal";
          emit frameWritten();
          frame = 0;
        }else{
          bufferCursor += written;
          ///qDebug() << "TX thd: bufferCursor [CF]: " << bufferCursor;
          Q_ASSERT(bufferCursor < (buffer + frame->size()));
          toWrite -= written;
          Q_ASSERT(toWrite >= 0);
        }
      }
      pvPort->unlockMutex();
    }
  }
}
