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
  pvReadBuffer = 0;
  pvReadBufferSize = 0;
  pvSocket = 0;
  pvPeerPort = 0;
  // Diseable auto reconnections
  pvMaxReconnect = 0;
}

void mdtTcpSocketThread::stop()
{
  Q_ASSERT(pvPort != 0);

  mdtTcpSocket *socket;

  // Unset the running flag
  pvPort->lockMutex();
  pvRunning = false;
  // Wake up - We must have a pointer to a mdtTcpSocket
  socket = dynamic_cast<mdtTcpSocket*>(pvPort);
  Q_ASSERT(socket != 0);
  pvPort->unlockMutex();
  socket->beginNewTransaction();

  // Wait the end of the thread
  while(!isFinished()){
    qApp->processEvents();
    msleep(50);
  }
}

void mdtTcpSocketThread::connectToHost(QString hostName, quint16 hostPort)
{
  // Mutex is hadled from pvPort, we not lock it here
  pvPeerName = hostName;
  pvPeerPort = hostPort;
}

void mdtTcpSocketThread::readFromSocket()
{
  char *bufferCursor;
  qint64 toStore;
  qint64 readen;
  qint64 stored;

  // Reset bufferCursor
  bufferCursor = pvReadBuffer;
  // Read data from port
  readen = pvPort->read(pvReadBuffer, pvReadBufferSize);
  if(readen < 0){
    readen = 0;
    emit(errorOccured(MDT_TCP_IO_ERROR));
    // Sleep a bit to prevent crazy ressources consumtion NOTE: mutex !?
    msleep(100);
  }
  toStore = readen;
  // Store readen data
  while(toStore > 0){
    // Check for new frame if needed
    while(pvReadCurrentFrame == 0){
      pvReadCurrentFrame = getNewFrameRead();
      if(pvReadCurrentFrame == 0){
        // Pool empty. Try later
        pvPort->unlockMutex();
        msleep(100);
        // Check about end of thread
        pvPort->lockMutex();
        if(!pvRunning){
          pvPort->unlockMutex();
          return;
        }
      }
    }
    // Store data
    stored = pvReadCurrentFrame->putData(bufferCursor, toStore);
    // If frame is full, enqueue to readen frames and get a new one
    if(pvReadCurrentFrame->bytesToStore() == 0){
      stored += pvReadCurrentFrame->eofSeqLen();
      pvPort->readenFrames().enqueue(pvReadCurrentFrame);
      // emit a Readen frame signal if complete
      emit newFrameReaden();
      pvReadCurrentFrame = getNewFrameRead();
    }
    // When frame becomes full and EOF condition was not reached, stored will be to big
    if(stored >= pvReadBufferSize){
      stored = pvReadBufferSize-1;
      toStore = 0;
    }else{
      toStore = toStore - stored;
    }
    bufferCursor = bufferCursor + stored;
    Q_ASSERT(bufferCursor < (pvReadBuffer + pvReadBufferSize));
  }
}

mdtFrame *mdtTcpSocketThread::getNewFrameRead()
{
  Q_ASSERT(pvPort != 0);

  mdtFrame *frame;

  if(pvPort->readFramesPool().size() < 1){
    mdtError e(MDT_TCP_IO_ERROR, "Read frames pool is empty", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtTcpSocketThread");
    e.commit();
    return 0;
  }
  frame = pvPort->readFramesPool().dequeue();
  Q_ASSERT(frame != 0);
  frame->clear();

  return frame;
}

void mdtTcpSocketThread::writeToSocket()
{
  char *bufferCursor = 0;
  qint64 toWrite = 0;
  qint64 written;

  // Check if we have something to transmit
  if(pvWriteCurrentFrame == 0){
    pvWriteCurrentFrame = getNewFrameWrite();
    if(pvWriteCurrentFrame != 0){
      // New frame to transmit
      bufferCursor = pvWriteCurrentFrame->data();
      toWrite = pvWriteCurrentFrame->size();
    }
  }
  if(pvWriteCurrentFrame != 0){
    Q_ASSERT(bufferCursor != 0);
    // Write data to port
    written = pvPort->write(bufferCursor, toWrite);
    if(written < 0){
      emit(errorOccured(MDT_TCP_IO_ERROR));
      written = 0;
      // Sleep a bit to prevent crazy ressources consumtion
      msleep(100);
    }
    pvWriteCurrentFrame->take(written);
    // Check if current frame was completly sent
    if(pvWriteCurrentFrame->isEmpty()){
      pvPort->writeFramesPool().enqueue(pvWriteCurrentFrame);
      ///emit frameWritten();
      pvWriteCurrentFrame = 0;
    }else{
      bufferCursor = pvWriteCurrentFrame->data();
      Q_ASSERT(bufferCursor < (pvWriteCurrentFrame->data() + pvWriteCurrentFrame->size()));
      toWrite -= written;
      Q_ASSERT(toWrite >= 0);
    }
  }
}

mdtFrame *mdtTcpSocketThread::getNewFrameWrite()
{
  Q_ASSERT(pvPort != 0);

  mdtFrame *frame;

  // Check if there is a frame to transmit
  if(pvPort->writeFrames().size() < 1){
    return 0;
  }
  frame = pvPort->writeFrames().dequeue();
  Q_ASSERT(frame != 0);

  return frame;
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
  mdtError e(MDT_TCP_IO_ERROR, "Cannot (re-)connect to host (Max retry reached)" , mdtError::Error);
  MDT_ERROR_SET_SRC(e, "mdtTcpSocketThread");
  e.commit();

  return false;
}

void mdtTcpSocketThread::run()
{
  Q_ASSERT(pvPort != 0);

  mdtTcpSocket *socket;

  pvPort->lockMutex();

  // Init write frame
  pvWriteCurrentFrame = 0;
  // Get a RX frame
  pvReadCurrentFrame = getNewFrameRead();
  if(pvReadCurrentFrame == 0){
    mdtError e(MDT_TCP_IO_ERROR, "No frame available in read frames pool, stopping thread", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtTcpSocketThread");
    e.commit();
    pvRunning = false;
    pvPort->unlockMutex();
    return;
  }
  // Alloc the local buffer
  pvReadBufferSize = pvReadCurrentFrame->capacity();
  pvReadBuffer = new char[pvReadBufferSize];
  if(pvReadBuffer == 0){
    mdtError e(MDT_TCP_IO_ERROR, "Cannot allocate memory for local read buffer" , mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtTcpSocketThread");
    e.commit();
    pvRunning = false;
    pvReadBufferSize = 0;
    pvPort->unlockMutex();
    return;
  }
  //bufferCursor = buffer;
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

  // Run...
  while(1){
    // Read thread state
    if(!pvRunning){
      break;
    }
    // Check if a connection was requested
    if(pvPeerPort == 0){
      socket->waitForNewTransaction();
    }
    if(pvSocket->state() == QAbstractSocket::ConnectedState){
      // Wait on write ready event
      if(!pvPort->waitEventWriteReady()){
        emit(errorOccured(MDT_TCP_IO_ERROR));
        /// NOTE: sleep ?
      }
      // Read thread state
      if(!pvRunning){
        break;
      }
      // **********************
      // Event occured, send the data to port - Check timeout state first
      if(!pvPort->writeTimeoutOccured()){
        // Write data to send
        writeToSocket();
      }
      // Wait until data is available for read
      if(pvPort->waitForReadyRead() != mdtAbstractPort::NoError){
        emit(errorOccured(MDT_TCP_IO_ERROR));
        /// NOTE: sleep ?
      }
      // Read thread state
      if(!pvRunning){
        break;
      }
      // **********************
      // Event occured, get the data from port - Check timeout state first
      if(!pvPort->readTimeoutOccured()){
        // We received data , read it
        readFromSocket();
      }
    }else{
      // Try to (Re-)connect
      if(!reconnectToHost()){
        qDebug() << "TCP THD: SIGNAL: Cannot reconnect - Retry at next transaction ...";
        socket->waitForNewTransaction();
      }
    }
  }

  // Put current frame into pool
  if(pvReadCurrentFrame != 0){
    pvPort->readFramesPool().enqueue(pvReadCurrentFrame);
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
  // Free memory
  Q_ASSERT(pvReadBuffer != 0);
  delete pvReadBuffer;

  pvPort->unlockMutex();
}

