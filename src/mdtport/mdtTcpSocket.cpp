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
}

mdtTcpSocket::~mdtTcpSocket()
{
  close();
}

void mdtTcpSocket::connectToHost(const QString &hostName, int hostPort)
{
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(hostPort > 0);

  pvMutex.lock();
  pvThread->connectToHost(hostName, hostPort);
  pvMutex.unlock();
}

void mdtTcpSocket::setThreadObjects(QTcpSocket *socket, mdtTcpSocketThread *thread)
{
  Q_ASSERT(socket != 0);
  Q_ASSERT(thread != 0);

  pvSocket = socket;
  pvThread = thread;
}

void mdtTcpSocket::setReadTimeout(int timeout)
{
  pvReadTimeout = timeout;
}

void mdtTcpSocket::setWriteTimeout(int timeout)
{
  pvWriteTimeout = timeout;
}

mdtAbstractPort::error_t mdtTcpSocket::waitForReadyRead()
{
  Q_ASSERT(pvSocket != 0);

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
        return UnknownError;
      }
    }
  }

  return NoError;
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

mdtAbstractPort::error_t mdtTcpSocket::waitEventWriteReady()
{
  Q_ASSERT(pvSocket != 0);

  // Check if something is to write , if yes, wait until there are written
  if(pvSocket->bytesToWrite() < 1){
    return NoError;
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
        return UnknownError;
      }
    }
  }

  return NoError;
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

mdtAbstractPort::error_t mdtTcpSocket::pvOpen()
{
  Q_ASSERT(!isOpen());

  return NoError;
}

void mdtTcpSocket::pvClose()
{
  Q_ASSERT(isOpen());
}

mdtAbstractPort::error_t mdtTcpSocket::pvSetup()
{
  Q_ASSERT(isOpen());

  // Set R/W timeouts
  setReadTimeout(config().readTimeout());
  setWriteTimeout(config().writeTimeout());

  return NoError;
}

void mdtTcpSocket::pvFlushIn()
{
}

void mdtTcpSocket::pvFlushOut()
{
}
