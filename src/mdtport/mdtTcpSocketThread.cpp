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
#include "mdtTcpSocketThread.h"
#include "mdtPortThreadHelperSocket.h"

#include "mdtPortThreadHelperPort.h"

#include "mdtTcpSocket.h"
#include "mdtError.h"
#include <QApplication>

#include <QDebug>

mdtTcpSocketThread::mdtTcpSocketThread(QObject *parent)
 : mdtPortThread(parent)
{
  pvSocket = 0;
}

bool mdtTcpSocketThread::isReader() const
{
  return true;
}

bool mdtTcpSocketThread::isWriter() const
{
  return true;
}

void mdtTcpSocketThread::run()
{
  Q_ASSERT(pvPort != 0);

  mdtTcpSocket *port = 0;
  ///mdtFrame *writeFrame = 0;
  ///mdtFrame *readFrame = 0;
  mdtAbstractPort::error_t portError = mdtAbstractPort::NoError;
  int completeFrames;
  ///mdtPortThreadHelperPort threadHelper;

  mdtPortThreadHelperSocket threadHelper;


  pvPort->lockMutex();
#ifdef Q_OS_UNIX
  pvNativePthreadObject = pthread_self();
  Q_ASSERT(pvNativePthreadObject != 0);
#endif

  // Create a Tcp socket
  pvSocket = new QTcpSocket;  /// \todo Check about delete !
  Q_ASSERT(pvSocket != 0);
  // We must pass this socket object to pvPort
  // pvPort must be a instance of mdtTcpSocket
  port = dynamic_cast<mdtTcpSocket*>(pvPort);
  Q_ASSERT(port != 0);
  port->setThreadObjects(pvSocket, this);
  
  
  ///pvPort->unlockMutex();
  
  threadHelper.setPort(pvPort);
  threadHelper.setThread(this);
  threadHelper.setSocket(pvSocket);
  
  // Set the running flag
  ///pvRunning = true;
  // Get frames
  if(!threadHelper.getNewFrameRead()){
    pvRunning = false;
    notifyError(mdtAbstractPort::Disconnected);
    /// \todo Cleanup
    return;
  }
  /**
  readFrame = getNewFrameRead();
  if(readFrame == 0){
    return;
  }
  */
  qDebug() << "TCPTHD: starting ...";
  pvPort->unlockMutex();
  ///threadHelper.reconnect(true);
  // Run...
  exec();
  /// \todo Cleanup
  notifyError(mdtAbstractPort::Disconnected);
  qDebug() << "TCPTHD: exec() END";
  return;
  
  while(1){
    // Read thread state
    if(!pvRunning){
      break;
    }
    // Check connection state
    if(pvSocket->state() != QAbstractSocket::ConnectedState){
      // Try to (Re-)connect
      qDebug() << "TCPTHD: connecting ...";
      portError = reconnect(true);
      if(portError != mdtAbstractPort::NoError){
        // Stop
        break;
      }
    }
    // Wait on next request
    if(!threadHelper.getNewFrameWrite()){
      break;
    }
    /**
    writeFrame = getNewFrameWrite();
    // Read thread state
    if(!pvRunning){
      break;
    }
    // If thread is stopping, it can happen that a Null pointer is returned
    if(writeFrame == 0){
      break;
    }
    */
    // Write
    ///portError = writeToPort(writeFrame, false, 0);
    ///portError = threadHelper.writeToPort(false, 0);
    if(portError != mdtAbstractPort::NoError){
      // Check about stopping
      if(!pvRunning){
        break;
      }
      // If a error was handled by writeToPort(), we go back IDLE
      if(portError == mdtAbstractPort::ErrorHandled){
        continue;
      }else{
        // Unhandled error - stop
        break;
      }
    }

    // Clear possibly old stored flush request
    pvPort->flushInRequestPending();
    // Read until 1 frame is completed (should be ok for most query/answer protocols)
    completeFrames = 0;
    while(completeFrames < 1){
      // Read thread state
      if(!pvRunning){
        break;
      }
      // Wait until data is available for read
      portError = pvPort->waitForReadyRead();
      // Check about flush request
      if(pvPort->flushInRequestPending()){
        // Put current frame back to pool, get new one and return idle
        threadHelper.restoreCurrentReadFrameToPool();
        // We don't need to check returned value, because it's false whenn pvRunning is allready false
        threadHelper.getNewFrameRead();
        /**
        pvPort->readFramesPool().enqueue(readFrame);
        readFrame = getNewFrameRead();
        if(readFrame == 0){
          pvRunning = false;
        }
        */
        break;  // go out of while(completeFrames < 1)
      }
      if(portError != mdtAbstractPort::NoError){
        // Check about stopping
        if(!pvRunning){
          break;
        }
        ///portError = handleCommonReadErrors(portError, &readFrame);
        portError = threadHelper.handleCommonReadErrors(portError);
        if(portError != mdtAbstractPort::ErrorHandled){
          // Unhandled error - stop
          pvRunning = false;
          break;
        }
        // Go back IDLE
        break;
      }
      // Read ...
      ///completeFrames = readFromPort(&readFrame);
      ///completeFrames = threadHelper.readFromPort(true);
      qDebug() << "TCPTHD, completeFrames: " << completeFrames;
      if(completeFrames < 0){
        // Check about stopping
        if(!pvRunning){
          break;
        }
        ///portError = handleCommonReadErrors((mdtAbstractPort::error_t)completeFrames, &readFrame);
        portError = threadHelper.handleCommonReadErrors((mdtAbstractPort::error_t)completeFrames);
        if(portError != mdtAbstractPort::ErrorHandled){
          // Unhandled error - stop
          pvRunning = false;
          break;
        }
        // Go back IDLE
        break;
      }
    }
    // In case of raw frames, loop until no more data comes with short timeouts
    if(port->unknownReadSize()){
      while(pvSocket->waitForReadyRead(50)){
        // Check about flush request
        if(pvPort->flushInRequestPending()){
          // Put current frame back to pool, get new one and return idle
          threadHelper.restoreCurrentReadFrameToPool();
          // We don't need to check returned value, because it's false whenn pvRunning is allready false
          threadHelper.getNewFrameRead();
          /**
          pvPort->readFramesPool().enqueue(readFrame);
          readFrame = getNewFrameRead();
          if(readFrame == 0){
            pvRunning = false;
          }
          */
          break;
        }
        // Read ...
        ///completeFrames = readFromPort(&readFrame);
        ///completeFrames = threadHelper.readFromPort(true);
        if(completeFrames < 0){
          // Check about stopping
          if(!pvRunning){
            break;
          }
          ///portError = handleCommonReadErrors((mdtAbstractPort::error_t)completeFrames, &readFrame);
          portError = threadHelper.handleCommonReadErrors((mdtAbstractPort::error_t)completeFrames);
          if(portError != mdtAbstractPort::ErrorHandled){
            // Unhandled error - stop
            pvRunning = false;
            break;
          }
          // Go back IDLE
          break;
        }
      }
    }
  }
  // Put current frame into pool
  /**
  if(readFrame != 0){
    pvPort->readFramesPool().enqueue(readFrame);
  }
  */
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

  qDebug() << "TCPTHD: portError: " << portError;
  if(portError == mdtAbstractPort::NoError){
    qDebug() << "TCPTHD: notify Disconnected";
    notifyError(mdtAbstractPort::Disconnected);
  }else{
    notifyError(portError);
  }
  pvRunning = false;
  pvPort->unlockMutex();
  qDebug() << "TCPTHD: END";
}

