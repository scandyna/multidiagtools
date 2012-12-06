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

  mdtTcpSocket *port;
  mdtFrame *writeFrame = 0;
  mdtFrame *readFrame = 0;
  mdtAbstractPort::error_t portError;
  int reconnectTimeout;
  int reconnectMaxRetry;
  int completeFrames;

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
  port = dynamic_cast<mdtTcpSocket*>(pvPort);
  Q_ASSERT(port != 0);
  port->setThreadObjects(pvSocket, this);
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
      break;
    }
    // Check connection state
    if(pvSocket->state() != QAbstractSocket::ConnectedState){
      // Try to (Re-)connect
      portError = reconnect(reconnectTimeout, reconnectMaxRetry, true);
      if(portError != mdtAbstractPort::NoError){
        // Stop
        break;
      }
    }
    // Write
    portError = writeToPort(writeFrame, false, 0);
    if(portError != mdtAbstractPort::NoError){
      // Check about connection
      if(portError == mdtAbstractPort::Disconnected){
        // Try to (Re-)connect
        portError = reconnect(reconnectTimeout, reconnectMaxRetry, true);
        if(portError != mdtAbstractPort::NoError){
          // Stop
          break;
        }
      }else{
        // stop
        notifyError(portError);
        break;
      }
    }

    // Read until 1 frame is completed (should be ok for most query/answer protocols)
    completeFrames = 0;
    while(completeFrames < 1){
      // Read thread state
      if(!pvRunning){
        break;
      }
      // Wait until data is available for read
      portError = pvPort->waitForReadyRead();
      if(portError != mdtAbstractPort::NoError){
        // Check about connection
        if(portError == mdtAbstractPort::Disconnected){
          // Try to (Re-)connect and go back idle
          portError = reconnect(reconnectTimeout, reconnectMaxRetry, true);
          if(portError != mdtAbstractPort::NoError){
            // Stop
            pvRunning = false;
            break;
          }
        }else if(portError == mdtAbstractPort::WaitingCanceled){  /// \todo Not implementd in mdtTcpSocket now
          // Stopping
          pvRunning = false;
          ///notifyError(portError);
          notifyError(mdtAbstractPort::Disconnected);
          break;
        }else{
          // stop
          pvRunning = false;
          notifyError(portError);
          break;
        }
      }
      // Check about timeout
      if(pvPort->readTimeoutOccured()){
        // Probably disconnected. Abort and Go back idle
        pvSocket->abort();
        portError = reconnect(reconnectTimeout, reconnectMaxRetry, true);
        if(portError != mdtAbstractPort::NoError){
          // Stop
          pvRunning = false;
          break;
        }
        // Go back idle
        break;
      }
      // Read ...
      completeFrames = readFromPort(&readFrame);
      if(completeFrames < 0){
        // Check about connection
        if(portError == mdtAbstractPort::Disconnected){
          // Try to (Re-)connect and go back idle
          portError = reconnect(reconnectTimeout, reconnectMaxRetry, true);
          if(portError != mdtAbstractPort::NoError){
            // Stop
            pvRunning = false;
            break;
          }
        }else if(portError == mdtAbstractPort::WaitingCanceled){  /// \todo Not implementd in mdtTcpSocket now
          // Stopping
          pvRunning = false;
          ///notifyError(portError);
          notifyError(mdtAbstractPort::Disconnected);
          break;
        }else{
          // stop
          pvRunning = false;
          notifyError(portError);
          break;
        }
      }
    }
    // In case of raw frames, loop until no more data comes with short timeouts
    if(port->unknownReadSize()){
      while(pvSocket->waitForReadyRead(50)){
        // Read ...
        completeFrames = readFromPort(&readFrame);
        if(completeFrames < 0){
          // Check about connection
          if(portError == mdtAbstractPort::Disconnected){
            // Try to (Re-)connect and go back idle
            portError = reconnect(reconnectTimeout, reconnectMaxRetry, true);
            if(portError != mdtAbstractPort::NoError){
              // Stop
              pvRunning = false;
              break;
            }
          }else if(portError == mdtAbstractPort::WaitingCanceled){  /// \todo Not implementd in mdtTcpSocket now
            // Stopping
            pvRunning = false;
            ///notifyError(portError);
            notifyError(mdtAbstractPort::Disconnected);
            break;
          }else{
            // stop
            pvRunning = false;
            notifyError(portError);
            break;
          }
        }
      }
    }
  }
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
  pvPort->unlockMutex();
}

