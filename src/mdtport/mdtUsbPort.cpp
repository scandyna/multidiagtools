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
  pvControlTransferComplete = 0;
  pvMessageInBuffer = 0;
  pvMessageInBufferSize = 0;
  pvMessageInEndpointAddress = 0;
  pvMessageInTransfer = 0;
  pvMessageInTransferPending = false;
  pvMessageInTransferComplete = 0;
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
  ///pvCancelWrite = false;
  pvWriteFrameAvailable.wakeAll();
}

QQueue<mdtFrameUsbControl*> &mdtUsbPort::controlFramesPool()
{
  return pvControlFramesPool;
}

mdtAbstractPort::error_t mdtUsbPort::handleControlQueries()
{
  int err;
  mdtFrameUsbControl *frame;
  error_t portError;

  // Check if a query frame exists
  if(pvControlQueryFrames.size() < 1){
    return NoError;
  }
  // If a transfer is pending, do nothing
  if(pvControlTransferPending){
    return NoError;
  }
  // If transfer is null, port was closed
  if(pvControlTransfer == 0){
    return UnhandledError;
  }
  // Get a frame
  frame = pvControlQueryFrames.head();
  Q_ASSERT(frame != 0);
  // Check that we have a large enough buffer
  if(pvControlBufferSize < frame->size()){
    mdtError e(MDT_USB_IO_ERROR, "Unable to init control transfert, internal buffer is to small", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return UnhandledError;
  }
  // Copy data to write
  memcpy(pvControlBuffer, frame->data(), frame->size());
  // Fill the transfer
  pvControlTransferComplete = 0;
  libusb_fill_control_transfer(pvControlTransfer, pvHandle, (unsigned char*)pvControlBuffer, transferCallback, (void*)&pvControlTransferComplete, pvWriteTimeout);
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
    return portError;
  }
  pvControlTransferPending = true;

  return NoError;
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
    qDebug() << "cancelControlTransfer() ...";
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
    pvControlTransferPending = false;
  }

  return NoError;
}

mdtAbstractPort::error_t mdtUsbPort::handleControlResponses()
{
  int *pComplete;
  mdtFrameUsbControl *frame;
  libusb_control_setup *controlSetup;
  char *controlData;

  // If no query request is pending, return
  if(pvControlQueryFrames.size() < 1){
    return NoError;
  }
  if(!pvControlTransferPending){
    return NoError;
  }
  // If transfer is null, port was closed
  if(pvControlTransfer == 0){
    // Stop thread
    return UnhandledError;
  }
  // Get and update complete flag
  pComplete = (int*)pvControlTransfer->user_data;
  pvControlTransferComplete = *pComplete;
  // If transfer is not complete, we simply return
  if(pvControlTransferComplete == 0){
    return NoError;
  }
  // Transfer complete, take the frame and check flags
  frame = pvControlQueryFrames.dequeue();
  Q_ASSERT(frame != 0);
  pvControlTransferPending = false;
  // Check if transfer has timed out
  if(pvControlTransfer->status & LIBUSB_TRANSFER_TIMED_OUT){
    updateWriteTimeoutState(true);
    pvControlFramesPool.enqueue(frame);
    return ControlTimeout;
  }
  // Check if transfer was cancelled
  if(pvControlTransfer->status & LIBUSB_TRANSFER_CANCELLED){
    qDebug() << "mdtUsbPort::handleControlResponses() return ControlCanceled";
    pvControlFramesPool.enqueue(frame);
    return ControlCanceled;
  }
  // Check if device is disconnected (not valid if cancelled)
  if(pvControlTransfer->status & LIBUSB_TRANSFER_NO_DEVICE){
    qDebug() << "mdtUsbPort::handleControlResponses(): device disconnected";
    pvControlFramesPool.enqueue(frame);
    return Disconnected;
  }
  // Check if transfer stall
  if(pvControlTransfer->status & LIBUSB_TRANSFER_STALL){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_STALL during a control transfer", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    pvControlFramesPool.enqueue(frame);
    return UnhandledError;
  }
  // Check if transfer failed (not valid if cancelled)
  if(pvControlTransfer->status & LIBUSB_TRANSFER_ERROR){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_ERROR during a control transfer", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    pvControlFramesPool.enqueue(frame);
    return UnhandledError;
  }
  // Check transfer overflow
  if(pvControlTransfer->status & LIBUSB_TRANSFER_OVERFLOW){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_OVERFLOW during a control transfer", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    pvControlFramesPool.enqueue(frame);
    return UnhandledError;
  }
  // Check transfer short frames NOK
  if(pvControlTransfer->status & LIBUSB_TRANSFER_SHORT_NOT_OK){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_SHORT_NOT_OK during a control transfer", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    pvControlFramesPool.enqueue(frame);
    return UnhandledError;
  }
  // All is Ok, copy data and enqueue to responses queue
  frame->clear();
  frame->clearSub();
  controlSetup =  libusb_control_transfer_get_setup(pvControlTransfer);
  Q_ASSERT(controlSetup != 0);
  frame->setbmRequestType(controlSetup->bmRequestType);
  frame->setbRequest(controlSetup->bRequest);
  frame->setwValue(controlSetup->wValue);
  frame->setwIndex(controlSetup->wIndex);
  frame->setwLength(controlSetup->wLength);
  controlData = (char*)libusb_control_transfer_get_data(pvControlTransfer);
  frame->append(controlData, pvControlTransfer->actual_length);
  pvControlResponseFrames.enqueue(frame);

  return NoError;
}

QQueue<mdtFrameUsbControl*> &mdtUsbPort::controlResponseFrames()
{
  return pvControlResponseFrames;
}

mdtAbstractPort::error_t mdtUsbPort::initReadTransfer(qint64 maxSize)
{
  Q_ASSERT(maxSize >= 0);

  int len;
  int err;
  error_t portError;

  // If transfer is null, port was closed
  if(pvReadTransfer == 0){
    // Stop thread
    return UnhandledError;
  }
  // Ajust possible max length
  if(maxSize > (qint64)pvReadBufferSize){
    len = pvReadBufferSize;
  }else{
    len = maxSize;
  }
  // Fill the transfer
  pvReadTransferComplete = 0;
  if(pvReadTransfertType == LIBUSB_TRANSFER_TYPE_BULK){
    libusb_fill_bulk_transfer(pvReadTransfer, pvHandle, pvReadEndpointAddress, (unsigned char*)pvReadBuffer, len, transferCallback, (void*)&pvReadTransferComplete, pvReadTimeout);
  }else if(pvReadTransfertType == LIBUSB_TRANSFER_TYPE_INTERRUPT){
    libusb_fill_interrupt_transfer(pvReadTransfer, pvHandle, pvReadEndpointAddress, (unsigned char*)pvReadBuffer, len, transferCallback, (void*)&pvReadTransferComplete, pvReadTimeout);
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
      mdtError e(MDT_USB_IO_ERROR, "libusb_submit_transfer() failed", mdtError::Error);
      e.setSystemError(err, errorText(err));
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
    }
    return portError;
  }
  pvReadTransferPending = true;

  return NoError;
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
    qDebug() << "cancelReadTransfer() ...";
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
    pvReadTransferPending = false;
  }

  return NoError;
}

mdtAbstractPort::error_t mdtUsbPort::waitForReadyRead()
{
  error_t portError;
  int *pComplete;

  // If transfer is null, port was closed
  if(pvReadTransfer == 0){
    // Stop thread
    return UnhandledError;
  }

  while(pvReadTransferComplete == 0){
    portError = handleUsbEvents(&pvReadTimeoutTv, pvReadEndpointAddress);
    if(portError != NoError){
      return portError;
    }
    // Check about control transfer
    portError = handleControlResponses();
    if(portError != NoError){
      return portError;
    }
    // Check about message IN transfer
    portError = handleMessageIn();
    if(portError != NoError){
      return portError;
    }
    // Get complete flag
    pComplete = (int*)pvReadTransfer->user_data;
    pvReadTransferComplete = *pComplete;
  }
  // Check if transfer has timed out
  if(pvReadTransfer->status & LIBUSB_TRANSFER_TIMED_OUT){
    updateReadTimeoutState(true);
    return ReadTimeout;
  }else{
    updateReadTimeoutState(false);
  }
  // Check if transfer was cancelled
  if(pvReadTransfer->status & LIBUSB_TRANSFER_CANCELLED){
    qDebug() << "mdtUsbPort::waitForReadyRead() return ReadCanceled";
    return ReadCanceled;
  }
  // Check if device is disconnected (not valid if cancelled)
  if(pvReadTransfer->status & LIBUSB_TRANSFER_NO_DEVICE){
    qDebug() << "mdtUsbPort::waitForReadyRead(): device disconnected";
    return Disconnected;
  }
  // Check if transfer stall
  if(pvReadTransfer->status & LIBUSB_TRANSFER_STALL){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_STALL", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return UnhandledError;
  }
  // Check if transfer failed (not valid if cancelled)
  if(pvReadTransfer->status & LIBUSB_TRANSFER_ERROR){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_ERROR", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return UnhandledError;
  }
  // Check transfer overflow
  if(pvReadTransfer->status & LIBUSB_TRANSFER_OVERFLOW){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_OVERFLOW", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return UnhandledError;
  }
  // Check transfer short frames NOK
  if(pvReadTransfer->status & LIBUSB_TRANSFER_SHORT_NOT_OK){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_SHORT_NOT_OK", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return UnhandledError;
  }

  return NoError;
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

  // Copy readen data
  retLen = pvReadTransfer->actual_length;
  memcpy(data, pvReadBuffer, retLen);
  // Update internal flag
  pvReadTransferPending = false;

  return retLen;
}

void mdtUsbPort::setSingleReadTransferRequest()
{
  pvSingleReadTransferRequestPending = true;
  pvWriteFrameAvailable.wakeAll();
}

bool mdtUsbPort::singleReadTransferRequestPending() const
{
  return pvSingleReadTransferRequestPending;
}

void mdtUsbPort::resetSingleReadTransferRequest()
{
  pvSingleReadTransferRequestPending = false;
}

int mdtUsbPort::readBufferSize() const
{
  return pvReadBufferSize;
}

mdtAbstractPort::error_t mdtUsbPort::initMessageInTransfer(qint64 maxSize)
{
  Q_ASSERT(maxSize >= 0);

  int len;
  int err;
  error_t portError;

  // Endpoint is optional
  if(pvMessageInTransfer == 0){
    return NoError;
  }
  // Ajust possible max length
  if(maxSize > (qint64)pvMessageInBufferSize){
    len = pvMessageInBufferSize;
  }else{
    len = maxSize;
  }
  // Fill the transfer
  pvMessageInTransferComplete = 0;
  libusb_fill_interrupt_transfer(pvMessageInTransfer, pvHandle, pvMessageInEndpointAddress, (unsigned char*)pvMessageInBuffer, len, transferCallback, (void*)&pvMessageInTransferComplete, 0);
  // Submit transfert
  err = libusb_submit_transfer(pvMessageInTransfer); /// \todo Handle retval
  if(err != 0){
    portError = mapUsbError(err, pvMessageInEndpointAddress);
    if(portError == UnhandledError){
      mdtError e(MDT_USB_IO_ERROR, "libusb_submit_transfer() failed", mdtError::Error);
      e.setSystemError(err, errorText(err));
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
    }
    return portError;
  }
  pvMessageInTransferPending = true;

  return NoError;
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
    qDebug() << "mdtUsbPort::cancelMessageInTransfer() ...";
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
    pvMessageInTransferPending = false;
    qDebug() << "mdtUsbPort::cancelMessageInTransfer() DONE";
  }

  return NoError;
}

mdtAbstractPort::error_t mdtUsbPort::handleMessageIn()
{
  int *pComplete;
  mdtFrame *frame;

  // Mesage IN is optional
  if(pvMessageInTransfer == 0){
    return NoError;
  }
  // Init a new transfer if needed
  if(!pvMessageInTransferPending){
    return initMessageInTransfer(pvMessageInBufferSize);
  }
  // Transfer pending, check about completion
  pComplete = (int*)pvMessageInTransfer->user_data;
  pvMessageInTransferComplete = *pComplete;
  // If transfer is not complete, we simply return
  if(pvMessageInTransferComplete == 0){
    return NoError;
  }
  // Transfer complete, take the frame and check flags
  if(pvMessageInFramesPool.size() < 1){
    mdtError e(MDT_USB_IO_ERROR, "Message IN frames pool is empty", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return ReadPoolEmpty;
  }
  frame = pvMessageInFramesPool.dequeue();
  Q_ASSERT(frame != 0);
  pvMessageInTransferPending = false;
  // Check if transfer has timed out
  if(pvMessageInTransfer->status & LIBUSB_TRANSFER_TIMED_OUT){
    updateReadTimeoutState(true);
    pvMessageInFramesPool.enqueue(frame);
    /// \todo Check if ok
    return ReadTimeout;
  }
  // Check if transfer was cancelled
  if(pvMessageInTransfer->status & LIBUSB_TRANSFER_CANCELLED){
    qDebug() << "mdtUsbPort::handleControlResponses() return ReadCanceled";
    pvMessageInFramesPool.enqueue(frame);
    return ReadCanceled;
  }
  // Check if device is disconnected (not valid if cancelled)
  if(pvMessageInTransfer->status & LIBUSB_TRANSFER_NO_DEVICE){
    qDebug() << "mdtUsbPort::handleControlResponses(): device disconnected";
    pvMessageInFramesPool.enqueue(frame);
    return Disconnected;
  }
  // Check if transfer stall
  if(pvMessageInTransfer->status & LIBUSB_TRANSFER_STALL){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_STALL during a control transfer", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    pvMessageInFramesPool.enqueue(frame);
    return UnhandledError;
  }
  // Check if transfer failed (not valid if cancelled)
  if(pvMessageInTransfer->status & LIBUSB_TRANSFER_ERROR){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_ERROR during a control transfer", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    pvMessageInFramesPool.enqueue(frame);
    return UnhandledError;
  }
  // Check transfer overflow
  if(pvMessageInTransfer->status & LIBUSB_TRANSFER_OVERFLOW){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_OVERFLOW during a control transfer", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    pvMessageInFramesPool.enqueue(frame);
    return UnhandledError;
  }
  // Check transfer short frames NOK
  if(pvMessageInTransfer->status & LIBUSB_TRANSFER_SHORT_NOT_OK){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_SHORT_NOT_OK during a control transfer", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    pvMessageInFramesPool.enqueue(frame);
    return UnhandledError;
  }
  // All is Ok, copy data and enqueue to messages IN queue
  frame->clear();
  frame->clearSub();
  frame->append(pvMessageInBuffer, pvMessageInTransfer->actual_length);
  pvMessageInFrames.append(frame);

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

mdtAbstractPort::error_t mdtUsbPort::initWriteTransfer(const char *data, qint64 maxSize)
{
  Q_ASSERT(data != 0);
  Q_ASSERT(maxSize >= 0);

  int err;
  error_t portError;
  int len;

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
  pvWriteTransferComplete = 0;
  if(pvWriteTransfertType == LIBUSB_TRANSFER_TYPE_BULK){
    libusb_fill_bulk_transfer(pvWriteTransfer, pvHandle, pvWriteEndpointAddress, (unsigned char*)pvWriteBuffer, len, transferCallback, (void*)&pvWriteTransferComplete, pvWriteTimeout);
  }else if(pvReadTransfertType == LIBUSB_TRANSFER_TYPE_INTERRUPT){
    libusb_fill_interrupt_transfer(pvWriteTransfer, pvHandle, pvWriteEndpointAddress, (unsigned char*)pvWriteBuffer, len, transferCallback, (void*)&pvWriteTransferComplete, pvWriteTimeout);
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
    qDebug() << "cancelWriteTransfer() ...";
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
    pvWriteTransferPending = false;
  }

  return NoError;
}

mdtAbstractPort::error_t mdtUsbPort::waitEventWriteReady()
{
  error_t portError;
  int *pComplete;

  // If transfer is null, port was closed
  if(pvWriteTransfer == 0){
    // Stop thread
    return UnhandledError;
  }

  while(pvWriteTransferComplete == 0){
    portError = handleUsbEvents(&pvWriteTimeoutTv, pvWriteEndpointAddress);
    if(portError != NoError){
      return portError;
    }
    // Check about control transfer
    portError = handleControlResponses();
    if(portError != NoError){
      return portError;
    }
    // Check about message IN transfer
    portError = handleMessageIn();
    if(portError != NoError){
      return portError;
    }
    // Get complete flag
    pComplete = (int*)pvWriteTransfer->user_data;
    pvWriteTransferComplete = *pComplete;
  }
  // Check if transfer has timed out
  if(pvWriteTransfer->status & LIBUSB_TRANSFER_TIMED_OUT){
    updateReadTimeoutState(true);
    return WriteTimeout;
  }else{
    updateWriteTimeoutState(false);
  }
  // Check if transfer was cancelled
  if(pvWriteTransfer->status & LIBUSB_TRANSFER_CANCELLED){
    return WriteCanceled;
  }
  // Check if device is disconnected (not valid if cancelled)
  if(pvWriteTransfer->status & LIBUSB_TRANSFER_NO_DEVICE){
    qDebug() << "mdtUsbPort::waitForReadyRead(): device disconnected";
    return Disconnected;
  }
  // Check if transfer stall
  if(pvWriteTransfer->status & LIBUSB_TRANSFER_STALL){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_STALL", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return UnhandledError;
  }
  // Check if transfer failed (not valid if cancelled)
  if(pvWriteTransfer->status & LIBUSB_TRANSFER_ERROR){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_ERROR", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return UnhandledError;
  }
  // Check transfer overflow
  if(pvWriteTransfer->status & LIBUSB_TRANSFER_OVERFLOW){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_OVERFLOW", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return UnhandledError;
  }
  // Check transfer short frames NOK
  if(pvWriteTransfer->status & LIBUSB_TRANSFER_SHORT_NOT_OK){
    mdtError e(MDT_USB_IO_ERROR, "Transfer status LIBUSB_TRANSFER_SHORT_NOT_OK", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return UnhandledError;
  }

  return NoError;
}

qint64 mdtUsbPort::write(const char *data, qint64 maxSize)
{
  Q_ASSERT(data != 0);

  int retLen;

  // If transfer is null, port was closed
  if(pvWriteTransfer == 0){
    // Stop thread
    return UnhandledError;
  }
  // Get size of written data
  retLen = pvWriteTransfer->actual_length;
  // Update internal flag
  pvWriteTransferPending = false;

  return retLen;
}

void mdtUsbPort::transferCallback(struct libusb_transfer *transfer)
{
  Q_ASSERT(transfer != 0);

  qDebug() << "Tranfer callback ...";
  
  qDebug() << "-> Endpoint: " << hex << transfer->endpoint;
  qDebug() << "-> actual_length: " << hex << transfer->actual_length;
  qDebug() << "-> short frame stransfert NOK state: " << (transfer->flags & LIBUSB_TRANSFER_SHORT_NOT_OK);
  qDebug() << "-> transfert complete: " << (transfer->status & LIBUSB_TRANSFER_COMPLETED);
  qDebug() << "-> transfert failed: " << (transfer->status & LIBUSB_TRANSFER_ERROR);
  qDebug() << "-> transfert timeout: " << (transfer->status & LIBUSB_TRANSFER_TIMED_OUT);
  qDebug() << "-> transfert cancelled: " << (transfer->status & LIBUSB_TRANSFER_CANCELLED);
  qDebug() << "-> transfert stall: " << (transfer->status & LIBUSB_TRANSFER_STALL);
  qDebug() << "-> device disconnected: " << (transfer->status & LIBUSB_TRANSFER_NO_DEVICE);
  qDebug() << "-> device sendt to much (overflow): " << (transfer->status & LIBUSB_TRANSFER_OVERFLOW);
  
  /**
  mdt_usb_port_transfer_data *data = (mdt_usb_port_transfer_data*)transfer->user_data;
  Q_ASSERT(data != 0);
  data->flag1 = transfer->status;
  */
  
  int *complete = (int*)transfer->user_data;
  *complete = 1;
}

mdtAbstractPort::error_t mdtUsbPort::cancelTransfers()
{
  error_t portError;

  qDebug() << "mdtUsbPort::cancelTransfers() ...";
  lockMutex();
  portError = cancelControlTransfer();
  if((portError != NoError)&&(portError != ControlCanceled)){
    unlockMutex();
    return portError;
  }
  portError = cancelWriteTransfer();
  if((portError != NoError)&&(portError != WriteCanceled)){
    unlockMutex();
    return portError;
  }
  portError = cancelReadTransfer();
  if((portError != NoError)&&(portError != ReadCanceled)){
    unlockMutex();
    return portError;
  }
  portError = cancelMessageInTransfer();
  if((portError != NoError)&&(portError != ReadCanceled)){
    unlockMutex();
    return portError;
  }
  unlockMutex();
  qDebug() << "mdtUsbPort::cancelTransfers() DONE";

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
  if(err != 0){
    portError = mapUsbError(err, endpoint);
    if(portError == UnhandledError){
      mdtError e(MDT_USB_IO_ERROR, "libusb_handle_events_timeout_completed() failed", mdtError::Error);
      e.setSystemError(err, errorText(err));
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
    }
    return portError;
  }

  return NoError;
}

quint8 mdtUsbPort::currentReadEndpointAddress() const
{
  return pvReadEndpointAddress;
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
  delete[] pvControlBuffer;
  pvControlBuffer = 0;
  pvControlBufferSize = 0;
  if(pvControlTransfer != 0){
    libusb_free_transfer(pvControlTransfer);
    pvControlTransfer = 0;
  }
  qDeleteAll(pvControlFramesPool);
  pvControlFramesPool.clear();
  qDeleteAll(pvControlQueryFrames);
  pvControlQueryFrames.clear();
  qDeleteAll(pvControlResponseFrames);
  pvControlResponseFrames.clear();
  delete[] pvReadBuffer;
  pvReadBuffer = 0;
  pvReadBufferSize = 0;
  if(pvReadTransfer != 0){
    libusb_free_transfer(pvReadTransfer);
    pvReadTransfer = 0;
  }
  delete[] pvWriteBuffer;
  pvWriteBuffer = 0;
  pvWriteBufferSize = 0;
  if(pvWriteTransfer != 0){
    libusb_free_transfer(pvWriteTransfer);
    pvWriteTransfer = 0;
  }
  delete[] pvMessageInBuffer;
  pvMessageInBuffer = 0;
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
    ///qDebug() << "Bulk OUT address: 0x" << hex << pvWriteEndpointAddress;
    ///qDebug() << " Max packet size: " << pvWriteBufferSize;
  }
  if(interruptEndpointDescriptor != 0){
    pvWriteBufferSize = interruptEndpointDescriptor->transactionsCountPerMicroFrame();
    if(pvWriteBufferSize < 1){
      pvWriteBufferSize = 1;
    }
    pvWriteBufferSize *= interruptEndpointDescriptor->maxPacketSize();
    pvWriteEndpointAddress = interruptEndpointDescriptor->address() | LIBUSB_ENDPOINT_OUT;
    pvWriteTransfertType = LIBUSB_TRANSFER_TYPE_INTERRUPT;
    ///qDebug() << "Interrupt OUT address: 0x" << hex << pvWriteEndpointAddress;
    ///qDebug() << " Max packet size: " << pvWriteBufferSize;
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
    ///qDebug() << "Bulk IN address: 0x" << hex << pvReadEndpointAddress;
    ///qDebug() << " Max packet size: " << pvReadBufferSize;
  }
  if(interruptEndpointDescriptor != 0){
    ///qDebug() << "Interrupt IN address: " << interruptEndpointDescriptor->address();
    ///qDebug() << " Max packet size: " << interruptEndpointDescriptor->maxPacketSize();
    // Some device have a Bulk IN + Interrupt IN
    if(bulkEndpointDescriptor != 0){
      ///qDebug() << "Additionnal interrupt IN , currently not supported..";
      // Use this interrupt IN endpoint as message IN
      pvMessageInBufferSize = interruptEndpointDescriptor->transactionsCountPerMicroFrame();
      if(pvMessageInBufferSize < 1){
        pvMessageInBufferSize = 1;
      }
      pvMessageInBufferSize *= interruptEndpointDescriptor->maxPacketSize();
      pvMessageInEndpointAddress = interruptEndpointDescriptor->address() | LIBUSB_ENDPOINT_IN;
      ///qDebug() << "Interrupt IN (for messages) address: 0x" << hex << pvMessageInEndpointAddress;
      ///qDebug() << " Max packet size: " << pvMessageInBufferSize;
    }else{
      // Use this interrupt IN endpoint as read
      pvReadBufferSize = interruptEndpointDescriptor->transactionsCountPerMicroFrame();
      if(pvReadBufferSize < 1){
        pvReadBufferSize = 1;
      }
      pvReadBufferSize *= interruptEndpointDescriptor->maxPacketSize();
      pvReadEndpointAddress = interruptEndpointDescriptor->address() | LIBUSB_ENDPOINT_IN;
      pvReadTransfertType = LIBUSB_TRANSFER_TYPE_INTERRUPT;
      ///qDebug() << "Interrupt IN address: 0x" << hex << pvReadEndpointAddress;
      ///qDebug() << " Max packet size: " << pvReadBufferSize;
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
  pvSingleReadTransferRequestPending = false;
  pvWriteTransferPending = false;
  pvMessageInTransferPending = false;

  return NoError;
}

void mdtUsbPort::pvFlushIn()
{
  qDebug() << "mdtUsbPort::pvFlushIn() ...";
  cancelReadTransfer();
}

void mdtUsbPort::pvFlushOut()
{
  qDebug() << "mdtUsbPort::pvFlushOut() ...";
  cancelWriteTransfer();
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
        updateWriteTimeoutState(true);
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
