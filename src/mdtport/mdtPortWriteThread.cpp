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
#include "mdtPortWriteThread.h"
#include "mdtPortThreadHelperPort.h"
#include "mdtAbstractPort.h"
#include "mdtFrame.h"
#include <QApplication>

//#include <QDebug>

mdtPortWriteThread::mdtPortWriteThread(QObject *parent)
 : mdtPortThread(parent)
{
}

void mdtPortWriteThread::stop()
{
  Q_ASSERT(pvPort != 0);

  pvPort->abortFrameToWriteWait();
  mdtPortThread::stop();
}

bool mdtPortWriteThread::isReader() const
{
  return false;
}

bool mdtPortWriteThread::isWriter() const
{
  return true;
}

void mdtPortWriteThread::run()
{
  Q_ASSERT(pvPort != 0);

  ///mdtFrame *frame = 0;
  int interframeTime = 0;
  int interByteTime = 0;
  bool bytePerByteWrite = false;
  mdtAbstractPort::error_t portError = mdtAbstractPort::NoError;
  mdtPortThreadHelperPort threadHelper;
  threadHelper.setPort(pvPort);
  threadHelper.setThread(this);

  pvPort->lockMutex();
#ifdef Q_OS_UNIX
  pvNativePthreadObject = pthread_self();
  Q_ASSERT(pvNativePthreadObject != 0);
#endif
  // Get setup
  interframeTime = pvPort->config().writeInterframeTime();
  interByteTime = pvPort->config().writeInterbyteTime();
  bytePerByteWrite = pvPort->config().bytePerByteWrite();
  // Set the running flag
  pvRunning = true;
  // Notify that we are ready
  notifyError(mdtAbstractPort::NoError);

  // Run...
  while(1){
    // Read thread state
    if(!pvRunning){
      break;
    }
    // Wait for interframe time if needed
    if(interframeTime > 0){
      pvPort->unlockMutex();
      msleep(interframeTime);
      pvPort->lockMutex();
    }
    // Get a frame - will block if nothing is to write
    ///frame = getNewFrameWrite();
    /**
    frame = threadHelper.getNewFrameWrite();
    // If thread is stopping, it can happen that a Null pointer is returned
    if(frame == 0){
      break;
    }
    */
    if(!threadHelper.getNewFrameWrite()){
      break;
    }
    // Write
    ///portError = writeToPort(frame, bytePerByteWrite, writeMinWaitTime);
    portError = threadHelper.writeToPort(bytePerByteWrite, interByteTime);
    if((portError != mdtAbstractPort::NoError)&&(portError != mdtAbstractPort::ErrorHandled)){
      // Stop
      break;
    }
  }

  if(portError == mdtAbstractPort::NoError){
    notifyError(mdtAbstractPort::Disconnected);
  }
  pvRunning = false;
  pvPort->unlockMutex();
}
