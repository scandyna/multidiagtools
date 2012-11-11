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
#include "mdtTcpSocketThread.h"
#include "mdtTcpSocket.h"
#include "mdtError.h"
#include <QApplication>

//#include <QDebug>

mdtTcpSocketThread::mdtTcpSocketThread(QObject *parent)
 : mdtPortThread(parent)
{
  pvSocket = 0;
}

void mdtTcpSocketThread::stop()
{
  Q_ASSERT(pvPort != 0);

  pvPort->abortFrameToWriteWait();
  mdtPortThread::stop();
}

bool mdtTcpSocketThread::isReader() const
{
  return true;
}

void mdtTcpSocketThread::run()
{
  Q_ASSERT(pvPort != 0);

  mdtTcpSocket *socket;
  mdtFrame *writeFrame = 0;
  mdtFrame *readFrame = 0;
  mdtAbstractPort::error_t portError;
  int reconnectTimeout;
  int reconnectMaxRetry;

  pvPort->lockMutex();
#ifdef Q_OS_UNIX
  pvNativePthreadObject = pthread_self();
  Q_ASSERT(pvNativePthreadObject != 0);
#endif

  // Create a Tcp socket
  pvSocket = new QTcpSocket;
  Q_ASSERT(pvSocket != 0);
  // We must pass this socket object to pvPort
  // pvPort must be a instance of mdtTcpSocket
  socket = dynamic_cast<mdtTcpSocket*>(pvPort);
  Q_ASSERT(socket != 0);
  socket->setThreadObjects(pvSocket, this);
  // Set the running flag
  pvRunning = true;
  /// Get setup \todo Take from config, or something
  reconnectTimeout = 5000;
  reconnectMaxRetry = 5;
  // Get frames
  readFrame = getNewFrameRead();
  if(readFrame == 0){
    return;
  }

  // Run...
  while(1){
    ///qDebug() << "TCPTHD: running ...";
    // Read thread state
    if(!pvRunning){
      break;
    }
    // Wait on next request
    writeFrame = getNewFrameWrite();
    // Read thread state
    if(!pvRunning){
      break;
    }
    // If thread is stopping, it can happen that a Null pointer is returned
    if(writeFrame == 0){
      qDebug() << "TCPTHD: aborting ...";
      break;
    }
    // Check connection state
    if(pvSocket->state() != QAbstractSocket::ConnectedState){
      // Try to (Re-)connect
      portError = reconnect(reconnectTimeout, reconnectMaxRetry);
      if(portError != mdtAbstractPort::NoError){
        // Stop
        emit(errorOccured(portError));
        break;
      }
    }
    ///qDebug() << "TCPTHD: connected ...";
    // Write
    portError = writeToPort(writeFrame, false, 0);
    if(portError != mdtAbstractPort::NoError){
      // Check about connection
      if(portError == mdtAbstractPort::Disconnected){
        // Try to (Re-)connect
        portError = reconnect(reconnectTimeout, reconnectMaxRetry);
        if(portError != mdtAbstractPort::NoError){
          // Stop
          emit(errorOccured(portError));
          break;
        }
      }else{
        // stop
        emit(errorOccured(portError));
        break;
      }
    }
    // Read thread state
    if(!pvRunning){
      break;
    }
    // Wait until all data was written
    portError = pvPort->waitEventWriteReady();
    if(portError != mdtAbstractPort::NoError){
      // Check about connection
      if(portError == mdtAbstractPort::Disconnected){
        // Try to (Re-)connect
        portError = reconnect(reconnectTimeout, reconnectMaxRetry);
        if(portError != mdtAbstractPort::NoError){
          // Stop
          emit(errorOccured(portError));
          break;
        }
      }else if(portError == mdtAbstractPort::WaitingCanceled){
        // Stopping
        break;
      }else{
        // stop
        emit(errorOccured(portError));
        break;
      }
    }
    ///qDebug() << "TCPTHD: written";
    // Read thread state
    if(!pvRunning){
      break;
    }
    // Wait until data is available for read
    portError = pvPort->waitForReadyRead();
    if(portError != mdtAbstractPort::NoError){
      // Check about connection
      if(portError == mdtAbstractPort::Disconnected){
        // Try to (Re-)connect
        portError = reconnect(reconnectTimeout, reconnectMaxRetry);
        if(portError != mdtAbstractPort::NoError){
          // Stop
          emit(errorOccured(portError));
          break;
        }
      }else if(portError == mdtAbstractPort::WaitingCanceled){  /// \todo Not implementd in mdtTcpSocket now
        // Stopping
        break;
      }else{
        // stop
        emit(errorOccured(portError));
        break;
      }
    }
    // Read thread state
    if(!pvRunning){
      break;
    }
    // Read until no more data is available
    while(pvSocket->bytesAvailable() > 0){
      // Read thread state
      if(!pvRunning){
        break;
      }
      // Check timeout state
      if(pvPort->readTimeoutOccured()){
        qDebug() << "TCPTHD: read timeout";
        break;
      }
      ///qDebug() << "Available: " << pvSocket->bytesAvailable();
      // Read
      readFrame = readFromPort(readFrame);
      if(readFrame == 0){
        pvRunning = false;
        break;
      }
      // Wait until new data is available for read (We use a short timeout here)
      ///qDebug() << "waitForReadyRead ...";
      portError = pvPort->waitForReadyRead(50);
      if(portError != mdtAbstractPort::NoError){
        // Check about connection
        if(portError == mdtAbstractPort::Disconnected){
          // Try to (Re-)connect
          portError = reconnect(reconnectTimeout, reconnectMaxRetry);
          if(portError != mdtAbstractPort::NoError){
            // Stop
            emit(errorOccured(portError));
            break;
          }
        }else if(portError == mdtAbstractPort::WaitingCanceled){  /// \todo Not implementd in mdtTcpSocket now
          // Stopping
          break;
        }else{
          // stop
          emit(errorOccured(portError));
          break;
        }
      }
    }
  }

  qDebug() << "TCPTHD: cleanup ...";

  // Put current frame into pool
  if(readFrame != 0){
    pvPort->readFramesPool().enqueue(readFrame);
  }
  // End connection
  if(pvSocket->state() == QAbstractSocket::ConnectedState){
    pvSocket->disconnectFromHost();
    if(pvSocket->state() != QAbstractSocket::UnconnectedState){
      // Wait on disconnected
      if(!pvSocket->waitForDisconnected(500)){
        mdtError e(MDT_TCP_IO_ERROR, "Cannot wait for disconnected" , mdtError::Error);
        e.setSystemError(pvSocket->error(), pvSocket->errorString());
        MDT_ERROR_SET_SRC(e, "mdtTcpSocketThread");
        e.commit();
      }
    }
  }

  qDebug() << "TCPTHD: END";

  pvPort->unlockMutex();
}

