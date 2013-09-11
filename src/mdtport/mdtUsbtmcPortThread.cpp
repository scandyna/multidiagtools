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
#include "mdtUsbtmcPortThread.h"
#include "mdtUsbPort.h"
#include "mdtError.h"
#include "mdtFrameUsbTmc.h"
#include <QApplication>

#include <QDebug>

mdtUsbtmcPortThread::mdtUsbtmcPortThread(QObject *parent)
 : mdtUsbPortThread(parent)
{
}

bool mdtUsbtmcPortThread::isReader() const
{
  return true;
}

bool mdtUsbtmcPortThread::isWriter() const
{
  return true;
}

mdtAbstractPort::error_t mdtUsbtmcPortThread::handleUsbtmcReadErrors(mdtAbstractPort::error_t portError, mdtPortThreadHelperPort &threadHelper)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(threadHelper.currentReadFrame() != 0);

  quint8 readFramebTag;
  bool readFrameIsEmpty;
  mdtFrameUsbTmc *usbtmcReadFrame = static_cast<mdtFrameUsbTmc*>(threadHelper.currentReadFrame());

  if(!pvRunning){
    mdtError e(MDT_PORT_IO_ERROR, "Non running thread wants to check about errors", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtPortThread");
    e.commit();
  }

  notifyError(portError); /// \todo Ok ??
  readFramebTag = usbtmcReadFrame->bTag();
  readFrameIsEmpty = usbtmcReadFrame->isEmpty();
  // We don't need this pointer anymore
  usbtmcReadFrame = 0;

  switch(portError){
    case mdtAbstractPort::Disconnected:
      threadHelper.restoreCurrentReadFrameToPool();
      // Try to reconnect
      portError = reconnect(true);
      if(portError != mdtAbstractPort::NoError){
        return mdtAbstractPort::UnhandledError;
      }
      // Abort current frame
      if((!readFrameIsEmpty)&&(readFramebTag > 0)){
        portError = abortBulkIn(readFramebTag, threadHelper);
        if(portError != mdtAbstractPort::NoError){
          if(portError == mdtAbstractPort::Disconnected){
            portError = reconnect(true);
            if(portError != mdtAbstractPort::NoError){
              return mdtAbstractPort::UnhandledError;
            }
          }else if(portError == mdtAbstractPort::UnhandledError){
            notifyError(mdtAbstractPort::UnhandledError);
            return mdtAbstractPort::UnhandledError;
          }else{
            notifyError(portError);
          }
        }
      }
      // Get a new read frame
      if(!threadHelper.getNewFrameRead()){
        return mdtAbstractPort::UnhandledError;
      }
      portError = mdtAbstractPort::ErrorHandled;
      break;
    case mdtAbstractPort::ReadCanceled:
      // Clear read frame
      threadHelper.currentReadFrame()->clear();
      threadHelper.currentReadFrame()->clearSub();
      // Abort current frame
      if((!readFrameIsEmpty)&&(readFramebTag > 0)){
        portError = abortBulkIn(readFramebTag, threadHelper);
        if(portError != mdtAbstractPort::NoError){
          if(portError == mdtAbstractPort::Disconnected){
            // Restore current read frame
            threadHelper.restoreCurrentReadFrameToPool();
            // Try to reconnect
            portError = reconnect(true);
            if(portError != mdtAbstractPort::NoError){
              return mdtAbstractPort::UnhandledError;
            }
            // Get a new read frame
            if(!threadHelper.getNewFrameRead()){
              return mdtAbstractPort::UnhandledError;
            }
          }else if(portError == mdtAbstractPort::UnhandledError){
            notifyError(mdtAbstractPort::UnhandledError);
            return mdtAbstractPort::UnhandledError;
          }else{
            notifyError(portError);
          }
        }
      }
      portError = mdtAbstractPort::ErrorHandled;
      break;
    case mdtAbstractPort::ReadTimeout:
      // Clear read frame
      threadHelper.currentReadFrame()->clear();
      threadHelper.currentReadFrame()->clearSub();
      // Abort current frame
      if((!readFrameIsEmpty)&&(readFramebTag > 0)){
        portError = abortBulkIn(readFramebTag, threadHelper);
        if(portError != mdtAbstractPort::NoError){
          if(portError == mdtAbstractPort::Disconnected){
            // Restore current read frame
            threadHelper.restoreCurrentReadFrameToPool();
            // Try to reconnect
            portError = reconnect(true);
            if(portError != mdtAbstractPort::NoError){
              return mdtAbstractPort::UnhandledError;
            }
            // Get a new read frame
            if(!threadHelper.getNewFrameRead()){
              return mdtAbstractPort::UnhandledError;
            }
          }else if(portError == mdtAbstractPort::UnhandledError){
            notifyError(mdtAbstractPort::UnhandledError);
            return mdtAbstractPort::UnhandledError;
          }else{
            notifyError(portError);
          }
        }
      }
      portError = mdtAbstractPort::ErrorHandled;
      break;
    case mdtAbstractPort::MessageInTimeout:
      portError = mdtAbstractPort::ErrorHandled;
      break;
    case mdtAbstractPort::MessageInCanceled:
      portError = mdtAbstractPort::ErrorHandled;
      break;
    default:
      portError = mdtAbstractPort::UnhandledError;
      break;
  }

  return portError;
}

/// \todo Implement !
mdtAbstractPort::error_t mdtUsbtmcPortThread::handleUsbtmcWriteErrors(mdtAbstractPort::error_t portError, mdtPortThreadHelperPort &threadHelper)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(threadHelper.currentWriteFrame() != 0);

  quint8 writeFramebTag;
  mdtFrameUsbTmc *usbtmcWriteFrame = static_cast<mdtFrameUsbTmc*>(threadHelper.currentWriteFrame());

  if(!pvRunning){
    mdtError e(MDT_PORT_IO_ERROR, "Non running thread wants to check about errors", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtPortThread");
    e.commit();
  }

  notifyError(portError); /// \todo Ok ??
  writeFramebTag = usbtmcWriteFrame->bTag();
  // We don't need this pointer anymore
  usbtmcWriteFrame = 0;

  switch(portError){
    case mdtAbstractPort::UsbWriteStall:
      notifyError(mdtAbstractPort::WriteCanceled);
      portError = abortBulkOut(writeFramebTag, threadHelper);
      // Restore write frame to pool
      threadHelper.restoreCurrentWriteFrameToPool();
      if(portError != mdtAbstractPort::NoError){
        if(portError == mdtAbstractPort::Disconnected){
          portError = reconnect(true);
          if(portError != mdtAbstractPort::NoError){
            notifyError(mdtAbstractPort::UnhandledError);
            return mdtAbstractPort::UnhandledError;
          }
        }
      }
      portError = mdtAbstractPort::ErrorHandled;
      break;
    default:
      portError = threadHelper.handleCommonWriteErrors(portError);
  }

  return portError;
}

/// \bug On disconnection, close/open will be called, and frames pools will be re-allocated again !
mdtAbstractPort::error_t mdtUsbtmcPortThread::handleUsbtmcReadWriteErrors(mdtAbstractPort::error_t portError, mdtPortThreadHelperPort &threadHelper)
{
  Q_ASSERT(pvPort != 0);

  if((threadHelper.currentReadFrame() != 0)&&(threadHelper.currentWriteFrame() != 0)){
    portError = handleUsbtmcReadErrors(portError, threadHelper);
    if(portError != mdtAbstractPort::ErrorHandled){
      portError = handleUsbtmcWriteErrors(portError, threadHelper);
    }
    return portError;
  }else if(threadHelper.currentReadFrame() != 0){
    return handleUsbtmcReadErrors(portError, threadHelper);
  }else{
    return handleUsbtmcWriteErrors(portError, threadHelper);
  }
}

mdtAbstractPort::error_t mdtUsbtmcPortThread::usbtmcWrite(mdtPortThreadHelperPort &threadHelper, bool *waitAnAnswer, QList<quint8> &expectedBulkInbTags)
{
  Q_ASSERT(waitAnAnswer != 0);
  Q_ASSERT(usbPort() != 0);

  qint64 written;
  mdtAbstractPort::error_t portError;

  // Check if we have something to write
  if(threadHelper.currentWriteFrame() == 0){
    if(usbPort()->writeFrames().size() < 1){
      return mdtAbstractPort::NoError;
    }
    threadHelper.setCurrentWriteFrame(usbPort()->writeFrames().dequeue());
  }
  Q_ASSERT(threadHelper.currentWriteFrame() != 0);
  emit(writeProcessBegin());
  // Write (will simply do nothing and return 0 if transfer is pending)
  written = threadHelper.writeDataToPort(-1);
  if(written < 0){
    // Check about stoping
    if(!pvRunning){
      return mdtAbstractPort::UnhandledError;
    }
    return handleUsbtmcWriteErrors((mdtAbstractPort::error_t)written, threadHelper);
  }
  // Check if frame was completly written
  Q_ASSERT(threadHelper.currentWriteFrame() != 0);
  if(threadHelper.currentWriteFrame()->isEmpty()){
    // Update waitAnAnswer flag and add bTag to the incomming expeced ones
    *waitAnAnswer = threadHelper.currentWriteFrame()->waitAnAnswer();
    if(*waitAnAnswer){
      expectedBulkInbTags.append((static_cast<mdtFrameUsbTmc*>(threadHelper.currentWriteFrame()))->bTag());
    }
    // Restore frame to pool
    threadHelper.restoreCurrentWriteFrameToPool();
    // Check if a new frame is to write
    if(usbPort()->writeFrames().size() > 0){
      threadHelper.setCurrentWriteFrame(usbPort()->writeFrames().dequeue());
      Q_ASSERT(threadHelper.currentWriteFrame() != 0);
    }
  }
  // Here, if frame is not Null, we have to init a new transfer
  if(threadHelper.currentWriteFrame() != 0){
    // Init a new write transfer (will only init if not pending)
    portError = usbPort()->initWriteTransfer(threadHelper.currentWriteFrame()->data(), threadHelper.currentWriteFrame()->size());
    if(portError != mdtAbstractPort::NoError){
      // Check about stoping
      if(!pvRunning){
        return mdtAbstractPort::UnhandledError;
      }
      return handleUsbtmcWriteErrors((mdtAbstractPort::error_t)written, threadHelper);
    }
  }

  return mdtAbstractPort::NoError;
}

mdtAbstractPort::error_t mdtUsbtmcPortThread::abortBulkOut(quint8 bTag, mdtPortThreadHelperPort &threadHelper)
{
  Q_ASSERT(usbPort() != 0);

  mdtAbstractPort::error_t portError;
  mdtFrameUsbControl *ctlFrame;
  quint8 status = 0;
  int maxTry;

  qDebug() << "*-* Abort bulk OUT beginns for bTag " << bTag << " ...";
  notifyError(mdtAbstractPort::ReadWriteBusy);

  // Check if a query is pending
  if(threadHelper.currentWriteFrame() != 0){
    if(threadHelper.currentWriteFrame()->waitAnAnswer()){
      portError = usbPort()->cancelWriteTransfer();
      if(portError != mdtAbstractPort::NoError){
        return portError;
      }
      threadHelper.restoreCurrentWriteFrameToPool();
    }
  }
  // Wait until we can do a control transfer
  portError = waitOnControlTransferPossible(true);
  if(portError != mdtAbstractPort::NoError){
    return portError;
  }
  // Get a control frame
  Q_ASSERT(usbPort()->controlFramesPool().size() > 0);
  ctlFrame = usbPort()->controlFramesPool().dequeue();
  Q_ASSERT(ctlFrame != 0);
  // INITIATE_ABORT_BULK_OUT
  maxTry = 5;
  while(1){
    // Check maxTry
    if(maxTry < 1){
      mdtError e(MDT_USB_IO_ERROR, "INITIATE_ABORT_BULK_OUT still pending after 5 try", mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
      e.commit();
      usbPort()->controlFramesPool().enqueue(ctlFrame);
      return mdtAbstractPort::UnhandledError;
    }
    --maxTry;
    // Do control transfer
    portError = initiateAbortBulkOut(bTag, ctlFrame, status);
    if(portError != mdtAbstractPort::NoError){
      // Control frame was restored by initiateAbortBulkOut()
      return portError;
    }
    // Check status
    if(status == 0x01){       // 0x01 - STATUS_SUCCESS - Continue abort process
      break;
    }else if(status == 0x81){ // 0x81 - STATUS_TRANSFER_NOT_IN_PROGRESS - Retry later
      msleep(100);
    }else{                    // 0x80 - STATUS_FAILED - Finish abort process (Possibly nothing was to abort)
      usbPort()->controlFramesPool().enqueue(ctlFrame);
      notifyError(mdtAbstractPort::NoError);
      return mdtAbstractPort::NoError;
    }
  }
  // CHECK_ABORT_BULK_OUT_STATUS
  maxTry = 5;
  while(1){
    // Check maxTry
    if(maxTry < 1){
      mdtError e(MDT_USB_IO_ERROR, "CHECK_ABORT_BULK_OUT_STATUS still pending after 5 try", mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
      e.commit();
      usbPort()->controlFramesPool().enqueue(ctlFrame);
      return mdtAbstractPort::UnhandledError;
    }
    --maxTry;
    // Do control transfer
    portError = checkAbortBulkOutStatus(bTag, ctlFrame, status);
    if(portError != mdtAbstractPort::NoError){
      // Control frame was restored by initiateAbortBulkOut()
      return portError;
    }
    // Check status
    if(status == 0x02){ // 0x02 - STATUS_PENDING - Retry later
      msleep(100);
    }else{              // 0x01 - STATUS_SUCCESS or other - Clear halt
      break;
    }
  }
  usbPort()->controlFramesPool().enqueue(ctlFrame);
  portError = usbPort()->clearWriteEndpointHalt();
  if(portError == mdtAbstractPort::NoError){
    notifyError(mdtAbstractPort::NoError);
  }

  return portError;
}

mdtAbstractPort::error_t mdtUsbtmcPortThread::initiateAbortBulkOut(quint8 bTagIn, mdtFrameUsbControl *ctlFrame, quint8 &status)
{
  Q_ASSERT(usbPort() != 0);
  Q_ASSERT(ctlFrame != 0);

  mdtAbstractPort::error_t portError;

  // Setup frame
  ctlFrame->setDirectionDeviceToHost(true);
  ctlFrame->setRequestType(mdtFrameUsbControl::RT_CLASS);
  ctlFrame->setRequestRecipient(mdtFrameUsbControl::RR_ENDPOINT);
  ctlFrame->setbRequest(1);   // INITIATE_ABORT_BULK_OUT
  ctlFrame->setwValue(bTagIn);  // D7..D0: bTag
  ctlFrame->setwIndex(usbPort()->currentWriteEndpointAddress());
  ctlFrame->setwLength(0x02);
  ctlFrame->encode();
  // Submit the control request and wait until transfer is done
  portError = sendControlQuery(ctlFrame);
  if(portError != mdtAbstractPort::NoError){
    return portError;
  }
  // more than 1 frame available should only happen if device had a control response queued
  if(usbPort()->controlResponseFrames().size() != 1){
    mdtError e(MDT_USB_IO_ERROR, "INITIATE_ABORT_BULK_OUT returned unexpected amount of response frames", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
    e.commit();
    // Restore frames and abort
    while(usbPort()->controlResponseFrames().size() > 0){
      usbPort()->controlFramesPool().enqueue(usbPort()->controlResponseFrames().dequeue());
    }
    return mdtAbstractPort::UnhandledError;
  }
  Q_ASSERT(usbPort()->controlResponseFrames().size() == 1);
  ctlFrame = usbPort()->controlResponseFrames().dequeue();
  Q_ASSERT(ctlFrame != 0);
  if(ctlFrame->size() != 2){
    mdtError e(MDT_USB_IO_ERROR, "INITIATE_ABORT_BULK_OUT returned unexpected response size (expected 2, received " + QString::number(ctlFrame->size()) + ")", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
    e.commit();
    // Restore frame and abort
    usbPort()->controlFramesPool().enqueue(ctlFrame);
    return mdtAbstractPort::UnhandledError;
  }
  // Ok, store status
  status = ctlFrame->at(0);

  return mdtAbstractPort::NoError;
}

mdtAbstractPort::error_t mdtUsbtmcPortThread::checkAbortBulkOutStatus(quint8 bTagIn, mdtFrameUsbControl *ctlFrame, quint8 &status)
{
  Q_ASSERT(usbPort() != 0);
  Q_ASSERT(ctlFrame != 0);

  mdtAbstractPort::error_t portError;

  // Setup frame
  ctlFrame->setDirectionDeviceToHost(true);
  ctlFrame->setRequestType(mdtFrameUsbControl::RT_CLASS);
  ctlFrame->setRequestRecipient(mdtFrameUsbControl::RR_ENDPOINT);
  ctlFrame->setbRequest(2); // CHECK_ABORT_BULK_OUT_STATUS
  ctlFrame->setwValue(0);   // D7..D0: bTag
  ctlFrame->setwIndex(usbPort()->currentWriteEndpointAddress());
  ctlFrame->setwLength(0x08);
  ctlFrame->encode();
  // Submit the control request and wait until transfer is done
  portError = sendControlQuery(ctlFrame);
  if(portError != mdtAbstractPort::NoError){
    return portError;
  }
  // more than 1 frame available should only happen if device had a control response queued
  if(usbPort()->controlResponseFrames().size() != 1){
    mdtError e(MDT_USB_IO_ERROR, "CHECK_ABORT_BULK_OUT_STATUS returned unexpected amount of response frames", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
    e.commit();
    // Restore frames and abort
    while(usbPort()->controlResponseFrames().size() > 0){
      usbPort()->controlFramesPool().enqueue(usbPort()->controlResponseFrames().dequeue());
    }
    return mdtAbstractPort::UnhandledError;
  }
  Q_ASSERT(usbPort()->controlResponseFrames().size() == 1);
  ctlFrame = usbPort()->controlResponseFrames().dequeue();
  Q_ASSERT(ctlFrame != 0);
  if(ctlFrame->size() != 8){
    mdtError e(MDT_USB_IO_ERROR, "CHECK_ABORT_BULK_OUT_STATUS returned unexpected response size (expected 8, received " + QString::number(ctlFrame->size()) + ")", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
    e.commit();
    // Restore frame and abort
    usbPort()->controlFramesPool().enqueue(ctlFrame);
    return mdtAbstractPort::UnhandledError;
  }
  // Ok, store status
  status = ctlFrame->at(0);

  return mdtAbstractPort::NoError;
}

mdtAbstractPort::error_t mdtUsbtmcPortThread::abortBulkInOld(quint8 bTag, mdtPortThreadHelperPort &threadHelper)
{
  Q_ASSERT(pvPort != 0);
  ///Q_ASSERT(threadHelper.currentWriteFrame() != 0);

  mdtUsbPort *port;
  mdtAbstractPort::error_t portError;
  ///QQueue<mdtFrameUsbControl*> ctlFramesPoolCopy;
  mdtFrameUsbControl *ctlFrame;
  quint8 status;
  int maxTry;

  // We need a mdtUsbPort object
  port = dynamic_cast<mdtUsbPort*>(pvPort);
  Q_ASSERT(port != 0);

  // Check if a query is pending
  if(threadHelper.currentWriteFrame() != 0){
    if(threadHelper.currentWriteFrame()->waitAnAnswer()){
      portError = port->cancelWriteTransfer();
      if(portError != mdtAbstractPort::NoError){
        return portError;
      }
      threadHelper.restoreCurrentWriteFrameToPool();
    }
  }
  // Wait until we can do a control transfer
  portError = waitOnControlTransferPossible(true);
  if(portError != mdtAbstractPort::NoError){
    return portError;
  }
  /**
  // Flush pending control frames
  while(port->controlQueryFrames().size() > 0){
    // Warn and remove frame
    mdtError e(MDT_USB_IO_ERROR, "Aborting a pending control transfer", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
    e.commit();
    port->controlFramesPool().enqueue(port->controlQueryFrames().dequeue());
  }
  */
  /**
  // Cancel possibly running control transfer
  portError = port->cancelControlTransfer();
  if(portError != mdtAbstractPort::NoError){
    return portError;
  }
  while(port->controlResponseFrames().size() > 0){
    // Warn and remove frame
    mdtError e(MDT_USB_IO_ERROR, "Aborting a pending control transfer", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
    e.commit();
    port->controlFramesPool().enqueue(port->controlResponseFrames().dequeue());
  }
  */
  // Get a control frame for working
  Q_ASSERT(port->controlFramesPool().size() > 0);
  ctlFrame = port->controlFramesPool().dequeue();
  Q_ASSERT(ctlFrame != 0);
  /**
  // Take all control frames from pool, so port manager cannot init a control transfer
  while(port->controlFramesPool().size() > 0){
    ctlFramesPoolCopy.enqueue(port->controlFramesPool().dequeue());
  }
  */
  // Send the INITIATE_ABORT_BULK_IN request and wait on response
  status = 0x81;  // STATUS_TRANSFER_NOT_IN_PROGRESS
  maxTry = 5;
  while((status == 0x81)&&(maxTry > 0)){
    // Check about stopping
    if(!pvRunning){
      // restore frames and return noerror
      port->controlFramesPool().enqueue(ctlFrame);
      /**
      while(ctlFramesPoolCopy.size() > 0){
        port->controlFramesPool().enqueue(ctlFramesPoolCopy.dequeue());
      }
      */
      return mdtAbstractPort::NoError;
    }
    // Send INITIATE_ABORT_BULK_IN request
    ///qDebug() << "*-* send INITIATE_ABORT_BULK_IN ...";
    portError = sendInitiateAbortBulkInRequest(bTag, ctlFrame);
    if(portError != mdtAbstractPort::NoError){
      /**
      // restore frames and abort
      while(ctlFramesPoolCopy.size() > 0){
        port->controlFramesPool().enqueue(ctlFramesPoolCopy.dequeue());
      }
      */
      return portError;
    }
    // more than 1 frame available should only happen if device had a control response queued
    if(port->controlResponseFrames().size() != 1){
      mdtError e(MDT_USB_IO_ERROR, "INITIATE_ABORT_BULK_IN returned unexpected amount of responses", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
      e.commit();
      /**
      // restore frames and abort
      while(ctlFramesPoolCopy.size() > 0){
        port->controlFramesPool().enqueue(ctlFramesPoolCopy.dequeue());
      }
      */
      return mdtAbstractPort::UnhandledError;
    }
    Q_ASSERT(port->controlResponseFrames().size() == 1);
    ctlFrame = port->controlResponseFrames().dequeue();
    if(ctlFrame->size() != 2){
      mdtError e(MDT_USB_IO_ERROR, "INITIATE_ABORT_BULK_IN returned unexpected response size (expected 2, received " + QString::number(ctlFrame->size()) + ")", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
      e.commit();
      // Restore frames and abort
      port->controlFramesPool().enqueue(ctlFrame);
      /**
      while(ctlFramesPoolCopy.size() > 0){
        port->controlFramesPool().enqueue(ctlFramesPoolCopy.dequeue());
      }
      */
      return mdtAbstractPort::UnhandledError;
    }
    // Check status
    status = ctlFrame->at(0);
    qDebug() << "*-* abortBulkIn() , status: 0x" << hex << status;
    if(status == 0x81){ // STATUS_TRANSFER_NOT_IN_PROGRESS
      msleep(500);
    }
    maxTry--;
  }
  if(maxTry < 1){
    mdtError e(MDT_USB_IO_ERROR, "INITIATE_ABORT_BULK_IN still pending after 10 try", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
    e.commit();
    // Continue working (possibly, nothing was to abort)
    port->controlFramesPool().enqueue(ctlFrame);
    /**
    while(ctlFramesPoolCopy.size() > 0){
      port->controlFramesPool().enqueue(ctlFramesPoolCopy.dequeue());
    }
    */
    if(pvRunning){
      notifyError(mdtAbstractPort::ReadCanceled, true); /// \todo Ok ?
    }
    return mdtAbstractPort::NoError;
  }
  // Check returned status
  if(status != 0x01){ // 0x01: STATUS_SUCCESS
    // No transfer in progress (device FIFO empty)
    port->controlFramesPool().enqueue(ctlFrame);
    /**
    while(ctlFramesPoolCopy.size() > 0){
      port->controlFramesPool().enqueue(ctlFramesPoolCopy.dequeue());
    }
    */
    portError = port->cancelReadTransfer();
    if(portError != mdtAbstractPort::NoError){
      return portError;
    }
    if(pvRunning){
      notifyError(mdtAbstractPort::ReadCanceled, true); /// \todo Ok ?
    }
    return mdtAbstractPort::NoError;
  }
  // Flush device
  portError = readUntilShortPacketReceived(100);
  if(portError != mdtAbstractPort::NoError){
    // restore frames and abort
    port->controlFramesPool().enqueue(ctlFrame);
    /**
    while(ctlFramesPoolCopy.size() > 0){
      port->controlFramesPool().enqueue(ctlFramesPoolCopy.dequeue());
    }
    */
    return portError;
  }
  // Send the CHECK_ABORT_BULK_IN_STATUS request and wait on response
  status = 0x02;  // STATUS_PENDING
  maxTry = 10;
  while((status == 0x02)&&(maxTry > 0)){
    // Send CHECK_ABORT_BULK_IN_STATUS
    ///qDebug() << "*-* send CHECK_ABORT_BULK_IN_STATUS ...";
    portError = sendCheckAbortBulkInStatusRequest(bTag, ctlFrame);
    if(portError != mdtAbstractPort::NoError){
      /**
      // restore frames and abort
      while(ctlFramesPoolCopy.size() > 0){
        port->controlFramesPool().enqueue(ctlFramesPoolCopy.dequeue());
      }
      */
      return portError;
    }
    // more than 1 frame available should only happen if device had a control response queued
    if(port->controlResponseFrames().size() != 1){
      mdtError e(MDT_USB_IO_ERROR, "CHECK_ABORT_BULK_IN_STATUS returned unexpected amount of responses", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
      e.commit();
      /**
      // restore frames and abort
      while(ctlFramesPoolCopy.size() > 0){
        port->controlFramesPool().enqueue(ctlFramesPoolCopy.dequeue());
      }
      */
      return mdtAbstractPort::UnhandledError;
    }
    ctlFrame = port->controlResponseFrames().dequeue();
    if(ctlFrame->size() != 8){
      mdtError e(MDT_USB_IO_ERROR, "CHECK_ABORT_BULK_IN_STATUS returned unexpected response size (expected 8, received " + QString::number(ctlFrame->size()) + ")", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
      e.commit();
      // restore frames and abort
      port->controlFramesPool().enqueue(ctlFrame);
      /**
      while(ctlFramesPoolCopy.size() > 0){
        port->controlFramesPool().enqueue(ctlFramesPoolCopy.dequeue());
      }
      */
      return mdtAbstractPort::UnhandledError;
    }
    // Check status
    status = ctlFrame->at(0);
    if(status == 0x02){ // STATUS_PENDING
      // Check bmAbortBulkIn.D0
      if((quint8)ctlFrame->at(1) & 0x01){
        qDebug() << "*-* Flushing device ...";
        // Flush device
        portError = readUntilShortPacketReceived(100);
        if(portError != mdtAbstractPort::NoError){
          // restore frames and abort
          port->controlFramesPool().enqueue(ctlFrame);
          /**
          while(ctlFramesPoolCopy.size() > 0){
            port->controlFramesPool().enqueue(ctlFramesPoolCopy.dequeue());
          }
          */
          return portError;
        }
      }
      wait(1000);
    }
    maxTry--;
  }
  if(maxTry < 1){
    mdtError e(MDT_USB_IO_ERROR, "CHECK_ABORT_BULK_IN_STATUS still pending after 10 try", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
    e.commit();
  }
  // End, restore frames and notify port manager
  port->controlFramesPool().enqueue(ctlFrame);
  /**
  while(ctlFramesPoolCopy.size() > 0){
    port->controlFramesPool().enqueue(ctlFramesPoolCopy.dequeue());
  }
  */
  if(pvRunning){
    notifyError(mdtAbstractPort::ReadCanceled, true);
  }

  ///qDebug() << "*-* abortBulkIn() DONE";
  return mdtAbstractPort::NoError;
}

mdtAbstractPort::error_t mdtUsbtmcPortThread::abortBulkIn(quint8 bTag, mdtPortThreadHelperPort &threadHelper)
{
  Q_ASSERT(usbPort() != 0);

  mdtAbstractPort::error_t portError;
  mdtFrameUsbControl *ctlFrame;
  quint8 status = 0;
  bool bmAbortBulkInD0 = false;
  int maxTry;

  qDebug() << "*-* Abort bulk IN beginns for bTag " << bTag << " ...";
  notifyError(mdtAbstractPort::ReadWriteBusy);

  // Check if a query is pending
  if(threadHelper.currentWriteFrame() != 0){
    if(threadHelper.currentWriteFrame()->waitAnAnswer()){
      portError = usbPort()->cancelWriteTransfer();
      if(portError != mdtAbstractPort::NoError){
        return portError;
      }
      threadHelper.restoreCurrentWriteFrameToPool();
    }
  }
  // Wait until we can do a control transfer
  portError = waitOnControlTransferPossible(true);
  if(portError != mdtAbstractPort::NoError){
    return portError;
  }
  // Get a control frame
  Q_ASSERT(usbPort()->controlFramesPool().size() > 0);
  ctlFrame = usbPort()->controlFramesPool().dequeue();
  Q_ASSERT(ctlFrame != 0);
  // INITIATE_ABORT_BULK_IN
  maxTry = 5;
  while(1){
    // Check maxTry
    if(maxTry < 1){
      mdtError e(MDT_USB_IO_ERROR, "INITIATE_ABORT_BULK_IN still pending after 5 try", mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
      e.commit();
      usbPort()->controlFramesPool().enqueue(ctlFrame);
      return mdtAbstractPort::UnhandledError;
    }
    --maxTry;
    // Do control transfer
    portError = initiateAbortBulkIn(bTag, ctlFrame, status);
    if(portError != mdtAbstractPort::NoError){
      // Control frame was restored by initiateAbortBulkIn()
      return portError;
    }
    // Check status
    if(status == 0x01){       // 0x01 - STATUS_SUCCESS - Read until short packet received and CHECK_ABORT_BULK_IN_STATUS
      portError = readUntilShortPacketReceived(100);
      if(portError != mdtAbstractPort::NoError){
        // Restore control frame and abort
        usbPort()->controlFramesPool().enqueue(ctlFrame);
        return portError;
      }
      // Continue with CHECK_ABORT_BULK_IN_STATUS
      break;
    }else if(status == 0x81){ // 0x81 - STATUS_TRANSFER_NOT_IN_PROGRESS - Retry later
      msleep(100);
    }else{                    // 0x80 - STATUS_FAILED - Finish abort process (Possibly nothing was to abort)
      usbPort()->controlFramesPool().enqueue(ctlFrame);
      portError = usbPort()->cancelReadTransfer();
      if(portError == mdtAbstractPort::NoError){
        notifyError(mdtAbstractPort::NoError);
      }
      return portError;
    }
  }
  // CHECK_ABORT_BULK_IN_STATUS
  maxTry = 5;
  while(1){
    // Check maxTry
    if(maxTry < 1){
      mdtError e(MDT_USB_IO_ERROR, "CHECK_ABORT_BULK_IN_STATUS still pending after 5 try", mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
      e.commit();
      usbPort()->controlFramesPool().enqueue(ctlFrame);
      return mdtAbstractPort::UnhandledError;
    }
    --maxTry;
    // Do control transfer
    portError = checkAbortBulkInStatus(bTag, ctlFrame, status, bmAbortBulkInD0);
    if(portError != mdtAbstractPort::NoError){
      // Control frame was restored by checkAbortBulkInStatus()
      return portError;
    }
    // Check status
    if(status == 0x02){ // 0x02 - STATUS_PENDING - Retry later - Flush bulk IN if requiered
      if(bmAbortBulkInD0){
        portError = readUntilShortPacketReceived(100);
        if(portError != mdtAbstractPort::NoError){
          // Restore control frame and abort
          usbPort()->controlFramesPool().enqueue(ctlFrame);
          return portError;
        }
      }
      msleep(100);
    }else{              // 0x01 - STATUS_SUCCESS or other - Process finished
      break;
    }
  }
  usbPort()->controlFramesPool().enqueue(ctlFrame);
  notifyError(mdtAbstractPort::NoError);

  return mdtAbstractPort::NoError;
}

mdtAbstractPort::error_t mdtUsbtmcPortThread::initiateAbortBulkIn(quint8 bTagIn, mdtFrameUsbControl *ctlFrame, quint8 &status)
{
  Q_ASSERT(usbPort() != 0);
  Q_ASSERT(ctlFrame != 0);

  mdtAbstractPort::error_t portError;

  // Setup frame
  ctlFrame->setDirectionDeviceToHost(true);
  ctlFrame->setRequestType(mdtFrameUsbControl::RT_CLASS);
  ctlFrame->setRequestRecipient(mdtFrameUsbControl::RR_ENDPOINT);
  ctlFrame->setbRequest(3);     // INITIATE_ABORT_BULK_IN
  ctlFrame->setwValue(bTagIn);  // D7..D0: bTag
  ctlFrame->setwIndex(usbPort()->currentReadEndpointAddress());
  ctlFrame->setwLength(0x02);
  ctlFrame->encode();
  // Submit the control request and wait until transfer is done
  portError = sendControlQuery(ctlFrame);
  if(portError != mdtAbstractPort::NoError){
    return portError;
  }
  // more than 1 frame available should only happen if device had a control response queued
  if(usbPort()->controlResponseFrames().size() != 1){
    mdtError e(MDT_USB_IO_ERROR, "INITIATE_ABORT_BULK_IN returned unexpected amount of response frames", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
    e.commit();
    // Restore frames and abort
    while(usbPort()->controlResponseFrames().size() > 0){
      usbPort()->controlFramesPool().enqueue(usbPort()->controlResponseFrames().dequeue());
    }
    return mdtAbstractPort::UnhandledError;
  }
  Q_ASSERT(usbPort()->controlResponseFrames().size() == 1);
  ctlFrame = usbPort()->controlResponseFrames().dequeue();
  Q_ASSERT(ctlFrame != 0);
  if(ctlFrame->size() != 2){
    mdtError e(MDT_USB_IO_ERROR, "INITIATE_ABORT_BULK_IN returned unexpected response size (expected 2, received " + QString::number(ctlFrame->size()) + ")", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
    e.commit();
    // Restore frame and abort
    usbPort()->controlFramesPool().enqueue(ctlFrame);
    return mdtAbstractPort::UnhandledError;
  }
  // Ok, store status
  status = ctlFrame->at(0);

  return mdtAbstractPort::NoError;
}

mdtAbstractPort::error_t mdtUsbtmcPortThread::checkAbortBulkInStatus(quint8 bTagIn, mdtFrameUsbControl *ctlFrame, quint8 &status, bool &bmAbortBulkInD0)
{
  Q_ASSERT(usbPort() != 0);
  Q_ASSERT(ctlFrame != 0);

  mdtAbstractPort::error_t portError;

  // Setup frame
  ctlFrame->setDirectionDeviceToHost(true);
  ctlFrame->setRequestType(mdtFrameUsbControl::RT_CLASS);
  ctlFrame->setRequestRecipient(mdtFrameUsbControl::RR_ENDPOINT);
  ctlFrame->setbRequest(4);   // CHECK_ABORT_BULK_IN_STATUS
  ctlFrame->setwValue(0);
  ctlFrame->setwIndex(usbPort()->currentReadEndpointAddress());
  ctlFrame->setwLength(0x08);
  ctlFrame->encode();
  // Submit the control request and wait until transfer is done
  portError = sendControlQuery(ctlFrame);
  if(portError != mdtAbstractPort::NoError){
    return portError;
  }
  // more than 1 frame available should only happen if device had a control response queued
  if(usbPort()->controlResponseFrames().size() != 1){
    mdtError e(MDT_USB_IO_ERROR, "CHECK_ABORT_BULK_IN_STATUS returned unexpected amount of response frames", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
    e.commit();
    // Restore frames and abort
    while(usbPort()->controlResponseFrames().size() > 0){
      usbPort()->controlFramesPool().enqueue(usbPort()->controlResponseFrames().dequeue());
    }
    return mdtAbstractPort::UnhandledError;
  }
  Q_ASSERT(usbPort()->controlResponseFrames().size() == 1);
  ctlFrame = usbPort()->controlResponseFrames().dequeue();
  Q_ASSERT(ctlFrame != 0);
  if(ctlFrame->size() != 8){
    mdtError e(MDT_USB_IO_ERROR, "CHECK_ABORT_BULK_IN_STATUS returned unexpected response size (expected 8, received " + QString::number(ctlFrame->size()) + ")", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
    e.commit();
    // Restore frame and abort
    usbPort()->controlFramesPool().enqueue(ctlFrame);
    return mdtAbstractPort::UnhandledError;
  }
  // Ok, store status
  status = ctlFrame->at(0);
  // Check if device bulk IN FIFO has data
  if(ctlFrame->at(1) & 0x01){
    bmAbortBulkInD0 = true;
  }else{
    bmAbortBulkInD0 = false;
  }

  return mdtAbstractPort::NoError;
}




mdtAbstractPort::error_t mdtUsbtmcPortThread::sendInitiateAbortBulkInRequest(quint8 bTag, mdtFrameUsbControl *ctlFrame)
{
  Q_ASSERT(pvPort != 0);
  Q_ASSERT(ctlFrame != 0);

  ///mdtAbstractPort::error_t portError;
  ///QList<mdtAbstractPort::error_t> errors;
  mdtUsbPort *port;
  ///int i;

  // We need a mdtUsbPort object
  port = dynamic_cast<mdtUsbPort*>(pvPort);
  Q_ASSERT(port != 0);

  // Setup frame
  ctlFrame->setDirectionDeviceToHost(true);
  ctlFrame->setRequestType(mdtFrameUsbControl::RT_CLASS);
  ctlFrame->setRequestRecipient(mdtFrameUsbControl::RR_ENDPOINT);
  ctlFrame->setbRequest(3);   // INITIATE_ABORT_BULK_IN
  ctlFrame->setwValue(bTag);  // D7..D0: bTag
  ctlFrame->setwIndex(port->currentReadEndpointAddress());
  ctlFrame->setwLength(0x02);
  ctlFrame->encode();
  // Submit the control request and wait until transfer is done
  return sendControlQuery(ctlFrame);
  /**
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
  */
}

mdtAbstractPort::error_t mdtUsbtmcPortThread::sendCheckAbortBulkInStatusRequest(quint8 bTag, mdtFrameUsbControl *ctlFrame)
{
  Q_ASSERT(usbPort() != 0);
  Q_ASSERT(ctlFrame != 0);

  mdtAbstractPort::error_t portError;
  QList<mdtAbstractPort::error_t> errors;
  ///mdtUsbPort *port;
  int i;

  // We need a mdtUsbPort object
  ///port = dynamic_cast<mdtUsbPort*>(pvPort);
  ///Q_ASSERT(port != 0);

  // Setup frame
  ctlFrame->setDirectionDeviceToHost(true);
  ctlFrame->setRequestType(mdtFrameUsbControl::RT_CLASS);
  ctlFrame->setRequestRecipient(mdtFrameUsbControl::RR_ENDPOINT);
  ctlFrame->setbRequest(4);   // CHECK_ABORT_BULK_IN_STATUS
  ctlFrame->setwValue(0);
  ctlFrame->setwIndex(usbPort()->currentReadEndpointAddress());
  ctlFrame->setwLength(0x08);
  ctlFrame->encode();
  // Submit the control request and wait until transfer is done
  usbPort()->addControlRequest(ctlFrame);
  while(usbPort()->controlResponseFrames().size() < 1){
    // Check about stoping
    if(!pvRunning){
      return mdtAbstractPort::NoError;
    }
    portError = usbPort()->handleUsbEvents();
    if(portError != mdtAbstractPort::NoError){
      return portError;
    }
    // Check if transfer callback reported a error
    errors = usbPort()->lastErrors();
    for(i=0; i<errors.size(); i++){
      portError = errors.at(i);
      if(portError != mdtAbstractPort::NoError){
        return portError;
      }
    }
  }

  return mdtAbstractPort::NoError;
}

void mdtUsbtmcPortThread::run()
{
  Q_ASSERT(pvPort != 0);

  mdtFrameUsbTmc *usbtmcFrame;
  mdtAbstractPort::error_t portError = mdtAbstractPort::NoError;
  int n;
  int i;
  mdtUsbPort *port;
  bool waitAnAnswer = false;
  QList<mdtAbstractPort::error_t> errors;
  ///QList<quint8> expectedBulkInbTags;
  quint8 bTagToAbort;
  bool ok;
  mdtPortThreadHelperPort threadHelper;

  pvPort->lockMutex();
#ifdef Q_OS_UNIX
  pvNativePthreadObject = pthread_self();
  Q_ASSERT(pvNativePthreadObject != 0);
#endif
  // We need a mdtUsbPort object here
  ///port = dynamic_cast<mdtUsbPort*> (pvPort);
  port = usbPort();
  Q_ASSERT(port != 0);
  threadHelper.setPort(pvPort);
  threadHelper.setThread(this);
  // Set the running flag
  pvRunning = true;
  // Get a frame for read
  if(!threadHelper.getNewFrameRead()){
    pvRunning = false;
    notifyError(mdtAbstractPort::Disconnected);
  }
  // Notify that we are ready
  ///notifyError(mdtAbstractPort::NoError);
  emit ready(this);
  emit connected();

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
      portError = handleUsbtmcReadWriteErrors(portError, threadHelper);
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
    for(i=0; i<errors.size(); i++){
      // Check about stoping
      if(!pvRunning){
        break;
      }
      portError = errors.at(i);
      if((portError == mdtAbstractPort::ControlCanceled)||(portError == mdtAbstractPort::ControlTimeout)){
        // mdtUsbPort does the job, we just have to notify the error
        notifyError(mdtAbstractPort::ControlCanceled);
      }else{
        portError = handleUsbtmcReadWriteErrors(portError, threadHelper);
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
      portError = handleUsbtmcReadWriteErrors(portError, threadHelper);
      if(portError != mdtAbstractPort::ErrorHandled){
        // Unhandled error - stop
        break;
      }
      // Error could be handled - go back IDLE
      continue;
    }
    // Write ...
    portError = usbtmcWrite(threadHelper, &waitAnAnswer, port->expectedBulkInbTags());
    ///qDebug() << "USBTMCTHD: write done, expectedBulkInbTags: " << expectedBulkInbTags;
    if(portError != mdtAbstractPort::NoError){
      if(portError != mdtAbstractPort::ErrorHandled){
        // Unhandled error - stop
        break;
      }else{
        // Handled error, go back IDLE
        continue;
      }
    }
    // Check about stoping
    if(!pvRunning){
      break;
    }
    // read/store available data
    Q_ASSERT(threadHelper.currentReadFrame() != 0);
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
        portError = handleUsbtmcReadErrors(portError, threadHelper);
        if(portError != mdtAbstractPort::ErrorHandled){
          // Unhandled error - stop
          break;
        }
        // Handled error, go back IDLE
        continue;
      }
    }
    // At this state, if somes frame(s) are complete, we must check them and emit the signal if ok
    QMutableListIterator<mdtFrame*> it(port->readenFrames());
    while(it.hasNext()){
      it.next();
      Q_ASSERT(it.value() != 0);
      // We need a USBTMC frame
      usbtmcFrame = static_cast<mdtFrameUsbTmc*>(it.value());
      qDebug() << "USBTMCTHD: current readFrame's bTag: " << usbtmcFrame->bTag() << " , expectedBulkInbTags: " << port->expectedBulkInbTags();
      ok = true;
      // Check if complete
      if(!usbtmcFrame->isComplete()){
        ok = false;
        mdtError e(MDT_USB_IO_ERROR, "Receive a uncomplete frame, will be aborted", mdtError::Warning);
        MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
        e.commit();
      }else if(!usbtmcFrame->bTagOk()){ // Check bTag/bTagInverse coherence
        ok = false;
        mdtError e(MDT_USB_IO_ERROR, "Receive a frame with incoherent bTag/bTagInverse, will be aborted", mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
        e.commit();
      }else if(!usbtmcFrame->MsgIDsupported()){ // Check if MsgID is supported
        ok = false;
        mdtError e(MDT_USB_IO_ERROR, "Receive a frame with unsupported MsgID, will be aborted", mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
        e.commit();
      }
      // Abort if error occured
      if(!ok){
        // First remove frame from readenFrames and put back to pool, because we will release port mutex
        bTagToAbort = usbtmcFrame->bTag();
        Q_ASSERT(!port->readenFrames().isEmpty());
        it.remove();
        port->readFramesPool().enqueue(usbtmcFrame);
        port->expectedBulkInbTags().removeOne(usbtmcFrame->bTag());
        usbtmcFrame = 0;
        // We must abort bulk IN
        qDebug() << "USBTMCTHD: Abort bulk IN after frame check error ...";
        ///Q_ASSERT(usbtmcFrame != 0);
        portError = abortBulkIn(bTagToAbort, threadHelper);
        if(portError != mdtAbstractPort::NoError){
          if(!pvRunning){
            break;
          }
          if(portError != mdtAbstractPort::NoError){
            break;
          }
        }
      }else{
        // Check if bTag was expected
        qDebug() << "USBTMCTHD (2): current readFrame's bTag: " << usbtmcFrame->bTag() << " , expectedBulkInbTags: " << port->expectedBulkInbTags();
        if(!port->expectedBulkInbTags().contains(usbtmcFrame->bTag())){
          mdtError e(MDT_USB_IO_ERROR, "Receive a frame with unexpeced bTag, will be skipped", mdtError::Error);
          MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortThread");
          e.commit();
          // Remove frame and put back to pool
          qDebug() << "Removing from readenFrames ... - queue: " << port->readenFrames();
          Q_ASSERT(!port->readenFrames().isEmpty());
          it.remove();
          qDebug() << "Restore to pool ...";
          port->readFramesPool().enqueue(usbtmcFrame);
          usbtmcFrame = 0;
          notifyError(mdtAbstractPort::ReadCanceled);
        }else{
          ///expectedBulkInbTags.removeOne(usbtmcFrame->bTag()); /// \bug Port manager should remove it when it has removed frame !!
          // Simply emit signal, port manager will take the frame and remove bTag from expectedBulkInbTags list when port mutext is unlocked
          qDebug() << "emit newFrameReaden() - bTag: " << usbtmcFrame->bTag();
          emit(newFrameReaden());
        }
      }
    }
  }

  port->cancelTransfers();

  /**
  if(portError == mdtAbstractPort::NoError){
    notifyError(mdtAbstractPort::Disconnected);
  }
  */
  pvRunning = false;
  pvPort->unlockMutex();
}

