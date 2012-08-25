/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "mdtPortWriteThread.h"
#include "mdtAbstractPort.h"
#include "mdtFrame.h"
#include <QApplication>

#include <QDebug>

mdtPortWriteThread::mdtPortWriteThread(QObject *parent)
 : mdtPortThread(parent)
{
}

void mdtPortWriteThread::stop()
{
  pvPort->abortFrameToWriteWait();
  mdtPortThread::stop();
}

/**
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
*/

void mdtPortWriteThread::run()
{
  qDebug() << "WRTHD: starting ...";
  Q_ASSERT(pvPort != 0);

  char *bufferCursor = 0;
  mdtFrame *frame = 0;
  qint64 toWrite = 0;
  qint64 written = 0;
  mdtAbstractPort::error_t portError;
  int interframeTime = 0;
  int writeMinWaitTime = 0;
  bool bytePerByteWrite = false;

  pvPort->lockMutex();
#ifdef Q_OS_UNIX
  pvNativePthreadObject = pthread_self();
  Q_ASSERT(pvNativePthreadObject != 0);
#endif
  // Get setup
  interframeTime = pvPort->config().writeInterframeTime();
  writeMinWaitTime = pvPort->config().writeMinWaitTime();
  bytePerByteWrite = pvPort->config().bytePerByteWrite();
  // Set the running flag
  pvRunning = true;

  // Run...
  while(1){
    qDebug() << "WTHD: run ...";
    // Read thread state
    if(!pvRunning){
      break;
    }
    // Wait for interframe time if needed
    if(interframeTime > 0){
      pvPort->unlockMutex();
      qDebug() << "WTHD: waiting " << interframeTime << " [ms]";
      msleep(interframeTime);
      pvPort->lockMutex();
    }
    // Get a frame - will block if nothing is to write
    qDebug() << "WTHD: getNewFrameWrite() ...";
    frame = getNewFrameWrite();
    // If thread is stopping, it can happen that a Null pointer is returned
    if(frame == 0){
      break;  /// \todo handle pvRunning flag ??
    }
    // Write
    qDebug() << "WTHD: writeToPort() ...";
    if(!writeToPort(frame, bytePerByteWrite, writeMinWaitTime)){
      // Stop request or fatal error
      break;  /// \todo handle pvRunning flag ??
    }
    
    /// \todo Implement a wait condition, not allways poling when nothing is to send..
    /**
    // Wait the minimal time if requierd - Used for byte per byte write
    if(writeMinWaitTime > 0){
      pvPort->unlockMutex();
      msleep(writeMinWaitTime);
      pvPort->lockMutex();
    }
    // Wait on write ready event
    portError = pvPort->waitEventWriteReady();
    if(portError == mdtAbstractPort::WaitingCanceled){
      // Stopping
      break;
    }else if(portError == mdtAbstractPort::UnknownError){
      // Unhandled error. Signal this and stop
      emit(errorOccured(MDT_PORT_IO_ERROR));
      break;
    }
    // Event occured, send the data to port - Check timeout state first
    if(pvPort->writeTimeoutOccured()){
      // Cannot write now, sleep some time and try later
      pvPort->unlockMutex();
      msleep(100);
      pvPort->lockMutex();
    }else{
      // Check if we have something to transmit
      if(frame == 0){
        frame = getNewFrame();
        if(frame != 0){
          // New frame to transmit
          bufferCursor = frame->data();
          toWrite = frame->size();
          // Wait before sending next frame, if requierd
          if(interframeTime > 0){
            pvPort->unlockMutex();
            qDebug() << "WTHD: waiting " << interframeTime << " [ms]";
            msleep(interframeTime);
            pvPort->lockMutex();
          }
        }
      }
      if(frame != 0){
        // Write data to port
        emit ioProcessBegin();
        if((toWrite > 0)&&(bytePerByteWrite)){
          written = pvPort->write(bufferCursor, 1);
        }else{
          written = pvPort->write(bufferCursor, toWrite);
        }
        ///qDebug() << "WTHD: written: " << written;
        if(written < 0){
          written = 0;
          pvPort->unlockMutex();
          msleep(100);
          pvPort->lockMutex();
          emit(errorOccured(MDT_PORT_IO_ERROR));
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
      }else{
        // Sleep a while
        pvPort->unlockMutex();
        msleep(100);
        pvPort->lockMutex();
      }
    }
    */
  }
  
  qDebug() << "WRTHD: Cleanup ...";

  pvPort->unlockMutex();
  qDebug() << "WRTHD: END";
}
