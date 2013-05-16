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
#include "mdtPortThreadHelper.h"
#include "mdtPortThread.h"
#include "mdtError.h"
#include "mdtFrame.h"

//#include <QDebug>

// We need a sleep function
#ifdef Q_OS_UNIX
 #define msleep(t) usleep(1000*t)
#endif
#ifdef Q_OS_WIN
 #include <windows.h>
 #define msleep(t) Sleep(t)
#endif

mdtPortThreadHelper::mdtPortThreadHelper(QObject *parent)
 : QObject(parent)
{
  pvPort = 0;
  pvThread = 0;
  pvCurrentReadFrame = 0;
  pvCurrentWriteFrame = 0;
}

mdtPortThreadHelper::~mdtPortThreadHelper()
{
}

void mdtPortThreadHelper::setPort(mdtAbstractPort *port)
{
  Q_ASSERT(port != 0);

  pvPort = port;
}

void mdtPortThreadHelper::setThread(mdtPortThread *thread)
{
  Q_ASSERT(thread != 0);

  pvThread = thread;
  connect(this, SIGNAL(newFrameReaden()), pvThread, SIGNAL(newFrameReaden()));
  connect(this, SIGNAL(readProcessBegin()), pvThread, SIGNAL(readProcessBegin()));
  connect(this, SIGNAL(writeProcessBegin()), pvThread, SIGNAL(writeProcessBegin()));
}

mdtAbstractPort::error_t mdtPortThreadHelper::handleCommonReadErrors(mdtAbstractPort::error_t portError)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);

  if(!pvThread->runningFlagSet()){
    mdtError e(MDT_PORT_IO_ERROR, "Non running thread wants to check about errors", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtPortThread");
    e.commit();
  }
  if(pvCurrentReadFrame == 0){
    mdtError e(MDT_PORT_IO_ERROR, "Try to handle a read error, but currently no read frame is set", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtPortThreadHelper");
    e.commit();
    return mdtAbstractPort::UnhandledError;
  }
  pvCurrentReadFrame->clear();
  pvCurrentReadFrame->clearSub();
  notifyError(portError);

  switch(portError){
    case mdtAbstractPort::Disconnected:
      pvPort->readFramesPool().enqueue(pvCurrentReadFrame);
      // Try to reconnect
      portError = reconnect(true);
      if(portError != mdtAbstractPort::NoError){
        pvCurrentReadFrame = 0;
        return mdtAbstractPort::UnhandledError;
      }
      pvCurrentReadFrame = getNewFrameRead();
      if(pvCurrentReadFrame == 0){
        return mdtAbstractPort::UnhandledError;
      }
      portError = mdtAbstractPort::ErrorHandled;
      break;
    case mdtAbstractPort::ReadCanceled:
      portError = mdtAbstractPort::ErrorHandled;
      break;
    case mdtAbstractPort::ReadTimeout:
      portError = mdtAbstractPort::ErrorHandled;
      break;
    default:
      portError = mdtAbstractPort::UnhandledError;
      break;
  }

  Q_ASSERT(pvCurrentReadFrame != 0);
  return portError;
}

mdtAbstractPort::error_t mdtPortThreadHelper::handleCommonWriteErrors(mdtAbstractPort::error_t portError)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);

  if(!pvThread->runningFlagSet()){
    mdtError e(MDT_PORT_IO_ERROR, "Non running thread wants to check about errors", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtPortThread");
    e.commit();
  }
  if(pvCurrentWriteFrame == 0){
    mdtError e(MDT_PORT_IO_ERROR, "Try to handle a write error, but currently no write frame is set", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtPortThreadHelper");
    e.commit();
    return mdtAbstractPort::UnhandledError;
  }
  pvPort->writeFramesPool().enqueue(pvCurrentWriteFrame);
  pvCurrentWriteFrame = 0;
  notifyError(portError);

  switch(portError){
    case mdtAbstractPort::Disconnected:
      // Try to reconnect
      portError = reconnect(true);
      if(portError != mdtAbstractPort::NoError){
        return mdtAbstractPort::UnhandledError;
      }
      portError = mdtAbstractPort::ErrorHandled;
      break;
    case mdtAbstractPort::ReadCanceled:
      portError = mdtAbstractPort::ErrorHandled;
      break;
    case mdtAbstractPort::ReadTimeout:
      portError = mdtAbstractPort::ErrorHandled;
      break;
    default:
      portError = mdtAbstractPort::UnhandledError;
      break;
  }

  return portError;
}

mdtAbstractPort::error_t mdtPortThreadHelper::handleCommonReadWriteErrors(mdtAbstractPort::error_t portError)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);

  if((pvCurrentReadFrame != 0)&&(pvCurrentWriteFrame != 0)){
    portError = handleCommonReadErrors(portError);
    if(portError != mdtAbstractPort::ErrorHandled){
      portError = handleCommonWriteErrors(portError);
    }
    return portError;
  }else if(pvCurrentReadFrame != 0){
    return handleCommonReadErrors(portError);
  }else{
    return handleCommonWriteErrors(portError);
  }
}

mdtFrame *mdtPortThreadHelper::getNewFrameRead()
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);

  mdtFrame *frame;

  if(pvPort->readFramesPool().size() < 1){
    mdtError e(MDT_PORT_QUEUE_EMPTY_ERROR, "Read frames pool is empty", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtPortThreadHelper");
    e.commit();
    notifyError(mdtAbstractPort::ReadPoolEmpty);
    // Wait until a frame is available again, or end of thread.
    while(pvPort->readFramesPool().size() < 1){
      pvPort->unlockMutex();
      msleep(100);
      pvPort->lockMutex();
      if(!pvThread->runningFlagSet()){
        return 0;
      }
    }
    // Put info into log that a frame is available again
    mdtError e2(MDT_NO_ERROR, "Read frames pool has now frames again (no longer empty)", mdtError::Info);
    MDT_ERROR_SET_SRC(e2, "mdtPortThreadHelper");
    e2.commit();
    notifyError(mdtAbstractPort::NoError);
  }
  frame = pvPort->readFramesPool().dequeue();
  Q_ASSERT(frame != 0);
  frame->clear();
  frame->clearSub();

  return frame;
}

int mdtPortThreadHelper::submitReadenData(const char *data, int size, bool emitNewFrameReaden)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(data != 0);

  char *bufferCursor = (char*)data;
  int toStore;
  int stored;
  int completeFrames = 0;

  if(size > 0){
    emit readProcessBegin();
    // Store readen data
    toStore = size;
    while(toStore > 0){
      // Check for new frame if needed
      if(pvCurrentReadFrame == 0){
        pvCurrentReadFrame = getNewFrameRead();
        if(pvCurrentReadFrame == 0){
          return mdtAbstractPort::UnhandledError;
        }
      }
      // Store data
      stored = pvCurrentReadFrame->putData(bufferCursor, toStore);
      // If frame is full, enqueue to readen frames and get a new one
      if(pvCurrentReadFrame->bytesToStore() == 0){
        stored += pvCurrentReadFrame->eofSeqLen();
        pvPort->readenFrames().enqueue(pvCurrentReadFrame);
        // emit a Readen frame signal
        if(emitNewFrameReaden){
          emit newFrameReaden();
        }
        pvCurrentReadFrame = getNewFrameRead();
        ++completeFrames;
      }
      // When frame becomes full and EOF seq was not reached, stored will be to big
      if(stored >= pvCurrentReadFrame->capacity()){
        stored = pvCurrentReadFrame->capacity()-1;
        toStore = 0;
      }else{
        toStore = toStore - stored;
      }
      bufferCursor = bufferCursor + stored;
      Q_ASSERT(bufferCursor < (data + pvCurrentReadFrame->capacity()));
    }
  }

  return completeFrames;
}

mdtFrame *mdtPortThreadHelper::getNewFrameWrite()
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);

  mdtFrame *frame;

  // Wait until a frame is available, or stopping
  do{
    if(!pvThread->runningFlagSet()){
      return 0;
    }
    frame = pvPort->getFrameToWrite();
  }while(frame == 0);

  return frame;
}

mdtFrame *mdtPortThreadHelper::currentReadFrame()
{
  return pvCurrentReadFrame;
}

mdtFrame *mdtPortThreadHelper::currentWriteFrame()
{
  return pvCurrentWriteFrame;
}

void mdtPortThreadHelper::notifyError(int error, bool renotifySameError)
{
  Q_ASSERT(pvThread != 0);

  pvThread->notifyError(error, renotifySameError);
}

mdtAbstractPort::error_t mdtPortThreadHelper::reconnect(bool notify)
{
  return mdtAbstractPort::UnhandledError;
}

/**
void mdtPortThreadHelper::setCurrentReadFrame(mdtFrame *frame)
{
  Q_ASSERT(frame != 0);

  pvCurrentReadFrame = frame;
}
*/

void mdtPortThreadHelper::requestWrite()
{
}
