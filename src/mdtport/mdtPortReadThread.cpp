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

//#include <QDebug>

mdtPortReadThread::mdtPortReadThread(QObject *parent)
 : mdtPortThread(parent)
{
  pvTransmissionSuspended = false;
  pvMinPoolSizeBeforeReadSuspend = 0;
}

bool mdtPortReadThread::isReader() const
{
  return true;
}

void mdtPortReadThread::run()
{
  Q_ASSERT(pvPort != 0);

  mdtFrame *frame;
  mdtAbstractPort::error_t portError = mdtAbstractPort::NoError;
  int n;
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
  // Get frame
  frame = getNewFrameRead();
  if(frame == 0){
    return;
  }
  // Notify that we are ready
  notifyError(mdtAbstractPort::NoError);

  // Run...
  while(1){
    // Read thread state
    if(!pvRunning){
      break;
    }
    Q_ASSERT(frame != 0);
    // Wait on Read event
    portError = pvPort->waitForReadyRead();
    if(portError != mdtAbstractPort::NoError){
      // Read thread state
      if(!pvRunning){
        break;
      }
      if((useReadTimeoutProtocol)&&(portError == mdtAbstractPort::ReadTimeout)){
        // In timeout protocol, the current frame is considered complete
        if(!frame->isEmpty()){
          frame->setComplete();
          pvPort->readenFrames().enqueue(frame);
          // emit a Readen frame signal
          emit newFrameReaden();
          frame = getNewFrameRead();
          if(frame == 0){
            break;
          }
        }
      }else{
        portError = handleCommonReadErrors(portError, &frame);
        if(portError != mdtAbstractPort::ErrorHandled){
          // Unhandled error, stop
          break;
        }
      }
    }
    // Check about flush request
    if(pvPort->flushInRequestPending()){
      // Put current frame back to pool, get new one and return idle
      pvPort->readFramesPool().enqueue(frame);
      frame = getNewFrameRead();
      if(frame == 0){
        break;
      }
      continue;
    }
    // Read
    n = readFromPort(&frame);
    if(n < 0){
      // Check about stopping
      if(!pvRunning){
        break;
      }
      portError = handleCommonReadErrors((mdtAbstractPort::error_t)n, &frame);
      if(portError != mdtAbstractPort::ErrorHandled){
        // Unhandled error, stop
        break;
      }
    }
  }

  // Put current frame into pool
  if(frame != 0){
    pvPort->readFramesPool().enqueue(frame);
  }

  if(portError == mdtAbstractPort::NoError){
    notifyError(mdtAbstractPort::Disconnected);
  }
  pvRunning = false;
  pvPort->unlockMutex();
}
