/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtUsbtmcTransferHandler.h"
#include "mdtUsbtmcControlTransfer.h"
#include "mdtUsbtmcBulkTransfer.h"

#include <QDebug>

mdtUsbtmcTransferHandler::mdtUsbtmcTransferHandler(libusb_context *usbContext)
 : pvUsbContext(usbContext),
   pvDeviceHandle(0),
   pvStateMachine(State_t::Stopped)
{
  Q_ASSERT(pvUsbContext != 0);
}

bool mdtUsbtmcTransferHandler::setup ( libusb_device_handle* handle, const mdtUsbDeviceDescriptor & descriptor, uint8_t bInterfaceNumber )
{
  Q_ASSERT(!descriptor.isEmpty());

  setCurrentState(State_t::Setup);
  pvDeviceDescriptor = descriptor;
  // Get descriptor of requested interface
  pvInterfaceDescriptor = pvDeviceDescriptor.interface(bInterfaceNumber);
  if(pvInterfaceDescriptor.isEmpty()){
    std::lock_guard<std::mutex> lelg(pvLastErrorMutex);
    QString msg = deviceIdString();
    msg += QObject::tr("requested bInterfaceNumber ") + QString::number(bInterfaceNumber) + QObject::tr(" was not found in given device descriptor.");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    return false;
  }
  /*
   * Check that interface is USBTMC compliant:
   *  - 1 bulk OUT endpoint
   *  - 1 bulk IN endpoint
   *  - Optionnaly, 1 interrupt IN endpoint
   */
  // Get bulk OUT endpoint descriptor
  if(pvInterfaceDescriptor.bulkOutEndpoints().size() != 1){
    std::lock_guard<std::mutex> lelg(pvLastErrorMutex);
    QString msg = deviceIdString();
    msg += QObject::tr("interface ") + QString::number(bInterfaceNumber) + QObject::tr(" has not exactly 1 bulk OUT endpoint (not USBTMC compliant).");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    return false;
  }
  pvBulkOutDescriptor = pvInterfaceDescriptor.bulkOutEndpoints().at(0);
  // Get bulk IN endpoint descriptor
  if(pvInterfaceDescriptor.bulkInEndpoints().size() != 1){
    std::lock_guard<std::mutex> lelg(pvLastErrorMutex);
    QString msg = deviceIdString();
    msg += QObject::tr("interface ") + QString::number(bInterfaceNumber) + QObject::tr(" has not exactly 1 bulk IN endpoint (not USBTMC compliant).");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    return false;
  }
  pvBulkInDescriptor = pvInterfaceDescriptor.bulkInEndpoints().at(0);
  // Setup control endpoint
  if(!pvControlTransferPool.allocTransfers(3, *this, handle)){
    std::lock_guard<std::mutex> lelg(pvLastErrorMutex);
    QString msg = deviceIdString();
    msg += QObject::tr("Allocation of control transfer pool failed.");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    return false;
  }
  /// \todo Setup bulk OUT endpoint
  
  /// \todo Setup bulk IN endpoint
  
  /// \todo Setup interrupt IN endpoint

  // Get USBTMC capabilities
  if(!getCapabilities(pvInterfaceDescriptor.bInterfaceNumber(), 10000)){
    return false;
  }

  return true;
}

void mdtUsbtmcTransferHandler::submitClearEndpointHalt(uint8_t endpointNumber, unsigned int timeout)
{
  if(currentState() != State_t::Running){
    return;
  }
  // Get a transfer from pool
  std::lock_guard<std::mutex> ctlg(pvControlTransferMutex);
  mdtUsbtmcControlTransfer *transfer = pvControlTransferPool.getTransfer(*this);
  Q_ASSERT(transfer != 0);
  // Setup transfer and submit it
  transfer->setupClearEndpointHalt(endpointNumber, timeout);
  if(!transfer->submit()){
    pvLastErrorMutex.lock();
    pvLastError = transfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
  }
}

bool mdtUsbtmcTransferHandler::beginAbortBulkOutTransfer()
{

}

void mdtUsbtmcTransferHandler::beginClearBulkIo()
{
  if(currentState() != State_t::Running){
    return;
  }
  /** \todo
   *  - change current state
   *  - Cancel bulk I/Os
   */
  // Get a transfer from pool
  std::lock_guard<std::mutex> ctlg(pvControlTransferMutex);
  mdtUsbtmcControlTransfer *transfer = pvControlTransferPool.getTransfer(*this);
  Q_ASSERT(transfer != 0);
  // Setup transfer and submit it
  qDebug() << "Submitting INITIATE_CLEAR ...";
  transfer->setupInitiateClear(pvInterfaceDescriptor.bInterfaceNumber(), 30000);
  if(!transfer->submit()){
    pvLastErrorMutex.lock();
    pvLastError = transfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
  }
}

void mdtUsbtmcTransferHandler::submitControlTransfersCancel()
{
  /*
   * By little experience (with a U3606A),
   * trying to cancel control transfer just before closing give bad results.
   * So, just do nothing.
   */
  if(currentState() != State_t::Running){
    return;
  }
  // Get pending transfer, if available
  std::lock_guard<std::mutex> ctlg(pvControlTransferMutex);
  mdtUsbtmcControlTransfer *transfer = pvControlTransferPool.getPendingTransfer();
  if(transfer == 0){
    return;
  }
  // Cancel transfer
  qDebug() << "Submitting a control transfer cancel ...";
  if(!transfer->cancel()){
    pvLastErrorMutex.lock();
    pvLastError = transfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
  }
}

void mdtUsbtmcTransferHandler::controlTransferCallback(libusb_transfer* transfer)
{
  Q_ASSERT(transfer != 0);

  mdtUsbtmcControlTransfer *uct = static_cast<mdtUsbtmcControlTransfer*>(transfer->user_data);
  uct->transferHandler().handleControlTransferComplete(uct);
}

void mdtUsbtmcTransferHandler::handleControlTransferComplete(mdtUsbtmcControlTransfer *transfer )
{
  Q_ASSERT(transfer != 0);

  QString msg;

  qDebug() << "Control transfer status: " << transfer->status() << " - length: " << transfer->actualLength();
  // For synchronous transfers, we only set them completed, and in any state
  if(transfer->isSync()){
    transfer->setCompleted();
    return;
  }
  // Check state before acting
  if(currentState() != State_t::Running){
    qDebug() << "No more running, do nothing more ...";
    return;
  }
  // Check transfer status
  switch(transfer->status()){
    case LIBUSB_TRANSFER_COMPLETED: // OK, not a error
      break;
    case LIBUSB_TRANSFER_STALL:
      // We sent a query that is not supported by device.
      pvLastErrorMutex.lock();
      msg = deviceIdString() + QObject::tr(": control query with bRequest '") + QString::number(transfer->bRequest()) + "' ";
      msg += QObject::tr("is not supported by device.");
      pvLastError.setError(msg, mdtError::Warning);
      MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
      pvLastError.commit();
      pvLastErrorMutex.unlock();
      // Restore frame to pool
      pvControlTransferMutex.lock();
      pvControlTransferPool.restoreTransfer(transfer);
      pvControlTransferMutex.unlock();
      return;
    case LIBUSB_TRANSFER_TIMED_OUT: /// \todo handle !
    case LIBUSB_TRANSFER_CANCELLED: /// \todo handle !
    case LIBUSB_TRANSFER_NO_DEVICE: /// \todo handle !
    case LIBUSB_TRANSFER_OVERFLOW: /// \todo handle !
    case LIBUSB_TRANSFER_ERROR: /// \todo handle !
      qDebug() << "Unhandled status from completed Control transfer: " << transfer->status();
      // Restore frame to pool
      pvControlTransferMutex.lock();
      pvControlTransferPool.restoreTransfer(transfer);
      pvControlTransferMutex.unlock();
      setCurrentState(State_t::Error);
      return;
  }
  // See if transfer contains a response that must be handled in sync way
  if(transfer->actualLength() > 0){
    switch(transfer->usbtmcRequest()){
      case mdtUsbtmcControlTransfer::USBTMCbRequest::INITIATE_CLEAR:
        // We simply let the transfer in pool's pending queue
        return;
      default:
        // We received a response that we don't handle
        pvLastErrorMutex.lock();
        QString msg = deviceIdString() + QObject::tr("received a unhandled control response:");
        msg += " " + transfer->usbtmcRequestText() + " . ";
        msg += QObject::tr("Will be ignored.");
        pvLastError.setError(msg, mdtError::Warning);
        MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
        pvLastError.commit();
        pvLastErrorMutex.unlock();
        // Restore frame to pool
        pvControlTransferMutex.lock();
        pvControlTransferPool.restoreTransfer(transfer);
        pvControlTransferMutex.unlock();
        return;
    }
  }
  // Nothing more - Restore transfer to pool
  std::lock_guard<std::mutex> lg(pvControlTransferMutex);
  pvControlTransferPool.restoreTransfer(transfer);
}

void mdtUsbtmcTransferHandler::bulkOutTransferCallback ( libusb_transfer* transfer )
{
  Q_ASSERT(transfer != 0);

  mdtUsbtmcBulkTransfer *ubt = static_cast<mdtUsbtmcBulkTransfer*>(transfer->user_data);
  ubt->transferHandler().handleBulkOutTransferComplete(ubt);
}

void mdtUsbtmcTransferHandler::handleBulkOutTransferComplete(mdtUsbtmcBulkTransfer* transfer)
{
  qDebug() << "Bulk-OUT transfer completed :-) - status: " << transfer->status();
}

void mdtUsbtmcTransferHandler::handleSyncEvents()
{
  // Handle control responses if any
  mdtUsbtmcControlTransfer * controlTransfer = pvControlTransferPool.getPendingTransfer();
  if(controlTransfer != 0){
    Q_ASSERT(controlTransfer->actualLength() > 0);
    qDebug() << "Have control transfer, USBTMC bRequest: " << static_cast<uint8_t>(controlTransfer->usbtmcRequest());
    switch(controlTransfer->usbtmcRequest()){
      case mdtUsbtmcControlTransfer::USBTMCbRequest::INITIATE_CLEAR:
        continueClearBulkIo(controlTransfer);
        return;
      default:
        // We received a response that we don't handle
        pvLastErrorMutex.lock();
        QString msg = deviceIdString() + QObject::tr("received a unhandled control response:");
        msg += " " + controlTransfer->usbtmcRequestText() + ". ";
        msg += QObject::tr("Will be ignored.");
        pvLastError.setError(msg, mdtError::Warning);
        MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
        pvLastError.commit();
        pvLastErrorMutex.unlock();
        // Restore frame to pool
        pvControlTransferMutex.lock();
        pvControlTransferPool.restoreTransfer(controlTransfer);
        pvControlTransferMutex.unlock();
        return;
    }
  }
  

}

bool mdtUsbtmcTransferHandler::hasPendingTransfers() const
{
  // Note: we ignore control transfers
  
  return false;
}

bool mdtUsbtmcTransferHandler::getCapabilities(uint8_t interfaceNumber, unsigned int timeout)
{
  if((currentState() != State_t::Running) && (currentState() != State_t::Setup)){
    return true;
  }
  // Get a transfer from pool
  pvControlTransferMutex.lock();
  mdtUsbtmcControlTransfer *transfer = pvControlTransferPool.getTransfer(*this);
  Q_ASSERT(transfer != 0);
  // Setup transfer
  qDebug() << "Submitting GET_CAPABILITIES ...";
  transfer->setupGetCapabilities(interfaceNumber, timeout);
  // Submit request
  pvControlTransferMutex.unlock();
  if(!processSyncControlTransfer(transfer)){
    return false;
  }
  // Check response
  if((transfer->actualLength() != 24) || (transfer->usbtmcStatus() != mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_SUCCESS)){
    std::lock_guard<std::mutex> lelg(pvLastErrorMutex);
    QString msg = deviceIdString() + QObject::tr("GET_CAPABILITIES failed.");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    // Restore transfer back to pool
    std::lock_guard<std::mutex> ctlg(pvControlTransferMutex);
    pvControlTransferPool.restoreTransfer(transfer);
    return false;
  }
  /// \todo Result...
  
  // Restore transfer back to pool
  pvControlTransferMutex.lock();
  pvControlTransferPool.restoreTransfer(transfer);
  pvControlTransferMutex.unlock();

  qDebug() << "GET_CAPABILITIES DONE";
  return true;
}

bool mdtUsbtmcTransferHandler::continueAbortBulkOutTransfer()
{


}

void mdtUsbtmcTransferHandler::continueClearBulkIo(mdtUsbtmcControlTransfer* transfer)
{
  Q_ASSERT(transfer != 0);

  qDebug() << "Continue INITIATE_CLEAR - status: " << transfer->usbtmcStatusText();
  // Check USBTMC_status
  if((int)transfer->usbtmcStatus() >= 0x80){
    std::lock_guard<std::mutex> lelg(pvLastErrorMutex);
    QString msg = deviceIdString();
    msg += QObject::tr("received error status after INITIATE_CLEAR. Status: ") + transfer->usbtmcStatusText() + " . ";
    msg += QObject::tr("Aborting bulk I/O clear.");
    pvLastError.setError(msg, mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    // Restore transfer back to pool
    std::lock_guard<std::mutex> ctlg(pvControlTransferMutex);
    pvControlTransferPool.restoreTransfer(transfer);
    return;
  }
  if(transfer->usbtmcStatus() != mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_SUCCESS){
    // Nothing more to do - Restore transfer
    std::lock_guard<std::mutex> ctlg(pvControlTransferMutex);
    pvControlTransferPool.restoreTransfer(transfer);
    return;
  }
  // Here we have STATUS_SUCCESS
  Q_ASSERT(transfer->usbtmcStatus() == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_SUCCESS);
  // Continue with CHECK_CLEAR_STATUS
  do{
    // CHECK_CLEAR_STATUS
    transfer->setupCheckClearStatus(pvInterfaceDescriptor.bInterfaceNumber(), 10000);
    if(!processSyncControlTransfer(transfer)){
      setCurrentState(State_t::Error);
      return;
    }
    qDebug() << "Continue CHECK_CLEAR_STATUS - status: " << transfer->usbtmcStatusText();
    // Check if we must read bulk-IN
    if(transfer->usbtmcStatus() == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_PENDING){
      if(transfer->byteValue(1) & 0x01){  // bmClear
        /// \todo READ bulk-IN
        
        /// \todo sleep
        
      }
    }
  }while(transfer->usbtmcStatus() == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_PENDING);
  Q_ASSERT(transfer->usbtmcStatus() != mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_PENDING);
  // OK: restore transfer to pool and request a clear bulk out halt and done
  pvControlTransferMutex.lock();
  pvControlTransferPool.restoreTransfer(transfer);
  pvControlTransferMutex.unlock();
  submitClearBulkOutEndpointHalt(10000);
}

bool mdtUsbtmcTransferHandler::processSyncControlTransfer(mdtUsbtmcControlTransfer *transfer)
{
  Q_ASSERT(transfer != 0);

  int err;

  // Submit transfer in sync mode
  if(!transfer->submit(true)){
    std::lock_guard<std::mutex> lg(pvLastErrorMutex);
    pvLastError = transfer->lastError();
    return false;
  }
  // Wait on its completion
  while(!transfer->isCompleted()){
    qDebug() << "USB sync wait ...";
    err = libusb_handle_events_completed(pvUsbContext, transfer->completedPointer());
    qDebug() << "USB sync Event !";
    if(err < 0){
      if(err == LIBUSB_ERROR_INTERRUPTED){
        continue;
      }
      // Unhandled error, cancel transfer
      pvLastErrorMutex.lock();
      pvLastError.setError(QObject::tr("USB event handling failed (libusb_handle_events())"), mdtError::Error);
      pvLastError.setSystemError(err, libusb_strerror((libusb_error)err));
      MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
      pvLastError.commit();
      pvLastErrorMutex.unlock();
      // Submit cancel request
      if(!transfer->cancel(true)){
        std::lock_guard<std::mutex> lelg(pvLastErrorMutex);
        pvLastError = transfer->lastError();
        return false;
      }
      // Wait on cancel completion
      while(!transfer->isCompleted()){
        err = libusb_handle_events_completed(pvUsbContext, transfer->completedPointer());
        if(err < 0){
          break;
        }
      }
      return false;
    }
  }

  return true;
}

QString mdtUsbtmcTransferHandler::deviceIdString() const
{
  return QObject::tr("Device") + " '" + pvDeviceDescriptor.idString() + "' : ";
}

void mdtUsbtmcTransferHandler::setCurrentStateFromLibusbError ( libusb_error err )
{
  /// \todo Handle better ...
  switch(err){
    case LIBUSB_SUCCESS:
      return;
    case LIBUSB_ERROR_IO:
    case LIBUSB_ERROR_INVALID_PARAM:
    case LIBUSB_ERROR_ACCESS:
    case LIBUSB_ERROR_NO_DEVICE:
    case LIBUSB_ERROR_NOT_FOUND:
    case LIBUSB_ERROR_BUSY:
    case LIBUSB_ERROR_TIMEOUT:
    case LIBUSB_ERROR_OVERFLOW:
    case LIBUSB_ERROR_PIPE:
    case LIBUSB_ERROR_INTERRUPTED:
    case LIBUSB_ERROR_NO_MEM:
    case LIBUSB_ERROR_NOT_SUPPORTED:
    case LIBUSB_ERROR_OTHER:
      setCurrentState(State_t::Error);
  }
}
