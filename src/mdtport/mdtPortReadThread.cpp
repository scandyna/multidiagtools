/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtPortThreadHelperPort.h"
#include <QApplication>

#include <QDebug>

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

  mdtAbstractPort::error_t portError = mdtAbstractPort::NoError;
  int n;
  bool useReadTimeoutProtocol = false;
  mdtPortThreadHelperPort threadHelper;
  threadHelper.setPort(pvPort);
  threadHelper.setThread(this);

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
  if(!threadHelper.getNewFrameRead()){
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
    Q_ASSERT(threadHelper.currentReadFrame() != 0);
    // Wait on Read event
    portError = pvPort->waitForReadyRead();
    if(portError != mdtAbstractPort::NoError){
      // Read thread state
      if(!pvRunning){
        break;
      }
      if((useReadTimeoutProtocol)&&(portError == mdtAbstractPort::ReadTimeout)){
        // In timeout protocol, the current frame is considered complete
        if(!threadHelper.currentReadFrame()->isEmpty()){
          threadHelper.currentReadFrame()->setComplete();
          // Submit frame and notify newFrameReaden
          threadHelper.submitCurrentReadFrame(true);
          if(!threadHelper.getNewFrameRead()){
            break;
          }
        }
      }else{
        portError = threadHelper.handleCommonReadErrors(portError);
        if(portError != mdtAbstractPort::ErrorHandled){
          // Unhandled error, stop
          break;
        }
      }
    }
    // Check about flush request
    if(pvPort->flushInRequestPending()){
      // Put current frame back to pool, get new one and return idle
      threadHelper.restoreCurrentReadFrameToPool();
      if(!threadHelper.getNewFrameRead()){
        break;
      }
      continue;
    }
    // Read
    n = threadHelper.readFromPort(true);
    if(n < 0){
      // Check about stopping
      if(!pvRunning){
        break;
      }
      portError = threadHelper.handleCommonReadErrors((mdtAbstractPort::error_t)n);
      if(portError != mdtAbstractPort::ErrorHandled){
        // Unhandled error, stop
        break;
      }
    }
  }

  if(portError == mdtAbstractPort::NoError){
    notifyError(mdtAbstractPort::Disconnected);
  }
  pvRunning = false;
  pvPort->unlockMutex();
  qDebug() << "RTHD: end";
}
