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
#include "mdtUsbPort.h"
#include "mdtError.h"
#include "mdtUsbDeviceDescriptor.h"
#include <QString>
#include <QStringList>
#include <QMap>

// We need a sleep function
#ifdef Q_OS_UNIX
 #define msleep(t) usleep(1000*t)
#endif
#ifdef Q_OS_WIN
 #include <windows.h>
 #define msleep(t) Sleep(t)
#endif

#include <QDebug>

mdtUsbPort::mdtUsbPort(QObject *parent)
 : mdtAbstractPort(parent)
{
  int retVal;

  pvReadTimeout = 0;
  pvWriteTimeout = 0;
  pvHandle = 0;
  pvbInterfaceNumber = 0;
  pvReadBuffer = 0;
  pvReadBufferSize = 0;
  pvReadEndpointAddress = 0;
  pvReadTransfer = 0;
  pvReadTransferPending = false;
  pvWriteBuffer = 0;
  pvWriteBufferSize = 0;
  pvWriteEndpointAddress = 0;
  pvWriteTransfer = 0;
  pvWriteTransferPending = false;
  pvControlBuffer = 0;
  pvControlBufferSize = 0;
  pvControlTransfer = 0;
  pvControlTransferPending = false;
  pvMessageInBuffer = 0;
  pvMessageInBufferSize = 0;
  pvMessageInEndpointAddress = 0;
  pvMessageInTransfer = 0;
  pvMessageInTransferPending = false;
  // Init libusb
  retVal = libusb_init(&pvLibusbContext);
  if(retVal != 0){
    pvLibusbContext = 0;
    mdtError e(MDT_USB_IO_ERROR, "libusb_init() failed", mdtError::Error);
    e.setSystemError(retVal, errorText(retVal));
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
  }
  Q_ASSERT(pvLibusbContext != 0);
}

mdtUsbPort::~mdtUsbPort()
{
  close();
  // Free libusb
  libusb_exit(pvLibusbContext);
}

mdtAbstractPort::error_t mdtUsbPort::reconnect(int timeout)
{
  error_t error;

  // Let libusb finish pending works
  handleUsbEvents(&pvReadTimeoutTv, 0);
  close();

  error = open();
  if(error != NoError){
    close();
    // Wait timeout and retry
    unlockMutex();
    msleep(timeout);
    lockMutex();
    return Disconnected;
  }
  error = setup();
  if(error != NoError){
    return Disconnected;
  }

  return NoError;
}

void mdtUsbPort::setReadTimeout(int timeout)
{
  if(timeout < 0){
    pvReadTimeout = 0;  // 0 means infinite on some functions in libusb
    pvReadTimeoutTv.tv_sec = -1;
    pvReadTimeoutTv.tv_usec = 0;
  }else{
    pvReadTimeout = timeout;
    pvReadTimeoutTv.tv_sec = timeout/1000;
    pvReadTimeoutTv.tv_usec = 1000*(timeout%1000);
  }
}

void mdtUsbPort::setWriteTimeout(int timeout)
{
  if(timeout < 0){
    pvWriteTimeout = 0;  // 0 means infinite on some functions in libusb
    pvWriteTimeoutTv.tv_sec = -1;
    pvWriteTimeoutTv.tv_usec = 0;
  }else{
    pvWriteTimeout = timeout;
    pvWriteTimeoutTv.tv_sec = timeout/1000;
    pvWriteTimeoutTv.tv_usec = 1000*(timeout%1000);
  }
}

void mdtUsbPort::addControlRequest(mdtFrameUsbControl *frame, bool setwIndexAsbInterfaceNumber)
{
  Q_ASSERT(frame != 0);

  if(setwIndexAsbInterfaceNumber){
    frame->setwIndex(pvbInterfaceNumber);
  }
  pvControlQueryFrames.enqueue(frame);
  submitControlQuery();
}

void mdtUsbPort::submitControlQuery()
{
  int err;
  error_t portError;

  // If transfer is null, port was closed
  if(pvControlTransfer == 0){
    return;
  }
  // Check if a transfer is pending
  if(pvControlTransferPending){
    return;
  }
  // Check if we have something to send
  if(pvControlQueryFrames.size() < 1){
    return;
  }
  pvCurrentControlFrame = pvControlQueryFrames.dequeue();
  Q_ASSERT(pvCurrentControlFrame != 0);
  // Check that we have a large enough buffer
  if(pvControlBufferSize < pvCurrentControlFrame->size()){
    mdtError e(MDT_USB_IO_ERROR, "Unable to init control transfert, internal buffer is to small", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    addError(UnhandledError);
    return;
  }
  // Copy data to write
  memcpy(pvControlBuffer, pvCurrentControlFrame->data(), pvCurrentControlFrame->size());
  // Fill the transfer
  libusb_fill_control_transfer(pvControlTransfer, pvHandle, (unsigned char*)pvControlBuffer, controlTransferCallback, (void*)this, pvWriteTimeout);
  // Submit transfer
  err = libusb_submit_transfer(pvControlTransfer);
  if(err != 0){
    portError = mapUsbError(err, 0);
    if(portError == UnhandledError){
      mdtError e(MDT_USB_IO_ERROR, "libusb_submit_transfer() failed for control endpoint", mdtError::Error);
      e.setSystemError(err, errorText(err));
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
    }
    controlFramesPool().enqueue(pvCurrentControlFrame);
    pvCurrentControlFrame = 0;
    addError(portError);
    return;
  }
  pvControlTransferPending = true;
}

void mdtUsbPort::controlTransferCallback(struct libusb_transfer *transfer)
{
  ///qDebug() << "mdtUsbPort::controlTransferCallback() ...";
  Q_ASSERT(transfer != 0);
  Q_ASSERT(transfer->user_data != 0);

  libusb_control_setup *controlSetup;
  char *controlData;

  // Get this instance
  mdtUsbPort *port = static_cast<mdtUsbPort*>(transfer->user_data);
  // Update flag here (else it will stay true on error)
  port->pvControlTransferPending = false;
  // It can happen that submitControlQuery() fails,
  //  in this case current frame will be null.
  if(port->pvCurrentControlFrame == 0){
    return;
  }
  /*
   * Check if some error occured.
   * On most error, we put frame back to pool.
   */
  // Check if transfer has timed out
  if(transfer->status & LIBUSB_TRANSFER_TIMED_OUT){
    ///qDebug() << "mdtUsbPort::controlTransferCallback(): timeout";
    port->updateReadTimeoutState(true);
    port->controlFramesPool().enqueue(port->pvCurrentControlFrame);
    port->pvCurrentControlFrame = 0;
    port->addError(ControlTimeout);
    return;
  }
  // Check if transfer was cancelled
  if(transfer->status & LIBUSB_TRANSFER_CANCELLED){
    ///qDebug() << "mdtUsbPort::controlTransferCallback(): cancelled";
    port->controlFramesPool().enqueue(port->pvCurrentControlFrame);
    port->pvCurrentControlFrame = 0;
    port->addError(ControlCanceled);
    return;
  }
  // Check if device is disconnected (not valid if cancelled)
  if(transfer->status & LIBUSB_TRANSFER_NO_DEVICE){
    ///qDebug() << "mdtUsbPort::controlTransferCallback(): disconnected";
    port->controlFramesPool().enqueue(port->pvCurrentControlFrame);
    port->pvCurrentControlFrame = 0;
    port->addError(Disconnected);
    return;
  }
  // Check if transfer stall
  if(transfer->status & LIBUSB_TRANSFER_STALL){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_STALL during a control transfer", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    port->controlFramesPool().enqueue(port->pvCurrentControlFrame);
    port->pvCurrentControlFrame = 0;
    port->addError(UnhandledError);
    return;
  }
  // Check if transfer failed (not valid if cancelled)
  if(transfer->status & LIBUSB_TRANSFER_ERROR){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_ERROR during a control transfer", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    port->controlFramesPool().enqueue(port->pvCurrentControlFrame);
    port->pvCurrentControlFrame = 0;
    port->addError(UnhandledError);
    return;
  }
  // Check transfer overflow
  if(transfer->status & LIBUSB_TRANSFER_OVERFLOW){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_OVERFLOW during a control transfer", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    port->controlFramesPool().enqueue(port->pvCurrentControlFrame);
    port->pvCurrentControlFrame = 0;
    port->addError(UnhandledError);
    return;
  }
  // Check transfer short frames NOK
  if(transfer->status & LIBUSB_TRANSFER_SHORT_NOT_OK){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_SHORT_NOT_OK during a control transfer", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    port->controlFramesPool().enqueue(port->pvCurrentControlFrame);
    port->pvCurrentControlFrame = 0;
    port->addError(UnhandledError);
    return;
  }
  // All is Ok, copy data and enqueue to responses queue
  port->pvCurrentControlFrame->clear();
  port->pvCurrentControlFrame->clearSub();
  controlSetup =  libusb_control_transfer_get_setup(transfer);
  Q_ASSERT(controlSetup != 0);
  port->pvCurrentControlFrame->setbmRequestType(controlSetup->bmRequestType);
  port->pvCurrentControlFrame->setbRequest(controlSetup->bRequest);
  port->pvCurrentControlFrame->setwValue(controlSetup->wValue);
  port->pvCurrentControlFrame->setwIndex(controlSetup->wIndex);
  port->pvCurrentControlFrame->setwLength(controlSetup->wLength);
  controlData = (char*)libusb_control_transfer_get_data(transfer);
  port->pvCurrentControlFrame->append(controlData, transfer->actual_length);
  port->lockMutex();
  port->controlResponseFrames().enqueue(port->pvCurrentControlFrame);
  port->pvCurrentControlFrame = 0;
  port->submitControlQuery();
  port->unlockMutex();
}

QQueue<mdtFrameUsbControl*> &mdtUsbPort::controlFramesPool()
{
  return pvControlFramesPool;
}

QQueue<mdtFrameUsbControl*> &mdtUsbPort::controlQueryFrames()
{
  return pvControlQueryFrames;
}

mdtAbstractPort::error_t mdtUsbPort::cancelControlTransfer()
{
  int err;
  error_t portError;

  // If transfer is null, port was closed
  if(pvControlTransfer == 0){
    return UnhandledError;
  }

  if(pvControlTransferPending){
    ///qDebug() << "cancelControlTransfer() ...";
    err = libusb_cancel_transfer(pvControlTransfer);
    if(err != 0){
      portError = mapUsbError(err, 0);
      if(portError == UnhandledError){
        mdtError e(MDT_USB_IO_ERROR, "libusb_cancel_transfer() failed for control endpoint", mdtError::Error);
        e.setSystemError(err, errorText(err));
        MDT_ERROR_SET_SRC(e, "mdtUsbPort");
        e.commit();
      }
      return portError;
    }
    // Wait until transfer is finished (i.e. until callback is called)
    while(pvControlTransferPending){
      portError = handleUsbEvents();
      if(portError != NoError){
        mdtError e(MDT_USB_IO_ERROR, "handleUsbEvents() failed for control endpoint transfer cancel", mdtError::Error);
        e.setSystemError(err, errorText(err));
        MDT_ERROR_SET_SRC(e, "mdtUsbPort");
        e.commit();
        pvControlTransferPending = false;
        return portError;
      }
    }
  }

  return NoError;
}

QQueue<mdtFrameUsbControl*> &mdtUsbPort::controlResponseFrames()
{
  return pvControlResponseFrames;
}

void mdtUsbPort::requestReadUntilShortPacketReceived()
{
  error_t portError;

  pvReadUntilShortPacketReceivedRequestPending = true;
  // Init the first transfer to wake up thread
  portError = initReadTransfer(pvReadBufferSize);
  if(portError != NoError){
    addError(portError);
  }
}

bool mdtUsbPort::readUntilShortPacketReceivedRequestPending()
{
  if(pvReadUntilShortPacketReceivedRequestPending){
    pvReadUntilShortPacketReceivedRequestPending = false;
    return true;
  }
  return false;
}

void mdtUsbPort::addbTagToAbort(quint8 bTag)
{
  pvbTagsToAbort.enqueue(bTag);
}

int mdtUsbPort::takebTagToAbort()
{
  if(pvbTagsToAbort.isEmpty()){
    return -1;
  }
  return pvbTagsToAbort.dequeue();
}

mdtAbstractPort::error_t mdtUsbPort::initReadTransfer(qint64 maxSize)
{
  ///qDebug() << "mdtUsbPort::initReadTransfer() called, maxSize: " << maxSize;
  int err;
  error_t portError;
  int len;

  // If a transfer is pending, simply return
  if(pvReadTransferPending){
    return NoError;
  }
  // If transfer is null, port was closed
  if(pvReadTransfer == 0){
    return UnhandledError;
  }
  // If nothing is to send, return
  if(maxSize < 1){
    return NoError;
  }
  // Ajust possible max length
  if(maxSize > (qint64)pvReadBufferSize){
    len = pvReadBufferSize;
  }else{
    len = maxSize;
  }
  // Fill the transfer
  ///qDebug() << "mdtUsbPort::initReadTransfer() INIT";
  if(pvReadTransfertType == LIBUSB_TRANSFER_TYPE_BULK){
    libusb_fill_bulk_transfer(pvReadTransfer, pvHandle, pvReadEndpointAddress, (unsigned char*)pvReadBuffer, len, readTransferCallback, this, pvReadTimeout);
  }else if(pvReadTransfertType == LIBUSB_TRANSFER_TYPE_INTERRUPT){
    libusb_fill_interrupt_transfer(pvReadTransfer, pvHandle, pvReadEndpointAddress, (unsigned char*)pvReadBuffer, len, readTransferCallback, this, pvReadTimeout);
  }else{
    mdtError e(MDT_USB_IO_ERROR, "Unknown transfert type", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return UnhandledError;
  }
  // Submit transfert
  err = libusb_submit_transfer(pvReadTransfer); /// \todo Handle retval
  if(err != 0){
    portError = mapUsbError(err, pvReadEndpointAddress);
    if(portError == UnhandledError){
      mdtError e(MDT_USB_IO_ERROR, "libusb_submit_transfer() failed for read endpoint", mdtError::Error);
      e.setSystemError(err, errorText(err));
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
    }
    return portError;
  }
  pvReadTransferPending = true;

  return NoError;
}

void mdtUsbPort::readTransferCallback(struct libusb_transfer *transfer)
{
  ///qDebug() << "mdtUsbPort::readTransferCallback() ...";
  Q_ASSERT(transfer != 0);
  Q_ASSERT(transfer->user_data != 0);

  // Get this instance
  mdtUsbPort *port = static_cast<mdtUsbPort*>(transfer->user_data);
  // Update flag here (else it will stay true on error)
  port->pvReadTransferPending = false;
  // Check if transfer has timed out
  if(transfer->status & LIBUSB_TRANSFER_TIMED_OUT){
    port->updateReadTimeoutState(true);
    port->addError(ReadTimeout);
    return;
  }else{
    port->updateReadTimeoutState(false);
  }
  // Check if transfer was cancelled
  if(transfer->status & LIBUSB_TRANSFER_CANCELLED){
    ///qDebug() << "mdtUsbPort::readTransferCallback() return ReadCanceled";
    port->addError(ReadCanceled);
    return;
  }
  // Check if device is disconnected (not valid if cancelled)
  if(transfer->status & LIBUSB_TRANSFER_NO_DEVICE){
    ///qDebug() << "mdtUsbPort::readTransferCallback(): device disconnected";
    port->addError(Disconnected);
    return;
  }
  // Check if transfer stall
  if(transfer->status & LIBUSB_TRANSFER_STALL){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_STALL", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    port->addError(UnhandledError);
    return;
  }
  // Check if transfer failed (not valid if cancelled)
  if(transfer->status & LIBUSB_TRANSFER_ERROR){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_ERROR", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    port->addError(UnhandledError);
    return;
  }
  // Check transfer overflow
  if(transfer->status & LIBUSB_TRANSFER_OVERFLOW){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_OVERFLOW", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    port->addError(UnhandledError);
    return;
  }
  // Check transfer short frames NOK
  if(transfer->status & LIBUSB_TRANSFER_SHORT_NOT_OK){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_SHORT_NOT_OK", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    port->addError(UnhandledError);
    return;
  }
}

mdtAbstractPort::error_t mdtUsbPort::cancelReadTransfer()
{
  int err;
  error_t portError;

  // If transfer is null, port was closed
  if(pvReadTransfer == 0){
    // Stop thread
    return UnhandledError;
  }

  if(pvReadTransferPending){
    ///qDebug() << "cancelReadTransfer() ...";
    err = libusb_cancel_transfer(pvReadTransfer);
    if(err != 0){
      portError = mapUsbError(err, pvReadEndpointAddress);
      if(portError == UnhandledError){
        mdtError e(MDT_USB_IO_ERROR, "libusb_cancel_transfer() failed", mdtError::Error);
        e.setSystemError(err, errorText(err));
        MDT_ERROR_SET_SRC(e, "mdtUsbPort");
        e.commit();
      }
      return portError;
    }
    // Wait until transfer is finished (i.e. until callback is called)
    while(pvReadTransferPending){
      portError = handleUsbEvents();
      if(portError != NoError){
        mdtError e(MDT_USB_IO_ERROR, "handleUsbEvents() failed for read endpoint transfer cancel", mdtError::Error);
        e.setSystemError(err, errorText(err));
        MDT_ERROR_SET_SRC(e, "mdtUsbPort");
        e.commit();
        pvReadTransferPending = false;
        return portError;
      }
    }
  }

  return NoError;
}

mdtAbstractPort::error_t mdtUsbPort::waitForReadyRead()
{
  return UnhandledError;
}

qint64 mdtUsbPort::read(char *data, qint64 maxSize)
{
  Q_ASSERT(data != 0);

  int retLen;

  // If transfer is null, port was closed
  if(pvReadTransfer == 0){
    // Stop thread
    return UnhandledError;
  }
  // If transfer is pending, simply return 0
  if(pvReadTransferPending){
    return 0;
  }
  // Copy readen data
  retLen = pvReadTransfer->actual_length;
  memcpy(data, pvReadBuffer, retLen);
  // Reset current len
  pvReadTransfer->actual_length = 0;

  return retLen;
}

int mdtUsbPort::readBufferSize() const
{
  return pvReadBufferSize;
}

mdtAbstractPort::error_t mdtUsbPort::initMessageInTransfer()
{
  int err;
  error_t portError;

  // If transfer is pending, simply return
  if(pvMessageInTransferPending){
    return NoError;
  }
  // Endpoint is optional
  if(pvMessageInTransfer == 0){
    return NoError;
  }
  // Fill the transfer
  libusb_fill_interrupt_transfer(pvMessageInTransfer, pvHandle, pvMessageInEndpointAddress, (unsigned char*)pvMessageInBuffer, pvMessageInBufferSize, messageInTransferCallback, (void*)this, 0);
  // Submit transfert
  err = libusb_submit_transfer(pvMessageInTransfer); /// \todo Handle retval
  if(err != 0){
    portError = mapUsbError(err, pvMessageInEndpointAddress);
    if(portError == UnhandledError){
      mdtError e(MDT_USB_IO_ERROR, "libusb_submit_transfer() failed for message IN endpoint", mdtError::Error);
      e.setSystemError(err, errorText(err));
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
    }
    return portError;
  }
  pvMessageInTransferPending = true;

  return NoError;
}

void mdtUsbPort::messageInTransferCallback(struct libusb_transfer *transfer)
{
  ///qDebug() << "mdtUsbPort::messageInTransferCallback() ...";
  Q_ASSERT(transfer != 0);
  Q_ASSERT(transfer->user_data != 0);

  mdtFrame *frame;

  // Get this instance
  mdtUsbPort *port = static_cast<mdtUsbPort*>(transfer->user_data);
  // Update flag here (else it will stay true on error)
  port->pvMessageInTransferPending = false;
  // Transfer complete, take the frame and check flags
  if(port->pvMessageInFramesPool.size() < 1){
    mdtError e(MDT_USB_IO_ERROR, "Message IN frames pool is empty", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    port->addError(ReadPoolEmpty);
    return;
  }
  frame = port->pvMessageInFramesPool.dequeue();
  Q_ASSERT(frame != 0);
  // Check if transfer has timed out
  if(transfer->status & LIBUSB_TRANSFER_TIMED_OUT){
    port->pvMessageInFramesPool.enqueue(frame);
    port->updateReadTimeoutState(true);
    port->addError(ReadTimeout);
    return;
  }else{
    port->updateReadTimeoutState(false);
  }
  // Check if transfer was cancelled
  if(transfer->status & LIBUSB_TRANSFER_CANCELLED){
    ///qDebug() << "mdtUsbPort::messageInTransferCallback() return ReadCanceled";
    port->pvMessageInFramesPool.enqueue(frame);
    port->addError(ReadCanceled);
    return;
  }
  // Check if device is disconnected (not valid if cancelled)
  if(transfer->status & LIBUSB_TRANSFER_NO_DEVICE){
    ///qDebug() << "mdtUsbPort::messageInTransferCallback(): device disconnected";
    port->pvMessageInFramesPool.enqueue(frame);
    port->addError(Disconnected);
    return;
  }
  // Check if transfer stall
  if(transfer->status & LIBUSB_TRANSFER_STALL){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_STALL", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    port->pvMessageInFramesPool.enqueue(frame);
    port->addError(UnhandledError);
    return;
  }
  // Check if transfer failed (not valid if cancelled)
  if(transfer->status & LIBUSB_TRANSFER_ERROR){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_ERROR", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    port->pvMessageInFramesPool.enqueue(frame);
    port->addError(UnhandledError);
    return;
  }
  // Check transfer overflow
  if(transfer->status & LIBUSB_TRANSFER_OVERFLOW){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_OVERFLOW", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    port->pvMessageInFramesPool.enqueue(frame);
    port->addError(UnhandledError);
    return;
  }
  // Check transfer short frames NOK
  if(transfer->status & LIBUSB_TRANSFER_SHORT_NOT_OK){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_SHORT_NOT_OK", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    port->pvMessageInFramesPool.enqueue(frame);
    port->addError(UnhandledError);
    return;
  }
  // All is Ok, copy data and enqueue to messages IN queue
  frame->clear();
  frame->clearSub();
  frame->append(port->pvMessageInBuffer, port->pvMessageInTransfer->actual_length);
  port->pvMessageInFrames.append(frame);
}

mdtAbstractPort::error_t mdtUsbPort::cancelMessageInTransfer()
{
  int err;
  error_t portError;

  // Mesage IN is optional
  if(pvMessageInTransfer == 0){
    return NoError;
  }

  if(pvMessageInTransferPending){
    ///qDebug() << "mdtUsbPort::cancelMessageInTransfer() ...";
    err = libusb_cancel_transfer(pvMessageInTransfer);
    if(err != 0){
      portError = mapUsbError(err, pvMessageInEndpointAddress);
      if(portError == UnhandledError){
        mdtError e(MDT_USB_IO_ERROR, "libusb_cancel_transfer() failed", mdtError::Error);
        e.setSystemError(err, errorText(err));
        MDT_ERROR_SET_SRC(e, "mdtUsbPort");
        e.commit();
      }
      return portError;
    }
    // Wait until transfer is finished (i.e. until callback is called)
    while(pvMessageInTransferPending){
      portError = handleUsbEvents();
      if(portError != NoError){
        mdtError e(MDT_USB_IO_ERROR, "handleUsbEvents() failed for message IN endpoint transfer cancel", mdtError::Error);
        e.setSystemError(err, errorText(err));
        MDT_ERROR_SET_SRC(e, "mdtUsbPort");
        e.commit();
        pvMessageInTransferPending = false;
        return portError;
      }
    }
    ///qDebug() << "mdtUsbPort::cancelMessageInTransfer() DONE";
  }

  return NoError;
}

QQueue<mdtFrame*> &mdtUsbPort::messageInFramesPool()
{
  return pvMessageInFramesPool;
}

QQueue<mdtFrame*> &mdtUsbPort::messageInFrames()
{
  return pvMessageInFrames;
}

void mdtUsbPort::addFrameToWrite(mdtFrame *frame)
{
  Q_ASSERT(frame != 0);

  error_t portError;

  pvWriteFrames.enqueue(frame);
  // If this is the only frame that thread
  //  has to write, it's possible that it is waiting.
  // In this case, we must submit the first transfer to wake it up.
  if(pvWriteFrames.size() == 1){
    portError = initWriteTransfer(frame->data(), frame->size());
    if(portError != NoError){
      addError(portError);
    }
  }
}

mdtAbstractPort::error_t mdtUsbPort::initWriteTransfer(const char *data, qint64 maxSize)
{
  ///qDebug() << "mdtUsbPort::initWriteTransfer() called ...";
  Q_ASSERT(data != 0);
  Q_ASSERT(maxSize >= 0);

  int err;
  error_t portError;
  int len;

  // If transfer is pending, simply return
  if(pvWriteTransferPending){
    return NoError;
  }
  // If transfer is null, port was closed
  if(pvWriteTransfer == 0){
    // Stop thread
    return UnhandledError;
  }
  // Ajust possible max length
  if(maxSize > (qint64)pvWriteBufferSize){
    len = pvWriteBufferSize;
  }else{
    len = maxSize;
  }
  // Copy data to write
  memcpy(pvWriteBuffer, data, len);
  // Fill the transfer
  ///qDebug() << "mdtUsbPort::initWriteTransfer() , maxSize: " << maxSize << " , data: " << data;
  if(pvWriteTransfertType == LIBUSB_TRANSFER_TYPE_BULK){
    libusb_fill_bulk_transfer(pvWriteTransfer, pvHandle, pvWriteEndpointAddress, (unsigned char*)pvWriteBuffer, len, writeTransferCallback, (void*)this, pvWriteTimeout);
  }else if(pvReadTransfertType == LIBUSB_TRANSFER_TYPE_INTERRUPT){
    libusb_fill_interrupt_transfer(pvWriteTransfer, pvHandle, pvWriteEndpointAddress, (unsigned char*)pvWriteBuffer, len, writeTransferCallback, (void*)this, pvWriteTimeout);
  }else{
    mdtError e(MDT_USB_IO_ERROR, "Unknown transfer type", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return UnhandledError;
  }
  // Submit transfer
  err = libusb_submit_transfer(pvWriteTransfer);
  if(err != 0){
    portError = mapUsbError(err, pvWriteEndpointAddress);
    if(portError == UnhandledError){
      mdtError e(MDT_USB_IO_ERROR, "libusb_submit_transfer() failed", mdtError::Error);
      e.setSystemError(err, errorText(err));
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
    }
    return portError;
  }
  pvWriteTransferPending = true;

  return NoError;
}

void mdtUsbPort::writeTransferCallback(struct libusb_transfer *transfer)
{
  Q_ASSERT(transfer != 0);
  Q_ASSERT(transfer->user_data != 0);

  qDebug() << "mdtUsbPort::writeTransferCallback() , actual_length: " << transfer->actual_length;

  // Get this instance
  mdtUsbPort *port = static_cast<mdtUsbPort*>(transfer->user_data);
  // Update flag here (else it will stay true on error)
  port->pvWriteTransferPending = false;
  // Check if transfer has timed out
  if(transfer->status & LIBUSB_TRANSFER_TIMED_OUT){
    ///port->updateWriteTimeoutState(true);
    port->addError(WriteTimeout);
    return;
  }/**else{
    port->updateWriteTimeoutState(false);
  }*/
  // Check if transfer was cancelled
  if(transfer->status & LIBUSB_TRANSFER_CANCELLED){
    ///qDebug() << "mdtUsbPort::writeTransferCallback() return WriteCanceled";
    port->addError(WriteCanceled);
    return;
  }
  // Check if device is disconnected (not valid if cancelled)
  if(transfer->status & LIBUSB_TRANSFER_NO_DEVICE){
    ///qDebug() << "mdtUsbPort::writeTransferCallback(): device disconnected";
    port->addError(Disconnected);
    return;
  }
  // Check if transfer stall
  if(transfer->status & LIBUSB_TRANSFER_STALL){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_STALL", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    port->addError(UnhandledError);
    return;
  }
  // Check if transfer failed (not valid if cancelled)
  if(transfer->status & LIBUSB_TRANSFER_ERROR){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_ERROR", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    port->addError(UnhandledError);
    return;
  }
  // Check transfer overflow
  if(transfer->status & LIBUSB_TRANSFER_OVERFLOW){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_OVERFLOW", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    port->addError(UnhandledError);
    return;
  }
  // Check transfer short frames NOK
  if(transfer->status & LIBUSB_TRANSFER_SHORT_NOT_OK){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_SHORT_NOT_OK", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    port->addError(UnhandledError);
    return;
  }
  
  qDebug() << "mdtUsbPort::writeTransferCallback() DONE";
  
}

mdtAbstractPort::error_t mdtUsbPort::cancelWriteTransfer()
{
  int err;
  error_t portError;

  // If transfer is null, port was closed
  if(pvWriteTransfer == 0){
    // Stop thread
    return UnhandledError;
  }

  if(pvWriteTransferPending){
    ///qDebug() << "cancelWriteTransfer() ...";
    err = libusb_cancel_transfer(pvWriteTransfer);
    if(err != 0){
      portError = mapUsbError(err, pvWriteEndpointAddress);
      if(portError == UnhandledError){
        mdtError e(MDT_USB_IO_ERROR, "libusb_cancel_transfer() failed", mdtError::Error);
        e.setSystemError(err, errorText(err));
        MDT_ERROR_SET_SRC(e, "mdtUsbPort");
        e.commit();
      }
      return portError;
    }
    // Wait until transfer is finished (i.e. until callback is called)
    while(pvWriteTransferPending){
      portError = handleUsbEvents();
      if(portError != NoError){
        mdtError e(MDT_USB_IO_ERROR, "handleUsbEvents() failed for write endpoint transfer cancel", mdtError::Error);
        e.setSystemError(err, errorText(err));
        MDT_ERROR_SET_SRC(e, "mdtUsbPort");
        e.commit();
        pvWriteTransferPending = false;
        return portError;
      }
    }
  }

  return NoError;
}


mdtAbstractPort::error_t mdtUsbPort::waitEventWriteReady()
{
  return UnhandledError;
}

qint64 mdtUsbPort::write(const char* /*data*/, qint64 /*maxSize*/)
{
  int retLen;

  // If transfer is pending, simply return 0
  if(pvWriteTransferPending){
    return 0;
  }
  // If transfer is null, port was closed
  if(pvWriteTransfer == 0){
    // Stop thread
    return UnhandledError;
  }
  // Get size of written data
  retLen = pvWriteTransfer->actual_length;
  // Reset written data len
  pvWriteTransfer->actual_length = 0;

  return retLen;
}

void mdtUsbPort::addError(mdtAbstractPort::error_t error)
{
  pvErrors.enqueue(error);
}

QList<mdtAbstractPort::error_t> mdtUsbPort::lastErrors()
{
  QList<error_t> errors;

  errors = pvErrors;
  pvErrors.clear();

  return errors;
}

mdtAbstractPort::error_t mdtUsbPort::cancelTransfers()
{
  error_t portError;

  ///qDebug() << "mdtUsbPort::cancelTransfers() ...";
  portError = cancelControlTransfer();
  if((portError != NoError)&&(portError != ControlCanceled)){
    return portError;
  }
  portError = cancelWriteTransfer();
  if((portError != NoError)&&(portError != WriteCanceled)){
    return portError;
  }
  portError = cancelReadTransfer();
  if((portError != NoError)&&(portError != ReadCanceled)){
    return portError;
  }
  portError = cancelMessageInTransfer();
  if((portError != NoError)&&(portError != ReadCanceled)){
    return portError;
  }
  ///qDebug() << "mdtUsbPort::cancelTransfers() DONE";

  return NoError;
}

mdtAbstractPort::error_t mdtUsbPort::handleUsbEvents(struct timeval *timeout, quint8 endpoint)
{
  int err;
  error_t portError;

  unlockMutex();
  if(timeout == 0){
    if(endpoint == pvWriteEndpointAddress){
      err = libusb_handle_events_timeout(pvLibusbContext, &pvWriteTimeoutTv);
    }else{
      err = libusb_handle_events_timeout(pvLibusbContext, &pvReadTimeoutTv);
    }
  }else{
    err = libusb_handle_events_timeout(pvLibusbContext, timeout);
  }
  lockMutex();
  /// \todo Think that timeout has no sense here, should be filtered
  if(err != 0){
    portError = mapUsbError(err, endpoint);
    if(portError == UnhandledError){
      mdtError e(MDT_USB_IO_ERROR, "libusb_handle_events_timeout() failed", mdtError::Error);
      e.setSystemError(err, errorText(err));
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
    }
    return portError;
  }

  return NoError;
}

mdtAbstractPort::error_t mdtUsbPort::handleUsbEvents()
{
  int err;

  unlockMutex();
  err = libusb_handle_events(pvLibusbContext);
  lockMutex();
  switch(err){
    case 0:
      return NoError;
    case LIBUSB_ERROR_ACCESS:
      return PortAccess;
    case LIBUSB_ERROR_NO_DEVICE:
      return Disconnected;
    case LIBUSB_ERROR_NOT_FOUND:
      return Disconnected;
    case LIBUSB_ERROR_BUSY:
      return Disconnected;
    case LIBUSB_ERROR_TIMEOUT:
      // We ignore timeout here, they are detected by callbacks
      return NoError;
    case LIBUSB_ERROR_INTERRUPTED:
      // We ignore interrupted error here, they are detected by callbacks
      return NoError;
  }
  // Here, we have a unhandled error
  mdtError e(MDT_USB_IO_ERROR, "libusb_handle_events() failed with a unhandled error", mdtError::Error);
  e.setSystemError(err, errorText(err));
  MDT_ERROR_SET_SRC(e, "mdtUsbPort");
  e.commit();

  return UnhandledError;
}

quint8 mdtUsbPort::currentReadEndpointAddress() const
{
  return pvReadEndpointAddress;
}

quint8 mdtUsbPort::currentWriteEndpointAddress() const
{
  return pvWriteEndpointAddress;
}

mdtAbstractPort::error_t mdtUsbPort::pvOpen()
{
  Q_ASSERT(!isOpen());

  QStringList lst;
  QStringList item;
  QMap<QString, QString> portNameAttributes;
  QString str;
  bool ok;
  quint16 vid = 0;
  quint16 pid = 0;
  QString sid;
  libusb_device **devicesList;
  ssize_t devicesCount = 0;
  mdtUsbDeviceDescriptor deviceDescriptor;
  ssize_t i = 0;
  int err;
  int j;
  error_t portError;

  // Extract items from port name
  lst = pvPortName.split(":");
  for(j=0; j<lst.size(); j++){
    item = lst.at(j).split("=");
    if(item.size() != 2){
      mdtError e(MDT_USB_IO_ERROR, "Portname has wrong format. Expected X=Y (port name: " + pvPortName + ")", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return SetupError;
    }
    portNameAttributes.insert(item.at(0), item.at(1));
  }
  // Extract VID
  str = portNameAttributes.value("VID", "");
  if(str.left(2) == "0x"){
    vid = str.toUInt(&ok, 16);
  }else{
    vid = str.toUInt(&ok, 10);
  }
  if(!ok){
    mdtError e(MDT_USB_IO_ERROR, "Cannot extract vendor ID in " + pvPortName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return SetupError;
  }
  // Extract PID
  str = portNameAttributes.value("PID", "");
  if(str.left(2) == "0x"){
    pid = str.toUInt(&ok, 16);
  }else{
    pid = str.toUInt(&ok, 10);
  }
  if(!ok){
    mdtError e(MDT_USB_IO_ERROR, "Cannot extract product ID in " + pvPortName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return SetupError;
  }
  // Extract SID
  sid = portNameAttributes.value("SID", "");
  // Extract bInterfaceNumber
  pvbInterfaceNumber = 0;
  str = portNameAttributes.value("bInterfaceNumber", "");
  if(!str.isEmpty()){
    if(str.left(2) == "0x"){
      pvbInterfaceNumber = str.toInt(&ok, 16);
    }else{
      pvbInterfaceNumber = str.toInt(&ok, 10);
    }
    if(!ok){
      mdtError e(MDT_USB_IO_ERROR, "Cannot extract bInterfaceNumber in " + pvPortName + " , choosing 0", mdtError::Warning);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      pvbInterfaceNumber = 0;
    }
  }
  // If no serial ID is given, use libusb open method
  if(sid.isEmpty()){
    pvHandle = libusb_open_device_with_vid_pid(pvLibusbContext, vid, pid);
    if(pvHandle == 0){
      mdtError e(MDT_USB_IO_ERROR, "Cannot open port with device " + pvPortName, mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return PortNotFound;
    }
    return NoError;
  }else{
    // Get devices list
    devicesCount = libusb_get_device_list(pvLibusbContext, &devicesList);
    if(devicesCount < 0){
      mdtError e(MDT_USB_IO_ERROR, "Cannot list devices", mdtError::Error);
      e.setSystemError(devicesCount, errorText(devicesCount));
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return UnhandledError;
    }
    // Search ...
    for(i=0; i<devicesCount; i++){
      err = deviceDescriptor.fetchAttributes(devicesList[i], true);
      if(err != 0){
        mdtError e(MDT_USB_IO_ERROR, "Device attributes fetch failed for port " + pvPortName, mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtUsbtmcPortManager");
        e.commit();
        continue;
      }
      // Device found ?
      if((deviceDescriptor.idVendor() == vid)&&(deviceDescriptor.idProduct() == pid)&&(deviceDescriptor.serialNumber() == sid)){
        // Open port
        err = libusb_open(devicesList[i], &pvHandle);
        if(err != 0){
          portError = mapUsbError(err, true);
          mdtError e(MDT_USB_IO_ERROR, "Cannot open port with device " + pvPortName, mdtError::Error);
          e.setSystemError(err, errorText(err));
          MDT_ERROR_SET_SRC(e, "mdtUsbPort");
          e.commit();
          // Release ressources and return
          libusb_free_device_list(devicesList, 1);
          return portError;
        }
        // Release ressources and return
        libusb_free_device_list(devicesList, 1);
        return NoError;
      }
    }
    // Release ressources
    libusb_free_device_list(devicesList, 1);
  }

  // Requested port not found
  mdtError e(MDT_USB_IO_ERROR, "Cannot find device " + pvPortName, mdtError::Error);
  MDT_ERROR_SET_SRC(e, "mdtUsbPort");
  e.commit();
  return PortNotFound;
}

void mdtUsbPort::pvClose()
{
  Q_ASSERT(isOpen());
  Q_ASSERT(pvHandle != 0);

  int err;
  bool devicePresent = true;

  // Release port
  err = libusb_release_interface(pvHandle, pvbInterfaceNumber);
  switch(err){
    case 0:
      break;
    ///case LIBUSB_ERROR_NOT_FOUND:
      ///break;
    case LIBUSB_ERROR_NO_DEVICE:
      devicePresent = false;
      break;
    default:
      mdtError e(MDT_USB_IO_ERROR, "libubs_release_interface() failed", mdtError::Error);
      e.setSystemError(err, errorText(err));
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
  }
  // Re-attach possibly detached driver in pvSetup()
#ifdef Q_OS_LINUX
  if(devicePresent){
    err = libusb_attach_kernel_driver(pvHandle, pvbInterfaceNumber);
    switch(err){
      case 0:
        break;
      case LIBUSB_ERROR_NOT_FOUND:  // No driver was detached
        break;
      case LIBUSB_ERROR_INVALID_PARAM:
      {
        mdtError e(MDT_USB_IO_ERROR, "Cannot re-attach kernel driver (interface not found) on device " + pvPortName, mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtUsbPort");
        e.commit();
        break;
      }
      case LIBUSB_ERROR_NO_DEVICE:
      {
        mdtError e(MDT_USB_IO_ERROR, "Cannot re-attach kernel driver (device not found/disconnected) on device " + pvPortName, mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtUsbPort");
        e.commit();
        break;
      }
      case LIBUSB_ERROR_NOT_SUPPORTED:
      {
        mdtError e(MDT_USB_IO_ERROR, "Cannot re-attach kernel driver (unload is not supported on current platform) on device " + pvPortName, mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtUsbPort");
        e.commit();
        break;
      }
      default:
      {
        mdtError e(MDT_USB_IO_ERROR, "Cannot re-attach kernel driver (unhandled error) on device " + pvPortName, mdtError::Error);
        e.setSystemError(err, errorText(err));
        MDT_ERROR_SET_SRC(e, "mdtUsbPort");
        e.commit();
        break;
      }
    }
  }
#endif
  // Close port
  libusb_close(pvHandle);
  pvHandle = 0;
  // Free memory
  ///qDebug() << "mdtUsbPort::pvClose(): delete pvControlBuffer ...";
  delete[] pvControlBuffer;
  pvControlBuffer = 0;
  ///qDebug() << "mdtUsbPort::pvClose(): delete pvControlBuffer DONE";
  pvControlBufferSize = 0;
  if(pvControlTransfer != 0){
    ///qDebug() << "mdtUsbPort::pvClose(): free pvControlTransfer ...";
    libusb_free_transfer(pvControlTransfer);
    pvControlTransfer = 0;
    ///qDebug() << "mdtUsbPort::pvClose(): free pvControlTransfer DONE";
  }
  ///qDebug() << "mdtUsbPort::pvClose(): delete pvControlFramesPool ...";
  qDeleteAll(pvControlFramesPool);
  pvControlFramesPool.clear();
  ///qDebug() << "mdtUsbPort::pvClose(): delete pvControlFramesPool DONE";
  ///qDebug() << "mdtUsbPort::pvClose(): delete pvControlQueryFrames ...";
  qDeleteAll(pvControlQueryFrames);
  pvControlQueryFrames.clear();
  ///qDebug() << "mdtUsbPort::pvClose(): delete pvControlQueryFrames DONE";
  ///qDebug() << "mdtUsbPort::pvClose(): delete pvControlResponseFrames ...";
  qDeleteAll(pvControlResponseFrames);
  pvControlResponseFrames.clear();
  ///qDebug() << "mdtUsbPort::pvClose(): delete pvControlResponseFrames DONE";
  ///qDebug() << "mdtUsbPort::pvClose(): delete pvReadBuffer ...";
  delete[] pvReadBuffer;
  pvReadBuffer = 0;
  ///qDebug() << "mdtUsbPort::pvClose(): delete pvReadBuffer DONE";
  pvReadBufferSize = 0;
  if(pvReadTransfer != 0){
    ///qDebug() << "mdtUsbPort::pvClose(): free pvReadTransfer ...";
    libusb_free_transfer(pvReadTransfer);
    pvReadTransfer = 0;
    ///qDebug() << "mdtUsbPort::pvClose(): free pvReadTransfer DONE";
  }
  ///qDebug() << "mdtUsbPort::pvClose(): delete pvWriteBuffer ...";
  delete[] pvWriteBuffer;
  pvWriteBuffer = 0;
  ///qDebug() << "mdtUsbPort::pvClose(): delete pvWriteBuffer DONE";
  pvWriteBufferSize = 0;
  if(pvWriteTransfer != 0){
    libusb_free_transfer(pvWriteTransfer);
    pvWriteTransfer = 0;
  }
  ///qDebug() << "mdtUsbPort::pvClose(): delete pvMessageInBuffer ...";
  delete[] pvMessageInBuffer;
  pvMessageInBuffer = 0;
  ///qDebug() << "mdtUsbPort::pvClose(): delete pvMessageInBuffer DONE";
  pvMessageInBufferSize = 0;
  if(pvMessageInTransfer != 0){
    libusb_free_transfer(pvMessageInTransfer);
    pvMessageInTransfer = 0;
  }
  qDeleteAll(pvMessageInFramesPool);
  pvMessageInFramesPool.clear();
  qDeleteAll(pvMessageInFrames);
  pvMessageInFrames.clear();
}

mdtAbstractPort::error_t mdtUsbPort::pvSetup()
{
  Q_ASSERT(isOpen());
  Q_ASSERT(pvHandle != 0);
  Q_ASSERT(pvControlBufferSize == 0);
  Q_ASSERT(pvControlTransfer == 0);
  Q_ASSERT(pvReadBufferSize == 0);
  Q_ASSERT(pvReadTransfer == 0);
  Q_ASSERT(pvWriteBufferSize == 0);
  Q_ASSERT(pvWriteTransfer == 0);
  Q_ASSERT(pvMessageInBufferSize == 0);
  Q_ASSERT(pvMessageInTransfer == 0);

  libusb_device *device;
  mdtUsbDeviceDescriptor deviceDescriptor;
  mdtUsbEndpointDescriptor *bulkEndpointDescriptor;
  mdtUsbEndpointDescriptor *interruptEndpointDescriptor;
  int err;

  // Search devices endpoints
  device = libusb_get_device(pvHandle);
  Q_ASSERT(device != 0);
  err = deviceDescriptor.fetchAttributes(device, true);
  if(err != 0){
    switch(err){
      case LIBUSB_ERROR_NOT_FOUND:
        return PortNotFound;
      default:
        return UnhandledError;
    }
  }
  // Find device's output endpoints
  bulkEndpointDescriptor = deviceDescriptor.firstBulkOutEndpoint(0, 0, true);
  interruptEndpointDescriptor = deviceDescriptor.firstInterruptOutEndpoint(0, 0, true);
  if((bulkEndpointDescriptor == 0)&&(interruptEndpointDescriptor == 0)){
    mdtError e(MDT_USB_IO_ERROR, "Found no output endpoint for data in device " + pvPortName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return SetupError;
  }
  if((bulkEndpointDescriptor != 0)&&(interruptEndpointDescriptor != 0)){
    mdtError e(MDT_USB_IO_ERROR, "Found output bulk and interrupt, choose bulk out endpoint for data in device " + pvPortName, mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    // Choose bulk out
    interruptEndpointDescriptor = 0;
  }
  if(bulkEndpointDescriptor != 0){
    pvWriteBufferSize = bulkEndpointDescriptor->transactionsCountPerMicroFrame();
    if(pvWriteBufferSize < 1){
      pvWriteBufferSize = 1;
    }
    pvWriteBufferSize *= bulkEndpointDescriptor->maxPacketSize();
    pvWriteEndpointAddress = bulkEndpointDescriptor->address() | LIBUSB_ENDPOINT_OUT;
    pvWriteTransfertType = LIBUSB_TRANSFER_TYPE_BULK;
    //////qDebug() << "Bulk OUT address: 0x" << hex << pvWriteEndpointAddress;
    //////qDebug() << " Max packet size: " << pvWriteBufferSize;
  }
  if(interruptEndpointDescriptor != 0){
    pvWriteBufferSize = interruptEndpointDescriptor->transactionsCountPerMicroFrame();
    if(pvWriteBufferSize < 1){
      pvWriteBufferSize = 1;
    }
    pvWriteBufferSize *= interruptEndpointDescriptor->maxPacketSize();
    pvWriteEndpointAddress = interruptEndpointDescriptor->address() | LIBUSB_ENDPOINT_OUT;
    pvWriteTransfertType = LIBUSB_TRANSFER_TYPE_INTERRUPT;
    //////qDebug() << "Interrupt OUT address: 0x" << hex << pvWriteEndpointAddress;
    //////qDebug() << " Max packet size: " << pvWriteBufferSize;
  }
  // Find device's input endpoints
  bulkEndpointDescriptor = deviceDescriptor.firstBulkInEndpoint(0, 0, true);
  interruptEndpointDescriptor = deviceDescriptor.firstInterruptInEndpoint(0, 0, true);
  if((bulkEndpointDescriptor == 0)&&(interruptEndpointDescriptor == 0)){
    mdtError e(MDT_USB_IO_ERROR, "Found no input endpoint for data in device " + pvPortName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return SetupError;
  }
  if(bulkEndpointDescriptor != 0){
    pvReadBufferSize = bulkEndpointDescriptor->transactionsCountPerMicroFrame();
    if(pvReadBufferSize < 1){
      pvReadBufferSize = 1;
    }
    pvReadBufferSize *= bulkEndpointDescriptor->maxPacketSize();
    pvReadEndpointAddress = bulkEndpointDescriptor->address() | LIBUSB_ENDPOINT_IN;
    pvReadTransfertType = LIBUSB_TRANSFER_TYPE_BULK;
    //////qDebug() << "Bulk IN address: 0x" << hex << pvReadEndpointAddress;
    //////qDebug() << " Max packet size: " << pvReadBufferSize;
  }
  if(interruptEndpointDescriptor != 0){
    //////qDebug() << "Interrupt IN address: " << interruptEndpointDescriptor->address();
    //////qDebug() << " Max packet size: " << interruptEndpointDescriptor->maxPacketSize();
    // Some device have a Bulk IN + Interrupt IN
    if(bulkEndpointDescriptor != 0){
      //////qDebug() << "Additionnal interrupt IN , currently not supported..";
      // Use this interrupt IN endpoint as message IN
      pvMessageInBufferSize = interruptEndpointDescriptor->transactionsCountPerMicroFrame();
      if(pvMessageInBufferSize < 1){
        pvMessageInBufferSize = 1;
      }
      pvMessageInBufferSize *= interruptEndpointDescriptor->maxPacketSize();
      pvMessageInEndpointAddress = interruptEndpointDescriptor->address() | LIBUSB_ENDPOINT_IN;
      //////qDebug() << "Interrupt IN (for messages) address: 0x" << hex << pvMessageInEndpointAddress;
      //////qDebug() << " Max packet size: " << pvMessageInBufferSize;
    }else{
      // Use this interrupt IN endpoint as read
      pvReadBufferSize = interruptEndpointDescriptor->transactionsCountPerMicroFrame();
      if(pvReadBufferSize < 1){
        pvReadBufferSize = 1;
      }
      pvReadBufferSize *= interruptEndpointDescriptor->maxPacketSize();
      pvReadEndpointAddress = interruptEndpointDescriptor->address() | LIBUSB_ENDPOINT_IN;
      pvReadTransfertType = LIBUSB_TRANSFER_TYPE_INTERRUPT;
      //////qDebug() << "Interrupt IN address: 0x" << hex << pvReadEndpointAddress;
      //////qDebug() << " Max packet size: " << pvReadBufferSize;
    }
  }
  // Unload possibly loaded driver that uses the device
#ifdef Q_OS_LINUX
  err = libusb_detach_kernel_driver(pvHandle, pvbInterfaceNumber);
  switch(err){
    case 0:
      break;
    case LIBUSB_ERROR_NOT_FOUND:  // No driver loaded
      break;
    case LIBUSB_ERROR_INVALID_PARAM:
    {
      mdtError e(MDT_USB_IO_ERROR, "Cannot detach kernel driver (interface not found) on device " + pvPortName, mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return PortNotFound;
    }
    case LIBUSB_ERROR_NO_DEVICE:
    {
      mdtError e(MDT_USB_IO_ERROR, "Cannot detach kernel driver (device not found/disconnected) on device " + pvPortName, mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return PortNotFound;
    }
    case LIBUSB_ERROR_NOT_SUPPORTED:
    {
      mdtError e(MDT_USB_IO_ERROR, "Cannot detach kernel driver (unload is not supported on current platform) on device " + pvPortName, mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return SetupError;
    }
    default:
    {
      mdtError e(MDT_USB_IO_ERROR, "Cannot detach kernel driver (unhandled error) on device " + pvPortName, mdtError::Error);
      e.setSystemError(err, errorText(err));
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return SetupError;
    }
  }
#endif
  // Claim the interface
  err = libusb_claim_interface(pvHandle, pvbInterfaceNumber);
  switch(err){
    case 0:
      break;
    case LIBUSB_ERROR_NOT_FOUND:
    {
      mdtError e(MDT_USB_IO_ERROR, "Cannot claim interface (interface not found) on device " + pvPortName, mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return PortNotFound;
    }
    case LIBUSB_ERROR_BUSY:
    {
      mdtError e(MDT_USB_IO_ERROR, "Cannot claim interface (interface busy) on device " + pvPortName, mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return PortLocked;
    }
    case LIBUSB_ERROR_NO_DEVICE:
    {
      mdtError e(MDT_USB_IO_ERROR, "Cannot claim interface (device not found/disconnected) on device " + pvPortName, mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return PortNotFound;
    }
    default:
    {
      mdtError e(MDT_USB_IO_ERROR, "Cannot claim interface (unhandled error) on device " + pvPortName, mdtError::Error);
      e.setSystemError(err, errorText(err));
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return UnhandledError;
    }
  }
  // Alloc control transfer and buffer
  pvControlTransfer = libusb_alloc_transfer(0);
  if(pvControlTransfer == 0){
    mdtError e(MDT_USB_IO_ERROR, "libusb_alloc_transfert() failed for control endpoint", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return UnhandledError;
  }
  /// \todo define a real size for control endpoint
  pvControlBufferSize = 1024;
  pvControlBuffer = new char[pvControlBufferSize];
  /// \todo How many frames in control pool ??
  pvControlFramesPool.enqueue(new mdtFrameUsbControl);
  pvCurrentControlFrame = 0;
  // Alloc read transfer and buffer
  pvReadTransfer = libusb_alloc_transfer(0);
  if(pvReadTransfer == 0){
    mdtError e(MDT_USB_IO_ERROR, "libusb_alloc_transfert() failed for read input", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return UnhandledError;
  }
  if(pvReadBufferSize > 0){
    pvReadBuffer = new char[pvReadBufferSize];
  }
  // Alloc write transfer and buffer
  pvWriteTransfer = libusb_alloc_transfer(0);
  if(pvWriteTransfer == 0){
    mdtError e(MDT_USB_IO_ERROR, "libusb_alloc_transfert() failed for write output", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return UnhandledError;
  }
  if(pvWriteBufferSize > 0){
    pvWriteBuffer = new char[pvWriteBufferSize];
  }
  // Alloc message IN transfert, buffer and frames pool
  if(pvMessageInBufferSize > 0){
    pvMessageInTransfer = libusb_alloc_transfer(0);
    if(pvMessageInTransfer == 0){
      mdtError e(MDT_USB_IO_ERROR, "libusb_alloc_transfert() failed for message input", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return UnhandledError;
    }
    // Internal buffer
    pvMessageInBuffer = new char[pvMessageInBufferSize];
    // Frames pool - We alloc same frames count as read pool
    mdtFrame *msgInFrame;
    for(int i=0; i<config().readQueueSize(); i++){
      msgInFrame = new mdtFrame;
      msgInFrame->reserve(pvMessageInBufferSize);
      msgInFrame->setDirectlyComplete(true);
      pvMessageInFramesPool.enqueue(msgInFrame);
    }
  }

  /// \note Current version needs 1 data IN + 1 data OUT
  Q_ASSERT(pvReadBuffer != 0);
  Q_ASSERT(pvWriteBuffer != 0);

  /// \todo Setup functions ......

  setReadTimeout(config().readTimeout());
  setWriteTimeout(config().writeTimeout());

  // Set some flags
  pvControlTransferPending = false;
  pvReadTransferPending = false;
  pvReadUntilShortPacketReceivedRequestPending = false;
  pvWriteTransferPending = false;
  pvMessageInTransferPending = false;

  return NoError;
}

void mdtUsbPort::pvFlushIn()
{
  while(pvMessageInFrames.size() > 0){
    pvMessageInFramesPool.enqueue(pvMessageInFrames.dequeue());
  }
  while(pvControlResponseFrames.size() > 0){
    pvControlFramesPool.enqueue(pvControlResponseFrames.dequeue());
  }
}

void mdtUsbPort::pvFlushOut()
{
  while(pvControlQueryFrames.size() > 0){
    pvControlFramesPool.enqueue(pvControlQueryFrames.dequeue());
  }
}

mdtAbstractPort::error_t mdtUsbPort::mapUsbError(int error, quint8 endpoint)
{
  switch(error){
    case LIBUSB_ERROR_IO:
      return UnhandledError;
    case LIBUSB_ERROR_INVALID_PARAM:
      return UnhandledError;
    case LIBUSB_ERROR_ACCESS:
      return PortAccess;
    case LIBUSB_ERROR_NO_DEVICE:
      return Disconnected;
    case LIBUSB_ERROR_NOT_FOUND:
      return Disconnected;
    case LIBUSB_ERROR_BUSY:
      return Disconnected;
    case LIBUSB_ERROR_TIMEOUT:
      if(endpoint == 0){
        return WriteTimeout;
      }else if(endpoint == pvReadEndpointAddress){
        updateReadTimeoutState(true);
        return ReadTimeout;
      }else if(endpoint == pvWriteEndpointAddress){
        ///updateWriteTimeoutState(true);
        return WriteTimeout;
      }else{
        mdtError e(MDT_USB_IO_ERROR, "Unable to define timeout type (unknown endpoint)", mdtError::Error);
        MDT_ERROR_SET_SRC(e, "mdtUsbPort");
        e.commit();
        return UnhandledError;
      }
    case LIBUSB_ERROR_OVERFLOW:
      return UnhandledError;
    case LIBUSB_ERROR_PIPE:
      return UnhandledError;
    case LIBUSB_ERROR_INTERRUPTED:
      if(endpoint == 0){
        return ControlCanceled;
      }else if(endpoint == pvReadEndpointAddress){
        return ReadCanceled;
      }else if(endpoint == pvWriteEndpointAddress){
        return WriteCanceled;
      }else{
        // Can be additionnal IN interrupt endpoint
        /// \todo Is this ok ?
        return ControlCanceled;
      }
    case LIBUSB_ERROR_NO_MEM:
      return UnhandledError;
    case LIBUSB_ERROR_NOT_SUPPORTED:
      return UnhandledError;
    default:
      return UnhandledError;
  }
}

QString mdtUsbPort::errorText(int errorCode) const
{
  switch(errorCode){
    case LIBUSB_ERROR_IO:
      return tr("Input/output error");
    case LIBUSB_ERROR_INVALID_PARAM:
      return tr("Invalid parameter");
    case LIBUSB_ERROR_ACCESS:
      return tr("Access denied (insufficient permissions)");
    case LIBUSB_ERROR_NO_DEVICE:
      return tr("No such device (it may have been disconnected)");
    case LIBUSB_ERROR_NOT_FOUND:
      return tr("Entity not found");
    case LIBUSB_ERROR_BUSY:
      return tr("Resource busy");
    case LIBUSB_ERROR_TIMEOUT:
      return tr("Operation timed out");
    case LIBUSB_ERROR_OVERFLOW:
      return tr("Overflow");
    case LIBUSB_ERROR_PIPE:
      return tr("Pipe error");
    case LIBUSB_ERROR_INTERRUPTED:
      return tr("System call interrupted (perhaps due to signal)");
    case LIBUSB_ERROR_NO_MEM:
      return tr("Insufficient memory");
    case LIBUSB_ERROR_NOT_SUPPORTED:
      return tr("Operation not supported or unimplemented on this platform");
    default:
      return tr("Unknown error");
  }
}
