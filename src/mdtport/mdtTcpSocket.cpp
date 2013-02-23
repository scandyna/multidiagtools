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
#include "mdtTcpSocketThread.h"
#include <QStringList>

mdtTcpSocket::mdtTcpSocket(QObject *parent)
 : mdtAbstractPort(parent)
{
  pvSocket = 0;
  pvPeerPort = 0;
  pvUnknownReadSize = true;
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

void mdtTcpSocket::setUnknownReadSize(bool unknown)
{
  pvUnknownReadSize = unknown;
}

bool mdtTcpSocket::unknownReadSize() const
{
  return pvUnknownReadSize;
}

mdtAbstractPort::error_t mdtTcpSocket::waitForReadyRead()
{
  Q_ASSERT(pvSocket != 0);

  bool ok;

  unlockMutex();
  ok = pvSocket->waitForReadyRead(pvReadTimeout);
  lockMutex();
  if(!ok){
    // Timeout error is probably due tu a connection loss
    if(pvSocket->error() == QAbstractSocket::SocketTimeoutError){
      pvSocket->abort();
      return Disconnected;
    }
    return mapSocketError(pvSocket->error(), true);
  }
  /**
  if(ok){
    updateReadTimeoutState(false);
  }else{
    return mapSocketError(pvSocket->error(), true);
  }
  */

  return NoError;
}

qint64 mdtTcpSocket::read(char *data, qint64 maxSize)
{
  Q_ASSERT(pvSocket != 0);

  qint64 n;

  n = pvSocket->read(data, maxSize);
  if(n < 0){
    // Timeout error is probably due tu a connection loss
    if(pvSocket->error() == QAbstractSocket::SocketTimeoutError){
      pvSocket->abort();
      return Disconnected;
    }
    return mapSocketError(pvSocket->error(), true);
  }

  return n;
}

/// \todo Adapt
mdtAbstractPort::error_t mdtTcpSocket::waitEventWriteReady()
{
  Q_ASSERT(pvSocket != 0);

  bool ok;

  // Check if something is to write , if yes, wait until there are written
  if(pvSocket->bytesToWrite() < 1){
    return NoError;
  }
  // Wait
  unlockMutex();
  ok = pvSocket->waitForBytesWritten(pvWriteTimeout);
  lockMutex();
  if(!ok){
    return mapSocketError(pvSocket->error(), false);
  }
  /**
  if(ok){
    updateReadTimeoutState(false);  /// ??????????????????'
  }else{
    return mapSocketError(pvSocket->error(), false);
  }
  */

  return NoError;
}

/// \todo Adapt
qint64 mdtTcpSocket::write(const char *data, qint64 maxSize)
{
  Q_ASSERT(pvSocket != 0);

  qint64 n;

  n = pvSocket->write(data, maxSize);
  if(n < 0){
    return mapSocketError(pvSocket->error(), false);
  }

  return n;
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
  ///pvCancelWrite = false;

  return NoError;
}

void mdtTcpSocket::pvFlushIn()
{
}

void mdtTcpSocket::pvFlushOut()
{
  ///pvCancelWrite = true;
}

mdtAbstractPort::error_t mdtTcpSocket::mapSocketError(QAbstractSocket::SocketError error, bool byRead)
{
  Q_ASSERT(pvSocket != 0);

  switch(error){
    case QAbstractSocket::ConnectionRefusedError:
      return PortAccess;
    case QAbstractSocket::RemoteHostClosedError:
      return Disconnected;
    case QAbstractSocket::HostNotFoundError:
      return PortNotFound;
    case QAbstractSocket::SocketAccessError:
      return PortAccess;
    case QAbstractSocket::SocketTimeoutError:
      if(byRead){
        return ReadTimeout;
      }else{
        return WriteTimeout;
      }
    case QAbstractSocket::NetworkError:
      return Disconnected;
    default:
      mdtError e(MDT_TCP_IO_ERROR, "Unhandled socket error occured.", mdtError::Error);
      e.setSystemError(error, pvSocket->errorString());
      MDT_ERROR_SET_SRC(e, "mdtTcpSocket");
      e.commit();
      return UnhandledError;
  }
  /**
  if(error == QAbstractSocket::ConnectionRefusedError){
    return PortAccess;
  }else if(error == QAbstractSocket::RemoteHostClosedError){
    return Disconnected;
  }else if(error == QAbstractSocket::HostNotFoundError){
    return PortNotFound;
  }else if(error == QAbstractSocket::SocketAccessError){
    return PortAccess;
  }else if(error == QAbstractSocket::SocketTimeoutError){
    if(byRead){
      updateReadTimeoutState(true);
      ///return ReadTimeout;
      return Disconnected;
    }else{
      ///updateWriteTimeoutState(true);
      return WriteTimeout;
    }
  }else if(error == QAbstractSocket::NetworkError){
    return Disconnected;
  }else{
    mdtError e(MDT_TCP_IO_ERROR, "Unhandled socket error occured.", mdtError::Error);
    e.setSystemError(error, pvSocket->errorString());
    MDT_ERROR_SET_SRC(e, "mdtTcpSocket");
    e.commit();
    return UnhandledError;
  }
  */
}
