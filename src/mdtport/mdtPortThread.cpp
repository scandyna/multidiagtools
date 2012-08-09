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
#include "mdtAbstractPort.h"
#include <QApplication>

#include <QDebug>

mdtPortThread::mdtPortThread(QObject *parent)
 : QThread(parent)
{
  pvPort = 0;
  pvRunning = false;
  pvMinPoolSizeBeforeReadSuspend = 0;
  pvWriteMinWaitTime = 0;
  pvUseReadTimeoutProtocol = false;
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
  /// NOTE: \todo Getting config here is BAD. Setup must be taken by each thread by starting !!
  pvMinPoolSizeBeforeReadSuspend = pvPort->config().readQueueSize() / 4;
  if((pvMinPoolSizeBeforeReadSuspend < 1)&&(pvPort->config().readQueueSize() > 0)){
    pvMinPoolSizeBeforeReadSuspend = 1;
  }
  pvWriteMinWaitTime = pvPort->config().writeMinWaitTime();
  pvBytePerByteWrite = pvPort->config().bytePerByteWrite();
  pvUseReadTimeoutProtocol = pvPort->config().useReadTimeoutProtocol();
  qDebug() << "mdtPortThread::setPort() , writeMinWaitTime: " << pvWriteMinWaitTime;
  qDebug() << "mdtPortThread::setPort() , bytePerByteWrite: " << pvBytePerByteWrite;
  qDebug() << "mdtPortThread::setPort() , useReadTimeoutProtocol: " << pvUseReadTimeoutProtocol;
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
      mdtError e(MDT_UNDEFINED_ERROR, "Thread start timeout reached (500 [ms]) -> abort", mdtError::Error);
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

  // Unset the running flag
  pvPort->lockMutex();
  pvRunning = false;
  pvPort->unlockMutex();
  ///pvPort->abortWaiting();
#ifdef Q_OS_UNIX
  pthread_kill(pvNativePthreadObject, SIGALRM);
#endif

  // Wait the end of the thread
  while(!isFinished()){
    qApp->processEvents();
    msleep(50);
  }
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

void mdtPortThread::sigactionHandle(int /* signum */)
{
}

/**
void mdtPortThread::setCancelStateEnabled(bool enabled)
{
  qDebug() << "mdtPortThread::setCancelStateEnabled(" << enabled << ")";
  setTerminationEnabled(enabled);
}
*/
