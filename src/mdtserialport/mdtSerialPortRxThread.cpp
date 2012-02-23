
#include "mdtSerialPortRxThread.h"
#include "mdtError.h"
#include <QDebug>
#include <QApplication>

mdtSerialPortRxThread::mdtSerialPortRxThread(QObject *parent)
 : mdtSerialPortThread(parent)
{
}

mdtFrame *mdtSerialPortRxThread::getNewFrame()
{
  Q_ASSERT(pvSerialPort != 0);

  mdtFrame *frame;

  if(pvSerialPort->rxFramesPool().size() < 1){
    mdtError e(MDT_UNDEFINED_ERROR, "RX frames pool is empty", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtSerialPortRxThread");
    e.commit();
    return 0;
  }
  frame = pvSerialPort->rxFramesPool().dequeue();
  Q_ASSERT(frame != 0);
  frame->clear();

  return frame;
}

void mdtSerialPortRxThread::run()
{
  Q_ASSERT(pvSerialPort != 0);

  int bufferSize;
  char *buffer = 0;
  char *bufferCursor = 0;
  mdtFrame *frame;
  int readen = 0;
  int stored = 0;
  int toStore = 0;

  // Set the running flag and get a RX frame
  pvSerialPort->lockMutex();
  pvRunning = true;
  frame = getNewFrame();
  if(frame == 0){
    mdtError e(MDT_UNDEFINED_ERROR, "No frame available in RX frames pool, stopping RX thread", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSerialPortRxThread");
    e.commit();
    pvRunning = false;
    pvSerialPort->unlockMutex();
    return;
  }
  pvSerialPort->unlockMutex();

  // Alloc the local buffer
  bufferSize = frame->capacity();
  buffer = new char[bufferSize];
  if(buffer == 0){
    mdtError e(MDT_UNDEFINED_ERROR, "Cannot allocate memory for local buffer" , mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtSerialPortRxThread");
    e.commit();
    return;
  }
  bufferCursor = buffer;

  // Run...
  while(1){
    // Read thread state
    pvSerialPort->lockMutex();
    if(!pvRunning){
      pvSerialPort->unlockMutex();
      break;
    }
    pvSerialPort->unlockMutex();

    // Wait on RX event
    if(!pvSerialPort->waitEventRx()){
      pvSerialPort->lockMutex();
      pvRunning = false;
      pvSerialPort->unlockMutex();
      break;
    }
    // Event occured, get the data from port - Check timeout state first
    if(!pvSerialPort->rxTimeoutOccured()){
      pvSerialPort->lockMutex();
      // Reset bufferCursor
      bufferCursor = buffer;
      // Read data from port
      readen = pvSerialPort->readData(buffer, bufferSize);
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
          pvSerialPort->rxFrames().enqueue(frame);
          frame = getNewFrame();
        }
        toStore = toStore - stored;
        Q_ASSERT(toStore >= 0);
        bufferCursor = bufferCursor + stored;
        Q_ASSERT(bufferCursor < (buffer + bufferSize));
      }
      pvSerialPort->unlockMutex();
    }
  }

  // Free memory
  Q_ASSERT(buffer != 0);
  delete buffer;
}
