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
#include "mdtPortThreadHelperSocket.h"
#include "mdtPortThread.h"
#include "mdtAbstractPort.h"
#include "mdtTcpSocket.h"
#include "mdtError.h"
#include <QTimer>
#include <QCoreApplication>

/// \todo Implement socket options (think also about mdtPortConfig)

//#include <QDebug>

mdtPortThreadHelperSocket::mdtPortThreadHelperSocket(QObject *parent)
 : mdtPortThreadHelper(parent)
{
  pvPortNumber = 0;
  pvSocket = 0;
  pvConnectMaxTry = 0;
  pvReadBuffer = 0;
  pvReadBufferSize = 0;

  // Setup connection timeout timer
  pvConnectionTimeoutTimer =  new QTimer(this);
  pvConnectionTimeoutTimer->setSingleShot(true);
  // Setup write timeout
  pvWriteTimeoutTimer = new QTimer(this);
  pvWriteTimeoutTimer->setSingleShot(true);
  // Setup read timeout
  pvReadTimeoutTimer = new QTimer(this);
  pvReadTimeoutTimer->setSingleShot(true);
}

mdtPortThreadHelperSocket::~mdtPortThreadHelperSocket()
{
  delete[] pvReadBuffer;
}

void mdtPortThreadHelperSocket::setSocket(QTcpSocket *socket)
{
  Q_ASSERT(socket != 0);
  Q_ASSERT(pvPort != 0);

  // We need a mdtTcpSocket object
  mdtTcpSocket *p = dynamic_cast<mdtTcpSocket*>(pvPort);
  Q_ASSERT(p != 0);

  // Get needed parameters
  pvHost = p->peerName();
  pvPortNumber = p->peerPort();
  pvConnectMaxTry = p->config().connectMaxTry();
  ///pvConnectTryLeft = pvConnectMaxTry;
  // Store socket pointer and make connections
  pvSocket = socket;
  connect(socket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
  connect(socket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
  connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));
  connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError(QAbstractSocket::SocketError)));
  connect(p, SIGNAL(newFrameToWrite()), this, SLOT(requestWrite()));
  connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(onSocketBytesWritten(qint64)));
  connect(socket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
  connect(socket, SIGNAL(aboutToClose()), this, SLOT(onSocketClosing()));
  // Setup connection timeout
  if(p->config().connectTimeout() < 0){
    pvConnectionTimeoutTimer->setInterval(1000*60*10);
  }else{
    pvConnectionTimeoutTimer->setInterval(p->config().connectTimeout());
  }
  connect(pvConnectionTimeoutTimer, SIGNAL(timeout()), this, SLOT(onConnectionTimeout()));
  // Setup write timeout
  if(p->config().writeTimeout() < 0){
    pvWriteTimeoutTimer->setInterval(1000*60*10);
  }else{
    pvWriteTimeoutTimer->setInterval(p->config().writeTimeout());
  }
  connect(pvWriteTimeoutTimer, SIGNAL(timeout()), this, SLOT(onWriteTimeout()));
  // Setup read timeout
  if(p->config().readTimeout() < 0){
    pvReadTimeoutTimer->setInterval(1000*60*10);
  }else{
    pvReadTimeoutTimer->setInterval(p->config().readTimeout());
  }
  connect(pvReadTimeoutTimer, SIGNAL(timeout()), this, SLOT(onReadTimeout()));
  // Connect
  pvSocket->connectToHost(pvHost, pvPortNumber);
  pvConnectionTimeoutTimer->start();
}

void mdtPortThreadHelperSocket::onSocketConnected()
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(pvSocket != 0);

  pvConnectionTimeoutTimer->stop();
  // We set the thread's running flag, so mdtPortThread::start() will return
  pvPort->lockMutex();
  pvThread->setRunningFlag(true);
  pvConnectMaxTry++;
  notifyError(mdtAbstractPort::NoError);
  pvPort->unlockMutex();
}

void mdtPortThreadHelperSocket::onSocketDisconnected()
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(pvSocket != 0);

  pvPort->lockMutex();
  notifyError(mdtAbstractPort::Disconnected);
  if(pvConnectMaxTry < 1){
    mdtError e(MDT_TCP_IO_ERROR, "Connection failed after max try", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
    e.commit();
    pvSocket->close();
  }else{
    if(pvThread->runningFlagSet()){
      pvConnectMaxTry--;
      pvSocket->connectToHost(pvHost, pvPortNumber);
      pvConnectionTimeoutTimer->start();
      qDebug() << "*** Connecting ... , try left: " << pvConnectMaxTry;
    }
  }
  pvPort->unlockMutex();
}

void mdtPortThreadHelperSocket::onSocketClosing()
{
  qDebug() << "*** onSocketClosing() ...";
  /// Essai
  qint64 readen;
  char buffer[1024];
  do{
    readen = pvSocket->read(buffer, 1024);
    qDebug() << "** flush " << readen << " bytes..";
  }while(readen > 0);

  restoreCurrentWriteFrameToPool();
  restoreCurrentReadFrameToPool();
}

void mdtPortThreadHelperSocket::onSocketError(QAbstractSocket::SocketError socketError)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(pvSocket != 0);

  switch(socketError){
    case QAbstractSocket::ConnectionRefusedError:
      {
        QString message = "Cannot connect to ";
        message += pvHost + " , port: ";
        message += QString::number(pvPortNumber);
        mdtError e(MDT_TCP_IO_ERROR, message, mdtError::Warning);
        e.setSystemError(socketError, pvSocket->errorString());
        MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
        e.commit();
      }
      // Reconnect
      onSocketDisconnected();
      break;
    case QAbstractSocket::RemoteHostClosedError:
      qDebug() << "* Host closed connection";
      // Will call onSocketDisconnected()
      break;
    case QAbstractSocket::HostNotFoundError:
      {
        QString message = "Cannot connect to ";
        message += pvHost + " , port: ";
        message += QString::number(pvPortNumber);
        mdtError e(MDT_TCP_IO_ERROR, message, mdtError::Warning);
        e.setSystemError(socketError, pvSocket->errorString());
        MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
        e.commit();
      }
      // Reconnect
      onSocketDisconnected();
      break;
    case QAbstractSocket::SocketAccessError:
      {
        QString message = "Socket access error";
        mdtError e(MDT_TCP_IO_ERROR, message, mdtError::Error);
        e.setSystemError(socketError, pvSocket->errorString());
        MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
        e.commit();
      }
      // Notify
      notifyError(mdtAbstractPort::PortAccess);
      break;
    case QAbstractSocket::SocketResourceError:
      {
        QString message = "Socket resource error";
        mdtError e(MDT_TCP_IO_ERROR, message, mdtError::Error);
        e.setSystemError(socketError, pvSocket->errorString());
        MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
        e.commit();
      }
      // Notify and stop
      notifyError(mdtAbstractPort::UnhandledError);
      exit(mdtAbstractPort::UnhandledError);
      break;
    case QAbstractSocket::SocketTimeoutError:
      {
        QString message = "Timeout reported by QTcpSocket";
        mdtError e(MDT_TCP_IO_ERROR, message, mdtError::Warning);
        e.setSystemError(socketError, pvSocket->errorString());
        MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
        e.commit();
      }
      // This should only occur if long timeouts are set in port config.
      // We cannot know what sort of timeout we have.
      // We abort the socket, witch will call the disconnect/reconnect process.
      pvSocket->abort();
      break;
    case QAbstractSocket::DatagramTooLargeError:
      {
        QString message = "Socket datagram too large";
        mdtError e(MDT_TCP_IO_ERROR, message, mdtError::Error);
        e.setSystemError(socketError, pvSocket->errorString());
        MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
        e.commit();
      }
      // Notify and stop
      notifyError(mdtAbstractPort::UnhandledError);
      exit(mdtAbstractPort::UnhandledError);
      break;
    case QAbstractSocket::NetworkError:
      {
        QString message = "Network error";
        mdtError e(MDT_TCP_IO_ERROR, message, mdtError::Warning);
        e.setSystemError(socketError, pvSocket->errorString());
        MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
        e.commit();
      }
      // We abort the socket, witch will call the disconnect/reconnect process.
      pvSocket->abort();
      break;
    case QAbstractSocket::UnsupportedSocketOperationError:
      {
        QString message = "Socket unsupported operation error";
        mdtError e(MDT_TCP_IO_ERROR, message, mdtError::Error);
        e.setSystemError(socketError, pvSocket->errorString());
        MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
        e.commit();
      }
      // Notify and stop
      notifyError(mdtAbstractPort::UnhandledError);
      exit(mdtAbstractPort::UnhandledError);
      break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
      {
        QString message = "Socket proxy authentification error";
        mdtError e(MDT_TCP_IO_ERROR, message, mdtError::Error);
        e.setSystemError(socketError, pvSocket->errorString());
        MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
        e.commit();
      }
      // Notify and stop
      notifyError(mdtAbstractPort::UnhandledError);
      exit(mdtAbstractPort::UnhandledError);
      break;
    case QAbstractSocket::UnfinishedSocketOperationError:
      // Used by QAbstractSocketEngine , we do nothing
      break;
    case QAbstractSocket::ProxyConnectionRefusedError:
      {
        QString message = "Socket proxy denied error";
        mdtError e(MDT_TCP_IO_ERROR, message, mdtError::Error);
        e.setSystemError(socketError, pvSocket->errorString());
        MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
        e.commit();
      }
      // Notify and stop
      notifyError(mdtAbstractPort::UnhandledError);
      exit(mdtAbstractPort::UnhandledError);
      break;
    case QAbstractSocket::ProxyConnectionClosedError:
      {
        QString message = "Socket proxy connection closed error";
        mdtError e(MDT_TCP_IO_ERROR, message, mdtError::Error);
        e.setSystemError(socketError, pvSocket->errorString());
        MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
        e.commit();
      }
      // Notify and stop
      notifyError(mdtAbstractPort::UnhandledError);
      exit(mdtAbstractPort::UnhandledError);
      break;
    case QAbstractSocket::ProxyConnectionTimeoutError:
      {
        QString message = "Socket proxy connection timeout error";
        mdtError e(MDT_TCP_IO_ERROR, message, mdtError::Error);
        e.setSystemError(socketError, pvSocket->errorString());
        MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
        e.commit();
      }
      // Notify and stop
      notifyError(mdtAbstractPort::UnhandledError);
      exit(mdtAbstractPort::UnhandledError);
      break;
    case QAbstractSocket::ProxyNotFoundError:
      {
        QString message = "Socket proxy not found";
        mdtError e(MDT_TCP_IO_ERROR, message, mdtError::Error);
        e.setSystemError(socketError, pvSocket->errorString());
        MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
        e.commit();
      }
      // Notify and stop
      notifyError(mdtAbstractPort::UnhandledError);
      exit(mdtAbstractPort::UnhandledError);
      break;
    case QAbstractSocket::ProxyProtocolError:
      {
        QString message = "Socket proxy protocol error";
        mdtError e(MDT_TCP_IO_ERROR, message, mdtError::Error);
        e.setSystemError(socketError, pvSocket->errorString());
        MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
        e.commit();
      }
      // Notify and stop
      notifyError(mdtAbstractPort::UnhandledError);
      exit(mdtAbstractPort::UnhandledError);
      break;
    default:
      {
        QString message = "Socket unknown error";
        mdtError e(MDT_TCP_IO_ERROR, message, mdtError::Error);
        e.setSystemError(socketError, pvSocket->errorString());
        MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
        e.commit();
      }
      // Notify and stop
      notifyError(mdtAbstractPort::UnhandledError);
      exit(mdtAbstractPort::UnhandledError);
      break;
  }
}

void mdtPortThreadHelperSocket::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{
  switch(socketState){
    case QAbstractSocket::HostLookupState:
      notifyError(mdtAbstractPort::Connecting);
      break;
    default:
      // We do nothing
      ;
  }
}

void mdtPortThreadHelperSocket::onSocketReadyRead()
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(pvSocket != 0);

  pvPort->lockMutex();
  // Errors are handled in onSocketError() or onReadTimeout()
  readFromSocket();
  pvPort->unlockMutex();
}

void mdtPortThreadHelperSocket::onSocketBytesWritten(qint64 bytes)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(pvSocket != 0);

  // We stop the write timeout timer
  pvWriteTimeoutTimer->stop();
}

void mdtPortThreadHelperSocket::onConnectionTimeout()
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(pvSocket != 0);

  qDebug() << "*** Connection timeout";
  pvConnectionTimeoutTimer->stop();
  // We abort the socket and reconnect
  pvSocket->abort();
  onSocketDisconnected();
}

void mdtPortThreadHelperSocket::onReadTimeout()
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(pvSocket != 0);

  qDebug() << "*** Read timeout";
  
  pvReadTimeoutTimer->stop();
  pvPort->lockMutex();
  if(currentReadFrame() != 0){
    currentReadFrame()->clear();
    currentReadFrame()->clearSub();
  }
  /// Essai
  qint64 readen;
  char buffer[1024];
  do{
    readen = pvSocket->read(buffer, 1024);
    qDebug() << "** flush " << readen << " bytes..";
  }while(readen > 0);
  
  notifyError(mdtAbstractPort::ReadTimeout);
  pvPort->unlockMutex();
  // We use timeout to detect network disconnection (because normal way can be very long)
  // We abort the socket, witch will call the disconnect/reconnect process.
  pvSocket->abort();
}

void mdtPortThreadHelperSocket::onWriteTimeout()
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(pvSocket != 0);

  qDebug() << "*** Write timeout";
  
  pvWriteTimeoutTimer->stop();
  pvPort->lockMutex();
  restoreCurrentWriteFrameToPool();
  notifyError(mdtAbstractPort::WriteTimeout);
  pvPort->unlockMutex();
  // We use timeout to detect network disconnection (because normal way can be very long)
  // We abort the socket, witch will call the disconnect/reconnect process.
  pvSocket->abort();
}

void mdtPortThreadHelperSocket::requestWrite()
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(pvSocket != 0);

  qDebug() << "* requestWrite() ...";
  if(pvSocket->state() != QAbstractSocket::ConnectedState){
    qDebug() << "* requestWrite() Cancel";
    restoreCurrentWriteFrameToPool();
    // We flush port's write queues - Will lock the mutex
    pvPort->flushOut();
    // Cancel the flush request flag
    pvPort->flushOutRequestPending();
    notifyError(mdtAbstractPort::WriteCanceled);
    return;
  }
  pvPort->lockMutex();
  // Errors are handled/notified in onSocketError() and onWriteTimeout()
  writeToSocket();
  pvPort->unlockMutex();
  qDebug() << "* requestWrite() DONE";
}

mdtAbstractPort::error_t mdtPortThreadHelperSocket::readFromSocket()
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(pvSocket != 0);

  int readen;
  mdtAbstractPort::error_t portError;
  int submittedFrames;

  // Check if a new frame is needed
  if(currentReadFrame() == 0){
    if(!getNewFrameRead()){
      return mdtAbstractPort::ReadPoolEmpty;
    }
  }
  // Alloc local buffer if needed
  if(pvReadBufferSize < currentReadFrame()->capacity()){
    delete[] pvReadBuffer;
    pvReadBufferSize = currentReadFrame()->capacity();
    pvReadBuffer = new char[pvReadBufferSize];
    if(pvReadBuffer == 0){
      pvReadBufferSize = 0;
      mdtError e(MDT_MEMORY_ALLOC_ERROR, "Cannot allocate memory for local buffer", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
      e.commit();
      return mdtAbstractPort::UnhandledError;
    }
  }
  // Read data from port
  readen = pvSocket->read(pvReadBuffer, pvReadBufferSize);
  qDebug() << "** readen: " << readen;
  if(readen < 0){
    portError = mapSocketError(pvSocket->error(), true);
    return portError;
  }
  // Store to frames
  submittedFrames = submitReadenData(pvReadBuffer, readen, true);
  qDebug() << "** submittedFrames: " << submittedFrames;
  if(submittedFrames < 0){
    return (mdtAbstractPort::error_t)submittedFrames;
  }
  // Stop read timeout timer
  if(submittedFrames > 0){
    pvReadTimeoutTimer->stop();
  }

  return mdtAbstractPort::NoError;
}

mdtAbstractPort::error_t mdtPortThreadHelperSocket::writeToSocket()
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(pvSocket != 0);

  int written = 0;
  mdtFrame *frame = 0;
  mdtAbstractPort::error_t portError;

  // Get frame to write
  Q_ASSERT(currentWriteFrame() == 0);
  if(pvPort->writeFrames().size() < 1){
    return mdtAbstractPort::NoError;
  }
  setCurrentWriteFrame(pvPort->writeFrames().dequeue());
  frame = currentWriteFrame();
  Q_ASSERT(frame != 0);
  // Start the write timeout timer
  pvWriteTimeoutTimer->start();
  // If waitAnAnswer flag is set, we start the read timeout timer
  if(frame->waitAnAnswer()){
    pvReadTimeoutTimer->start();
  }
  // Write the frame to socket
  while(!frame->isEmpty()){
    // Check about flush request
    if(pvPort->flushOutRequestPending()){
      pvWriteTimeoutTimer->stop();
      // Restore frame to pool and return
      restoreCurrentWriteFrameToPool();
      return mdtAbstractPort::NoError;
    }
    // Send data
    emit writeProcessBegin();
    written = pvSocket->write(frame->data(), frame->size());
    if(written < 0){
      if(!pvThread->runningFlagSet()){
        return mdtAbstractPort::ErrorHandled;
      }
      portError = mapSocketError(pvSocket->error(), false);
      return portError;
    }
    frame->take(written);
  }
  // Here, frame is completly sent
  Q_ASSERT(frame->isEmpty());
  restoreCurrentWriteFrameToPool();

  return mdtAbstractPort::NoError;
}

mdtAbstractPort::error_t mdtPortThreadHelperSocket::mapSocketError(QAbstractSocket::SocketError error, bool byRead)
{
  Q_ASSERT(pvSocket != 0);

  switch(error){
    case QAbstractSocket::ConnectionRefusedError:
      return mdtAbstractPort::PortAccess;
    case QAbstractSocket::RemoteHostClosedError:
      return mdtAbstractPort::Disconnected;
    case QAbstractSocket::HostNotFoundError:
      return mdtAbstractPort::PortNotFound;
    case QAbstractSocket::SocketAccessError:
      return mdtAbstractPort::PortAccess;
    case QAbstractSocket::SocketTimeoutError:
      if(byRead){
        return mdtAbstractPort::ReadTimeout;
      }else{
        return mdtAbstractPort::WriteTimeout;
      }
    case QAbstractSocket::NetworkError:
      return mdtAbstractPort::Disconnected;
    default:
      mdtError e(MDT_TCP_IO_ERROR, "Unhandled socket error occured.", mdtError::Error);
      e.setSystemError(error, pvSocket->errorString());
      MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
      e.commit();
      return mdtAbstractPort::UnhandledError;
  }
}
