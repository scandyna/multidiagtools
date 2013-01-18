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
#include "mdtUsbPortThread.h"
#include "mdtUsbPort.h"
#include "mdtError.h"
#include <QApplication>

#include <QDebug>

mdtUsbPortThread::mdtUsbPortThread(QObject *parent)
 : mdtPortThread(parent)
{
}

/**
void mdtUsbPortThread::stop()
{
  Q_ASSERT(pvPort != 0);

  // We need a pointer to mdtUsbPort
  mdtUsbPort *port = dynamic_cast<mdtUsbPort*> (pvPort);
  Q_ASSERT(port != 0);

  port->abortFrameToWriteWait();
  port->cancelTransfers();
  mdtPortThread::stop();
}
*/

bool mdtUsbPortThread::isReader() const
{
  return true;
}

mdtAbstractPort::error_t mdtUsbPortThread::readUntilShortPacketReceived(int maxReadTransfers)
{
  Q_ASSERT(pvPort != 0);

  mdtAbstractPort::error_t portError;
  mdtUsbPort *port = dynamic_cast<mdtUsbPort*>(pvPort);
  Q_ASSERT(port != 0);
  char *buffer = new char[port->readBufferSize()];
  int readen;

  while(maxReadTransfers > 0){
    portError = port->initReadTransfer(port->readBufferSize());
    if(portError != mdtAbstractPort::NoError){
      delete[] buffer;
      return portError;
    }
    portError = port->handleUsbEvents();
    if(portError != mdtAbstractPort::NoError){
      delete[] buffer;
      return portError;
    }
    readen = port->read(buffer, port->readBufferSize());
    qDebug() << "-*-* mdtUsbPortThread::readUntilShortPacketReceived() , readen: " << readen;
    if(readen < 0){
      delete[] buffer;
      return (mdtAbstractPort::error_t)readen;
    }
    if(readen < port->readBufferSize()){
      delete[] buffer;
      return mdtAbstractPort::NoError;
    }
    maxReadTransfers--;
  }
  delete[] buffer;

  return mdtAbstractPort::ReadTimeout;
}

/**
mdtFrame *mdtUsbPortThread::getNewFrameWrite()
{
  Q_ASSERT(pvPort != 0);

  return pvPort->getFrameToWrite();
}
*/

/**
mdtAbstractPort::error_t mdtUsbPortThread::writeToPort(mdtUsbPort *port, mdtFrame *frame, int maxWriteTry)
{
  Q_ASSERT(port != 0);
  Q_ASSERT(frame != 0);

  char *bufferCursor = 0;
  qint64 toWrite = 0;
  qint64 written = 0;
  mdtAbstractPort::error_t portError;

  // Write the frame to port
  bufferCursor = frame->data();
  toWrite = frame->size();
  while(toWrite > 0){
    // Init transfert
    portError = port->initWriteTransfer(bufferCursor, toWrite);
    if(portError != mdtAbstractPort::NoError){
      if(portError == mdtAbstractPort::WriteCanceled){
        // Restore frame to pool and return
        pvPort->writeFramesPool().enqueue(frame);
        if(pvRunning){
          notifyError(mdtAbstractPort::WriteCanceled);
        }
        return mdtAbstractPort::NoError;
      }else{
        return portError;
      }
    }
    // Wait on write ready event
    portError = port->waitEventWriteReady();
    // Check about flush request
    if(pvPort->flushOutRequestPending()){
      // Restore frame to pool and return
      qDebug() << "USBPTHD write: flush out request";
      pvPort->writeFramesPool().enqueue(frame);
      return mdtAbstractPort::NoError;
    }
    if(portError != mdtAbstractPort::NoError){
      if(portError == mdtAbstractPort::WriteCanceled){
        // Restore frame to pool and return
        pvPort->writeFramesPool().enqueue(frame);
        if(pvRunning){
          notifyError(mdtAbstractPort::WriteCanceled);
        }
        return mdtAbstractPort::NoError;
      }else{
        return portError;
      }
    }
    // Transfer complete, check timeout state first, then see what was readen
    if(port->writeTimeoutOccured()){
      if(maxWriteTry <= 0){
        // Notify error and restore frame into write pool
        mdtError e(MDT_PORT_IO_ERROR, "Max write try reached after write timeout", mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtUsbPortThread");
        e.commit();
        if(pvRunning){
          notifyError(mdtAbstractPort::WriteTimeout);
        }
        pvPort->writeFramesPool().enqueue(frame);
        // Thread has nothing else to do
        return mdtAbstractPort::NoError;
      }
      // Cannot write now, sleep some time and try later
      port->unlockMutex();
      msleep(100);
      port->lockMutex();
      maxWriteTry--;
    }else{
      // Write data to port
      emit ioProcessBegin();
      written = port->write(bufferCursor, toWrite);
      if(written < 0){
        return (mdtAbstractPort::error_t)written;
      }
      frame->take(written);
      // Update cursor and toWrite
      if(frame->isEmpty()){
        toWrite = 0;
      }else{
        bufferCursor = frame->data();
        Q_ASSERT(bufferCursor < (frame->data() + frame->size()));
        toWrite -= written;
        Q_ASSERT(toWrite >= 0);
      }
    }
  }
  // Here, frame is completly sent
  Q_ASSERT(frame->isEmpty());
  port->writeFramesPool().enqueue(frame);

  return mdtAbstractPort::NoError;
}
*/

void mdtUsbPortThread::run()
{
  Q_ASSERT(pvPort != 0);

  ///int pollIntervall;
  ///bool readPollMode;
  ///int interframeTime = 0;
  mdtFrame *writeFrame = 0;
  mdtFrame *readFrame = 0;
  mdtAbstractPort::error_t portError;
  int n;
  int i;
  mdtUsbPort *port;
  bool waitAnAnswer = false;
  ///int toRead = 0;
  int reconnectTimeout;
  int reconnectMaxRetry;
  qint64 written;
  QList<mdtAbstractPort::error_t> errors;

  pvPort->lockMutex();
#ifdef Q_OS_UNIX
  pvNativePthreadObject = pthread_self();
  Q_ASSERT(pvNativePthreadObject != 0);
#endif
  // We need a mdtUsbPort object here
  port = dynamic_cast<mdtUsbPort*> (pvPort);
  Q_ASSERT(port != 0);
  // Set the running flag
  pvRunning = true;
  /// Get setup \todo Take from config, or something
  ///pollIntervall = 50;
  ///readPollMode = false;
  ///interframeTime = pvPort->config().writeInterframeTime();
  reconnectTimeout = 5000;
  reconnectMaxRetry = 5;
  // Get a frame for read
  readFrame = getNewFrameRead();
  if(readFrame == 0){
    return;
  }
  // Notify that we are ready
  notifyError(mdtAbstractPort::NoError);

  // Run...
  while(1){
    // Read thread state
    if(!pvRunning){
      break;
    }
    qDebug() << "USBPTHD: going idle ...";
    portError = port->handleUsbEvents();
    qDebug() << "USBPTHD: event !";
    // Check about event handling errors
    if(portError != mdtAbstractPort::NoError){
      // Check about stoping
      if(!pvRunning){
        break;
      }
      if(portError == mdtAbstractPort::Disconnected){
        // Try to reconnect
        portError = reconnect(reconnectTimeout, reconnectMaxRetry, true);
        if(portError != mdtAbstractPort::NoError){
          // Stop
          pvRunning = false;
          break;
        }
      }else{
        // Errors that must be signaled + stop the thread
        notifyError(portError);
        pvRunning = false;
        break;
      }
    }
    // Check about port errors
    errors = port->lastErrors();
    for(i=0; i<errors.size(); i++){
      /// \todo Handle errors
      qDebug() << "USBPTHD: error Nb " << errors.at(i);
    }
    // Check if a control response is available
    if(port->controlResponseFrames().size() > 0){
      emit(controlResponseReaden());
    }
    // Check if a message IN is available
    if(port->messageInFrames().size() > 0){
      emit(messageInReaden());
    }
    // Init a new message IN transfert (will only init if needed)
    portError = port->initMessageInTransfer();
    if(portError != mdtAbstractPort::NoError){
      // Check about stoping
      if(!pvRunning){
        break;
      }
      if(portError == mdtAbstractPort::Disconnected){
        // Try to reconnect
        portError = reconnect(reconnectTimeout, reconnectMaxRetry, true);
        if(portError != mdtAbstractPort::NoError){
          // Stop
          pvRunning = false;
          break;
        }
      }else{
        // Errors that must be signaled + stop the thread
        notifyError(portError);
        pvRunning = false;
        break;
      }
    }
    
    
    // Check about writeFrame
    /**
    if(writeFrame == 0){
      if(port->writeFrames().size() > 0){
        writeFrame = getNewFrameWrite();
      }else{
        writeFrame = 0;
      }
    }
    */
    // Check if we have something to write
    if(writeFrame == 0){
      if(port->writeFrames().size() > 0){
        writeFrame = port->writeFrames().dequeue();
        Q_ASSERT(writeFrame != 0);
      }
    }
    // Set waitAnAnswer flag false (avoid set it for each write error)
    ///waitAnAnswer = false;
    // Write ...
    if(writeFrame != 0){
      qDebug() << "USBPTHD: to write: " << writeFrame->size();
      // Write (will simply do nothing and return 0 if transfer is pending)
      written = writeDataToPort(writeFrame);
      if(written < 0){
        // Check about stoping
        if(!pvRunning){
          break;
        }
        portError = (mdtAbstractPort::error_t)written;
        // Check about disconnection
        if(portError == mdtAbstractPort::Disconnected){
          // Try to reconnect
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
      // Check if frame was completly written
      if(writeFrame->isEmpty()){
        // Update waitAnAnswer flag
        waitAnAnswer = writeFrame->waitAnAnswer();
        // Restore frame to pool
        port->writeFramesPool().enqueue(writeFrame);
        // Check if a new frame is to write
        if(port->writeFrames().size() > 0){
          writeFrame = port->writeFrames().dequeue();
          Q_ASSERT(writeFrame != 0);
        }else{
          writeFrame = 0;
        }
      }
      // Here, if frame is not Null, we have to init a new transfer
      if(writeFrame != 0){
        // Init a new write transfer (will only init if not pending)
        portError = port->initWriteTransfer(writeFrame->data(), writeFrame->size());
        if(portError != mdtAbstractPort::NoError){
          // Check about stoping
          if(!pvRunning){
            break;
          }
          // Check about disconnection
          if(portError == mdtAbstractPort::Disconnected){
            // Try to reconnect
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
      }
    }
    // Check about device flush
    if(port->readUntilShortPacketReceivedRequestPending()){
      portError = readUntilShortPacketReceived(100);
      if(portError != mdtAbstractPort::NoError){
        // Check about stoping
        if(!pvRunning){
          break;
        }
        // Check about disconnection
        if(portError == mdtAbstractPort::Disconnected){
          // Try to reconnect
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
    }
    // read/store available data
    if(readFrame == 0){
      if(pvRunning){
        mdtError e(MDT_USB_IO_ERROR, "readFrame is Null, aborting. This is a bug", mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtUsbPortThread");
        e.commit();
        notifyError(mdtAbstractPort::UnhandledError);
      }
      break;
    }
    n = -1;
    if(!port->readTimeoutOccured()){
      n = readFromPort(&readFrame);
      if(n < 0){
        // Unhandled error: notify and stop
        notifyError(n);
        break;
      }
      // Reset waitAnAnswer flag if a frame was received
      if(n > 0){
        waitAnAnswer = false;
      }
    }else{
      notifyError(mdtAbstractPort::ReadTimeout);
    }
    qDebug() << "USBPTHD: waitAnAnswer: " << waitAnAnswer << " , n: " << n;
    // Two conditions to init a read transfer:
    // - A query/reply is pending (waitAnAnswer)
    // - Current read frame is not complete
    if((waitAnAnswer)&&(n == 0)){
      // Init a new read transfer (will only init if not pending)
      qDebug() << "USBPTHD: to read: " << readFrame->bytesToStore();
      portError = port->initReadTransfer(readFrame->bytesToStore());
      if(portError != mdtAbstractPort::NoError){
        // Check about stoping
        if(!pvRunning){
          break;
        }
        if(portError == mdtAbstractPort::ReadCanceled){
          // We submit the uncomplete frame
          pvPort->readenFrames().enqueue(readFrame);
          emit newFrameReaden();
          qDebug() << "mdtUsbPortThread::run(): read cancel";
          readFrame = getNewFrameRead();
          break;
        }else if(portError == mdtAbstractPort::Disconnected){
          // Try to reconnect
          portError = reconnect(reconnectTimeout, reconnectMaxRetry, true);
          if(portError != mdtAbstractPort::NoError){
            // Stop
            pvRunning = false;
            break;
          }
        }else{
          // Errors that must be signaled + stop the thread
          notifyError(portError);
          pvRunning = false;
          break;
        }
      }
    }

    
    /// Get a frame for write. In poll mode, we take one only if directly available, else, we block here until one is available
    /**
    if(readPollMode){
      port->unlockMutex();
      msleep(pollIntervall);
      port->lockMutex();
      qDebug() << "USBTHD: write frames size: " << port->writeFrames().size();
      if(port->writeFrames().size() > 0){
        writeFrame = getNewFrameWrite();
      }else{
        writeFrame = 0;
      }
    }else{
      qDebug() << "USBPTHD: waiting ...";
      writeFrame = getNewFrameWrite();
    }
    // Read thread state
    if(!pvRunning){
      break;
    }
    // Wait for interframe time if needed
    /// \todo Is this usefull in USB ?
    if(interframeTime > 0){
      pvPort->unlockMutex();
      msleep(interframeTime);
      pvPort->lockMutex();
      // Read thread state
      if(!pvRunning){
        break;
      }
    }
    */
    // Write if something is to write (getNewFrameWrite() can return a null pointer 
    //  if it was woken after a control request or other).
    /**
    if(writeFrame != 0){
      qDebug() << "USBPTHD: writing ...";
      waitAnAnswer = writeFrame->waitAnAnswer();
      portError = writeToPort(port, writeFrame);
      if(portError != mdtAbstractPort::NoError){
        // Check about stoping
        if(!pvRunning){
          break;
        }
        // Check about disconnection
        if(portError == mdtAbstractPort::Disconnected){
          // Try to reconnect
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
    }else{
      // We were woken up because of a control request
      waitAnAnswer = false;
      while(port->controlResponseFrames().size() < 1){
        // It can happen that timeout is not detected (by stoping)
        if(!pvRunning){
          break;
        }
        // Handle events
        qDebug() << "USBPTHD: handleUsbEvents ...";
        portError = port->handleUsbEvents(0, 0);
        qDebug() << "USBPTHD: handleUsbEvents DONE, retval: " << portError;
        if(portError != mdtAbstractPort::NoError){
          // Check about stoping
          if(!pvRunning){
            break;
          }
          // Check about disconnection
          if(portError == mdtAbstractPort::Disconnected){
            // Try to reconnect
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
      }
    }
    */
    // Read process
    /**
    if(waitAnAnswer || port->singleReadTransferRequestPending()){
      if(readFrame == 0){
        break;
      }
      // See what we have to read
      if(port->singleReadTransferRequestPending()){
        toRead = port->readBufferSize();
      }else{
        toRead = readFrame->bytesToStore();
      }
      while(toRead > 0){
        qDebug() << "USBPTHD: reading ... (toRead: " << toRead << ")";
        if(readFrame == 0){
          pvRunning = false;
          break;
        }
        // Read thread state
        if(!pvRunning){
          break;
        }
        // Init a read request
       
        portError = port->initReadTransfer(toRead);
        if(portError != mdtAbstractPort::NoError){
          // Check about stoping
          if(!pvRunning){
            break;
          }
          if(portError == mdtAbstractPort::ReadCanceled){
            // We submit the uncomplete frame
            pvPort->readenFrames().enqueue(readFrame);
            emit newFrameReaden();
            qDebug() << "mdtUsbPortThread::run(): read cancel";
            readFrame = getNewFrameRead();
            break;
          }else if(portError == mdtAbstractPort::Disconnected){
            // Try to reconnect
            portError = reconnect(reconnectTimeout, reconnectMaxRetry, true);
            if(portError != mdtAbstractPort::NoError){
              // Errors that must be signaled + stop the thread
              ///notifyError(portError);
              // Stop
              pvRunning = false;
              break;
            }
          }else{
            // Errors that must be signaled + stop the thread
            notifyError(portError);
            pvRunning = false;
            break;
          }
        }
       
        // Read thread state
        if(!pvRunning){
          break;
        }

        // Check if a control response is available
        if(port->controlResponseFrames().size() > 0){
          emit(controlResponseReaden());
        }
        // Check if a message IN is available
        if(port->messageInFrames().size() > 0){
          emit(messageInReaden());
        }
        // Read thread state
        if(!pvRunning){
          break;
        }
        // read/store
        if(!port->readTimeoutOccured()){
          n = readFromPort(&readFrame);
          if(n < 0){
            // Unhandled error: notify and stop
            notifyError(n);
            pvRunning = false;
            break;
          }
          Q_ASSERT(readFrame != 0);
          // If no frame was complete, it's possible that more data is needed
          if(n == 0){
            if(port->singleReadTransferRequestPending()){
              toRead = 0;
              // We submit the uncomplete frame
              pvPort->readenFrames().enqueue(readFrame);
              emit newFrameReaden();
              readFrame = getNewFrameRead();
            }else{
              toRead = readFrame->bytesToStore();
            }
          }else{
            toRead = 0;
          }
        }else{
          notifyError(mdtAbstractPort::ReadTimeout);
        }
        port->resetSingleReadTransferRequest();
      }
    }
    */
  }

  qDebug() << "USBTHD: cleanup ...";
  port->cancelTransfers();

  // Finish event handling
  /**
  struct timeval finalTimeout;
  finalTimeout.tv_sec = 5;
  finalTimeout.tv_usec = 0;
  portError = port->handleUsbEvents(&finalTimeout, 0);
  if(portError != mdtAbstractPort::NoError){
    notifyError(portError);
  }
  */

  // Put current frame into pool
  if(readFrame != 0){
    port->readFramesPool().enqueue(readFrame);
  }

  qDebug() << "USBTHD: END";

  pvPort->unlockMutex();
  notifyError(mdtAbstractPort::Disconnected);
}

