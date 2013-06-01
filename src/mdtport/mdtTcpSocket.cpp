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
#include <QApplication>

// We need a sleep function
#ifdef Q_OS_UNIX
 #define msleep(t) usleep(1000*t)
#endif
#ifdef Q_OS_WIN
 #include <windows.h>
 #define msleep(t) Sleep(t)
#endif

mdtTcpSocket::mdtTcpSocket(QObject *parent)
 : mdtAbstractPort(parent)
{
  ///pvSocket = 0;
  pvPeerPort = 0;
  /// pvUnknownReadSize = true;
}

mdtTcpSocket::~mdtTcpSocket()
{
  close();
}

/**
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
*/

QString mdtTcpSocket::peerName() const
{
  return pvPeerName;
}

quint16 mdtTcpSocket::peerPort() const
{
  return pvPeerPort;
}

/**
void mdtTcpSocket::setThreadObjects(QTcpSocket *socket, mdtTcpSocketThread *thread)
{
  Q_ASSERT(socket != 0);
  Q_ASSERT(thread != 0);

  ///pvSocket = socket;
  ///pvThread = thread;
}
*/

void mdtTcpSocket::setReadTimeout(int timeout)
{
  ///pvReadTimeout = timeout;
}

void mdtTcpSocket::setWriteTimeout(int timeout)
{
  ///pvWriteTimeout = timeout;
}

/**
void mdtTcpSocket::setUnknownReadSize(bool unknown)
{
  pvUnknownReadSize = unknown;
}
*/
/**
bool mdtTcpSocket::unknownReadSize() const
{
  return pvUnknownReadSize;
}
*/

mdtAbstractPort::error_t mdtTcpSocket::waitForReadyRead()
{
  return mdtAbstractPort::UnhandledError;
  /**
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

  return NoError;
  */
}

qint64 mdtTcpSocket::read(char *data, qint64 maxSize)
{
  return mdtAbstractPort::UnhandledError;
  /**
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
  */
}

mdtAbstractPort::error_t mdtTcpSocket::waitEventWriteReady()
{
  return mdtAbstractPort::UnhandledError;
  /**
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

  return NoError;
  */
}

qint64 mdtTcpSocket::write(const char *data, qint64 maxSize)
{
  return mdtAbstractPort::UnhandledError;
  /**
  Q_ASSERT(pvSocket != 0);

  qint64 n;

  n = pvSocket->write(data, maxSize);
  if(n < 0){
    return mapSocketError(pvSocket->error(), false);
  }

  return n;
  */
}

void mdtTcpSocket::addFrameToWrite(mdtFrame *frame)
{
  Q_ASSERT(frame != 0);

  pvWriteFrames.enqueue(frame);
  emit newFrameToWrite();
}

mdtAbstractPort::error_t mdtTcpSocket::pvOpen()
{
  Q_ASSERT(!isOpen());

  QStringList items;
  bool ok;
  ///QTcpSocket socket;
  ///int timeout = 5000; /// \todo Get a valid connection timeout
  ///int maxIter;

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

  return NoError;

  /**
  // Try to connect
  socket.connectToHost(pvPeerName, pvPeerPort);
  maxIter = timeout / 50;
  ok = false;
  while(socket.state() != QAbstractSocket::ConnectedState){
    if(maxIter <= 0){
      break;
    }
    qApp->processEvents();
    msleep(50);
    maxIter--;
  }
  if(socket.state() != QAbstractSocket::UnconnectedState){
    // Check if connection was successfull
    if(socket.state() == QAbstractSocket::ConnectedState){
      ok = true;
    }
    // Disconnect
    socket.abort();
    while((socket.state() != QAbstractSocket::ClosingState)&&(socket.state() != QAbstractSocket::UnconnectedState)){
      qApp->processEvents();
      msleep(50);
    }
  }
  if(!ok){
    mdtError e(MDT_TCP_IO_ERROR, "Cannot connect to " + pvPortName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtTcpSocket");
    e.commit();
    return SetupError;
  }
  // No more action here. When thread starts, the QTcpSocket object will be created,
  // and thread will get the peer name + port, and really connect.

  return NoError;
  */
}

void mdtTcpSocket::pvClose()
{
  Q_ASSERT(isOpen());

  ///pvThread = 0;
  ///pvSocket = 0;
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

/**
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
}
*/
