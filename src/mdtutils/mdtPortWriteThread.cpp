
#include "mdtPortWriteThread.h"
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

  char *bufferCursor = 0;
  mdtFrame *frame = 0;
  qint64 toWrite = 0;
  qint64 written = 0;
 
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
      emit(errorOccured(MDT_PORT_IO_ERROR));
    }
    // Event occured, send the data to port - Check timeout state first
    if(!pvPort->writeTimeoutOccured()){
      pvPort->lockMutex();
      // Check if we have something to transmit
      if(frame == 0){
        frame = getNewFrame();
        if(frame != 0){
          // New frame to transmit
          bufferCursor = frame->data();
          toWrite = frame->size();
        }
      }
      if(frame != 0){
        // Write data to port
        if((toWrite > 0)&&(pvBytePerByteWrite)){
          written = pvPort->write(bufferCursor, 1);
        }else{
          written = pvPort->write(bufferCursor, toWrite);
        }
        if(written < 0){
          emit(errorOccured(MDT_PORT_IO_ERROR));
          written = 0;
        }
        frame->take(written);
        // Check if current frame was completly sent
        if(frame->isEmpty()){
          pvPort->writeFramesPool().enqueue(frame);
          emit frameWritten();
          frame = 0;
        }else{
          bufferCursor = frame->data();
          Q_ASSERT(bufferCursor < (frame->data() + frame->size()));
          toWrite -= written;
          Q_ASSERT(toWrite >= 0);
        }
      }
      pvPort->unlockMutex();
    }
  }
}
