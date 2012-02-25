
#include "mdtSerialPortTxThread.h"
#include <QDebug>
#include <QApplication>

mdtSerialPortTxThread::mdtSerialPortTxThread(QObject *parent)
 : mdtSerialPortThread(parent)
{
}

mdtFrame *mdtSerialPortTxThread::getNewFrame()
{
  Q_ASSERT(pvSerialPort != 0);

  mdtFrame *frame;

  // Check if there is a frame to transmit
  if(pvSerialPort->writeFrames().size() < 1){
    return 0;
  }
  frame = pvSerialPort->writeFrames().dequeue();
  Q_ASSERT(frame != 0);

  return frame;
}

void mdtSerialPortTxThread::run()
{
  Q_ASSERT(pvSerialPort != 0);

  char *buffer;
  char *bufferCursor = 0;
  mdtFrame *frame = 0;
  int toWrite = 0;
  int written = 0;
  
  // Set the running flag
  pvSerialPort->lockMutex();
  pvRunning = true;
  pvSerialPort->unlockMutex();

  // Run...
  while(1){
    // Read thread state
    pvSerialPort->lockMutex();
    if(!pvRunning){
      pvSerialPort->unlockMutex();
      break;
    }
    pvSerialPort->unlockMutex();

    // Wait on TX ready event
    if(!pvSerialPort->waitEventWriteReady()){
      pvSerialPort->lockMutex();
      pvRunning = false;
      pvSerialPort->unlockMutex();
      break;
    }
    // Event occured, send the data to port - Check timeout state first
    if(!pvSerialPort->writeTimeoutOccured()){
      pvSerialPort->lockMutex();
      // Check if we have something to transmit
      if(frame == 0){
        frame = getNewFrame();
        if(frame != 0){
          // New frame to transmit
          buffer = frame->data();
          bufferCursor = buffer;
          qDebug() << "TX thd: bufferCursor [NF]: " << bufferCursor;
          toWrite = frame->size();
        }
      }
      if(frame != 0){
        // Write data to port
        written = pvSerialPort->writeData(bufferCursor, toWrite);
        qDebug() << "TX thd: written: " << written;
        frame->take(written);
        qDebug() << "TX thd: frame size: " << frame->size();
        // Check if current frame was completly sent
        if(frame->isEmpty()){
          pvSerialPort->writeFramesPool().enqueue(frame);
          frame = 0;
        }else{
          bufferCursor += written;
          qDebug() << "TX thd: bufferCursor [CF]: " << bufferCursor;
          Q_ASSERT(bufferCursor < (buffer + frame->size()));
          toWrite -= written;
          Q_ASSERT(toWrite >= 0);
        }
      }
      pvSerialPort->unlockMutex();
    }
  }
}
