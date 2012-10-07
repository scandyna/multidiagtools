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

#include <poll.h>
#include <errno.h>
#include <string.h>

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
  pvReadBuffer = 0;
  pvReadBufferSize = 0;
  pvReadenLength = 0;
  pvReadEndpointAddress = 0;
  pvWriteBuffer = 0;
  pvWriteBufferSize = 0;
  pvWrittenLength = 0;
  pvWriteEndpointAddress = 0;
  pvInterruptInBuffer = 0;
  pvInterruptInBufferSize = 0;
  pvReadTransfer = 0;
  pvReadTransferPending = false;
  pvWriteTransfer = 0;
  pvWriteTransferPending = false;
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

mdtAbstractPort::error_t mdtUsbPort ::reconnect(int timeout)
{
  error_t error;

  close();

  error = open();
  if(error != NoError){
    // Wait timeout and retry
    unlockMutex();
    msleep(timeout);
    lockMutex();
    error = open();
    if(error != NoError){
      return Disconnected;
    }
  }
  error = setup();
  if(error != NoError){
    // Wait timeout and retry
    unlockMutex();
    msleep(timeout);
    lockMutex();
    error = setup();
    if(error != NoError){
      return Disconnected;
    }
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

mdtAbstractPort::error_t mdtUsbPort::initReadTransfer(qint64 maxSize)
{
  Q_ASSERT(maxSize >= 0);

  int len;
  int err;

  qDebug() << "RD: init transfer ...";
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
    if(err == LIBUSB_ERROR_NO_DEVICE){
      return Disconnected;
    }else{
      mdtError e(MDT_USB_IO_ERROR, "libusb_submit_transfer() failed", mdtError::Error);
      e.setSystemError(err, errorText(err));
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return UnhandledError;
    }
  }
  pvReadTransferPending = true;

  return NoError;
}

mdtAbstractPort::error_t mdtUsbPort::cancelReadTransfer()
{
  int err;

  qDebug() << "cancelReadTransfer() ...";
  // If transfer is null, port was closed
  if(pvReadTransfer == 0){
    // Stop thread
    return UnhandledError;
  }

  if(pvReadTransferPending){
    err = libusb_cancel_transfer(pvReadTransfer);
    if(err != 0){
      if(err == LIBUSB_ERROR_NO_DEVICE){
        return Disconnected;
      }else{
        // Unhandled error
        mdtError e(MDT_USB_IO_ERROR, "libusb_cancel_transfer() failed", mdtError::Error);
        e.setSystemError(err, errorText(err));
        MDT_ERROR_SET_SRC(e, "mdtUsbPort");
        e.commit();
        return UnhandledError;
      }
    }
  }

  return NoError;
}

mdtAbstractPort::error_t mdtUsbPort::waitForReadyRead()
{
  qDebug() << "mdtUsbPort::waitForReadyRead() ...";

  int err;
  int *pComplete;
  struct timeval tv = pvReadTimeoutTv;

  // If transfer is null, port was closed
  if(pvReadTransfer == 0){
    // Stop thread
    return UnhandledError;
  }

  while(pvReadTransferComplete == 0){
    qDebug() << "mdtUsbPort::waitForReadyRead() handle events ...";
    unlockMutex();
    err = libusb_handle_events_timeout(pvLibusbContext, &tv);
    lockMutex();
    qDebug() << "mdtUsbPort::waitForReadyRead() handle events DONE";
    if(err != 0){
      // Handled errors
      if(err == LIBUSB_ERROR_TIMEOUT){
        updateReadTimeoutState(true);
        return NoError;
      }else if(err == LIBUSB_ERROR_INTERRUPTED){
        // End of thread
        return WaitingCanceled;
      }else if(err == LIBUSB_ERROR_NO_DEVICE){
        return Disconnected;
      }else{
        // Unhandled error
        mdtError e(MDT_USB_IO_ERROR, "libusb_handle_events_timeout_completed() failed", mdtError::Error);
        e.setSystemError(err, errorText(err));
        MDT_ERROR_SET_SRC(e, "mdtUsbPort");
        e.commit();
        return UnhandledError;
      }
    }
    // Check if transfer was cancelled
    if(pvReadTransfer->status & LIBUSB_TRANSFER_CANCELLED){
      qDebug() << "mdtUsbPort::waitForReadyRead() return ReadCanceled";
      return ReadCanceled;
    }
    // Check if transfer has timed out (not valid if cancelled)
    if(pvReadTransfer->status & LIBUSB_TRANSFER_TIMED_OUT){
      updateReadTimeoutState(true);
      return NoError;
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
    // Get complete flag
    pComplete = (int*)pvReadTransfer->user_data;
    pvReadTransferComplete = *pComplete;
    updateReadTimeoutState(false);
  }

  qDebug() << "mdtUsbPort::waitForReadyRead() DONE";
  return NoError;
}

qint64 mdtUsbPort::read(char *data, qint64 maxSize)
{
  Q_ASSERT(data != 0);

  int retLen;

  qDebug() << "mdtUsbPort::read() ...";
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

  qDebug() << "Readen: " << retLen;
  return retLen;
}

mdtAbstractPort::error_t mdtUsbPort::initWriteTransfer(const char *data, qint64 maxSize)
{
  Q_ASSERT(data != 0);
  Q_ASSERT(maxSize >= 0);

  int err;
  int len;

  qDebug() << "WR: init transfer ...";
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
  qDebug() << "WR: fill a new transfert, size: " << len;
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
    if(err == LIBUSB_ERROR_NO_DEVICE){
      return Disconnected;
    }else{
      mdtError e(MDT_USB_IO_ERROR, "libusb_submit_transfer() failed", mdtError::Error);
      e.setSystemError(err, errorText(err));
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return UnhandledError;
    }
  }
  pvWriteTransferPending = true;

  return NoError;
}

mdtAbstractPort::error_t mdtUsbPort::cancelWriteTransfer()
{
  int err;

  qDebug() << "cancelWriteTransfer() ...";
  // If transfer is null, port was closed
  if(pvWriteTransfer == 0){
    // Stop thread
    return UnhandledError;
  }

  if(pvWriteTransferPending){
    err = libusb_cancel_transfer(pvWriteTransfer);
    if(err != 0){
      if(err == LIBUSB_ERROR_NO_DEVICE){
        return Disconnected;
      }else{
        // Unhandled error
        mdtError e(MDT_USB_IO_ERROR, "libusb_cancel_transfer() failed", mdtError::Error);
        e.setSystemError(err, errorText(err));
        MDT_ERROR_SET_SRC(e, "mdtUsbPort");
        e.commit();
        return UnhandledError;
      }
    }
  }

  return NoError;
}

mdtAbstractPort::error_t mdtUsbPort::waitEventWriteReady()
{
  qDebug() << "mdtUsbPort::waitEventWriteReady() ...";

  int err;
  int *pComplete;
  struct timeval tv = pvWriteTimeoutTv;

  // If transfer is null, port was closed
  if(pvWriteTransfer == 0){
    // Stop thread
    return UnhandledError;
  }

  while(pvWriteTransferComplete == 0){
    unlockMutex();
    err = libusb_handle_events_timeout(pvLibusbContext, &tv);
    lockMutex();
    if(err != 0){
      // Handled errors
      if(err == LIBUSB_ERROR_TIMEOUT){
        updateWriteTimeoutState(true);
        return NoError;
      }else if(err == LIBUSB_ERROR_INTERRUPTED){
        // End of thread
        return UnhandledError;
      }else if(err == LIBUSB_ERROR_NO_DEVICE){
        return Disconnected;
      }else{
        // Unhandled error
        mdtError e(MDT_USB_IO_ERROR, "libusb_handle_events_timeout_completed() failed", mdtError::Error);
        e.setSystemError(err, errorText(err));
        MDT_ERROR_SET_SRC(e, "mdtUsbPort");
        e.commit();
        return UnhandledError;
      }
    }
    // Check if transfer was cancelled
    if(pvWriteTransfer->status & LIBUSB_TRANSFER_CANCELLED){
      return WriteCanceled;
    }
    // Check if transfer has timed out (not valid if cancelled)
    if(pvWriteTransfer->status & LIBUSB_TRANSFER_TIMED_OUT){
      updateReadTimeoutState(true);
      return NoError;
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
    // Get complete flag
    pComplete = (int*)pvWriteTransfer->user_data;
    pvWriteTransferComplete = *pComplete;
    updateWriteTimeoutState(false);
  }

  return NoError;
}

qint64 mdtUsbPort::write(const char *data, qint64 maxSize)
{
  Q_ASSERT(data != 0);

  int retLen;

  qDebug() << "mdtUsbPort::write() ...";
  // If transfer is null, port was closed
  if(pvWriteTransfer == 0){
    // Stop thread
    return UnhandledError;
  }

  // Get size of written data
  retLen = pvWriteTransfer->actual_length;
  // Update internal flag
  pvWriteTransferPending = false;

  qDebug() << "Written: " << retLen;
  return retLen;
}

void mdtUsbPort::transferCallback(struct libusb_transfer *transfer)
{
  Q_ASSERT(transfer != 0);

  qDebug() << "Tranfert callback ...";
  
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
  mdtAbstractPort::error_t err;

  lockMutex();
  err = cancelWriteTransfer();
  if(err != 0){
    unlockMutex();
    return err;
  }
  err = cancelReadTransfer();
  if(err != 0){
    unlockMutex();
    return err;
  }
  unlockMutex();

  return NoError;
}

mdtAbstractPort::error_t mdtUsbPort::pvOpen()
{
  Q_ASSERT(!isOpen());

  quint8 busNumber = 0;
  quint8 deviceAddress = 0;
  quint16 vid = 0;
  quint16 pid = 0;
  QString str;
  QStringList lst;
  bool ok = false;
  ///libusb_device *device = 0;
  libusb_device **devicesList;
  ssize_t devicesCount = 0;
  struct libusb_device_descriptor deviceDescriptor;
  ssize_t i = 0;
  int err;

  // Extract items from port name
  lst = pvPortName.split(":");
  // If we have 2 items, we have BusNumber:deviceAddress format
  if(lst.size() == 2){
    // Extract BusNumber
    str = lst.at(0);
    if(str.left(2) == "0x"){
      busNumber = str.toUInt(&ok, 16);
    }else{
      busNumber = str.toUInt(&ok, 10);
    }
    if(!ok){
      mdtError e(MDT_USB_IO_ERROR, "Cannot extract bus number in " + pvPortName, mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return SetupError;
    }
    // Extract deviceAddress
    str = lst.at(1);
    if(str.left(2) == "0x"){
      deviceAddress = str.toUInt(&ok, 16);
    }else{
      deviceAddress = str.toUInt(&ok, 10);
    }
    if(!ok){
      mdtError e(MDT_USB_IO_ERROR, "Cannot extract device number in " + pvPortName, mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return SetupError;
    }
  }else if(lst.size() == 3){
    // We have the :vendorID:productID format,
    // extract vendor ID
    str = lst.at(1);
    if(str.left(2) == "0x"){
      vid = str.toUInt(&ok, 16);
    }else{
      vid = str.toUInt(&ok, 10);
    }
    if(!ok){
      mdtError e(MDT_USB_IO_ERROR, "Cannot extract idVendor in " + pvPortName, mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return SetupError;
    }
    // Extract product ID
    str = lst.at(2);
    if(str.left(2) == "0x"){
      pid = str.toUInt(&ok, 16);
    }else{
      pid = str.toUInt(&ok, 10);
    }
    if(!ok){
      mdtError e(MDT_USB_IO_ERROR, "Cannot extract idProduct in " + pvPortName, mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return SetupError;
    }
  }else if(lst.size() == 4){
    // We have the BusNumber:deviceAddress:VendorID:ProductID format,
    // extract BusNumber
    str = lst.at(0);
    if(str.left(2) == "0x"){
      busNumber = str.toUInt(&ok, 16);
    }else{
      busNumber = str.toUInt(&ok, 10);
    }
    if(!ok){
      mdtError e(MDT_USB_IO_ERROR, "Cannot extract bus number in " + pvPortName, mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return SetupError;
    }
    // Extract deviceAddress
    str = lst.at(1);
    if(str.left(2) == "0x"){
      deviceAddress = str.toUInt(&ok, 16);
    }else{
      deviceAddress = str.toUInt(&ok, 10);
    }
    if(!ok){
      mdtError e(MDT_USB_IO_ERROR, "Cannot extract device number in " + pvPortName, mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return SetupError;
    }
    // Extract vendor ID
    str = lst.at(2);
    if(str.left(2) == "0x"){
      vid = str.toUInt(&ok, 16);
    }else{
      vid = str.toUInt(&ok, 10);
    }
    if(!ok){
      mdtError e(MDT_USB_IO_ERROR, "Cannot extract idVendor in " + pvPortName, mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return SetupError;
    }
    // Extract product ID
    str = lst.at(3);
    if(str.left(2) == "0x"){
      pid = str.toUInt(&ok, 16);
    }else{
      pid = str.toUInt(&ok, 10);
    }
    if(!ok){
      mdtError e(MDT_USB_IO_ERROR, "Cannot extract idProduct in " + pvPortName, mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return SetupError;
    }
  }else{
    // Wrong format
    mdtError e(MDT_USB_IO_ERROR, "Error in port format for " + pvPortName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return SetupError;
  }
  // Open port: choose between VID:PID or BusNumber:deviceAddress(:VID:PID) method
  if((busNumber == 0)&&(deviceAddress == 0)){
    pvHandle = libusb_open_device_with_vid_pid(pvLibusbContext, vid, pid);
    if(pvHandle == 0){
      mdtError e(MDT_USB_IO_ERROR, "Cannot open device " + pvPortName, mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return PortNotFound;
    }
    return NoError;
  }else{
    // Get devices list
    devicesCount = libusb_get_device_list(pvLibusbContext, &devicesList);
    if(devicesCount < 0){
      mdtError e(MDT_PORT_IO_ERROR, "Cannot list devices", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return UnhandledError;
    }
    // Scan ...
    for(i=0; i<devicesCount; i++){
      // See if busNumber and deviceAddress matches
      if((libusb_get_bus_number(devicesList[i]) == busNumber)&&(libusb_get_device_address(devicesList[i]) == deviceAddress)){
        qDebug() << "FOUND bus:dev ...";
        // Check if expected device is attached (if specified)
        if((vid != 0)&&(pid != 0)){
          qDebug() << "Checking device attahced...";
          err = libusb_get_device_descriptor(devicesList[i], &deviceDescriptor);
          if(err != 0){
            mdtError e(MDT_PORT_IO_ERROR, "Cannot get a device descriptor", mdtError::Error);
            e.setSystemError(err, errorText(err));
            MDT_ERROR_SET_SRC(e, "mdtUsbPort");
            e.commit();
            return UnhandledError;
          }
          if((deviceDescriptor.idVendor != vid)||(deviceDescriptor.idProduct != pid)){
            mdtError e(MDT_PORT_IO_ERROR, "Requested device not found on requested port (port name: " + pvPortName + ")", mdtError::Error);
            MDT_ERROR_SET_SRC(e, "mdtUsbPort");
            e.commit();
            return SetupError;
          }
        }
        // Open port
        err = libusb_open(devicesList[i], &pvHandle);
        if(err != 0){
          mdtError e(MDT_PORT_IO_ERROR, "Cannot get a device descriptor", mdtError::Error);
          e.setSystemError(err, errorText(err));
          MDT_ERROR_SET_SRC(e, "mdtUsbPort");
          e.commit();
          return UnhandledError;
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
  mdtError e(MDT_USB_IO_ERROR, "Cannot find port " + pvPortName, mdtError::Error);
  MDT_ERROR_SET_SRC(e, "mdtUsbPort");
  e.commit();
  return PortNotFound;
}

void mdtUsbPort::pvClose()
{
  Q_ASSERT(isOpen());
  Q_ASSERT(pvHandle != 0);

  int err;

  // Release port
  err = libusb_release_interface(pvHandle, 0);  /// \todo interface nunber hardcoded, BAD
  switch(err){
    case 0:
      break;
    case LIBUSB_ERROR_NOT_FOUND:
      break;
    case LIBUSB_ERROR_NO_DEVICE:
      break;
    default:
      mdtError e(MDT_USB_IO_ERROR, "libubs_release_interface() failed", mdtError::Error);
      e.setSystemError(err, errorText(err));
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
  }
  // Re-attach possibly detached driver in pvSetup()
#ifdef Q_OS_LINUX
  err = libusb_attach_kernel_driver(pvHandle, 0);  /// \todo interface nunber hardcoded, BAD
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
#endif
  // Close port
  libusb_close(pvHandle);
  // Free memory
  delete[] pvReadBuffer;
  pvReadBuffer = 0;
  delete[] pvWriteBuffer;
  pvWriteBuffer = 0;
  libusb_free_transfer(pvReadTransfer);
  pvReadTransfer = 0;
  libusb_free_transfer(pvWriteTransfer);
  pvWriteTransfer = 0;
}

mdtAbstractPort::error_t mdtUsbPort::pvSetup()
{
  Q_ASSERT(isOpen());
  ///Q_ASSERT(pvConfig != 0);
  Q_ASSERT(pvHandle != 0);

  // Essais
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
        return UnknownError;
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
    ///qDebug() << "Bulk OUT address: " << bulkEndpointDescriptor->address();
    ///qDebug() << " Max packet size: " << bulkEndpointDescriptor->maxPacketSize();
    pvWriteBufferSize = bulkEndpointDescriptor->transactionsCountPerMicroFrame();
    if(pvWriteBufferSize < 1){
      pvWriteBufferSize = 1;
    }
    pvWriteBufferSize *= bulkEndpointDescriptor->maxPacketSize();
    pvWriteEndpointAddress = bulkEndpointDescriptor->address() | LIBUSB_ENDPOINT_OUT;
    pvWriteBuffer = new char[pvWriteBufferSize];
    pvWriteTransfertType = LIBUSB_TRANSFER_TYPE_BULK;
    qDebug() << "Bulk OUT address: 0x" << hex << pvWriteEndpointAddress;
    qDebug() << " Max packet size: " << pvWriteBufferSize;
  }
  if(interruptEndpointDescriptor != 0){
    ///qDebug() << "Interrupt OUT address: " << interruptEndpointDescriptor->address();
    ///qDebug() << " Max packet size: " << interruptEndpointDescriptor->maxPacketSize();
    pvWriteBufferSize = interruptEndpointDescriptor->transactionsCountPerMicroFrame();
    if(pvWriteBufferSize < 1){
      pvWriteBufferSize = 1;
    }
    pvWriteBufferSize *= interruptEndpointDescriptor->maxPacketSize();
    pvWriteEndpointAddress = interruptEndpointDescriptor->address() | LIBUSB_ENDPOINT_OUT;
    pvWriteBuffer = new char[pvWriteBufferSize];
    pvWriteTransfertType = LIBUSB_TRANSFER_TYPE_INTERRUPT;
    qDebug() << "Interrupt OUT address: 0x" << hex << pvWriteEndpointAddress;
    qDebug() << " Max packet size: " << pvWriteBufferSize;
  }
  Q_ASSERT(pvWriteBuffer != 0);
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
    pvReadBuffer = new char[pvReadBufferSize];
    pvReadTransfertType = LIBUSB_TRANSFER_TYPE_BULK;
    qDebug() << "Bulk IN address: 0x" << hex << pvReadEndpointAddress;
    qDebug() << " Max packet size: " << pvReadBufferSize;
  }
  if(interruptEndpointDescriptor != 0){
    ///qDebug() << "Interrupt IN address: " << interruptEndpointDescriptor->address();
    ///qDebug() << " Max packet size: " << interruptEndpointDescriptor->maxPacketSize();
    // Some device have a Bulk IN + Interrupt IN
    if(bulkEndpointDescriptor != 0){
      qDebug() << "Additionnal interrupt IN , currently not supported..";
    }else{
      pvReadBufferSize = interruptEndpointDescriptor->transactionsCountPerMicroFrame();
      if(pvReadBufferSize < 1){
        pvReadBufferSize = 1;
      }
      pvReadBufferSize *= interruptEndpointDescriptor->maxPacketSize();
      pvReadEndpointAddress = interruptEndpointDescriptor->address() | LIBUSB_ENDPOINT_IN;
      pvReadBuffer = new char[pvReadBufferSize];
      pvReadTransfertType = LIBUSB_TRANSFER_TYPE_INTERRUPT;
      qDebug() << "Interrupt IN address: 0x" << hex << pvReadEndpointAddress;
      qDebug() << " Max packet size: " << pvReadBufferSize;
    }
  }
  Q_ASSERT(pvReadBuffer != 0);
  // Unload possibly loaded driver that uses the device
#ifdef Q_OS_LINUX
  err = libusb_detach_kernel_driver(pvHandle, 0);  /// \todo interface nunber hardcoded, BAD
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
  err = libusb_claim_interface(pvHandle, 0);  /// \todo interface nunber hardcoded, BAD
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
      return SetupError;
    }
  }
  // Alloc read transfer
  pvReadTransfer = libusb_alloc_transfer(0);
  if(pvReadTransfer == 0){
    mdtError e(MDT_USB_IO_ERROR, "libusb_alloc_transfert() failed", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return UnhandledError;
  }
  // Alloc write transfer
  pvWriteTransfer = libusb_alloc_transfer(0);
  if(pvWriteTransfer == 0){
    mdtError e(MDT_USB_IO_ERROR, "libusb_alloc_transfert() failed", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return UnhandledError;
  }


  /// \todo Setup functions ......

  /// \todo Set timeouts ...
  ///setReadTimeout(config().readTimeout());
  setReadTimeout(-1);
  ///setWriteTimeout(config().writeTimeout());
  setWriteTimeout(-1);
  
  // Set some flags
  pvReadTransferPending = false;
  pvWriteTransferPending = false;
  
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

  