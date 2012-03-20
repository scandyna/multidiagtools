
#include "mdtPortReadThread.h"
#include "mdtError.h"
#include <QApplication>

//#include <QDebug>

mdtPortReadThread::mdtPortReadThread(QObject *parent)
 : mdtPortThread(parent)
{
}

mdtFrame *mdtPortReadThread::getNewFrame()
{
  Q_ASSERT(pvPort != 0);

  mdtFrame *frame;

  if(pvPort->readFramesPool().size() < 1){
    mdtError e(MDT_UNDEFINED_ERROR, "Read frames pool is empty", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtPortReadThread");
    e.commit();
    return 0;
  }
  frame = pvPort->readFramesPool().dequeue();
  Q_ASSERT(frame != 0);
  frame->clear();

  return frame;
}

void mdtPortReadThread::run()
{
  Q_ASSERT(pvPort != 0);

  qint64 bufferSize;
  char *buffer = 0;
  char *bufferCursor = 0;
  mdtFrame *frame;
  qint64 readen = 0;
  qint64 stored = 0;
  qint64 toStore = 0;

  // Set the running flag and get a RX frame
  pvPort->lockMutex();
  pvRunning = true;
  frame = getNewFrame();
  if(frame == 0){
    mdtError e(MDT_UNDEFINED_ERROR, "No frame available in read frames pool, stopping read thread", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtPortReadThread");
    e.commit();
    pvRunning = false;
    pvPort->unlockMutex();
    return;
  }
  // Alloc the local buffer
  bufferSize = frame->capacity();
  buffer = new char[bufferSize];
  if(buffer == 0){
    mdtError e(MDT_UNDEFINED_ERROR, "Cannot allocate memory for local buffer" , mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtPortReadThread");
    e.commit();
    pvRunning = false;
    pvPort->unlockMutex();
    return;
  }
  bufferCursor = buffer;
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
    if(pvReadMinWaitTime > 0){
      msleep(pvReadMinWaitTime);
    }
    // Wait on Read event
    if(!pvPort->waitForReadyRead()){
      emit(errorOccured(MDT_PORT_IO_ERROR));
    }
    // Event occured, get the data from port - Check timeout state first
    if(!pvPort->readTimeoutOccured()){
      pvPort->lockMutex();
      // Reset bufferCursor
      bufferCursor = buffer;
      // Read data from port
      readen = pvPort->read(buffer, bufferSize);
      //qDebug() << "RX thread, readen: " << readen << " , buffer: " << buffer;
      if(readen < 0){
        readen = 0;
        emit(errorOccured(MDT_PORT_IO_ERROR));
      }
      toStore = readen;
      // Store readen data
      while(toStore > 0){
        // Check for new frame if needed
        while(frame == 0){
          frame = getNewFrame();
          if(frame == 0){
            // Pool empty. Try later
            pvPort->unlockMutex();
            msleep(100);
            // Check about end of thread
            pvPort->lockMutex();
            if(!pvRunning){
              pvPort->unlockMutex();
              return;
            }
          }
        }
        // Store data
        stored = frame->putData(bufferCursor, toStore);
        // If frame is full, enqueue to readen frames and get a new one
        if(frame->bytesToStore() == 0){
          stored += frame->eofSeqLen();
          pvPort->readenFrames().enqueue(frame);
          pvPort->unlockMutex();
          // emit a Readen frame signal if complete
          if(frame->isComplete()){
            emit newFrameReaden();
          }
          frame = getNewFrame();
        }
        // When frame becomes full and EOF seq was not reached, stored will be to big
        if(stored >= bufferSize){
          stored = bufferSize-1;
          toStore = 0;
        }else{
          toStore = toStore - stored;
        }
        bufferCursor = bufferCursor + stored;
        Q_ASSERT(bufferCursor < (buffer + bufferSize));
      }
      pvPort->unlockMutex();
    }
  }

  // Put current frame into pool
  if(frame != 0){
    pvPort->readFramesPool().enqueue(frame);
  }

  // Free memory
  Q_ASSERT(buffer != 0);
  delete buffer;
}
