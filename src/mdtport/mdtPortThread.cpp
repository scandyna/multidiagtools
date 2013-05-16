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
#include "mdtPortThread.h"
#include "mdtError.h"
#include <errno.h>
#include <QApplication>

#include <QDebug>

mdtPortThread::mdtPortThread(QObject *parent)
 : QThread(parent)
{
  pvPort = 0;
  pvRunning = false;
  pvReadBuffer = 0;
  pvReadBufferSize = 0;
  pvCurrentError = mdtAbstractPort::UnhandledError;
#ifdef Q_OS_UNIX
  pvNativePthreadObject = 0;
  // We must catch the SIGALRM signal, else the application process
  // will be killed by pthread_kill()
  sigemptyset(&(pvSigaction.sa_mask));
  pvSigaction.sa_flags = 0;
  pvSigaction.sa_handler = sigactionHandle;
  sigaction(SIGALRM, &pvSigaction, NULL);
#endif
  pvReconnectTimeout = 5000;
  pvReconnectMaxTry = 100;
}

mdtPortThread::~mdtPortThread()
{
  if(isRunning()){
    stop();
  }
}

void mdtPortThread::setPort(mdtAbstractPort *port)
{
  Q_ASSERT(port != 0);
  Q_ASSERT(!QThread::isRunning());

  pvPort = port;
}

void mdtPortThread::detachPort(bool releaseMemory)
{
  Q_ASSERT(!QThread::isRunning());

  if(releaseMemory){
    delete pvPort;
  }
  pvPort = 0;
}

bool mdtPortThread::start()
{
  Q_ASSERT(pvPort != 0);

  int i = 0;

  // Get thread's common setup
  pvReconnectTimeout = pvPort->config().connectTimeout();
  pvReconnectMaxTry = pvPort->config().connectMaxTry();
  // Start..
  QThread::start();

  // Wait until the thread started
  while(!isRunning()){
    qApp->processEvents();
    msleep(50);
    i++;
    // When something goes wrong on starting, it happens sometimes that isRunning() ruturns false forever
    // We let the thrad 500 [ms] time to start, else generate an error
    if(i >= 10){
      mdtError e(MDT_PORT_IO_ERROR, "Thread start timeout reached (500 [ms]) -> abort", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtPortThread");
      e.commit();
      return false;
    }
  }

  return true;
}

void mdtPortThread::stop()
{
  Q_ASSERT(pvPort != 0);

  int err;

  // Unset the running flag
  qDebug() << "mdtPortThread::stop() ...";
  pvPort->lockMutex();
  if(!pvRunning){
    pvPort->unlockMutex();
    return;
  }
  pvRunning = false;
  if(isWriter()){
    pvPort->abortFrameToWriteWait();
  }
#ifdef Q_OS_UNIX
  if(QThread::isRunning()){
    Q_ASSERT(pvNativePthreadObject != 0);
    err = pthread_kill(pvNativePthreadObject, SIGALRM);
    if(err != 0){
      mdtError e(MDT_PORT_IO_ERROR, "pthread_kill() failed", mdtError::Error);
      switch(err){
        case EINVAL:
          e.setSystemError(err, "Invalid signal was specified (EINVAL)");
        case ESRCH:
          e.setSystemError(err, "Specified thread ID not found (ESRCH)");
      }
      MDT_ERROR_SET_SRC(e, "mdtPortThread");
      e.commit();
    }
  }
#endif
  pvPort->unlockMutex();

  // Wait the end of the thread
  while(!isFinished()){
    qApp->processEvents();
    msleep(50);
  }
  // Free read/write buffers
  delete[] pvReadBuffer;
  pvReadBuffer = 0;
  pvReadBufferSize = 0;
  qDebug() << "mdtPortThread::stop() DONE";
}

bool mdtPortThread::isRunning() const
{
  ///Q_ASSERT(pvPort != 0);
  if(pvPort == 0){
    return QThread::isRunning();
  }

  if(!QThread::isRunning()){
    return false;
  }
  pvPort->lockMutex();
  if(pvRunning){
    pvPort->unlockMutex();
    return true;
  }
  pvPort->unlockMutex();
  return false;
}

bool mdtPortThread::runningFlagSet() const
{
  return pvRunning;
}

bool mdtPortThread::isFinished() const
{
  ///Q_ASSERT(pvPort != 0);
  if(pvPort == 0){
    return QThread::isFinished();
  }

  pvPort->lockMutex();
  if(pvRunning){
    pvPort->unlockMutex();
    return false;
  }
  pvPort->unlockMutex();

  return QThread::isFinished();
}

bool mdtPortThread::isReader() const
{
  return false;
}

bool mdtPortThread::isWriter() const
{
  return false;
}

void mdtPortThread::notifyError(int error, bool renotifySameError)
{
  if((error != pvCurrentError)||(renotifySameError)){
    qDebug() << "mdtPortThread::notifyError(): error " << error << " occured";
    pvCurrentError = error;
    emit(errorOccured(error));
  }
}

mdtAbstractPort::error_t mdtPortThread::handleCommonReadErrors(mdtAbstractPort::error_t portError, mdtFrame **frame)
{
  Q_ASSERT(frame != 0);
  Q_ASSERT(*frame != 0);
  Q_ASSERT(pvPort != 0);

  if(!pvRunning){
    mdtError e(MDT_PORT_IO_ERROR, "Non running thread wants to check about errors", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtPortThread");
    e.commit();
  }
  (*frame)->clear();
  (*frame)->clearSub();
  notifyError(portError);

  switch(portError){
    case mdtAbstractPort::Disconnected:
      pvPort->readFramesPool().enqueue(*frame);
      // Try to reconnect
      portError = reconnect(true);
      if(portError != mdtAbstractPort::NoError){
        *frame = 0;
        return mdtAbstractPort::UnhandledError;
      }
      *frame = getNewFrameRead();
      if(*frame == 0){
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

  Q_ASSERT(*frame != 0);
  return portError;
}

mdtAbstractPort::error_t mdtPortThread::handleCommonWriteErrors(mdtAbstractPort::error_t portError, mdtFrame **frame)
{
  Q_ASSERT(frame != 0);
  Q_ASSERT(*frame != 0);
  Q_ASSERT(pvPort != 0);

  if(!pvRunning){
    mdtError e(MDT_PORT_IO_ERROR, "Non running thread wants to check about errors", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtPortThread");
    e.commit();
  }
  pvPort->writeFramesPool().enqueue(*frame);
  *frame = 0;
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

mdtAbstractPort::error_t mdtPortThread::handleCommonReadWriteErrors(mdtAbstractPort::error_t portError, mdtFrame **readFrame, mdtFrame **writeFrame)
{
  Q_ASSERT(readFrame != 0);
  Q_ASSERT(writeFrame != 0);

  if((*readFrame != 0)&&(*writeFrame != 0)){
    portError = handleCommonReadErrors(portError, readFrame);
    if(portError != mdtAbstractPort::ErrorHandled){
      portError = handleCommonWriteErrors(portError, writeFrame);
    }
    return portError;
  }else if(*readFrame != 0){
    return handleCommonReadErrors(portError, readFrame);
  }else{
    return handleCommonWriteErrors(portError, writeFrame);
  }
}

mdtFrame *mdtPortThread::getNewFrameRead()
{
  Q_ASSERT(pvPort != 0);

  mdtFrame *frame;

  if(pvPort->readFramesPool().size() < 1){
    mdtError e(MDT_PORT_QUEUE_EMPTY_ERROR, "Read frames pool is empty", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtPortThread");
    e.commit();
    notifyError(mdtAbstractPort::ReadPoolEmpty);
    // Wait until a frame is available again, or end of thread.
    while(pvPort->readFramesPool().size() < 1){
      pvPort->unlockMutex();
      msleep(100);
      pvPort->lockMutex();
      if(!pvRunning){
        return 0;
      }
    }
    // Put info into log that a frame is available again
    mdtError e2(MDT_NO_ERROR, "Read frames pool has now frames again (no longer empty)", mdtError::Info);
    MDT_ERROR_SET_SRC(e2, "mdtPortThread");
    e2.commit();
    notifyError(mdtAbstractPort::NoError);
  }
  frame = pvPort->readFramesPool().dequeue();
  Q_ASSERT(frame != 0);
  frame->clear();
  frame->clearSub();

  return frame;
}

int mdtPortThread::readFromPort(mdtFrame **frame, bool emitNewFrameReaden)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(frame != 0);
  Q_ASSERT(*frame != 0);

  char *bufferCursor = 0;
  qint64 readen = 0;
  qint64 stored = 0;
  qint64 toStore = 0;
  int completeFrames = 0;

  // Alloc local buffer if needed
  if(pvReadBufferSize < (*frame)->capacity()){
    delete[] pvReadBuffer;
    pvReadBufferSize = (*frame)->capacity();
    pvReadBuffer = new char[pvReadBufferSize];
    if(pvReadBuffer == 0){
      pvReadBufferSize = 0;
      mdtError e(MDT_MEMORY_ALLOC_ERROR, "Cannot allocate memory for local buffer", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtPortThread");
      e.commit();
      return 0;
    }
  }
  // Reset bufferCursor
  bufferCursor = pvReadBuffer;
  // Read data from port
  readen = pvPort->read(pvReadBuffer, pvReadBufferSize);
  if(readen < 0){
    ///pvPort->readFramesPool().enqueue(*frame);
    return readen;
  }
  if(readen > 0){
    emit readProcessBegin();
  }
  // Store readen data
  toStore = readen;
  while(toStore > 0){
    // Check for new frame if needed
    if((*frame) == 0){
      *frame = getNewFrameRead();
      if((*frame) == 0){
        return mdtAbstractPort::UnhandledError;
      }
    }
    // Store data
    stored = (*frame)->putData(bufferCursor, toStore);
    // If frame is full, enqueue to readen frames and get a new one
    if((*frame)->bytesToStore() == 0){
      stored += (*frame)->eofSeqLen();
      pvPort->readenFrames().enqueue(*frame);
      // emit a Readen frame signal
      if(emitNewFrameReaden){
        emit newFrameReaden();
      }
      *frame = getNewFrameRead();
      completeFrames++;
    }
    // When frame becomes full and EOF seq was not reached, stored will be to big
    if(stored >= pvReadBufferSize){
      stored = pvReadBufferSize-1;
      toStore = 0;
    }else{
      toStore = toStore - stored;
    }
    bufferCursor = bufferCursor + stored;
    Q_ASSERT(bufferCursor < (pvReadBuffer + pvReadBufferSize));
  }

  return completeFrames;
}

mdtFrame *mdtPortThread::getNewFrameWrite()
{
  Q_ASSERT(pvPort != 0);

  mdtFrame *frame;

  // Wait until a frame is available, or stopping
  do{
    if(!pvRunning){
      return 0;
    }
    frame = pvPort->getFrameToWrite();
  }while(frame == 0);

  return frame;
}

qint64 mdtPortThread::writeDataToPort(mdtFrame *frame, int maxSize)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(frame != 0);

  qint64 written = 0;

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

mdtAbstractPort::error_t mdtPortThread::writeToPort(mdtFrame *frame, bool bytePerByteWrite, int interByteTime, int maxWriteTry)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(frame != 0);

  qint64 written = 0;
  mdtAbstractPort::error_t portError;

  // Write the frame to port
  while(!frame->isEmpty()){
    // Wait on write ready event
    portError = pvPort->waitEventWriteReady();
    // Check about flush request
    if(pvPort->flushOutRequestPending()){
      // Restore frame to pool and return
      qDebug() << "PTHD write: flush out request";
      pvPort->writeFramesPool().enqueue(frame);
      return mdtAbstractPort::NoError;
    }
    if(portError != mdtAbstractPort::NoError){
      if(!pvRunning){
        return mdtAbstractPort::ErrorHandled;
      }
      return handleCommonWriteErrors(portError, &frame);
    }
    // Event occured, send the data to port
    if(bytePerByteWrite){
      written = writeDataToPort(frame, 1);
      pvPort->unlockMutex();
      msleep(interByteTime);
      pvPort->lockMutex();
    }else{
      written = writeDataToPort(frame);
    }
    if((written == 0)&&(!frame->isEmpty())){
      if(maxWriteTry <= 0){
        // Notify error and restore frame into write pool
        mdtError e(MDT_PORT_IO_ERROR, "Max write try reached after write busy", mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtPortThread");
        e.commit();
        if(pvRunning){
          notifyError(mdtAbstractPort::WriteTimeout);
        }
        pvPort->writeFramesPool().enqueue(frame);
        // Thread has nothing else to do
        return mdtAbstractPort::ErrorHandled;
      }
      // Cannot write now, sleep some time and try later
      pvPort->unlockMutex();
      msleep(100);
      pvPort->lockMutex();
      maxWriteTry--;
    }else if(written < 0){
      if(!pvRunning){
        return mdtAbstractPort::ErrorHandled;
      }
      return handleCommonWriteErrors(portError, &frame);
    }
  }
  // Here, frame is completly sent
  Q_ASSERT(frame->isEmpty());
  pvPort->writeFramesPool().enqueue(frame);

  return mdtAbstractPort::NoError;
}

mdtAbstractPort::error_t mdtPortThread::reconnect(bool notify)
{
  Q_ASSERT(pvPort != 0);

  int count = pvReconnectMaxTry;
  mdtAbstractPort::error_t error;

  // Check about buggy caller
  if(!pvRunning){
    mdtError e(MDT_PORT_IO_ERROR, "Thread called reconnect() during a stop process, aborting", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtPortThread");
    e.commit();
    return mdtAbstractPort::UnhandledError;
  }
  if(notify){
    notifyError(mdtAbstractPort::Connecting);
  }
  while(count > 0){
    if(!pvRunning){
      return mdtAbstractPort::Disconnected;
    }
    qDebug() << "mdtPortThread::reconnect(): trying ...";
    error = pvPort->reconnect(pvReconnectTimeout);
    if(error == mdtAbstractPort::NoError){
      if(notify){
        notifyError(mdtAbstractPort::NoError);
      }
      return mdtAbstractPort::NoError;
    }
    if(error != mdtAbstractPort::Disconnected){
      if(notify){
        notifyError(error);
      }
      return error;
    }
    count--;
  }
  // Max try reached
  if(notify){
    mdtError e(MDT_PORT_IO_ERROR, "Connection failed after max try", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtPortThread");
    e.commit();
    notifyError(mdtAbstractPort::Disconnected);
  }

  return mdtAbstractPort::UnhandledError;
}

#ifdef Q_OS_UNIX
void mdtPortThread::sigactionHandle(int /* signum */)
{
  ///qDebug() << "mdtPortThread::sigactionHandle() called, TID: " << pthread_self();
}
#endif
