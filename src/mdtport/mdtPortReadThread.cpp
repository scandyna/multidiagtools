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
#include "mdtPortReadThread.h"
#include "mdtError.h"
#include "mdtAbstractPort.h"
#include <QApplication>

#include <QDebug>

mdtPortReadThread::mdtPortReadThread(QObject *parent)
 : mdtPortThread(parent)
{
  pvTransmissionSuspended = false;
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
  if(pvPort->readFramesPool().size() < pvMinPoolSizeBeforeReadSuspend){
    if(!pvPort->suspendTransmission()){
      pvRunning = false;
      return 0;
    }
    pvTransmissionSuspended = true;
    qDebug() << "RTHD: requ SUSPEND";
  }else{
    if(pvTransmissionSuspended){
      if(!pvPort->resumeTransmission()){
        pvRunning = false;
        return 0;
      }
      pvTransmissionSuspended = false;
      qDebug() << "RTHD: requ Resume";
    }
  }
  frame = pvPort->readFramesPool().dequeue();
  Q_ASSERT(frame != 0);
  frame->clear();

  return frame;
}

void mdtPortReadThread::run()
{
  qDebug() << "RDTHD: starting ...";
  Q_ASSERT(pvPort != 0);

  qint64 bufferSize;
  char *buffer = 0;
  char *bufferCursor = 0;
  mdtFrame *frame;
  qint64 readen = 0;
  qint64 stored = 0;
  qint64 toStore = 0;


  pvPort->lockMutex();
#ifdef Q_OS_UNIX
  ///pvPort->setNativePthreadObject(pthread_self());
  pvNativePthreadObject = pthread_self();
  Q_ASSERT(pvNativePthreadObject != 0);
#endif
  // Set the running flag and get a RX frame
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

  // Run...
  while(1){
    // Read thread state
    if(!pvRunning){
      break;
    }

    // Wait the minimal time if requierd NOTE: obesete ??
    /**
    if(pvReadMinWaitTime > 0){
      msleep(pvReadMinWaitTime);
    }
    */
    // Wait on Read event
    if(!pvPort->waitForReadyRead()){
      emit(errorOccured(MDT_PORT_IO_ERROR));
      pvPort->unlockMutex();
      msleep(100);
      pvPort->lockMutex();
    }
    // Event occured, get the data from port - Check timeout state first
    if(pvPort->readTimeoutOccured()){
      // In timeout protocol, the current frame is considered complete
      if(pvUseReadTimeoutProtocol){
        ///qDebug() << "TO et TOP";
        if(frame != 0){
          if(!frame->isEmpty()){
            // NOTE: smowath ugly, we reset timeout state to false, this emits signal again !
            pvPort->updateReadTimeoutState(false);
            frame->setComplete();
            pvPort->readenFrames().enqueue(frame);
            // emit a Readen frame signal
            emit newFrameReaden();
            frame = getNewFrame();
          }
        }
      }
    }else{
      // Reset bufferCursor
      bufferCursor = buffer;
      // Read data from port
      emit ioProcessBegin();
      readen = pvPort->read(buffer, bufferSize);
      //qDebug() << "RX thread, readen: " << readen << " , buffer: " << buffer;
      if(readen < 0){
        readen = 0;
        pvPort->unlockMutex();
        msleep(100);
        pvPort->lockMutex();
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
            emit(errorOccured(MDT_PORT_QUEUE_EMPY_ERROR));
            msleep(100);
            // Check about end of thread
            pvPort->lockMutex();
            if(!pvRunning){
              break;
            }
          }
        }
        // Check about end of thread
        if(!pvRunning){
          break;
        }
        // Store data
        stored = frame->putData(bufferCursor, toStore);
        // If frame is full, enqueue to readen frames and get a new one
        if(frame->bytesToStore() == 0){
          stored += frame->eofSeqLen();
          pvPort->readenFrames().enqueue(frame);
          // emit a Readen frame signal if complete
          ///if(frame->isComplete()){
            emit newFrameReaden();
          ///}
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
    }
  }

  qDebug() << "RDTHD: Cleanup ...";

  // Put current frame into pool
  if(frame != 0){
    pvPort->readFramesPool().enqueue(frame);
  }

  // Free memory
  Q_ASSERT(buffer != 0);
  delete buffer;

  pvPort->unlockMutex();
  qDebug() << "RDTHD: END";
}
