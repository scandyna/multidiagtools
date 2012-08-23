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
  pvMinPoolSizeBeforeReadSuspend = 0;
}

/**
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
      pvRunning = false;  /// \todo ???
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
*/

void mdtPortReadThread::run()
{
  qDebug() << "RDTHD: starting ...";
  Q_ASSERT(pvPort != 0);

  mdtFrame *frame;
  mdtAbstractPort::error_t portError;
  bool useReadTimeoutProtocol = false;

  pvPort->lockMutex();
#ifdef Q_OS_UNIX
  pvNativePthreadObject = pthread_self();
  Q_ASSERT(pvNativePthreadObject != 0);
#endif
  // Get setup
  useReadTimeoutProtocol = pvPort->config().useReadTimeoutProtocol();
  pvMinPoolSizeBeforeReadSuspend = pvPort->config().readQueueSize() / 4;
  if((pvMinPoolSizeBeforeReadSuspend < 1)&&(pvPort->config().readQueueSize() > 0)){
    pvMinPoolSizeBeforeReadSuspend = 1;
  }
  // Set the running flag and get a read frame
  pvRunning = true;
  frame = getNewFrameRead();
  if(frame == 0){
    return;
  }
  // Run...
  while(1){
    ///qDebug() << "RTHD: run ...";
    // Read thread state
    if(!pvRunning){
      break;
    }
    // Wait on Read event
    portError = pvPort->waitForReadyRead();
    if(portError == mdtAbstractPort::WaitingCanceled){
      // Stopping
      break;
    }else if(portError == mdtAbstractPort::UnknownError){
      // Unhandled error. Signal this and stop
      emit(errorOccured(MDT_PORT_IO_ERROR));
      break;
    }
    // Event occured, get the data from port - Check timeout state first
    if(pvPort->readTimeoutOccured()){
      // In timeout protocol, the current frame is considered complete
      if(useReadTimeoutProtocol){
        if(frame != 0){
          if(!frame->isEmpty()){
            frame->setComplete();
            pvPort->readenFrames().enqueue(frame);
            // emit a Readen frame signal
            emit newFrameReaden();
            frame = getNewFrameRead();
          }
        }
      } /// NOTE: else ???
    }else{
      frame = readFromPort(frame);
      if(frame == 0){
        break;
      }
    }
  }

  qDebug() << "RDTHD: Cleanup ...";

  // Put current frame into pool
  if(frame != 0){
    pvPort->readFramesPool().enqueue(frame);
  }

  pvPort->unlockMutex();
  qDebug() << "RDTHD: END";
}
