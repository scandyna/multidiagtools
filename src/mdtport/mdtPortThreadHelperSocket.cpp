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

#include <QDebug>

/// \todo Ne pas oublier Mutex du port
/// \todo WriteTimeout: chaque write, relancer timer. Si frmae->waitAnAnswer, lancer readtimeoutimer

// We need a sleep function
#ifdef Q_OS_UNIX
 #define msleep(t) usleep(1000*t)
#endif
#ifdef Q_OS_WIN
 #include <windows.h>
 #define msleep(t) Sleep(t)
#endif

mdtPortThreadHelperSocket::mdtPortThreadHelperSocket(QObject *parent)
 : mdtPortThreadHelper(parent)
{
  pvPortNumber = 0;
  pvSocket = 0;
  pvConnectMaxTry = 0;
  ///pvConnectionTimeout = 0;
  ///pvReadTimeout = 0;
  ///pvWriteTimeout = 0;
  pvReadBuffer = 0;
  pvReadBufferSize = 0;

    
  // Setup connection timeout timer
  pvConnectionTimeoutTimer =  new QTimer(this);
  pvConnectionTimeoutTimer->setSingleShot(true);
  // Setup write timeout
  pvWriteTimeoutTimer = new QTimer(this);
  pvWriteTimeoutTimer->setSingleShot(true);
  // Setup read timeout
  ///pvReadTimeout = p->config().readTimeout();
  pvReadTimeoutTimer = new QTimer(this);
  pvReadTimeoutTimer->setSingleShot(true);

  
  ///bool pvConnected;
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
  ///p->lockMutex();
  pvHost = p->peerName();
  pvPortNumber = p->peerPort();
  ///pvConnectionTimeout = p->config().connectTimeout();
  pvConnectMaxTry = p->config().connectMaxTry();
  pvConnectTryLeft = pvConnectMaxTry;
  ///pvWriteTimeout = p->config().writeTimeout();
  ///p->unlockMutex();
  // Store socket pointer and make connections
  pvSocket = socket;
  connect(socket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
  connect(socket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
  connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError(QAbstractSocket::SocketError)));
  connect(p, SIGNAL(newFrameToWrite()), this, SLOT(requestWrite()));
  connect(socket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
  
  /// provisoire
  connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(onSocketBytesWritten(qint64)));
  
  connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));
  /**
  connect(socket, SIGNAL(), this, SLOT());
  connect(socket, SIGNAL(), this, SLOT());
  connect(socket, SIGNAL(), this, SLOT());
  */
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
  
  qDebug() << "--> Connection timeout: " << pvConnectionTimeoutTimer->interval();
  qDebug() << "--> Write timeout: " << pvWriteTimeoutTimer->interval();
  qDebug() << "--> Read timeout: " << pvReadTimeoutTimer->interval();
  
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
  if(pvThread->runningFlagSet()){
    pvSocket->connectToHost(pvHost, pvPortNumber);
  } /**else{
    pvThread->exit(0);
  }
  */
  pvPort->unlockMutex();
}

/// \todo Utile ??
void mdtPortThreadHelperSocket::onSocketClosing()
{
  restoreCurrentWriteFrameToPool();
  restoreCurrentReadFrameToPool();
}

void mdtPortThreadHelperSocket::onSocketError(QAbstractSocket::SocketError socketError)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(pvSocket != 0);

  mdtAbstractPort::error_t portError;

  qDebug() << "mdtPortThreadHelperSocket::onSocketError(): " << socketError;
  /**
  if(!pvThread->runningFlagSet()){
    pvThread->exit(0);
    return;
  }
  */
  switch(socketError){
    case QAbstractSocket::ConnectionRefusedError:
      {
        QString message = "Cannot connect to ";
        message += pvHost + " , port: ";
        message += QString::number(pvPortNumber);
        mdtError e(MDT_TCP_IO_ERROR, message, mdtError::Warning);
        e.setSystemError(socketError, pvSocket->errorString());
        e.commit();
      }
      // Reconnect
      onSocketDisconnected();
      break;
    case QAbstractSocket::RemoteHostClosedError:
      // Will call onSocketClosing()
      break;
    case QAbstractSocket::HostNotFoundError:
      {
        QString message = "Cannot connect to ";
        message += pvHost + " , port: ";
        message += QString::number(pvPortNumber);
        mdtError e(MDT_TCP_IO_ERROR, message, mdtError::Warning);
        e.setSystemError(socketError, pvSocket->errorString());
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
        e.commit();
      }
      // Notify and stop
      notifyError(mdtAbstractPort::UnhandledError);
      exit(mdtAbstractPort::UnhandledError);
      break;
  }
  
  return;
  
  /*
   * QTcpSocket does not differentiate read and write timeout.
   * We will check current read/write frame and react on it,
   * in the hope that this is reliable enough.
   */
  pvPort->lockMutex();
  qDebug() << "-> Port's readenFrames (A): " << pvPort->readenFrames();
  if(currentReadFrame() != 0){
    portError = mapSocketError(socketError, true);
    portError = handleCommonReadErrors(portError);
  }
  if(currentWriteFrame() != 0){
    portError = mapSocketError(socketError, false);
    portError = handleCommonWriteErrors(portError);
  }
  qDebug() << "-> Port's readenFrames (B): " << pvPort->readenFrames();
  pvPort->unlockMutex();
}

/// \todo Utile ?
void mdtPortThreadHelperSocket::onSocketHostFound()
{
}

void mdtPortThreadHelperSocket::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{
  
  qDebug() << "mdtPortThreadHelperSocket::onSocketStateChanged(): " << socketState;
  
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

  ///mdtAbstractPort::error_t portError;

  qDebug() << "mdtPortThreadHelperSocket::onSocketReadyRead() ...";
  
  pvPort->lockMutex();
  ///portError = readFromSocket();
  // Errors are handled in onSocketError() or onReadTimeout()
  readFromSocket();
  ///pvPort->unlockMutex();
  // We handle only timeout (that are reported from QTcpSocket) here
  /**
  if(portError == mdtAbstractPort::ReadTimeout){
    if(currentReadFrame() != 0){
      currentReadFrame()->clear();
      currentReadFrame()->clearSub();
    }
    notifyError(mdtAbstractPort::ReadTimeout);
  }
  */
  pvPort->unlockMutex();
  /**
  if(portError != mdtAbstractPort::NoError){
    notifyError(portError); /// \todo check if not redoundant with onSocketError()
  }
  */
  qDebug() << "mdtPortThreadHelperSocket::onSocketReadyRead() DONE";
}

void mdtPortThreadHelperSocket::onSocketBytesWritten(qint64 bytes)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(pvSocket != 0);

  qDebug() << "mdtPortThreadHelperSocket::onSocketBytesWritten(), n: " << bytes;
  // We stop the write timeout timer
  pvWriteTimeoutTimer->stop();
}

void mdtPortThreadHelperSocket::onConnectionTimeout()
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(pvSocket != 0);

  // We abort the socket, witch will call the disconnect/reconnect process.
  pvSocket->abort();

  /**
  pvPort->lockMutex();
  notifyError(mdtAbstractPort::Disconnected);
  if(pvThread->runningFlagSet()){
    pvSocket->connectToHost(pvHost, pvPortNumber);
  }
  pvPort->unlockMutex();
  */
}

void mdtPortThreadHelperSocket::onReadTimeout()
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(pvSocket != 0);

  qDebug() << "-**-> Read timeout";
  
  pvPort->lockMutex();
  if(currentReadFrame() != 0){
    currentReadFrame()->clear();
    currentReadFrame()->clearSub();
  }
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

  qDebug() << "-**-> Write timeout";
  
  pvPort->lockMutex();
  restoreCurrentWriteFrameToPool();
  notifyError(mdtAbstractPort::WriteTimeout);
  pvPort->unlockMutex();
  // We use timeout to detect network disconnection (because normal way can be very long)
  // We abort the socket, witch will call the disconnect/reconnect process.
  pvSocket->abort();
}

/// \todo handle timeouts
void mdtPortThreadHelperSocket::requestWrite()
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(pvSocket != 0);

  ///mdtAbstractPort::error_t portError;

  qDebug() << "mdtPortThreadHelperSocket::requestWrite() ...";
  
  pvPort->lockMutex();
  ///portError = writeToSocket();
  // Errors are handled in onSocketError() and onWriteTimeout()
  writeToSocket();
  pvPort->unlockMutex();
  /**
  if(portError != mdtAbstractPort::NoError){
    notifyError(portError); /// \todo check if not redoundant with onSocketError()
  }
  */
  qDebug() << "mdtPortThreadHelperSocket::requestWrite() DONE";
}

mdtAbstractPort::error_t mdtPortThreadHelperSocket::readFromSocket()
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(pvSocket != 0);

  int readen;
  mdtAbstractPort::error_t portError;
  int submittedFrames;

  qDebug() << "-> Socket's bytesAvailable (A): " << pvSocket->bytesAvailable();
  
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
  if(readen < 0){
    portError = mapSocketError(pvSocket->error(), true);
    ///return handleCommonReadErrors(portError);
    return portError;
  }
  // Store to frames
  submittedFrames = submitReadenData(pvReadBuffer, readen, true);
  qDebug() << "mdtPortThreadHelperSocket::readFromSocket() , submitted " << submittedFrames << " frames";
  qDebug() << "-> Port's readen frames: " << pvPort->readenFrames();
  qDebug() << "-> Socket's bytesAvailable (B): " << pvSocket->bytesAvailable();
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
    qDebug() << "-*-> Wait an answer true, starting read timeout timer";
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
      ///return handleCommonWriteErrors(portError);
      return portError;
    }
    frame->take(written);
  }
  // Here, frame is completly sent
  Q_ASSERT(frame->isEmpty());
  restoreCurrentWriteFrameToPool();

  return mdtAbstractPort::NoError;
}

/**
mdtAbstractPort::error_t mdtPortThreadHelperSocket::reconnect(bool notify)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(pvThread != 0);
  Q_ASSERT(pvSocket != 0);

  return mdtAbstractPort::NoError;
  
  int maxConnectTimeoutIter = 0; ///pvConnectionTimeout / 50;

  qDebug() << "mdtPortThreadHelperSocket::reconnect() ...";
  
  // If we are connected, return simply
  if(pvSocket->state() == QAbstractSocket::ConnectedState){
    qDebug() << "mdtPortThreadHelperSocket::reconnect() OK (1)";
    return mdtAbstractPort::NoError;
  }
  // Check max try
  if(pvConnectTryLeft <= 0){
    mdtError e(MDT_TCP_IO_ERROR, "Unable to connect to host " + pvHost + " afetr max try (" + QString::number(pvConnectMaxTry) + ")", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
    e.commit();
    if(notify){
      notifyError(mdtAbstractPort::UnhandledError);
    }
    return mdtAbstractPort::UnhandledError;
  }
  pvConnectTryLeft--;
  // If a connection is about to be established, wait and return
  if((pvSocket->state() == QAbstractSocket::HostLookupState)||(pvSocket->state() == QAbstractSocket::ConnectingState)){
    while(pvSocket->state() != QAbstractSocket::ConnectedState){
      /// \todo check about port's mutex handling
      // Check about running flag
      if(!pvThread->runningFlagSet()){
        pvSocket->abort();
        if(notify){
          notifyError(mdtAbstractPort::Disconnected);
        }
        qDebug() << "mdtPortThreadHelperSocket::reconnect() running flag false, Abort (1)";
        return mdtAbstractPort::Disconnected;
      }
      // Check about timeout
      if(maxConnectTimeoutIter <= 0){
        pvSocket->abort();
        if(notify){
          notifyError(mdtAbstractPort::Disconnected);
        }
        qDebug() << "mdtPortThreadHelperSocket::reconnect() timeout, Abort (1)";
        return mdtAbstractPort::Disconnected;
      }
      // Wait
      msleep(50);
      QCoreApplication::processEvents();
    }
    ///unlockMutex();
    
    if(!pvSocket->waitForConnected(timeout)){
      lockMutex();
      mdtError e(MDT_TCP_IO_ERROR, "Unable to reconnect to host", mdtError::Error);
      e.setSystemError(pvSocket->error(), pvSocket->errorString());
      MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
      e.commit();
      return UnhandledError;
    }
    
    ///lockMutex();
    pvConnectTryLeft = pvConnectMaxTry;
    qDebug() << "mdtPortThreadHelperSocket::reconnect() OK (2)";
    return mdtAbstractPort::NoError;
  }
  // Here, we can be disconnecting or disconnected , wait to be shure
  if(pvSocket->state() != QAbstractSocket::UnconnectedState){
    while(pvSocket->state() != QAbstractSocket::UnconnectedState){
      /// \todo check about port's mutex handling
      // Check about running flag
      if(!pvThread->runningFlagSet()){
        pvSocket->abort();
        if(notify){
          notifyError(mdtAbstractPort::Disconnected);
        }
        qDebug() << "mdtPortThreadHelperSocket::reconnect() running flag false, Abort (2)";
        return mdtAbstractPort::Disconnected;
      }
      // Check about timeout
      if(maxConnectTimeoutIter <= 0){
        pvSocket->abort();
        if(notify){
          notifyError(mdtAbstractPort::Disconnected);
        }
        qDebug() << "mdtPortThreadHelperSocket::reconnect() timeout, Abort (2)";
        return mdtAbstractPort::Disconnected;
      }
      // Wait
      msleep(50);
      QCoreApplication::processEvents();
    }
    ///unlockMutex();
    
    if(!pvSocket->waitForDisconnected(timeout)){
      ///lockMutex();
      mdtError e(MDT_TCP_IO_ERROR, "waitForDisconnected() failed", mdtError::Error);
      e.setSystemError(pvSocket->error(), pvSocket->errorString());
      MDT_ERROR_SET_SRC(e, "mdtPortThreadHelperSocket");
      e.commit();
      return UnhandledError;
    }
    
    ///lockMutex();
    qDebug() << "mdtPortThreadHelperSocket::reconnect() ???? , Abort (1)";
    return mdtAbstractPort::Disconnected;
  }
  // Try to reconnect
  pvSocket->connectToHost(pvHost, pvPortNumber);
  while(pvSocket->state() != QAbstractSocket::ConnectedState){
    /// \todo check about port's mutex handling
    // Check about running flag
    if(!pvThread->runningFlagSet()){
      pvSocket->abort();
      if(notify){
        notifyError(mdtAbstractPort::Disconnected);
      }
      qDebug() << "mdtPortThreadHelperSocket::reconnect() running flag false, Abort (3)";
      return mdtAbstractPort::Disconnected;
    }
    // Check about timeout
    if(maxConnectTimeoutIter <= 0){
      pvSocket->abort();
      if(notify){
        notifyError(mdtAbstractPort::Disconnected);
      }
      qDebug() << "mdtPortThreadHelperSocket::reconnect() timeout, Abort (3)";
      return mdtAbstractPort::Disconnected;
    }
    // Wait
    msleep(50);
    QCoreApplication::processEvents();
  }

  ///unlockMutex();
  
  if(pvSocket->waitForConnected(timeout)){
    ///lockMutex();
    return NoError;
  }
  
  ///lockMutex();

  qDebug() << "mdtPortThreadHelperSocket::reconnect() OK (3)";
  return mdtAbstractPort::NoError;

}
*/

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

