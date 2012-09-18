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
  pvPeerPort = 0;
  // Diseable auto reconnections
  pvMaxReconnect = 0;
}

void mdtTcpSocketThread::stop()
{
  Q_ASSERT(pvPort != 0);

  pvPort->abortFrameToWriteWait();
  mdtPortThread::stop();
}

void mdtTcpSocketThread::connectToHost(QString hostName, quint16 hostPort)
{
  // Mutex is hadled from pvPort, we not lock it here
  pvPeerName = hostName;
  pvPeerPort = hostPort;
}

bool mdtTcpSocketThread::reconnectToHost()
{
  Q_ASSERT(pvPort != 0);

  int counter = pvMaxReconnect + 1;
  int timeout = 500;  // Initial timeout

  // If we are connected, return simply
  if(pvSocket->state() == QAbstractSocket::ConnectedState){
    return true;
  }
  // If a connection is about to be established, wait and return
  if((pvSocket->state() == QAbstractSocket::HostLookupState)||(pvSocket->state() == QAbstractSocket::ConnectingState)){
    pvPort->unlockMutex();
    if(!pvSocket->waitForConnected(timeout)){
      pvPort->lockMutex();
      mdtError e(MDT_TCP_IO_ERROR, "Unable to reconnect to host", mdtError::Error);
      e.setSystemError(pvSocket->error(), pvSocket->errorString());
      MDT_ERROR_SET_SRC(e, "mdtTcpSocketThread");
      e.commit();
      return false;
    }
    pvPort->lockMutex();
    return true;
  }
  // Here, we can be disconnecting or disconnected , wait to be shure
  if(pvSocket->state() != QAbstractSocket::UnconnectedState){
    pvPort->unlockMutex();
    if(!pvSocket->waitForDisconnected(timeout)){
      pvPort->lockMutex();
      mdtError e(MDT_TCP_IO_ERROR, "waitForDisconnected() failed", mdtError::Error);
      e.setSystemError(pvSocket->error(), pvSocket->errorString());
      MDT_ERROR_SET_SRC(e, "mdtTcpSocketThread");
      e.commit();
      return false;
    }
    pvPort->lockMutex();
  }
  // Ok, try reconnect
  while(counter > 0){
    pvSocket->connectToHost(pvPeerName, pvPeerPort);
    pvPort->unlockMutex();
    if(pvSocket->waitForConnected(timeout)){
      pvPort->lockMutex();
      return true;
    }
    pvPort->lockMutex();
    timeout += 100;
    counter--;
  }
  // Here , the maximum retry is reached
  mdtError e(MDT_TCP_IO_ERROR, "Cannot (re-)connect to host " + pvPeerName + ":" + QString::number(pvPeerPort) + " (Max retry reached)" , mdtError::Error);
  MDT_ERROR_SET_SRC(e, "mdtTcpSocketThread");
  e.commit();

  return false;
}

void mdtTcpSocketThread::run()
{
  Q_ASSERT(pvPort != 0);

  mdtTcpSocket *socket;
  mdtFrame *writeFrame = 0;
  mdtFrame *readFrame = 0;
  mdtAbstractPort::error_t portError;

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
  // Get frames
  readFrame = getNewFrameRead();
  if(readFrame == 0){
    return;
  }

  // Run...
  while(1){
    qDebug() << "TCPTHD: running ...";
    // Read thread state
    if(!pvRunning){
      break;
    }
    // Check connection state
    if(pvSocket->state() != QAbstractSocket::ConnectedState){
      // If we have a not empty frame here, connection was broken during write
      if(writeFrame != 0){
        if(writeFrame->size() > 0){
          mdtError e(MDT_TCP_IO_ERROR, "Connection lost during write, putting frame back to pool" , mdtError::Error);
          MDT_ERROR_SET_SRC(e, "mdtTcpSocketThread");
          e.commit();
          emit(errorOccured(MDT_TCP_IO_ERROR));
          pvPort->writeFramesPool().enqueue(writeFrame);
        }
      }
      // Read thread state
      if(!pvRunning){
        break;
      }
      // Wait on next request
      writeFrame = getNewFrameWrite();
      // If thread is stopping, it can happen that a Null pointer is returned
      if(writeFrame == 0){
        qDebug() << "TCPTHD: aborting ...";
        break;
      }
      // Try to (Re-)connect
      if(!reconnectToHost()){
        qDebug() << "TCP THD: SIGNAL: Cannot reconnect";
        emit(errorOccured(MDT_TCP_IO_ERROR));
      }
    }else{
      // Read thread state
      if(!pvRunning){
        break;
      }
      qDebug() << "TCPTHD: connected ...";
      // Get a frame if needed - will block if nothing is to write
      if(writeFrame == 0){
        qDebug() << "TCPTHD: get a new frame for write (was Null) ...";
        writeFrame = getNewFrameWrite();
      }
      // If thread is stopping, it can happen that a Null pointer is returned
      if(writeFrame == 0){
        qDebug() << "TCPTHD: aborting ...";
        break;
      }
      qDebug() << "TCPTHD: have a frame, writing ...";
      // Write
      portError = writeToPort(writeFrame, false, 0);
      if(portError != mdtAbstractPort::NoError){
        // Stop
        break;
      }
      /**
      if(!writeToPort(writeFrame, false, 0)){
        // Stop request or fatal error
        break;
      }
      */
      writeFrame = 0;
      qDebug() << "TCPTHD: have a frame, writing DONE, waiting written ...";
      // Wait until all data was written
      portError = pvPort->waitEventWriteReady();
      if(portError == mdtAbstractPort::WaitingCanceled){  // Not implementd in mdtTcpSocket now
        // Stopping
        break;
      }else if(portError == mdtAbstractPort::UnknownError){
        // Unhandled error. Signal this and stop
        emit(errorOccured(MDT_TCP_IO_ERROR));
        break;
      }
      qDebug() << "TCPTHD: written";
      // Read thread state
      if(!pvRunning){
        break;
      }
      // Wait until data is available for read
      portError = pvPort->waitForReadyRead();
      if(portError == mdtAbstractPort::WaitingCanceled){  // Not implementd in mdtTcpSocket now
        // Stopping
        break;
      }else if(portError == mdtAbstractPort::UnknownError){
        // Unhandled error. Signal this and stop
        emit(errorOccured(MDT_TCP_IO_ERROR));
        break;
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
        qDebug() << "Available: " << pvSocket->bytesAvailable();
        // Read
        readFrame = readFromPort(readFrame);
        if(readFrame == 0){
          pvRunning = false;
          break;
        }
        // Wait until new data is available for read (We use a short timeout here)
        qDebug() << "waitForReadyRead ...";
        portError = pvPort->waitForReadyRead(50);
        if(portError == mdtAbstractPort::WaitingCanceled){  // Not implementd in mdtTcpSocket now
          // Stopping
          break;
        }else if(portError == mdtAbstractPort::UnknownError){
          // Unhandled error. Signal this and stop
          emit(errorOccured(MDT_TCP_IO_ERROR));
          pvRunning = false;
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

