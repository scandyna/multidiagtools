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
#include "mdtPortThread.h"
#include "mdtError.h"
#include "mdtFrame.h"
#include <errno.h>
#include <QCoreApplication>
#include <QEventLoop>

#include <QDebug>

mdtPortThread::mdtPortThread(QObject *parent)
 : QThread(parent)
{
  pvPort = 0;
  pvRunning = false;
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
  connect(this, SIGNAL(finished()), this, SLOT(onThreadFinished()));
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

mdtAbstractPort *mdtPortThread::port()
{
  return pvPort;
}

void mdtPortThread::detachPort(bool releaseMemory)
{
  Q_ASSERT(!QThread::isRunning());

  if(releaseMemory){
    delete pvPort;
  }
  pvPort = 0;
}

void mdtPortThread::start()
{
  Q_ASSERT(pvPort != 0);

  // Get thread's common setup
  pvReconnectTimeout = pvPort->config().connectTimeout();
  pvReconnectMaxTry = pvPort->config().connectMaxTry();
  // Start..
  pvCurrentError = mdtAbstractPort::UnhandledError;
  QThread::start();
}

void mdtPortThread::stop()
{
  Q_ASSERT(pvPort != 0);

  int err;

  // Unset the running flag
  qDebug() << "mdtPortThread::stop() ...";
  pvPort->lockMutex();
  qDebug() << "mdtPortThread::stop() mutex locked";
  if(!pvRunning){
    pvPort->unlockMutex();
    qDebug() << "mdtPortThread::stop() running flag unset";
    // Exit event loop (for subclass that use a event loop)
    exit();
    return;
  }
  pvRunning = false;
  pvCurrentError = mdtAbstractPort::UnhandledError;
  if(isWriter()){
    pvPort->abortFrameToWriteWait();
  }
#ifdef Q_OS_UNIX
  if(QThread::isRunning()){
    Q_ASSERT(pvNativePthreadObject != 0);
    err = pthread_kill(pvNativePthreadObject, SIGALRM);
//     if(err != 0){
// //       mdtError e(MDT_PORT_IO_ERROR, "pthread_kill() failed", mdtError::Error);
//       switch(err){
//         case EINVAL:
// //           e.setSystemError(err, "Invalid signal was specified (EINVAL)");
//         case ESRCH:
// //           e.setSystemError(err, "Specified thread ID not found (ESRCH)");
//       }
// //       MDT_ERROR_SET_SRC(e, "mdtPortThread");
// //       e.commit();
//     }
  }
#endif
  pvPort->unlockMutex();

  // Exit event loop (for subclass that use a event loop)
  exit();
  
  qDebug() << "mdtPortThread::stop() DONE";
}

void mdtPortThread::waitReady()
{
  while(!isRunning()){
    // Several threads have no event loop, we can wait on more event
    QCoreApplication::processEvents(QEventLoop::AllEvents);
    msleep(50);
  }
}

void mdtPortThread::waitFinished()
{
  while(!isFinished()){
    QCoreApplication::processEvents(QEventLoop::AllEvents);
    // Several threads have no event loop, we can wait on more event
    msleep(50);
  }
}

bool mdtPortThread::isRunning() const
{
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

void mdtPortThread::setRunningFlag(bool running)
{
  pvRunning = running;
}

bool mdtPortThread::runningFlagSet() const
{
  return pvRunning;
}

bool mdtPortThread::isFinished() const
{
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

bool mdtPortThread::handlesTimeout() const
{
  return false;
}

void mdtPortThread::notifyError(mdtAbstractPort::error_t error, bool renotifySameError)
{
  if((error != pvCurrentError)||(renotifySameError)){
    qDebug() << "mdtPortThread::notifyError(): error " << error;
    pvCurrentError = error;
    emit(errorOccured(error));
  }
}

mdtAbstractPort::error_t mdtPortThread::currentError() const
{
  return pvCurrentError;
}

mdtAbstractPort::error_t mdtPortThread::reconnect(bool notify)
{
  Q_ASSERT(pvPort != 0);

  int count = pvReconnectMaxTry;
  mdtAbstractPort::error_t error;

  // Check about buggy caller
  if(!pvRunning){
//     mdtError e(MDT_PORT_IO_ERROR, "Thread called reconnect() during a stop process, aborting", mdtError::Warning);
//     MDT_ERROR_SET_SRC(e, "mdtPortThread");
//     e.commit();
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
        ///notifyError(mdtAbstractPort::NoError);  /// \todo emit connected()
        emit connected();
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
//     mdtError e(MDT_PORT_IO_ERROR, "Connection failed after max try", mdtError::Error);
//     MDT_ERROR_SET_SRC(e, "mdtPortThread");
//     e.commit();
    ///notifyError(mdtAbstractPort::Disconnected); /// \todo notify ConnectionFailed
    notifyError(mdtAbstractPort::ConnectionFailed);
  }

  return mdtAbstractPort::UnhandledError;
}

#ifdef Q_OS_UNIX
void mdtPortThread::sigactionHandle(int /* signum */)
{
}
#endif

void mdtPortThread::onThreadFinished()
{
  emit finished(this);
}
