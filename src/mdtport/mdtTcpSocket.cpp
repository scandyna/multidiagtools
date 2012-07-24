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
#include "mdtTcpSocket.h"
#include "mdtError.h"
#include <QStringList>

#include "mdtTcpSocketThread.h"

//#include <QDebug>

mdtTcpSocket::mdtTcpSocket(QObject *parent)
 : mdtAbstractPort(parent)
{
  pvSocket = 0;
  pvTransactionsCount = 0;
}

mdtTcpSocket::~mdtTcpSocket()
{
  close();
}

/**
bool mdtTcpSocket::setAttributes(const QString &portName)
{
  //qDebug() << "setAttributes(): " << portName.split(":");
  
  return true;
}
*/

mdtAbstractPort::error_t mdtTcpSocket::tryOpen()
{
  return NoError;
}

bool mdtTcpSocket::open(mdtPortConfig &cfg)
{
  // Close previous opened connection
  close();
  // Open new connection
  lockMutex();
  qDebug() << "Open ....";

  // Set R/W timeouts
  setReadTimeout(cfg.readTimeout());
  setWriteTimeout(cfg.writeTimeout());

  return mdtAbstractPort::open(cfg);
}

void mdtTcpSocket::close()
{
  lockMutex();
  pvTransactionsCount = 0;
  mdtAbstractPort::close();
}

void mdtTcpSocket::connectToHost(const QString &hostName, int hostPort)
{
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(hostPort > 0);

  pvMutex.lock();
  pvThread->connectToHost(hostName, hostPort);
  pvNewTransaction.wakeAll();
  pvMutex.unlock();
}

void mdtTcpSocket::setThreadObjects(QTcpSocket *socket, mdtTcpSocketThread *thread)
{
  Q_ASSERT(socket != 0);
  Q_ASSERT(thread != 0);

  pvSocket = socket;
  pvThread = thread;
  connect(pvThread, SIGNAL(newFrameReaden()), this, SLOT(decrementTransactionsCounter()), Qt::DirectConnection);
}

void mdtTcpSocket::setReadTimeout(int timeout)
{
  pvReadTimeout = timeout;
}

void mdtTcpSocket::setWriteTimeout(int timeout)
{
  pvWriteTimeout = timeout;
}

void mdtTcpSocket::beginNewTransaction()
{
  pvMutex.lock();
  pvTransactionsCount++;
  pvNewTransaction.wakeAll();
  pvMutex.unlock();
}

void mdtTcpSocket::waitForNewTransaction()
{
  pvNewTransaction.wait(&pvMutex);
}

bool mdtTcpSocket::waitForReadyRead()
{
  Q_ASSERT(pvSocket != 0);

  // Check if something is to do, if not, wait
  if((pvTransactionsCount < 1)&&(pvSocket->bytesToWrite() < 1)){
    pvNewTransaction.wait(&pvMutex);
  }
  
  // Check if it is possible to read now
  if(!pvSocket->waitForReadyRead(pvReadTimeout)){
    // Check if we have a timeout
    if(pvSocket->error() == QAbstractSocket::SocketTimeoutError){
      updateReadTimeoutState(true);
    }else{
      updateReadTimeoutState(false);
      // It can happen that host closes the connexion, this is Ok (thread will try to reconnect) - Register all other errors
      if(pvSocket->error() != QAbstractSocket::RemoteHostClosedError){
        mdtError e(MDT_TCP_IO_ERROR, "waitForReadyRead() failed" , mdtError::Error);
        e.setSystemError(pvSocket->error(), pvSocket->errorString());
        MDT_ERROR_SET_SRC(e, "mdtTcpSocket");
        e.commit();
        return false;
      }
    }
  }

  return true;
}

qint64 mdtTcpSocket::read(char *data, qint64 maxSize)
{
  Q_ASSERT(pvSocket != 0);

  qint64 n;

  n = pvSocket->read(data, maxSize);
  if(n < 0){
    mdtError e(MDT_TCP_IO_ERROR, "read() failed" , mdtError::Error);
    e.setSystemError(pvSocket->error(), pvSocket->errorString());
    MDT_ERROR_SET_SRC(e, "mdtTcpSocket");
    e.commit();
  }

  return n;
}

bool mdtTcpSocket::waitEventWriteReady()
{
  Q_ASSERT(pvSocket != 0);

  // Check if something is to do, if not, wait
  if((pvTransactionsCount < 1)&&(pvSocket->bytesToWrite() < 1)){
    pvNewTransaction.wait(&pvMutex);
  }
  // Check if something is to write , if yes, wait until there are written
  if(pvSocket->bytesToWrite() < 1){
    return true;
  }
  if(!pvSocket->waitForBytesWritten(pvWriteTimeout)){
    // Check if we have a timeout
    if(pvSocket->error() == QAbstractSocket::SocketTimeoutError){
      updateReadTimeoutState(true);
    }else{
      updateReadTimeoutState(false);
      // It can happen that host closes the connexion, this is Ok (thread will try to reconnect) - Register all other errors
      if(pvSocket->error() != QAbstractSocket::RemoteHostClosedError){
        mdtError e(MDT_TCP_IO_ERROR, "waitForBytesWritten() failed" , mdtError::Error);
        e.setSystemError(pvSocket->error(), pvSocket->errorString());
        MDT_ERROR_SET_SRC(e, "mdtTcpSocket");
        e.commit();
        return false;
      }
    }
  }

  return true;
}

qint64 mdtTcpSocket::write(const char *data, qint64 maxSize)
{
  Q_ASSERT(pvSocket != 0);

  qint64 n;

  n = pvSocket->write(data, maxSize);
  if(n < 0){
    mdtError e(MDT_TCP_IO_ERROR, "write() failed" , mdtError::Error);
    e.setSystemError(pvSocket->error(), pvSocket->errorString());
    MDT_ERROR_SET_SRC(e, "mdtTcpSocket");
    e.commit();
  }

  return n;
}

void mdtTcpSocket::decrementTransactionsCounter()
{
  if(pvTransactionsCount > 0){
    pvTransactionsCount--;
  }
}
