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

bool mdtUsbPortThread::isReader() const
{
  return true;
}

mdtAbstractPort::error_t mdtUsbPortThread::writeToPort(mdtUsbPort *port, mdtFrame *frame)
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
        return mdtAbstractPort::NoError;
      }else{
        return portError;
      }
    }
    // Wait on write ready event
    portError = port->waitEventWriteReady();
    if(portError != mdtAbstractPort::NoError){
      if(portError == mdtAbstractPort::WriteCanceled){
        // Restore frame to pool and return
        pvPort->writeFramesPool().enqueue(frame);
        return mdtAbstractPort::NoError;
      }else{
        return portError;
      }
    }
    // Transfer complete, check timeout state first, then see what was readen
    if(port->writeTimeoutOccured()){
      // Cannot write now, sleep some time and try later
      notifyError(mdtAbstractPort::WriteTimeout);
      port->unlockMutex();
      msleep(100);
      port->lockMutex();
    }else{
      // Write data to port
      emit ioProcessBegin();
      /// \todo Return port's error
      written = port->write(bufferCursor, toWrite);
      if(written < 0){
        return mdtAbstractPort::UnhandledError;
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

void mdtUsbPortThread::run()
{
  Q_ASSERT(pvPort != 0);

  int pollIntervall;
  bool readPollMode;
  int interframeTime = 0;
  mdtFrame *writeFrame = 0;
  mdtFrame *readFrame = 0;
  mdtAbstractPort::error_t portError;
  int n;
  mdtUsbPort *port;
  bool waitAnAnswer = true;
  int toRead = 0;
  int reconnectTimeout;
  int reconnectMaxRetry;
  /// \todo Dynamic buffer ??
  char messageInBuffer[1024];

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
  pollIntervall = 50;
  readPollMode = false;
  interframeTime = pvPort->config().writeInterframeTime();
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
    ///qDebug() << "USBTHD: running ...";
    // Read thread state
    if(!pvRunning){
      break;
    }
    // Check about message input (additional interrupt IN endpoint)
    n = port->readMessageIn(messageInBuffer, 1024);
    qDebug() << "USBTHD: message len: " << n;
    /// Prio interrupt request here ?
    
    /// Get a frame for write. In poll mode, we take one only if directly available, else, we block here until one is available
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
      ///qDebug() << "USBTHD: write frames size: " << port->writeFrames().size();
      writeFrame = getNewFrameWrite();
    }
    ///qDebug() << "USBTHD: write frame get DONE";
    // Read thread state
    if(!pvRunning){
      break;
    }
    // Check about control transfer
    portError = port->handleControlQueries();
    if(portError != mdtAbstractPort::NoError){
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
    waitAnAnswer = writeFrame->waitAnAnswer();
    // Write if something is to write (getNewFrameWrite() can return a null pointer 
    //  if it was woken after a control request or other).
    if(writeFrame != 0){
      portError = writeToPort(port, writeFrame);
      if(portError != mdtAbstractPort::NoError){
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
    // Check if a control response is available
    if(port->controlResponseFrames().size() > 0){
      qDebug() << "USBTHD: ******** CTL RESP !";
    }
    
    // Check about message input (additional interrupt IN endpoint)
    n = port->readMessageIn(messageInBuffer, 1024);
    qDebug() << "USBTHD: message len: " << n;
    
    ///qDebug() << "USBTHD: waitAnAnswer: " << waitAnAnswer;
    // Read process
    if(waitAnAnswer){
      if(readFrame == 0){
        break;
      }
      // See what we have to read
      toRead = readFrame->bytesToStore();
      while(toRead > 0){
        ///qDebug() << "USBTHD: toRead: " << toRead;
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
              notifyError(portError);
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
        // Wait until read transfer completed
        portError = port->waitForReadyRead();
        if(portError != mdtAbstractPort::NoError){
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
              notifyError(portError);
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
        // Check if a control response is available
        if(port->controlResponseFrames().size() > 0){
          qDebug() << "USBTHD: ******** CTL RESP !!!!!";
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
            toRead = readFrame->bytesToStore();
            ///qDebug() << "USBTHD: waiting more data, toRead: " << toRead;
          }else{
            ///qDebug() << "USBTHD: received all expected data";
            toRead = 0;
          }
        }else{
          notifyError(mdtAbstractPort::ReadTimeout);
        }
      }
    }
  }

  qDebug() << "USBTHD: cleanup ...";

  // Put current frame into pool
  if(readFrame != 0){
    port->readFramesPool().enqueue(readFrame);
  }

  qDebug() << "USBTHD: END";

  pvPort->unlockMutex();
}

