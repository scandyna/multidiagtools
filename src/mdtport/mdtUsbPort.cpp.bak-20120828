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

/// NOTE: \todo : Update timeout states !
mdtAbstractPort::error_t mdtUsbPort::waitForReadyRead()
{
  qDebug() << "mdtUsbPort::waitForReadyRead() ...";

  bool complete;
  bool *pComplete;
  struct pollfd *fds;
  int numFds = 0;
  int err;

  // If we have no transfer pending, simply return (read() will init a new one)
  if(!pvReadTransferPending){
    qDebug() << "mdtUsbPort::waitForReadyRead(): no transfer pending, return";
    return NoError;
  }
  // We have a pending transfer here, wait until complete, but give a chance to other thread to handle events
  qDebug() << "RD: Getting complete flag ...";
  Q_ASSERT(pvReadTransfer != 0);
  pComplete = (bool*)pvReadTransfer->user_data;
  complete = *pComplete;
  qDebug() << "RD: Getting complete flag DONE";
  // Now, we release the I/O mutex, and deal with events
  unlockMutex();
  usleep(50000);  /// NOTE: provisoire !!
  while(!complete){
    // Try to get the event lock
    qDebug() << "RD: try to get event lock ...";
    if(libusb_try_lock_events(pvLibusbContext) == 0){
      qDebug() << "RD: have the lock, check if can handle events now ...";
      // We have the event lock here, check if it's really possible to handle events
      if(!libusb_event_handling_ok(pvLibusbContext)){
        // Not possible now, unlock event lock and retry later
        qDebug() << "RD: Cannot handle events now, unlock ...";
        libusb_unlock_events(pvLibusbContext);
        // It can happen that other thread has complete the transfer, take a look
        lockMutex();
        Q_ASSERT(pvReadTransfer != 0);
        pComplete = (bool*)pvReadTransfer->user_data;
        complete = *pComplete;
        unlockMutex();
        continue;
      }else{
        // Wait on file descriptors
        fds = getLibusbPollFds(&numFds);
        err = poll(fds, numFds, -1);   /// NOTE: \todo timeout ?
        free(fds);
        if(err == 0){
          updateReadTimeoutState(true);
        }else{
          updateReadTimeoutState(false);
          if(err < 0){
            libusb_unlock_events(pvLibusbContext);
            switch(errno){
              case EINTR:
                /// \todo abort events ??
                return WaitingCanceled;
              default:
              {
                mdtError e(MDT_USB_IO_ERROR, "poll() call failed", mdtError::Error);
                e.setSystemError(errno, strerror(errno));
                MDT_ERROR_SET_SRC(e, "mdtUsbPort");
                e.commit();
                return UnknownError;
              }
            }
          }else{
            qDebug() << "RD: Can handle events, go !";
            // Can do event handling here
            err = libusb_handle_events_locked(pvLibusbContext, &pvReadTimeoutTv);  /// \todo: timeout ?
            switch(err){
              case 0:
                break;  // All ok, continue
              case LIBUSB_ERROR_INTERRUPTED:  /// \todo can be because device was deconnected, handle this..
                libusb_unlock_events(pvLibusbContext);
                return PortNotFound;
              default:
              {
                mdtError e(MDT_USB_IO_ERROR, "libusb_handle_events_locked", mdtError::Error);
                e.setSystemError(err, errorText(err));
                MDT_ERROR_SET_SRC(e, "mdtUsbPort");
                e.commit();
                libusb_unlock_events(pvLibusbContext);
                return UnknownError;
              }
            }
            // Check if transfer is finish
            qDebug() << "RD: handle event DONE";
          }
        }
        // Check if transfer is finish
        /// \todo Should not be the events locked ??
        qDebug() << "RD: unlock events ...";
        libusb_unlock_events(pvLibusbContext);
        qDebug() << "RD: checking states ...";
        lockMutex();
        Q_ASSERT(pvReadTransfer != 0);
        pComplete = (bool*)pvReadTransfer->user_data;
        complete = *pComplete;
        unlockMutex();
        qDebug() << "RD: checking states DONE";
      }
      // Release event lock
      ///libusb_unlock_events(pvLibusbContext);
    }else{
      // Other thread is doing event handling, wait
      libusb_lock_event_waiters(pvLibusbContext);
      // Wait ...
      libusb_wait_for_event(pvLibusbContext, 0);  ///  \todo : timeouts ?
      // Release event waiter lock
      libusb_unlock_event_waiters(pvLibusbContext);
    }
  }
  qDebug() << "RD: Locking I/O mutex ...";
  lockMutex();
  qDebug() << "RD: I/O mutext locked, end, return NoError";

  return NoError;
}

/// \todo Echanger fin et début transfert (sinon ne se fait pas de suite...)
qint64 mdtUsbPort::read(char *data, qint64 maxSize)
{
  Q_ASSERT(data != 0);

  int err;
  int len;
  int retLen;
  bool *pComplete;

  qDebug() << "mdtUsbPort::read() ...";

  if(pvReadTransferPending){
    // We have a transfer pending here
    Q_ASSERT(pvReadTransfer != 0);
    pComplete = (bool*)pvReadTransfer->user_data;
    Q_ASSERT(*pComplete == true);
    // Copy readen data
    retLen = pvReadTransfer->actual_length;
    memcpy(data, pvReadBuffer, retLen);
    // Free transfer (NOTE: should be done in close, or something..)
    libusb_free_transfer(pvReadTransfer);
    pvReadTransfer = 0;
    pvReadTransferPending = false;
  }else{
    retLen = 0;
  }
  // If no transfer is pending, create one
  if(!pvReadTransferPending){
    qDebug() << "RD: init transfer ...";
    // Alloc the new transfer
    pvReadTransfer = libusb_alloc_transfer(0);  /// \todo Could be done once by setup ?
    if(pvReadTransfer == 0){
      mdtError e(MDT_USB_IO_ERROR, "libusb_alloc_transfert() failed", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return -1;
    }
    pvReadTransferPending = true;
    // Ajust possible max length
    if(maxSize > (qint64)pvReadBufferSize){
      len = pvReadBufferSize;
    }else{
      len = maxSize;
    }
    // Fill the transfer
    pvReadTransferComplete = false;
    if(pvReadTransfertType == LIBUSB_TRANSFER_TYPE_BULK){
      libusb_fill_bulk_transfer(pvReadTransfer, pvHandle, pvReadEndpointAddress, (unsigned char*)pvReadBuffer, len, transferCallback, (void*)&pvReadTransferComplete, pvReadTimeout);
    }else if(pvReadTransfertType == LIBUSB_TRANSFER_TYPE_INTERRUPT){
      libusb_fill_interrupt_transfer(pvReadTransfer, pvHandle, pvReadEndpointAddress, (unsigned char*)pvReadBuffer, len, transferCallback, (void*)&pvReadTransferComplete, pvReadTimeout);
    }else{
      libusb_free_transfer(pvReadTransfer);
      mdtError e(MDT_USB_IO_ERROR, "Unknown transfert type", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return -1;
    }
    // Submit transfert
    err = libusb_submit_transfer(pvReadTransfer); /// \todo Handle retval
    if(err != 0){
      libusb_free_transfer(pvReadTransfer);
      mdtError e(MDT_USB_IO_ERROR, "libusb_submit_transfer() failed", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return -1;
    }
    // Report that 0 data was readen
    //return 0; /// \todo Case of breaking: transfer will not be freed
  }

  qDebug() << "Readen: " << retLen;
  return retLen;
}

void mdtUsbPort::flushIn()
{
  lockMutex();
  mdtAbstractPort::flushIn();
}

/// NOTE: \todo : Update timeout states !
mdtAbstractPort::error_t mdtUsbPort::waitEventWriteReady()
{
  qDebug() << "mdtUsbPort::waitEventWriteReady() ...";

  bool complete;
  bool *pComplete;
  struct pollfd *fds;
  int numFds = 0;
  int err;

  // If we have no transfer pending, simply return (write() will init a new one if needed)
  if(!pvWriteTransferPending){
    qDebug() << "WR: mdtUsbPort::waitEventWriteReady(): no transfer pending, return";
    return NoError;
  }
  // We have a pending transfer here, wait until complete, but give a chance to other thread to handle events
  qDebug() << "WR: Getting complete flag ...";
  Q_ASSERT(pvWriteTransfer != 0);
  pComplete = (bool*)pvWriteTransfer->user_data;
  complete = *pComplete;
  qDebug() << "WR: Getting complete flag DONE";
  // Now, we release the I/O mutex, and deal with events
  unlockMutex();
  usleep(50000);  /// NOTE: provisoire !!
  while(!complete){
    // Try to get the event lock
    qDebug() << "WR: try to get event lock ...";
    if(libusb_try_lock_events(pvLibusbContext) == 0){
      qDebug() << "WR: have the lock, check if can handle events now ...";
      // We have the event lock here, check if it's really possible to handle events
      if(!libusb_event_handling_ok(pvLibusbContext)){
        // Not possible now, unlock event lock and retry later
        qDebug() << "WR: Cannot handle events now, unlock ...";
        libusb_unlock_events(pvLibusbContext);
        // It can happen that other thread has complete the transfer, take a look
        lockMutex();
        Q_ASSERT(pvWriteTransfer != 0);
        pComplete = (bool*)pvWriteTransfer->user_data;
        complete = *pComplete;
        unlockMutex();
        continue;
      }else{
        // Wait on file descriptors
        fds = getLibusbPollFds(&numFds);
        err = poll(fds, numFds, -1);   /// NOTE: \todo timeout ?
        free(fds);
        if(err == 0){
          updateWriteTimeoutState(true);
        }else{
          updateWriteTimeoutState(false);
          if(err < 0){
            libusb_unlock_events(pvLibusbContext);
            switch(errno){
              case EINTR:
                /// \todo abort events ??
                return WaitingCanceled;
              default:
              {
                mdtError e(MDT_USB_IO_ERROR, "poll() call failed", mdtError::Error);
                e.setSystemError(errno, strerror(errno));
                MDT_ERROR_SET_SRC(e, "mdtUsbPort");
                e.commit();
                return UnknownError;
              }
            }
          }else{
            qDebug() << "WR: Can handle events, go !";
            // Can do event handling here
            err = libusb_handle_events_locked(pvLibusbContext, &pvWriteTimeoutTv);  /// \todo: timeout ?
            switch(err){
              case 0:
                break;  // All ok, continue
              case LIBUSB_ERROR_INTERRUPTED:  /// \todo can be because device was deconnected, handle this..
                libusb_unlock_events(pvLibusbContext);
                return PortNotFound;
              default:
              {
                mdtError e(MDT_USB_IO_ERROR, "libusb_handle_events_locked", mdtError::Error);
                e.setSystemError(err, errorText(err));
                MDT_ERROR_SET_SRC(e, "mdtUsbPort");
                e.commit();
                libusb_unlock_events(pvLibusbContext);
                return UnknownError;
              }
            }
            // Check if transfer is finish
            qDebug() << "WR: handle event DONE";
          }
        }
        // Check if transfer is finish
        /// \todo Should not be the events locked ??
        qDebug() << "WR: unlock events ...";
        libusb_unlock_events(pvLibusbContext);
        qDebug() << "WR: checking states ...";
        lockMutex();
        Q_ASSERT(pvWriteTransfer != 0);
        pComplete = (bool*)pvWriteTransfer->user_data;
        complete = *pComplete;
        unlockMutex();
        qDebug() << "WR: checking states DONE";
      }
      // Release event lock
      ///libusb_unlock_events(pvLibusbContext);
    }else{
      // Other thread is doing event handling, wait
      libusb_lock_event_waiters(pvLibusbContext);
      // Wait ...
      libusb_wait_for_event(pvLibusbContext, 0);  ///  \todo : timeouts ?
      // Release event waiter lock
      libusb_unlock_event_waiters(pvLibusbContext);
    }
  }
  qDebug() << "WR: Locking I/O mutex ...";
  lockMutex();
  qDebug() << "WR: I/O mutext locked, end, return NoError";

  return NoError;
}

qint64 mdtUsbPort::write(const char *data, qint64 maxSize)
{
  Q_ASSERT(data != 0);

  int err;
  int len;
  int retLen;
  bool *pComplete;

  qDebug() << "mdtUsbPort::write() ...";

  /*
   * If a transfer is pending, terminate it
   *  and return the length of written data.
   * We must not init a new transfert directly,
   *  because the caller must first do his job
   *  with actually pending data.
   * On next call, the transfer will be initalized
   *  and submitted, and this method returns a length of 0.
   */
  if(pvWriteTransferPending){
    // We have a transfer pending here
    Q_ASSERT(pvWriteTransfer != 0);
    pComplete = (bool*)pvWriteTransfer->user_data;
    Q_ASSERT(*pComplete == true);
    // Get size of written data
    retLen = pvWriteTransfer->actual_length;
    // Free transfer (NOTE: should be done in close, or something..)
    libusb_free_transfer(pvWriteTransfer);
    pvWriteTransfer = 0;
    pvWriteTransferPending = false;
    return retLen;
  }
  // If no transfer is pending, create one
  Q_ASSERT(!pvWriteTransferPending);
  retLen = 0;
  if(!pvWriteTransferPending){
    qDebug() << "WR: init transfer ...";
    // Alloc the new transfer
    pvWriteTransfer = libusb_alloc_transfer(0);  /// \todo Could be done once by setup ?
    if(pvWriteTransfer == 0){
      mdtError e(MDT_USB_IO_ERROR, "libusb_alloc_transfert() failed", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return -1;
    }
    pvWriteTransferPending = true;
    // Ajust possible max length
    if(maxSize > (qint64)pvWriteBufferSize){
      len = pvWriteBufferSize;
    }else{
      len = maxSize;
    }
    // Copy data to write
    memcpy(pvWriteBuffer, data, len);
    // Fill the transfer
    pvWriteTransferComplete = false;
    qDebug() << "WR: fill a new transfert, size: " << len;
    if(pvWriteTransfertType == LIBUSB_TRANSFER_TYPE_BULK){
      libusb_fill_bulk_transfer(pvWriteTransfer, pvHandle, pvWriteEndpointAddress, (unsigned char*)pvWriteBuffer, len, transferCallback, (void*)&pvWriteTransferComplete, pvWriteTimeout);
    }else if(pvReadTransfertType == LIBUSB_TRANSFER_TYPE_INTERRUPT){
      libusb_fill_interrupt_transfer(pvWriteTransfer, pvHandle, pvWriteEndpointAddress, (unsigned char*)pvWriteBuffer, len, transferCallback, (void*)&pvWriteTransferComplete, pvWriteTimeout);
    }else{
      libusb_free_transfer(pvWriteTransfer);
      mdtError e(MDT_USB_IO_ERROR, "Unknown transfer type", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return -1;
    }
    // Submit transfer
    err = libusb_submit_transfer(pvWriteTransfer); /// \todo Handle retval
    if(err != 0){
      libusb_free_transfer(pvWriteTransfer);
      mdtError e(MDT_USB_IO_ERROR, "libusb_submit_transfer() failed", mdtError::Error);
      MDT_ERROR_SET_SRC(e, "mdtUsbPort");
      e.commit();
      return -1;
    }
    // Report that 0 data was readen
    //return 0; /// \todo Case of breaking: transfer will not be freed
  }

  qDebug() << "Written: " << retLen;
  return retLen;
}

void mdtUsbPort::flushOut()
{
  lockMutex();
  mdtAbstractPort::flushOut();
}

void mdtUsbPort::transferCallback(struct libusb_transfer *transfer)
{
  Q_ASSERT(transfer != 0);

  qDebug() << "Tranfert callback ...";
  /*
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
  */
  /**
  mdt_usb_port_transfer_data *data = (mdt_usb_port_transfer_data*)transfer->user_data;
  Q_ASSERT(data != 0);
  data->flag1 = transfer->status;
  */
  
  bool *complete = (bool*)transfer->user_data;
  *complete = true;
}

mdtAbstractPort::error_t mdtUsbPort::pvOpen()
{
  Q_ASSERT(!isOpen());

  quint16 vid;
  quint16 pid;
  QString str;
  QStringList lst;
  bool ok = false;

  ///qDebug() << "mdtUsbPort::pvOpen() ... (Not implemented yet)";

  // Extract vendor ID and product ID
  lst = pvPortName.split(":");
  if(lst.size() < 2){
    mdtError e(MDT_USB_IO_ERROR, "Error in port format for " + pvPortName + " (must be idVendor:idProduct)", mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return SetupError;
  }
  // Check vendor ID format and convert
  str = lst.at(0);
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
  // Check product ID format and convert
  str = lst.at(1);
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
  // Open port
  pvHandle = libusb_open_device_with_vid_pid(pvLibusbContext, vid, pid);
  if(pvHandle == 0){
    mdtError e(MDT_USB_IO_ERROR, "Cannot open device " + pvPortName, mdtError::Error);
    MDT_ERROR_SET_SRC(e, "mdtUsbPort");
    e.commit();
    return PortNotFound;
  }

  return NoError;
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
}

void mdtUsbPort::pvFlushOut()
{
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

struct pollfd *mdtUsbPort::getLibusbPollFds(int *numFds)
{
  Q_ASSERT(pvLibusbContext != 0);

  int i;
  const struct libusb_pollfd **libusbPollFds;

  libusbPollFds = libusb_get_pollfds(pvLibusbContext);
  Q_ASSERT(libusbPollFds != 0); // NOTE: not available on Windows with libusb <= 1.0.9
  i=0;
  while(libusbPollFds[i] != 0){
    i++;
  }
  *numFds = i;
  qDebug() << "Alloc for " << *numFds << " fds";
  struct pollfd *fds = new struct pollfd[*numFds];
  // Copy
  for(i=0; i<*numFds; i++){
    fds[i].fd = libusbPollFds[i]->fd;
    fds[i].events = libusbPollFds[i]->events;
    fds[i].revents = 0;
  }
  free(libusbPollFds);
  Q_ASSERT(fds != 0);

  return fds;
}
