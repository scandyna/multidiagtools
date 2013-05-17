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
#include "mdtPortThreadHelperPort.h"
#include "mdtPortThread.h"
#include "mdtFrame.h"
#include "mdtError.h"

#include <QDebug>

// We need a sleep function
#ifdef Q_OS_UNIX
 #define msleep(t) usleep(1000*t)
#endif
#ifdef Q_OS_WIN
 #include <windows.h>
 #define msleep(t) Sleep(t)
#endif

/**
mdtAbstractPort::error_t mdtPortThreadHelperPort::reconnect(bool notify) {
}
*/

mdtPortThreadHelperPort::mdtPortThreadHelperPort(QObject *parent)
 : mdtPortThreadHelper(parent)
{
  pvReadBuffer = 0;
  pvReadBufferSize = 0;
}

mdtPortThreadHelperPort::~mdtPortThreadHelperPort()
{
  qDebug() << "mdtPortThreadHelperPort::~mdtPortThreadHelperPort() delete pvReadBuffer ...";
  delete[] pvReadBuffer;
  qDebug() << "mdtPortThreadHelperPort::~mdtPortThreadHelperPort() delete pvReadBuffer OK";
}

int mdtPortThreadHelperPort::readFromPort(bool emitNewFrameReaden)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);

  int readen;

  // Check if a new frame is needed
  if(currentReadFrame() == 0){
    if(!getNewFrameRead()){
      return mdtAbstractPort::ReadPoolEmpty;
    }
  }
  // Alloc local buffer if needed
  if(pvReadBufferSize < currentReadFrame()->capacity()){
    delete[] pvReadBuffer;
    pvReadBufferSize = currentReadFrame()->capacity();
    pvReadBuffer = new char[pvReadBufferSize];
    if(pvReadBuffer == 0){
      pvReadBufferSize = 0;
      mdtError e(MDT_MEMORY_ALLOC_ERROR, "Cannot allocate memory for local buffer", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperPort");
      e.commit();
      return 0;
    }
  }
  // Read data from port
  readen = pvPort->read(pvReadBuffer, pvReadBufferSize);
  if(readen < 0){
    return readen;
  }

  return submitReadenData(pvReadBuffer, readen, emitNewFrameReaden);
}

int mdtPortThreadHelperPort::writeDataToPort(int maxSize)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);

  int written = 0;
  mdtFrame *frame = currentWriteFrame();

  if(frame == 0){
    return 0;
  }
  // Write data to port
  emit writeProcessBegin();
  if(maxSize > -1){
    // Check max possible
    if(maxSize > frame->size()){
      maxSize = frame->size();
    }
    written = pvPort->write(frame->data(), maxSize);
  }else{
    written = pvPort->write(frame->data(), frame->size());
  }
  if(written < 0){
    return written;
  }
  frame->take(written);

  return written;
}

mdtAbstractPort::error_t mdtPortThreadHelperPort::writeToPort(bool bytePerByteWrite, int interByteTime, int maxWriteTry)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);

  int written = 0;
  mdtAbstractPort::error_t portError;
  mdtFrame *frame = currentWriteFrame();

  if(frame == 0){
    return mdtAbstractPort::NoError;
  }
  // Write the frame to port
  while(!frame->isEmpty()){
    // Wait on write ready event
    portError = pvPort->waitEventWriteReady();
    // Check about flush request
    if(pvPort->flushOutRequestPending()){
      // Restore frame to pool and return
      /**
      qDebug() << "mdtPortThreadHelperPort::writeToPort(): flush out request";
      pvPort->writeFramesPool().enqueue(frame);
      setCurrentWriteFrame(0);
      */
      restoreCurrentWriteFrameToPool();
      return mdtAbstractPort::NoError;
    }
    if(portError != mdtAbstractPort::NoError){
      if(!pvThread->runningFlagSet()){
        return mdtAbstractPort::ErrorHandled;
      }
      return handleCommonWriteErrors(portError);
    }
    // Event occured, send the data to port
    if(bytePerByteWrite){
      written = writeDataToPort(1);
      pvPort->unlockMutex();
      msleep(interByteTime);
      pvPort->lockMutex();
    }else{
      written = writeDataToPort(-1);
    }
    if((written == 0)&&(!frame->isEmpty())){
      if(maxWriteTry <= 0){
        // Notify error and restore frame into write pool
        mdtError e(MDT_PORT_IO_ERROR, "Max write try reached after write busy", mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperPort");
        e.commit();
        if(pvThread->runningFlagSet()){
          notifyError(mdtAbstractPort::WriteTimeout);
        }
        /**
        pvPort->writeFramesPool().enqueue(frame);
        setCurrentWriteFrame(0);
        */
        restoreCurrentWriteFrameToPool();
        // Thread has nothing else to do
        return mdtAbstractPort::ErrorHandled;
      }
      // Cannot write now, sleep some time and try later
      pvPort->unlockMutex();
      msleep(100);
      pvPort->lockMutex();
      maxWriteTry--;
    }else if(written < 0){
      if(!pvThread->runningFlagSet()){
        return mdtAbstractPort::ErrorHandled;
      }
      return handleCommonWriteErrors(portError);
    }
  }
  // Here, frame is completly sent
  Q_ASSERT(frame->isEmpty());
  /**
  pvPort->writeFramesPool().enqueue(frame);
  setCurrentWriteFrame(0);
  */
  restoreCurrentWriteFrameToPool();

  return mdtAbstractPort::NoError;
}
