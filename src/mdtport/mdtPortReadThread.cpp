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

bool mdtPortReadThread::isReader() const
{
  return true;
}

void mdtPortReadThread::run()
{
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
    if(portError != mdtAbstractPort::NoError){
      if(portError == mdtAbstractPort::WaitingCanceled){
        // Stopping
        notifyError(mdtAbstractPort::Disconnected);
        break;
      }else{
        // Unhandled error - notify and end
        notifyError(portError);
        break;
      }
    }
    /**
    }else if(portError == mdtAbstractPort::UnhandledError){
      // Unhandled error. Signal this and stop
      notifyError(portError);
      break;
    }
    */
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
      }else{
        notifyError(mdtAbstractPort::ReadTimeout);
      }
    }else{
      frame = readFromPort(frame);
      if(frame == 0){
        break;
      }
    }
  }

  // Put current frame into pool
  if(frame != 0){
    pvPort->readFramesPool().enqueue(frame);
  }

  pvPort->unlockMutex();
}
