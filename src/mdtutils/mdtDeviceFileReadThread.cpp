
#include "mdtDeviceFileReadThread.h"
#include "mdtError.h"
#include <QDebug>
#include <QApplication>

mdtDeviceFileReadThread::mdtDeviceFileReadThread(QObject *parent)
 : mdtDeviceFileThread(parent)
{
}

mdtFrame *mdtDeviceFileReadThread::getNewFrame()
{
  Q_ASSERT(pvDeviceFile != 0);

  mdtFrame *frame;

  if(pvDeviceFile->readFramesPool().size() < 1){
    mdtError e(MDT_UNDEFINED_ERROR, "RX frames pool is empty", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtDeviceFileReadThread");
    e.commit();
    return 0;
  }
  frame = pvDeviceFile->readFramesPool().dequeue();
  Q_ASSERT(frame != 0);
  frame->clear();

  return frame;
}

void mdtDeviceFileReadThread::run()
{
  Q_ASSERT(pvDeviceFile != 0);

  int bufferSize;
  char *buffer = 0;
  char *bufferCursor = 0;
  mdtFrame *frame;
  int readen = 0;
  int stored = 0;
  int toStore = 0;

  // Set the running flag and get a RX frame
  pvDeviceFile->lockMutex();
  pvRunning = true;
  frame = getNewFrame();
  if(frame == 0){
    mdtError e(MDT_UNDEFINED_ERROR, "No frame available in RX frames pool, stopping RX thread", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceFileReadThread");
    e.commit();
    pvRunning = false;
    pvDeviceFile->unlockMutex();
    return;
  }
  pvDeviceFile->unlockMutex();

  // Alloc the local buffer
  bufferSize = frame->capacity();
  buffer = new char[bufferSize];
  if(buffer == 0){
    mdtError e(MDT_UNDEFINED_ERROR, "Cannot allocate memory for local buffer" , mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtDeviceFileReadThread");
    e.commit();
    return;
  }
  bufferCursor = buffer;

  // Run...
  while(1){
    // Read thread state
    pvDeviceFile->lockMutex();
    if(!pvRunning){
      pvDeviceFile->unlockMutex();
      break;
    }
    pvDeviceFile->unlockMutex();

    // Wait on RX event
    if(!pvDeviceFile->waitEventRead()){
      pvDeviceFile->lockMutex();
      pvRunning = false;
      pvDeviceFile->unlockMutex();
      break;
    }
    // Event occured, get the data from port - Check timeout state first
    if(!pvDeviceFile->readTimeoutOccured()){
      pvDeviceFile->lockMutex();
      // Reset bufferCursor
      bufferCursor = buffer;
      // Read data from port
      readen = pvDeviceFile->readData(buffer, bufferSize);
      toStore = readen;
      ///qDebug() << "Read: " << buffer;
      ///qDebug() << "readen: " << readen;
      while((toStore > 0)&&(frame != 0)){
        // Store data
        ///qDebug() << "- Cursor: " << bufferCursor;
        ///qDebug() << "- toStore: " << toStore;
        stored = frame->putData(bufferCursor, toStore);
        ///qDebug() << "- stored: " << stored;
        // If frame is full, enque to RX frames and get a new one
        if(frame->bytesToStore() == 0){
          qDebug() << "RX --> Frame: " << frame->data();
          toStore = toStore - frame->eofSeqLen();
          Q_ASSERT(toStore >= 0);
          bufferCursor = bufferCursor + frame->eofSeqLen();
          Q_ASSERT(bufferCursor < (buffer + bufferSize));
          pvDeviceFile->readenFrames().enqueue(frame);
          frame = getNewFrame();
        }
        toStore = toStore - stored;
        Q_ASSERT(toStore >= 0);
        bufferCursor = bufferCursor + stored;
        Q_ASSERT(bufferCursor < (buffer + bufferSize));
      }
      pvDeviceFile->unlockMutex();
    }
  }

  // Free memory
  Q_ASSERT(buffer != 0);
  delete buffer;
}
