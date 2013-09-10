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
#include "mdtUsbPortThread.h"
#include "mdtPortThreadHelperPort.h"
#include "mdtUsbPort.h"
#include "mdtFrameUsbControl.h"
#include "mdtError.h"
#include <QApplication>

//#include <QDebug>

mdtUsbPortThread::mdtUsbPortThread(QObject *parent)
 : mdtPortThread(parent)
{
}

bool mdtUsbPortThread::isReader() const
{
  return true;
}

bool mdtUsbPortThread::isWriter() const
{
  return true;
}

bool mdtUsbPortThread::handlesTimeout() const
{
  return true;
}

mdtAbstractPort::error_t mdtUsbPortThread::waitOnControlTransferPossible(bool notifyOnControlTransferComplete)
{
  Q_ASSERT(pvPort != 0);

  mdtAbstractPort::error_t portError;
  QList<mdtAbstractPort::error_t> errors;
  int i;
  mdtUsbPort *port = dynamic_cast<mdtUsbPort*>(pvPort);
  Q_ASSERT(port != 0);

  while(port->controlFramesPool().size() < 1){
    // Check about stoping
    if(!pvRunning){
      return mdtAbstractPort::NoError;
    }
    portError = port->handleUsbEvents();
    if(portError != mdtAbstractPort::NoError){
      return portError;
    }
    // Check if transfer callback reported a error
    errors = port->lastErrors();
    for(i=0; i<errors.size(); i++){
      portError = errors.at(i);
      if(portError != mdtAbstractPort::NoError){
        return portError;
      }
    }
    // Check if a control transfer finished
    if(port->controlResponseFrames().size() > 0){
      // If we notify port manager, we have to unlock port mutext sometimes
      if(notifyOnControlTransferComplete){
        emit(controlResponseReaden());
        port->unlockMutex();
        msleep(50);
        port->lockMutex();
      }
    }else{
      // Here, we hope that caller knows what hi is doing - simply restore frame to pool
      mdtError e(MDT_USB_IO_ERROR, "Loosing a control transfer response frame", mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtUsbPortThread");
      e.commit();
      port->controlFramesPool().enqueue(port->controlResponseFrames().dequeue());
    }
  }
  Q_ASSERT(port->controlFramesPool().size() > 0);

  return mdtAbstractPort::NoError;
}

mdtAbstractPort::error_t mdtUsbPortThread::sendControlQuery(mdtFrameUsbControl *ctlFrame)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(ctlFrame != 0);

  mdtAbstractPort::error_t portError;
  QList<mdtAbstractPort::error_t> errors;
  int i;
  mdtUsbPort *port = dynamic_cast<mdtUsbPort*>(pvPort);
  Q_ASSERT(port != 0);

  // Submit the control request and wait until transfer is done
  port->addControlRequest(ctlFrame);
  while(port->controlResponseFrames().size() < 1){
    // Check about stoping
    if(!pvRunning){
      return mdtAbstractPort::NoError;
    }
    portError = port->handleUsbEvents();
    if(portError != mdtAbstractPort::NoError){
      return portError;
    }
    // Check if transfer callback reported a error
    errors = port->lastErrors();
    for(i=0; i<errors.size(); i++){
      portError = errors.at(i);
      if(portError != mdtAbstractPort::NoError){
        return portError;
      }
    }
  }

  return mdtAbstractPort::NoError;
}

mdtAbstractPort::error_t mdtUsbPortThread::readUntilShortPacketReceived(int maxReadTransfers)
{
  Q_ASSERT(pvPort != 0);

  mdtAbstractPort::error_t portError;
  QList<mdtAbstractPort::error_t> errors;
  mdtUsbPort *port = dynamic_cast<mdtUsbPort*>(pvPort);
  Q_ASSERT(port != 0);
  char *buffer = new char[port->readBufferSize()];
  int readen;
  int i;

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
    errors = port->lastErrors();
    for(i=0; i<errors.size(); i++){
      if(errors.at(i) != mdtAbstractPort::NoError){
        delete[] buffer;
        return portError;
      }
    }
    readen = port->read(buffer, port->readBufferSize());
    ///qDebug() << "mdtUsbPortThread::readUntilShortPacketReceived() , readen: " << readen;
    if(readen < 0){
      delete[] buffer;
      qDebug() << "mdtUsbPortThread::readUntilShortPacketReceived() , error " << readen;
      return (mdtAbstractPort::error_t)readen;
    }
    if(readen < port->readBufferSize()){
      ///qDebug() << "mdtUsbPortThread::readUntilShortPacketReceived() , END , Ok";
      delete[] buffer;
      return mdtAbstractPort::NoError;
    }
    maxReadTransfers--;
  }
  delete[] buffer;
  mdtError e(MDT_USB_IO_ERROR, "No short packet received after maxReadTransfers", mdtError::Error);
  MDT_ERROR_SET_SRC(e, "mdtUsbPortThread");
  e.commit();

  return mdtAbstractPort::ReadTimeout;
}

void mdtUsbPortThread::run()
{
  Q_ASSERT(pvPort != 0);

  mdtAbstractPort::error_t portError = mdtAbstractPort::NoError;
  int n;
  int i;
  mdtUsbPort *port;
  bool waitAnAnswer = false;
  qint64 written;
  QList<mdtAbstractPort::error_t> errors;
  mdtPortThreadHelperPort threadHelper;
  threadHelper.setPort(pvPort);
  threadHelper.setThread(this);

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
  // Get a frame for read
  if(!threadHelper.getNewFrameRead()){
    pvRunning = false;
    notifyError(mdtAbstractPort::Disconnected);
    return;
  }
  // Notify that we are ready
  ///notifyError(mdtAbstractPort::NoError);
  emit ready(this);

  // Run...
  while(1){
    // Read thread state
    if(!pvRunning){
      break;
    }
    portError = port->handleUsbEvents();
    // Check about event handling errors
    if(portError != mdtAbstractPort::NoError){
      // Check about stoping
      if(!pvRunning){
        break;
      }
      portError = threadHelper.handleCommonReadWriteErrors(portError);
      if(portError != mdtAbstractPort::ErrorHandled){
        // Unhandled error - stop
        break;
      }
      // Error could be handled - go back IDLE
      continue;
    }
    // Check about port errors
    errors = port->lastErrors();
    portError = mdtAbstractPort::NoError;
    for(i=0; i<errors.size(); ++i){
      // Check about stoping
      if(!pvRunning){
        break;
      }
      portError = errors.at(i);
      if((portError == mdtAbstractPort::ControlCanceled)||(portError == mdtAbstractPort::ControlTimeout)){
        // mdtUsbPort does the job, we just have to notify the error
        notifyError(mdtAbstractPort::ControlCanceled);
      }else{
        portError = threadHelper.handleCommonReadWriteErrors(portError);
        if(portError != mdtAbstractPort::ErrorHandled){
          // Unhandled error - stop
          pvRunning = false;
          break;
        }
      }
    }
    // Check about stoping
    if(!pvRunning){
      break;
    }
    // If we had a error, go back IDLE
    if(portError == mdtAbstractPort::ErrorHandled){
      continue;
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
      portError = threadHelper.handleCommonReadWriteErrors(portError);
      if(portError != mdtAbstractPort::ErrorHandled){
        // Unhandled error - stop
        break;
      }
      // Error could be handled - go back IDLE
      continue;
    }
    // Check about write endpoint flush
    if((threadHelper.currentWriteFrame() != 0)&&(port->flushOutRequestPending())){
      threadHelper.restoreCurrentWriteFrameToPool();
    }else{
      // Check if we have something to write (we not want to block here)
      if(threadHelper.currentWriteFrame() == 0){
        if(port->writeFrames().size() > 0){
          threadHelper.setCurrentWriteFrame(port->writeFrames().dequeue());
          Q_ASSERT(threadHelper.currentWriteFrame() != 0);
          emit(writeProcessBegin());
        }
      }
    }
    // Write ...
    if(threadHelper.currentWriteFrame() != 0){
      // Write (will simply do nothing and return 0 if transfer is pending)
      written = threadHelper.writeDataToPort(-1);
      if(written < 0){
        // Check about stoping
        if(!pvRunning){
          break;
        }
        portError = threadHelper.handleCommonWriteErrors((mdtAbstractPort::error_t)written);
        if(portError != mdtAbstractPort::ErrorHandled){
          // Unhandled error - stop
          break;
        }
        // Error could be handled - go back IDLE
        continue;
      }
      // Check if frame was completly written
      if(threadHelper.currentWriteFrame()->isEmpty()){
        // Update waitAnAnswer flag
        waitAnAnswer = threadHelper.currentWriteFrame()->waitAnAnswer();
        // Restore frame to pool
        threadHelper.restoreCurrentWriteFrameToPool();
        // Check if a new frame is to write
        if(port->writeFrames().size() > 0){
          threadHelper.setCurrentWriteFrame(port->writeFrames().dequeue());
          Q_ASSERT(threadHelper.currentWriteFrame() != 0);
        }
      }
      // Here, if frame is not Null, we have to init a new transfer
      if(threadHelper.currentWriteFrame() != 0){
        // Init a new write transfer (will only init if not pending)
        portError = port->initWriteTransfer(threadHelper.currentWriteFrame()->data(), threadHelper.currentWriteFrame()->size());
        if(portError != mdtAbstractPort::NoError){
          // Check about stoping
          if(!pvRunning){
            break;
          }
          portError = threadHelper.handleCommonWriteErrors(portError);
          if(portError != mdtAbstractPort::ErrorHandled){
            // Unhandled error - stop
            break;
          }
          // Error could be handled - go back IDLE
          continue;
        }
      }
    }
    Q_ASSERT(threadHelper.currentReadFrame() != 0);
    // Check about read endpoint flush
    if(port->flushInRequestPending()){
      threadHelper.restoreCurrentReadFrameToPool();
      if(!threadHelper.getNewFrameRead()){
        break;
      }
      // We do nothing else, return idle
      continue;
    }
    // read/store available data
    n = threadHelper.readFromPort(true);
    if(n < 0){
      // Unhandled error: notify and stop
      portError = (mdtAbstractPort::error_t)n;
      notifyError(portError);
      break;
    }
    // Reset waitAnAnswer flag if a frame was received
    if(n > 0){
      waitAnAnswer = false;
    }
    // Two conditions to init a read transfer:
    // - A query/reply is pending (waitAnAnswer)
    // - Current read frame is not complete
    if((waitAnAnswer)&&(n == 0)){
      // Init a new read transfer (will only init if not pending)
      portError = port->initReadTransfer(threadHelper.currentReadFrame()->bytesToStore());
      if(portError != mdtAbstractPort::NoError){
        // Check about stoping
        if(!pvRunning){
          break;
        }
        portError = threadHelper.handleCommonReadErrors(portError);
        if(portError != mdtAbstractPort::ErrorHandled){
          // Unhandled error - stop
          break;
        }
      }
    }
  }

  port->cancelTransfers();

  pvRunning = false;
  /**
  if(portError == mdtAbstractPort::NoError){
    notifyError(mdtAbstractPort::Disconnected);
  }
  */
  pvPort->unlockMutex();
}

