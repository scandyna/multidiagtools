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
#ifdef Q_OS_UNIX
  pvNativePthreadObject = 0;
  // We must catch the SIGALRM signal, else the application process
  // will be killed by pthread_kill()
  sigemptyset(&(pvSigaction.sa_mask));
  pvSigaction.sa_flags = 0;
  pvSigaction.sa_handler = sigactionHandle;
  sigaction(SIGALRM, &pvSigaction, NULL);
#endif
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
  pvPort->lockMutex();
  if(!pvRunning){
    pvPort->unlockMutex();
    return;
  }
  pvRunning = false;
  pvPort->unlockMutex();
#ifdef Q_OS_UNIX
  Q_ASSERT(pvNativePthreadObject != 0);
  qDebug() << "mdtPortThread::stop(): sending SIGALRM ...";
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
  qDebug() << "mdtPortThread::stop(): SIGALRM sent";
#endif

  // Wait the end of the thread
  while(!isFinished()){
    qApp->processEvents();
    qDebug() << "mdtPortThread::stop(): process events ...";
    msleep(50);
  }
  // Free read/write buffers
  delete[] pvReadBuffer;
  pvReadBuffer = 0;
  pvReadBufferSize = 0;
  qDebug() << "mdtPortThread::stop(): END";
}

bool mdtPortThread::isRunning() const
{
  Q_ASSERT(pvPort != 0);

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

bool mdtPortThread::isFinished() const
{
  Q_ASSERT(pvPort != 0);

  pvPort->lockMutex();
  if(pvRunning){
    pvPort->unlockMutex();
    return false;
  }
  pvPort->unlockMutex();

  return QThread::isFinished();
}

mdtFrame *mdtPortThread::getNewFrameRead()
{
  Q_ASSERT(pvPort != 0);

  mdtFrame *frame;

  if(pvPort->readFramesPool().size() < 1){
    mdtError e(MDT_PORT_QUEUE_EMPTY_ERROR, "Read frames pool is empty", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtPortThread");
    e.commit();
    emit(errorOccured(MDT_PORT_QUEUE_EMPTY_ERROR));
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
  }
  frame = pvPort->readFramesPool().dequeue();
  Q_ASSERT(frame != 0);
  frame->clear();
  frame->clearSub();

  return frame;
}

mdtFrame *mdtPortThread::readFromPort(mdtFrame *frame)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(frame != 0);

  char *bufferCursor = 0;
  qint64 readen = 0;
  qint64 stored = 0;
  qint64 toStore = 0;
  mdtFrame *f = frame;

  // Alloc local buffer if needed
  if(pvReadBufferSize < f->capacity()){
    delete[] pvReadBuffer;
    pvReadBufferSize = f->capacity();
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
  emit ioProcessBegin();
  readen = pvPort->read(pvReadBuffer, pvReadBufferSize);
  if(readen < 0){
    return 0;
  }
  // Store readen data
  toStore = readen;
  while(toStore > 0){
    // Check end of thread flag
    if(!pvRunning){
      return 0;
    }
    // Check for new frame if needed
    if(f == 0){
      ///qDebug() << "mdtPortThread::readFromPort(): frame Null, get new one";
      f = getNewFrameRead();
      if(f == 0){
        return 0;
      }
    }
    // Store data
    stored = f->putData(bufferCursor, toStore);
    // If frame is full, enqueue to readen frames and get a new one
    if(f->bytesToStore() == 0){
      stored += f->eofSeqLen();
      pvPort->readenFrames().enqueue(f);
      // emit a Readen frame signal
      emit newFrameReaden();
      ///qDebug() << "mdtPortThread::readFromPort(): frame complete, get new one";
      f = getNewFrameRead();
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

  return f;
}

mdtFrame *mdtPortThread::getNewFrameWrite()
{
  Q_ASSERT(pvPort != 0);

  return pvPort->getFrameToWrite();
}

mdtAbstractPort::error_t mdtPortThread::writeToPort(mdtFrame *frame, bool bytePerByteWrite, int interByteTime)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(frame != 0);

  char *bufferCursor = 0;
  qint64 toWrite = 0;
  qint64 written = 0;
  mdtAbstractPort::error_t portError;

  // Write the frame to port
  bufferCursor = frame->data();
  toWrite = frame->size();
  while(toWrite > 0){
    // Wait on write ready event
    portError = pvPort->waitEventWriteReady();
    if(portError != mdtAbstractPort::NoError){
      if(portError == mdtAbstractPort::WriteCanceled){
        // Restore frame to pool and return
        pvPort->writeFramesPool().enqueue(frame);
        return mdtAbstractPort::NoError;
      }else{
        return portError;
      }
    }
    // Event occured, send the data to port - Check timeout state first
    if(pvPort->writeTimeoutOccured()){
      // Cannot write now, sleep some time and try later
      pvPort->unlockMutex();
      msleep(100);
      pvPort->lockMutex();
    }else{
      // Write data to port
      emit ioProcessBegin();
      if(bytePerByteWrite){
        written = pvPort->write(bufferCursor, 1);
        pvPort->unlockMutex();
        msleep(interByteTime);
        pvPort->lockMutex();
      }else{
        written = pvPort->write(bufferCursor, toWrite);
      }
      if(written < 0){
        return mdtAbstractPort::UnhandledError;
      }
      frame->take(written);
      // Update cursor and toWrite
      if(frame->isEmpty()){
        toWrite = 0;
      }else{
        bufferCursor = frame->data();
        Q_ASSERT(bufferCursor < (frame->data() + frame->size()));
        toWrite -= written;
        Q_ASSERT(toWrite >= 0);
      }
    }
  }
  // Here, frame is completly sent
  Q_ASSERT(frame->isEmpty());
  pvPort->writeFramesPool().enqueue(frame);

  return mdtAbstractPort::NoError;
}

mdtAbstractPort::error_t mdtPortThread::reconnect(int timeout, int maxTry)
{
  Q_ASSERT(pvPort != 0);

  int count = maxTry;
  mdtAbstractPort::error_t error;

  while(count > 0){
    qDebug() << "mdtPortThread::reconnect(): trying ...";
    error = pvPort->reconnect(timeout);
    if(error == mdtAbstractPort::NoError){
      return mdtAbstractPort::NoError;
    }
    if(error != mdtAbstractPort::Disconnected){
      return error;
    }
    count--;
  }

  qDebug() << "mdtPortThread::reconnect(): max retry, reconnection FAILED :-(";
  return mdtAbstractPort::UnhandledError;
}

#ifdef Q_OS_UNIX
void mdtPortThread::sigactionHandle(int /* signum */)
{
  qDebug() << "mdtPortThread::sigactionHandle() called";
}
#endif
