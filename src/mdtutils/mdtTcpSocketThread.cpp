
#include "mdtTcpSocketThread.h"
#include "mdtError.h"
#include <QApplication>

#include <QDebug>

mdtTcpSocketThread::mdtTcpSocketThread(QObject *parent)
 : mdtPortThread(parent)
{
  pvReadBuffer = 0;
  pvReadBufferSize = 0;
  pvSocket = 0;
  pvPeerPort = 0;
  // Diseable auto reconnections
  pvMaxReconnect = 10;
}

void mdtTcpSocketThread::stop()
{
  Q_ASSERT(pvPort != 0);

  // Unset the running flag
  pvPort->lockMutex();
  pvRunning = false;
  pvPort->unlockMutex();
  // Wake up
  pvMutex.lock();
  pvNewTransaction.wakeAll();
  pvMutex.unlock();

  // Wait the end of the thread
  while(!isFinished()){
    qApp->processEvents();
    msleep(50);
  }
}

void mdtTcpSocketThread::connectToHost(QString hostName, quint16 hostPort)
{
  pvMutex.lock();
  pvPeerName = hostName;
  pvPeerPort = hostPort;
  pvNewTransaction.wakeAll();
  pvMutex.unlock();
}

void mdtTcpSocketThread::beginNewTransaction()
{
  pvMutex.lock();
  pvNewTransaction.wakeAll();
  qDebug() << "New transaction !";
  pvMutex.unlock();
}

void mdtTcpSocketThread::readFromSocket()
{
  char *bufferCursor;
  qint64 toStore;
  qint64 readen;
  qint64 stored;

  pvPort->lockMutex();
  // Reset bufferCursor
  bufferCursor = pvReadBuffer;
  // Read data from port
  readen = pvSocket->read(pvReadBuffer, pvReadBufferSize);
  if(readen < 0){
    readen = 0;
    emit(errorOccured(MDT_TCP_IO_ERROR));
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
      pvPort->unlockMutex();
      // emit a Readen frame signal if complete
      if(pvReadCurrentFrame->isComplete()){
        emit newFrameReaden();
      }
      pvReadCurrentFrame = getNewFrameRead();
    }
    toStore = toStore - stored;
    // When frame becomes full and EOF seq was not reached, stored will be to big
    // We simply look that toStore is never < 0
    if(toStore < 0){
      toStore = 0;
    }
    bufferCursor = bufferCursor + stored;
    Q_ASSERT(bufferCursor < (pvReadBuffer + pvReadBufferSize));
  }
  pvPort->unlockMutex();
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
  char *bufferCursor;
  qint64 toWrite;
  qint64 written;

  pvPort->lockMutex();
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
    // Write data to port
    written = pvSocket->write(bufferCursor, toWrite);
    if(written < 0){
      emit(errorOccured(MDT_TCP_IO_ERROR));
      written = 0;
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
  pvPort->unlockMutex();
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
  int counter = pvMaxReconnect + 1;
  int timeout = 500;  // Initial timeout

  // If we are connected, return simply
  if(pvSocket->state() == QAbstractSocket::ConnectedState){
    return true;
  }
  // If a connection is about to be established, wait and return
  if((pvSocket->state() == QAbstractSocket::HostLookupState)||(pvSocket->state() == QAbstractSocket::ConnectingState)){
    if(!pvSocket->waitForConnected(timeout)){
      mdtError e(MDT_TCP_IO_ERROR, "Unable to reconnect to host", mdtError::Error);
      e.setSystemError(pvSocket->error(), pvSocket->errorString());
      MDT_ERROR_SET_SRC(e, "mdtTcpSocketThread");
      e.commit();
      return false;
    }
    return true;
  }
  // Here, we can be disconnecting or disconnected , wait to be shure
  if(pvSocket->state() != QAbstractSocket::UnconnectedState){
    if(!pvSocket->waitForDisconnected(timeout)){
      mdtError e(MDT_TCP_IO_ERROR, "waitForDisconnected() failed", mdtError::Error);
      e.setSystemError(pvSocket->error(), pvSocket->errorString());
      MDT_ERROR_SET_SRC(e, "mdtTcpSocketThread");
      e.commit();
      return false;
    }
  }
  // Ok, try reconnect
  while(counter > 0){
    pvMutex.lock();
    pvSocket->connectToHost(pvPeerName, pvPeerPort);
    pvMutex.unlock();
    qDebug() << "Try connecting ...";
    if(pvSocket->waitForConnected(timeout)){
      return true;
    }
    timeout += 100;
    counter--;
  }
  // Here , the maximum retry is reached
  mdtError e(MDT_TCP_IO_ERROR, "Cannot (re-)connect to host (Max retry reached" , mdtError::Error);
  MDT_ERROR_SET_SRC(e, "mdtTcpSocketThread");
  e.commit();

  return false;
}

void mdtTcpSocketThread::run()
{
  Q_ASSERT(pvPort != 0);

  qDebug() << "TCP THD: start ...";

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
  // Clear transactions list
  pvTransactionIds.clear();
  // Set the running flag
  pvRunning = true;
  pvPort->unlockMutex();

  /// NOTE: essai
  pvTransactionIds.enqueue(25);
  
  // Run...
  while(1){
    // If we have nothing to write, and no pending transaction, go sleep
    pvPort->lockMutex();
    pvMutex.lock();
    if((pvTransactionIds.size() < 1)&&(pvSocket->bytesToWrite() < 1)){
      qDebug() << "TCP THD: No transaction, wait ...";
      pvPort->unlockMutex();
      pvNewTransaction.wait(&pvMutex);
      pvPort->lockMutex();
    }
    pvMutex.unlock();
    // Read thread state
    if(!pvRunning){
      pvPort->unlockMutex();
      break;
    }
    pvPort->unlockMutex();
    msleep(100);  /// NOTE: provisiore
    // (Re-)connect to host
    if(pvSocket->state() != QAbstractSocket::ConnectedState){
      if(!reconnectToHost()){
        qDebug() << "TCP THD: SIGNAL: Cannot reconnect !";
        break;
      }
    }
    // Write data to send
    writeToSocket();
    qDebug() << "Have to write: " << pvSocket->bytesToWrite();
    // Wait until data was written
    if(pvSocket->bytesToWrite() > 0){
      if(!pvSocket->waitForBytesWritten(500)){  /// NOTE: timeout
        if((pvSocket->error() == QAbstractSocket::SocketTimeoutError)||(pvSocket->error() == QAbstractSocket::RemoteHostClosedError)){
          // On timeout or disconnected host, we retry later. Sleep a bit here to prevent crazy ressource consumption
          msleep(100);
        }else{
          // Unmanaged error, abort
          mdtError e(MDT_TCP_IO_ERROR, "waitForBytesWritten() failed - Stopping thread" , mdtError::Error);
          e.setSystemError(pvSocket->error(), pvSocket->errorString());
          e.commit();
          break;
        }
      }
    }
    // Wait until data is available for read
    if(!pvSocket->waitForReadyRead(500)){  /// NOTE: timeout
      if((pvSocket->error() == QAbstractSocket::SocketTimeoutError)||(pvSocket->error() == QAbstractSocket::RemoteHostClosedError)){
        // On timeout or disconnected host, we retry later. Sleep a bit here to prevent crazy ressource consumption
        msleep(100);
      }else{
        // Unmanaged error, abort
        mdtError e(MDT_TCP_IO_ERROR, "waitForReadyRead() failed - Stopping thread" , mdtError::Error);
        e.setSystemError(pvSocket->error(), pvSocket->errorString());
        e.commit();
        break;
      }
    }else{
      // We received data , read it
      readFromSocket();
      /// NOTE: transactions
      if(pvTransactionIds.size() > 0){
        pvTransactionIds.dequeue();
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
      if(!pvSocket->waitForDisconnected(1000)){
        qDebug() << "TCP THD: Cannot wait for disconnected: " << pvSocket->errorString();
      }
    }
  }

  
  // Free memory
  Q_ASSERT(pvReadBuffer != 0);
  delete pvReadBuffer;
  qDebug() << "TCP THD: END";
}

