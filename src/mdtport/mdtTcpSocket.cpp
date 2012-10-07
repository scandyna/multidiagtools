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
  pvPeerPort = 0;
}

mdtTcpSocket::~mdtTcpSocket()
{
  close();
}

mdtAbstractPort::error_t mdtTcpSocket::reconnect(int timeout)
{
  Q_ASSERT(pvSocket != 0);

  // If we are connected, return simply
  if(pvSocket->state() == QAbstractSocket::ConnectedState){
    return NoError;
  }
  // If a connection is about to be established, wait and return
  if((pvSocket->state() == QAbstractSocket::HostLookupState)||(pvSocket->state() == QAbstractSocket::ConnectingState)){
    unlockMutex();
    if(!pvSocket->waitForConnected(timeout)){
      lockMutex();
      mdtError e(MDT_TCP_IO_ERROR, "Unable to reconnect to host", mdtError::Error);
      e.setSystemError(pvSocket->error(), pvSocket->errorString());
      MDT_ERROR_SET_SRC(e, "mdtTcpSocket");
      e.commit();
      return UnhandledError;
    }
    lockMutex();
    return NoError;
  }
  // Here, we can be disconnecting or disconnected , wait to be shure
  if(pvSocket->state() != QAbstractSocket::UnconnectedState){
    unlockMutex();
    if(!pvSocket->waitForDisconnected(timeout)){
      lockMutex();
      mdtError e(MDT_TCP_IO_ERROR, "waitForDisconnected() failed", mdtError::Error);
      e.setSystemError(pvSocket->error(), pvSocket->errorString());
      MDT_ERROR_SET_SRC(e, "mdtTcpSocket");
      e.commit();
      return UnhandledError;
    }
    lockMutex();
    return Disconnected;
  }
  // Try to reconnect
  pvSocket->connectToHost(pvPeerName, pvPeerPort);
  unlockMutex();
  if(pvSocket->waitForConnected(timeout)){
    lockMutex();
    return NoError;
  }
  lockMutex();

  return Disconnected;
}

/// \todo Obelète
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
      if(pvSocket->error() == QAbstractSocket::RemoteHostClosedError){
        return Disconnected;
      }else{
        mdtError e(MDT_TCP_IO_ERROR, "waitForReadyRead() failed" , mdtError::Error);
        e.setSystemError(pvSocket->error(), pvSocket->errorString());
        MDT_ERROR_SET_SRC(e, "mdtTcpSocket");
        e.commit();
        return UnhandledError;
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
      if(pvSocket->error() == QAbstractSocket::RemoteHostClosedError){
        return Disconnected;
      }else{
        mdtError e(MDT_TCP_IO_ERROR, "waitForBytesWritten() failed" , mdtError::Error);
        e.setSystemError(pvSocket->error(), pvSocket->errorString());
        MDT_ERROR_SET_SRC(e, "mdtTcpSocket");
        e.commit();
        return UnhandledError;
      }
    }
  }
  // Check about flushOut
  if(pvCancelWrite){
    return WriteCanceled;
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

QString mdtTcpSocket::peerName() const
{
  return pvPeerName;
}

quint16 mdtTcpSocket::peerPort() const
{
  return pvPeerPort;
}

mdtAbstractPort::error_t mdtTcpSocket::pvOpen()
{
  Q_ASSERT(!isOpen());

  QStringList items;
  bool ok;

  // Extract host name and port number from port name
  items = pvPortName.split(":");
  if(items.size() != 2){
    mdtError e(MDT_TCP_IO_ERROR, "Cannot extract host name and port name in (format must be hostname:port) " + pvPortName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtTcpSocket");
    e.commit();
    return SetupError;
  }
  pvPeerName = items.at(0);
  pvPeerPort = items.at(1).toUShort(&ok);
  if(!ok){
    mdtError e(MDT_TCP_IO_ERROR, "Cannot extract port in (format must be hostname:port) " + pvPortName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtTcpSocket");
    e.commit();
    return SetupError;
  }
  // No more action here. When thread starts, the QTcpSocket object will be created,
  // and thread will get the peer name + port, and really connect.

  return NoError;
}

void mdtTcpSocket::pvClose()
{
  Q_ASSERT(isOpen());

  pvThread = 0;
  pvSocket = 0;
}

mdtAbstractPort::error_t mdtTcpSocket::pvSetup()
{
  Q_ASSERT(isOpen());

  // Set R/W timeouts
  setReadTimeout(config().readTimeout());
  setWriteTimeout(config().writeTimeout());
  // Init flags
  pvCancelWrite = false;

  return NoError;
}

void mdtTcpSocket::pvFlushIn()
{
}

void mdtTcpSocket::pvFlushOut()
{
  pvCancelWrite = true;
}
