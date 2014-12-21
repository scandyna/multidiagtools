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
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono

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
  // Setup bulk-OUT endpoint
  if(!pvBulkOutTransferPool.allocTransfers(1, *this, pvBulkOutDescriptor, handle)){
    std::lock_guard<std::mutex> lelg(pvLastErrorMutex);
    QString msg = deviceIdString();
    msg += QObject::tr("Allocation of bulk-OUT transfer pool failed.");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    return false;
  }
  // Setup bulk-IN endpoint
  if(!pvBulkInTransferPool.allocTransfers(3, *this, pvBulkInDescriptor, handle)){
    std::lock_guard<std::mutex> lelg(pvLastErrorMutex);
    QString msg = deviceIdString();
    msg += QObject::tr("Allocation of bulk-IN transfer pool failed.");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    return false;
  }

  /// \todo Setup interrupt IN endpoint

  // Get USBTMC capabilities
  if(!getCapabilities(pvInterfaceDescriptor.bInterfaceNumber(), 10000)){
    return false;
  }

  return true;
}

void mdtUsbtmcTransferHandler::submitCommand(mdtUsbtmcMessage & message, bool responseExpected, unsigned int timeout)
{
  State_t state = currentState();
  if(state != State_t::Running){
    // If we are aborting a bulk transfer, warn
    if(state == State_t::AbortingBulkIo){
      pvLastErrorMutex.lock();
      QString msg = deviceIdString() + QObject::tr(": trying to send a command while aborting a bulk transfer - Will be ignored !");
      pvLastError.setError(msg, mdtError::Warning);
      MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
      pvLastError.commit();
      pvLastErrorMutex.unlock();
    }
    return;
  }
  // Get a transfer from pool
  std::lock_guard<std::mutex> ctlg(pvBulkOutTransferMutex);
  mdtUsbtmcBulkTransfer *transfer = pvBulkOutTransferPool.getTransfer(*this, pvBulkOutDescriptor);
  Q_ASSERT(transfer != 0);
  // Setup transfer and submit it
  transfer->setupDevDepMsgOut(nextBulkOutbTag(), message, responseExpected, timeout);
  if(!transfer->submit()){
    pvBulkOutTransferPool.restoreTransfer(transfer);
    pvLastErrorMutex.lock();
    pvLastError = transfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
  }
}

// void mdtUsbtmcTransferHandler::submitBulkOutTransfersCancellation()
// {
//   if(currentState() != State_t::Running){
//     return;
//   }
//   std::lock_guard<std::mutex> ctlg(pvBulkOutTransferMutex);
//   auto pendingTransfers = pvBulkOutTransferPool.getAllPendingTransfers();
//   if(pendingTransfers.empty()){
//     return;
//   }
//   qDebug() << "Bulk-OUT: have " << pendingTransfers.size() << " transfers to cancel";
//   // Change state to AbortingBulkIo to prevent more transfer requests
//   setCurrentState(State_t::AbortingBulkIo);
//   // Submit cancellation for all transfers
//   for(unsigned int i = 0; i < (pendingTransfers.size() - 1); ++i){
//     Q_ASSERT(pendingTransfers[i] != 0);
//     submitRetireBulkIRPs(pendingTransfers[i]);
//   }
//   // For the last transfer, we must tell handleBulkOutTransferComplete() that it must beginn INITIATE_ABORT_BULK_OUT
//   auto transfer = pendingTransfers[pendingTransfers.size() - 1];
//   transfer->setInitiateAction(mdtUsbtmcBulkTransfer::InitiateAction_t::INITIATE_ABORT);
//   /**
//   bool first = true;
//   for(auto transfer : pendingTransfers){
//     Q_ASSERT(transfer != 0);
//     if(first){
//       transfer->setInitiateAction(mdtUsbtmcBulkTransfer::InitiateAction_t::INITIATE_ABORT);
//       first = false;
//     }
//   }
//   */
// }

// void mdtUsbtmcTransferHandler::submitBulkInTransfersCancellation()
// {
//   if(currentState() != State_t::Running){
//     return;
//   }
//   std::lock_guard<std::mutex> ctlg(pvBulkInTransferMutex);
//   auto pendingTransfers = pvBulkInTransferPool.getAllPendingTransfers();
//   for(auto transfer : pendingTransfers){
//     Q_ASSERT(transfer != 0);
//     if(!transfer->cancel()){
//       pvLastErrorMutex.lock();
//       pvLastError = transfer->lastError();
//       pvLastErrorMutex.unlock();
//       setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
//       return;
//     }
//   }
// }

void mdtUsbtmcTransferHandler::dbgSubmitCustomBulkOutTransfer(uint8_t msgID, uint8_t bTag, uint8_t bTagInverse, uint32_t transferSize, uint8_t bmTransferAttributes, uint8_t termChar,
                                                              bool responseExpected, int transferBufferLength, unsigned int timeout)
{
  if(currentState() != State_t::Running){
    return;
  }
  // Get a transfer from pool
  std::lock_guard<std::mutex> ctlg(pvBulkOutTransferMutex);
  mdtUsbtmcBulkTransfer *transfer = pvBulkOutTransferPool.getTransfer(*this, pvBulkOutDescriptor);
  Q_ASSERT(transfer != 0);
  // Setup transfer and submit it
  transfer->dbgSetupCustom(msgID, bTag, bTagInverse, transferSize, bmTransferAttributes, termChar, responseExpected, transferBufferLength, timeout);
  if(!transfer->submit()){
    pvBulkOutTransferPool.restoreTransfer(transfer);
    pvLastErrorMutex.lock();
    pvLastError = transfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
  }
}

void mdtUsbtmcTransferHandler::dbgSubmitAbortOnePendingBulkOutTransfer()
{
  if(currentState() != State_t::Running){
    return;
  }
  std::lock_guard<std::mutex> ctlg(pvBulkOutTransferMutex);
  auto transfer = pvBulkOutTransferPool.getPendingTransfer();
  if(transfer == 0){
    return;
  }
  setCurrentState(State_t::AbortingBulkIo);
  transfer->setSplitAction(mdtUsbtmcBulkTransfer::SplitAction_t::INITIATE_ABORT);
  submitRetireBulkIRPs(transfer);
}

void mdtUsbtmcTransferHandler::dbgSubmitAbortOnePendingBulkInTransfer()
{
  if(currentState() != State_t::Running){
    return;
  }
  std::lock_guard<std::mutex> ctlg(pvBulkInTransferMutex);
  auto transfer = pvBulkInTransferPool.getPendingTransfer();
  if(transfer != 0){
    beginAbortBulkInTransfer(transfer->bTag());
  }
}

void mdtUsbtmcTransferHandler::submitClearEndpointHalt(uint8_t endpointNumber, unsigned int timeout)
{
  auto state = currentState();
  ///if((state != State_t::Running) && (state != State_t::AbortingBulkOut) && (state != State_t::AbortingBulkIn)){
  if((state != State_t::Running) && (state != State_t::AbortingBulkIo)){
    return;
  }
  // Get a transfer from pool
  std::lock_guard<std::mutex> ctlg(pvControlTransferMutex);
  mdtUsbtmcControlTransfer *transfer = pvControlTransferPool.getTransfer(*this);
  Q_ASSERT(transfer != 0);
  // Setup transfer and submit it
  transfer->setupClearEndpointHalt(endpointNumber, timeout);
  if(!transfer->submit()){
    pvControlTransferPool.restoreTransfer(transfer);
    pvLastErrorMutex.lock();
    pvLastError = transfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
  }
}

void mdtUsbtmcTransferHandler::beginClearBulkIo()
{
  if(currentState() != State_t::Running){ /// \todo Check state !
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


void mdtUsbtmcTransferHandler::bulkOutTransferCallback ( libusb_transfer* transfer )
{
  Q_ASSERT(transfer != 0);

  mdtUsbtmcBulkTransfer *ubt = static_cast<mdtUsbtmcBulkTransfer*>(transfer->user_data);
  ubt->transferHandler().handleBulkOutTransferComplete(ubt);
}

void mdtUsbtmcTransferHandler::bulkInTransferCallback ( libusb_transfer* transfer )
{
  Q_ASSERT(transfer != 0);

  mdtUsbtmcBulkTransfer *ubt = static_cast<mdtUsbtmcBulkTransfer*>(transfer->user_data);
  ubt->transferHandler().handleBulkInTransferComplete(ubt);
}

// void mdtUsbtmcTransferHandler::handleSyncEvents()
// {
//   // Handle control responses if any
//   mdtUsbtmcControlTransfer * controlTransfer = pvControlTransferPool.getPendingTransfer();
//   if(controlTransfer != 0){
//     // We should not receive 0 length control responses here.
//     if(controlTransfer->actualLength() < 1){
//       pvLastErrorMutex.lock();
//       QString msg = deviceIdString() + QObject::tr(": received a control response without data. ");
//       msg += QObject::tr("Will be ignored.");
//       pvLastError.setError(msg, mdtError::Warning);
//       MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
//       pvLastError.commit();
//       pvLastErrorMutex.unlock();
//       // Restore frame to pool
//       pvControlTransferMutex.lock();
//       pvControlTransferPool.restoreTransfer(controlTransfer);
//       pvControlTransferMutex.unlock();
//       return;
//     }
//     Q_ASSERT(controlTransfer->actualLength() > 0);
//     qDebug() << "handleSyncEvents(): have control transfer, USBTMC bRquest: " << controlTransfer->usbtmcRequestText();
//     qDebug() << "-> transfer status: " << controlTransfer->status();
//     switch(controlTransfer->usbtmcRequest()){
//       case mdtUsbtmcControlTransfer::USBTMCbRequest::INITIATE_ABORT_BULK_OUT:
//         continueAbortBulkOutTransfer(controlTransfer);
//         return;
//       case mdtUsbtmcControlTransfer::USBTMCbRequest::INITIATE_CLEAR:
//         continueClearBulkIo(controlTransfer);
//         return;
//       default:
//         // We received a response that we don't handle
//         pvLastErrorMutex.lock();
//         QString msg = deviceIdString() + QObject::tr(": received a unhandled control response:");
//         msg += " " + controlTransfer->usbtmcRequestText() + ". ";
//         msg += QObject::tr("Will be ignored.");
//         pvLastError.setError(msg, mdtError::Warning);
//         MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
//         pvLastError.commit();
//         pvLastErrorMutex.unlock();
//         // Restore frame to pool
//         pvControlTransferMutex.lock();
//         pvControlTransferPool.restoreTransfer(controlTransfer);
//         pvControlTransferMutex.unlock();
//         return;
//     }
//   }
//   
// 
// }

bool mdtUsbtmcTransferHandler::hasPendingTransfers() const
{
  // Note: we ignore control transfers
  
  return false;
}

void mdtUsbtmcTransferHandler::handleBulkOutTransferComplete(mdtUsbtmcBulkTransfer* transfer)
{
  Q_ASSERT(transfer != 0);


  // Check current state
  auto state = currentState();
  /*
   * If we are in Running or AbortingBulkIo state and transfer was cancelled,
   *  we restore transfer back to pool, and begin INITIATE action if required.
   */
  if((state == State_t::Running) || (state == State_t::AbortingBulkIo)){
    if(transfer->status() == LIBUSB_TRANSFER_CANCELLED){
      qDebug() << "Bulk-OUT transfer cancelled - bTag: " << transfer->bTag();
      auto bTag = transfer->bTag();
      restoreBulkOutTransferToPool(transfer, true);
      switch(transfer->splitAction()){
        case mdtUsbtmcBulkTransfer::SplitAction_t::NoAction:
          return;
        case mdtUsbtmcBulkTransfer::SplitAction_t::INITIATE_ABORT:
          beginAbortBulkOutTransfer(bTag);
          return;
      }
    }
  }
  /*
   * All other states than Running (=currently not properly handled)
   */
  if(state != State_t::Running){
    restoreBulkOutTransferToPool(transfer, true);
    std::lock_guard<std::mutex> lelg(pvLastErrorMutex);
    QString msg = deviceIdString() + QObject::tr("Bulk-OUT transfer completed in a unhandled state. Will be lost.");
    pvLastError.setError(msg, mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    return;
  }
  /*
   * Here we are in Running state
   */
  Q_ASSERT(state == State_t::Running);

  qDebug() << "Bulk-OUT transfer completed :-) - status: " << transfer->status();
  qDebug() << "-> bTag: " << transfer->bTag();

  // Check transfer status
  switch(transfer->status()){
    case LIBUSB_TRANSFER_COMPLETED: // OK, not a error
      break;
    case LIBUSB_TRANSFER_STALL: /// \todo handle !
      qDebug() << "Bulk-OUT error status: LIBUSB_TRANSFER_STALL";
      break;
    case LIBUSB_TRANSFER_TIMED_OUT: /// \todo handle !
      qDebug() << "Bulk-OUT error status: LIBUSB_TRANSFER_TIMED_OUT";
      break;
    case LIBUSB_TRANSFER_CANCELLED: /// \todo handle !
      qDebug() << "Bulk-OUT error status: LIBUSB_TRANSFER_CANCELLED";
      return;
    case LIBUSB_TRANSFER_NO_DEVICE: /// \todo handle !
      qDebug() << "Bulk-OUT error status: LIBUSB_TRANSFER_NO_DEVICE";
      restoreBulkOutTransferToPool(transfer, true);
      setLastErrorFromBulkOutTransferStatus(transfer, mdtError::Warning, true);
      setCurrentState(State_t::Disconnected);
      return;
    case LIBUSB_TRANSFER_OVERFLOW: /// \todo handle !
      qDebug() << "Bulk-OUT error status: LIBUSB_TRANSFER_OVERFLOW";
      restoreBulkOutTransferToPool(transfer, true);
      setLastErrorFromBulkOutTransferStatus(transfer, mdtError::Error, true);
      setCurrentState(State_t::Error);
      return;
    case LIBUSB_TRANSFER_ERROR: /// \todo handle !
      qDebug() << "Bulk-OUT error status: LIBUSB_TRANSFER_ERROR";
      restoreBulkOutTransferToPool(transfer, true);
      setLastErrorFromBulkOutTransferStatus(transfer, mdtError::Error, true);
      setCurrentState(State_t::Error);
      return;
  }
  // Restore transfer to pool
  pvBulkOutTransferMutex.lock();
  pvBulkOutTransferPool.restoreTransfer(transfer);
  pvBulkOutTransferMutex.unlock();
  // ...
  if((transfer->msgID() == mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_OUT) && (transfer->responseExpected())){
    /// \todo Reprendre timeout initialement demandé.
    submitRequestDevDepMsgIn(30000);
    return;
  }
  if(transfer->msgID() == mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_IN){
    /// \todo Reprendre timeout initialement demandé.
    submitBulkInTransfer(30000);
    return;
  }

}

void mdtUsbtmcTransferHandler::beginAbortBulkOutTransfer(uint8_t bTag)
{
  if(currentState() != State_t::AbortingBulkIo){
    return;
  }
  // Get a contron transfer from pool
  std::lock_guard<std::mutex> ctlg(pvControlTransferMutex);
  mdtUsbtmcControlTransfer *transfer = pvControlTransferPool.getTransfer(*this);
  Q_ASSERT(transfer != 0);
  // Setup transfer and submit it
  qDebug() << "Submitting INITIATE_ABORT_BULK_OUT ...";
  transfer->setupInitiateAbortBulkOut(bTag, pvBulkOutDescriptor.number(), 30000);
  if(!transfer->submit()){
    pvLastErrorMutex.lock();
    pvLastError = transfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
  }
}

void mdtUsbtmcTransferHandler::handleInitiateAbortBulkOutResponse(mdtUsbtmcControlTransfer* transfer)
{
  Q_ASSERT(transfer != 0);
  Q_ASSERT(transfer->actualLength() > 0); // Can be > 2
  Q_ASSERT(transfer->usbtmcRequest() == mdtUsbtmcControlTransfer::USBTMCbRequest::INITIATE_ABORT_BULK_OUT);

  if(currentState() != State_t::AbortingBulkIo){
    restoreControlTransferToPool(transfer, true);
    return;
  }
  // Check transfer size - must be 2
  qDebug() << "actual len: " << transfer->actualLength() << " - wLength: " << transfer->wLength();
  if(transfer->wLength() != 2){
    pvLastErrorMutex.lock();
    QString msg = deviceIdString() + QObject::tr("INITIATE_ABORT_BULK_OUT request returned unexpected amount of data (expected: 2)");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    pvLastErrorMutex.unlock();
    setCurrentState(State_t::Error);
    return;
  }

  qDebug() << "Continue INITIATE_ABORT_BULK_OUT - status: " << transfer->usbtmcStatusText();
  
  // Check status
  auto status = transfer->usbtmcStatus();
  // On any failed status, bulk-OUT fifo is empty and we are done
  if((status != mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_SUCCESS) && (status != mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_TRANSFER_NOT_IN_PROGRESS)){
    restoreControlTransferToPool(transfer, true);
    setCurrentState(State_t::Running);
    return;
  }
  // By STATUS_TRANSFER_NOT_IN_PROGRESS, let device some time and resubmit INITIATE_ABORT_BULK_OUT
  if(status == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_TRANSFER_NOT_IN_PROGRESS){
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    if(!transfer->submit()){
      pvLastErrorMutex.lock();
      pvLastError = transfer->lastError();
      pvLastErrorMutex.unlock();
      setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
      return;
    }
    return;
  }
  // Here we are in STATUS_SUCCESS - Lets begin check abort status
  Q_ASSERT(status == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_SUCCESS);
  transfer->setupCheckAbortBulkOutStatus(pvBulkOutDescriptor.number(), 30000);
  if(!transfer->submit()){
    pvLastErrorMutex.lock();
    pvLastError = transfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
  }
}

void mdtUsbtmcTransferHandler::handleCheckAbortBulkOutStatusResponse(mdtUsbtmcControlTransfer* transfer)
{
  Q_ASSERT(transfer != 0);
  Q_ASSERT(transfer->actualLength() > 0); // Can be > 2
  Q_ASSERT(transfer->usbtmcRequest() == mdtUsbtmcControlTransfer::USBTMCbRequest::CHECK_ABORT_BULK_OUT_STATUS);

  if(currentState() != State_t::AbortingBulkIo){
    restoreControlTransferToPool(transfer, true);
    return;
  }
  // Check wLength
  if(transfer->wLength() != 4){
    pvLastErrorMutex.lock();
    QString msg = deviceIdString() + QObject::tr("CHECK_ABORT_BULK_OUT_STATUS request returned unexpected amount of data (expected: 4)");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    pvLastErrorMutex.unlock();
    setCurrentState(State_t::Error);
    return;
  }

  qDebug() << "CHECK_ABORT_BULK_OUT_STATUS - status: " << transfer->usbtmcStatusText();

  // Check status
  auto status = transfer->usbtmcStatus();
  // By STATUS_PENDING, let device some time and resubmit CHECK_ABORT_BULK_OUT_STATUS
  if(status == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_PENDING){
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    if(!transfer->submit()){
      pvLastErrorMutex.lock();
      pvLastError = transfer->lastError();
      pvLastErrorMutex.unlock();
      setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
      return;
    }
    return;
  }
  // Here we are done - Submit a CLEAR_HALT reqiest for Bulk-OUT
  Q_ASSERT(status != mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_PENDING);
  restoreControlTransferToPool(transfer, true);
  submitClearBulkOutEndpointHalt(10000);
  setCurrentState(State_t::Running);
}


// void mdtUsbtmcTransferHandler::continueAbortBulkOutTransfer(mdtUsbtmcControlTransfer *transfer)
// {
//   Q_ASSERT(transfer != 0);
//   Q_ASSERT(transfer->actualLength() > 0); // Can be > 2
// 
//   if(currentState() != State_t::AbortingBulkIo){
//     /// \todo Restore transfer to pool ?
//     return;
//   }
//   // Check transfer size - must be 2
//   qDebug() << "actual len: " << transfer->actualLength() << " - wLength: " << transfer->wLength();
//   if(transfer->wLength() != 2){
//     pvLastErrorMutex.lock();
//     QString msg = deviceIdString() + QObject::tr(": INITIATE_ABORT_BULK_OUT request returned unexpected amount of data (expected: 2)");
//     pvLastError.setError(msg, mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
//     pvLastError.commit();
//     pvLastErrorMutex.unlock();
//     setCurrentState(State_t::Error);
//     return;
//   }
//   
//   qDebug() << "Continue INITIATE_ABORT_BULK_OUT - status: " << transfer->usbtmcStatusText();
//   
//   // Check status
//   auto status = transfer->usbtmcStatus();
//   // On any failed status, bulk-OUT fifo is empty and we are done
//   if((status != mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_SUCCESS) && (status != mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_TRANSFER_NOT_IN_PROGRESS)){
//     restoreControlTransferToPool(transfer, true);
//     setCurrentState(State_t::Running);
//     return;
//   }
//   // By STATUS_TRANSFER_NOT_IN_PROGRESS, let device some time
//   int maxTry = 100;
//   while(status == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_TRANSFER_NOT_IN_PROGRESS){
//     if(maxTry < 1){
//       pvLastErrorMutex.lock();
//       QString msg = deviceIdString() + QObject::tr(": INITIATE_ABORT_BULK_OUT status allways not in progress after 100 try - Aborting.");
//       pvLastError.setError(msg, mdtError::Error);
//       MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
//       pvLastError.commit();
//       pvLastErrorMutex.unlock();
//       restoreControlTransferToPool(transfer, true);
//       setCurrentState(State_t::Error);
//       return;
//     }
//     --maxTry;
//     // Let device some time..
//     std::this_thread::sleep_for(std::chrono::milliseconds(50));
//     // Send INITIATE_ABORT_BULK_OUT again - Because libusb writes respone data after setup packet, we can simply resubmit transfer
//     /// \note Non, risque de retourner "Resource busy" - ou pas
//     if(!processSyncControlTransfer(transfer)){
//       restoreControlTransferToPool(transfer, true);
//       setCurrentState(State_t::Error);
//       return;
//     }
//     // Check wLength
//     if(transfer->wLength() != 2){
//       pvLastErrorMutex.lock();
//       QString msg = deviceIdString() + QObject::tr(": INITIATE_ABORT_BULK_OUT request returned unexpected amount of data (expected: 2)");
//       pvLastError.setError(msg, mdtError::Error);
//       MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
//       pvLastError.commit();
//       pvLastErrorMutex.unlock();
//       setCurrentState(State_t::Error);
//       return;
//     }
//     // Get status
//     status = transfer->usbtmcStatus();
//   }
//   // Check status again - On any failed status, bulk-OUT fifo is empty and we are done
//   Q_ASSERT(status != mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_TRANSFER_NOT_IN_PROGRESS);
//   if(status != mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_SUCCESS){
//     restoreControlTransferToPool(transfer, true);
//     setCurrentState(State_t::Running);
//     return;
//   }
//   // Here we are in STATUS_SUCCESS - Lets check abort status
//   Q_ASSERT(status == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_SUCCESS);
//   transfer->setupCheckAbortBulkOutStatus(pvBulkOutDescriptor.number(), 30000);
//   do{
//     qDebug() << "Sending CHECK_ABORT_BULK_OUT_STATUS ...";
//     if(!processSyncControlTransfer(transfer)){
//       qDebug() << "Synch transfer failed - restoring transfer to pool ..";
//       restoreControlTransferToPool(transfer, true);
//       qDebug() << "Going to error state ...";
//       setCurrentState(State_t::Error);
//       qDebug() << "Finished";
//       return;
//     }
//     // Check wLength
//     if(transfer->wLength() != 4){
//       pvLastErrorMutex.lock();
//       QString msg = deviceIdString() + QObject::tr(": CHECK_ABORT_BULK_OUT_STATUS request returned unexpected amount of data (expected: 2)");
//       pvLastError.setError(msg, mdtError::Error);
//       MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
//       pvLastError.commit();
//       pvLastErrorMutex.unlock();
//       setCurrentState(State_t::Error);
//       return;
//     }
//     // Get status
//     status = transfer->usbtmcStatus();
//   }while(status == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_PENDING);
//   // Here we are done - Submit a CLEAR_HALT reqiest for Bulk-OUT
//   Q_ASSERT(status != mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_PENDING);
//   restoreControlTransferToPool(transfer, true);
//   submitClearBulkOutEndpointHalt(10000);
//   setCurrentState(State_t::Running);
// 
//   qDebug() << "Continue INITIATE_ABORT_BULK_OUT - DONE";
// }

void mdtUsbtmcTransferHandler::submitRetireBulkIRPs(mdtUsbtmcBulkTransfer* transfer)
{
  Q_ASSERT(transfer != 0);

  qDebug() << "Retire IRPs for transfer bTag " << transfer->bTag();
  if(!transfer->cancel()){
    pvLastErrorMutex.lock();
    pvLastError = transfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
  }
}

void mdtUsbtmcTransferHandler::submitRequestDevDepMsgIn(unsigned int timeout)
{
  if(currentState() != State_t::Running){
    return;
  }
  
  qDebug() << "Submit REQUEST_DEV_DEP_MSG_IN ...";
  
  // Get a transfer from pool
  std::lock_guard<std::mutex> ctlg(pvBulkOutTransferMutex);
  mdtUsbtmcBulkTransfer *transfer = pvBulkOutTransferPool.getTransfer(*this, pvBulkOutDescriptor);
  Q_ASSERT(transfer != 0);
  // Setup transfer and submit it
  transfer->setupRequestDevDepMsgIn(nextBulkOutbTag(), false, '\0', timeout);
  if(!transfer->submit()){
    pvBulkOutTransferPool.restoreTransfer(transfer);
    pvLastErrorMutex.lock();
    pvLastError = transfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
  }
}

void mdtUsbtmcTransferHandler::submitBulkInTransfer(unsigned int timeout, mdtUsbtmcBulkTransfer::SplitAction_t action)
{
  if(currentState() != State_t::Running){
    return;
  }
  
  qDebug() << "Submit bulk-IN transfer ...";
  
  // Get a transfer from pool
  std::lock_guard<std::mutex> ctlg(pvBulkInTransferMutex);
  mdtUsbtmcBulkTransfer *transfer = pvBulkInTransferPool.getTransfer(*this, pvBulkInDescriptor);
  Q_ASSERT(transfer != 0);
  // Setup transfer and submit it
  transfer->setupBulkInRequest(timeout);
  transfer->setSplitAction(action);
  if(!transfer->submit()){
    pvBulkInTransferPool.restoreTransfer(transfer);
    pvLastErrorMutex.lock();
    pvLastError = transfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
  }
}

void mdtUsbtmcTransferHandler::handleBulkInTransferComplete(mdtUsbtmcBulkTransfer* transfer)
{
  Q_ASSERT(transfer != 0);

  auto state = currentState();
  /*
   * If we are in Running or AbortingBulkIo state,
   *  check if a Split action was requested.
   */
  if((state == State_t::Running) || (state == State_t::AbortingBulkIo)){
    if(transfer->splitAction() == mdtUsbtmcBulkTransfer::SplitAction_t::CHECK_ABORT_STATUS){
      restoreBulkInTransferToPool(transfer, true);
      submitCheckAbortBulkInStatusRequest();
      return;
    }
  }
  /*
   * All other states than Running (=currently not properly handled)
   */
  if(state != State_t::Running){
    restoreBulkInTransferToPool(transfer, true);
    std::lock_guard<std::mutex> lelg(pvLastErrorMutex);
    QString msg = deviceIdString() + QObject::tr("Bulk-IN transfer completed in a unhandled state. Will be lost.");
    pvLastError.setError(msg, mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    return;
  }
  /*
   * Here we are in Running state
   */
  Q_ASSERT(state == State_t::Running);

  qDebug() << "Bulk-IN transfer completed :-) - status: " << transfer->status();
  qDebug() << "-> bTag: " << transfer->bTag() << " - OK: " << transfer->bTagIsOk();
  qDebug() << "-> All data RX: " << transfer->receivedAllData(transfer->actualLength());
  
  // Check transfer status
  switch(transfer->status()){
    case LIBUSB_TRANSFER_COMPLETED: // OK, not a error
      break;
    case LIBUSB_TRANSFER_STALL: /// \todo handle !
      qDebug() << "Bulk-IN error status: LIBUSB_TRANSFER_STALL";
      break;
    case LIBUSB_TRANSFER_TIMED_OUT: /// \todo handle !
      qDebug() << "Bulk-IN error status: LIBUSB_TRANSFER_TIMED_OUT";
      break;
    case LIBUSB_TRANSFER_CANCELLED: /// \todo handle !
      qDebug() << "Bulk-IN error status: LIBUSB_TRANSFER_CANCELLED";
      ///bTag = transfer->bTag();
      restoreBulkInTransferToPool(transfer, true);
      ///beginAbortBulkOutTransfer(bTag);
      return;
    case LIBUSB_TRANSFER_NO_DEVICE: /// \todo handle !
      qDebug() << "Bulk-IN error status: LIBUSB_TRANSFER_NO_DEVICE";
      restoreBulkInTransferToPool(transfer, true);
      setLastErrorFromBulkInTransferStatus(transfer, mdtError::Warning, true);
      setCurrentState(State_t::Disconnected);
      return;
    case LIBUSB_TRANSFER_OVERFLOW: /// \todo handle !
      qDebug() << "Bulk-IN error status: LIBUSB_TRANSFER_OVERFLOW";
      restoreBulkInTransferToPool(transfer, true);
      setLastErrorFromBulkInTransferStatus(transfer, mdtError::Error, true);
      setCurrentState(State_t::Error);
      return;
    case LIBUSB_TRANSFER_ERROR: /// \todo handle !
      qDebug() << "Bulk-IN error status: LIBUSB_TRANSFER_ERROR";
      restoreBulkInTransferToPool(transfer, true);
      setLastErrorFromBulkInTransferStatus(transfer, mdtError::Error, true);
      setCurrentState(State_t::Error);
      return;
  }

  
  QByteArray ba;
  mdtUsbtmcMessage message(ba);
  
  transfer->getData(message);
  qDebug() << "Bulk-IN data: " << ba;
  
  restoreBulkInTransferToPool(transfer, true);
}

void mdtUsbtmcTransferHandler::beginAbortBulkInTransfer(uint8_t bTag)
{
  // Update state to AbortingBulkIo to prevent new bulk requests to be accpted
  setCurrentState(State_t::AbortingBulkIo);
  /*
   * Retire all IRPs of pending Bulk-OUT transfers that expect a response.
   * Note: here we can submit all cancel requests while mutext is locked,
   *       bacause, as long as libusb_handle_events() is not called, callback will not be called.
   */
  pvBulkOutTransferMutex.lock();
  auto pendingBulkOutTransfers = pvBulkOutTransferPool.getAllPendingTransfers();
  for(auto pendingTransfer : pendingBulkOutTransfers){
    Q_ASSERT(pendingTransfer != 0);
    if(pendingTransfer->responseExpected()){
      submitRetireBulkIRPs(pendingTransfer);
    }
  }
  pvBulkOutTransferMutex.unlock();
  // Send a INITIATE_ABORT_BULK_IN control request
  std::lock_guard<std::mutex> ctlg(pvControlTransferMutex);
  mdtUsbtmcControlTransfer *controlTransfer = pvControlTransferPool.getTransfer(*this);
  Q_ASSERT(controlTransfer != 0);
  // Setup transfer and submit it
  qDebug() << "Submitting INITIATE_ABORT_BULK_IN ...";
  controlTransfer->setupInitiateAbortBulkIn(bTag, pvBulkInDescriptor.number(), 30000);
  if(!controlTransfer->submit()){
    pvLastErrorMutex.lock();
    pvLastError = controlTransfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
  }
}

void mdtUsbtmcTransferHandler::handleInitiateAbortBulkInResponse(mdtUsbtmcControlTransfer* transfer)
{
  Q_ASSERT(transfer != 0);
  Q_ASSERT(transfer->actualLength() > 0); // Can be > 2
  Q_ASSERT(transfer->usbtmcRequest() == mdtUsbtmcControlTransfer::USBTMCbRequest::INITIATE_ABORT_BULK_IN);

  if(currentState() != State_t::AbortingBulkIo){
    restoreControlTransferToPool(transfer, true);
    return;
  }
  // Check transfer size - must be 2
  qDebug() << "actual len: " << transfer->actualLength() << " - wLength: " << transfer->wLength();
  if(transfer->wLength() != 2){
    pvLastErrorMutex.lock();
    QString msg = deviceIdString() + QObject::tr("INITIATE_ABORT_BULK_IN request returned unexpected amount of data (expected: 2)");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    pvLastErrorMutex.unlock();
    setCurrentState(State_t::Error);
    return;
  }

  qDebug() << "Continue INITIATE_ABORT_BULK_IN - status: " << transfer->usbtmcStatusText();
  
  // Check status
  auto status = transfer->usbtmcStatus();
  // For STATUS_TRANSFER_NOT_IN_PROGRESS, let device some time, and resubmit INITIATE_ABORT_BULK_IN request
  if(status == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_TRANSFER_NOT_IN_PROGRESS){
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    if(!transfer->submit()){
      pvLastErrorMutex.lock();
      pvLastError = transfer->lastError();
      pvLastErrorMutex.unlock();
      setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
      return;
    }
    return;
  }
  // For STATUS_SUCCESS we must continue reading and send CHECK_ABORT_BULK_IN_STATUS once Bulk-IN transfer done
  if(status == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_SUCCESS){
    pvBulkInTransferMutex.lock();
    auto bulkInTransfer = pvBulkInTransferPool.getPendingTransfer();
    if(bulkInTransfer != 0){
      // Tell handleBulkInTransferComplete() that it must submit CHECK_ABORT_BULK_IN_STATUS
      bulkInTransfer->setSplitAction(mdtUsbtmcBulkTransfer::SplitAction_t::CHECK_ABORT_STATUS);
    }
    pvBulkInTransferMutex.unlock();
    if(bulkInTransfer == 0){
      // No pending bulk-IN transfer - Send a new request and tell handleBulkInTransferComplete() that it must submit CHECK_ABORT_BULK_IN_STATUS
      submitBulkInTransfer(30000, mdtUsbtmcBulkTransfer::SplitAction_t::CHECK_ABORT_STATUS);
    }
    return;
  }
  // For all other STATUS, we must retire IRPs and we are done
  pvBulkInTransferMutex.lock();
  auto pendingBulkInTransfers = pvBulkInTransferPool.getAllPendingTransfers();
  for(auto pendingTransfer : pendingBulkInTransfers){
    Q_ASSERT(pendingTransfer != 0);
    submitRetireBulkIRPs(pendingTransfer);
  }
  pvBulkInTransferMutex.unlock();
  setCurrentState(State_t::Running);
}

void mdtUsbtmcTransferHandler::submitCheckAbortBulkInStatusRequest(mdtUsbtmcControlTransfer *controlTransfer)
{
  // transfer can be Null - Get one if required
  if(controlTransfer == 0){
    std::lock_guard<std::mutex> ctlg(pvControlTransferMutex);
    controlTransfer = pvControlTransferPool.getTransfer(*this);
  }
  // Send a INITIATE_ABORT_BULK_IN control request
  Q_ASSERT(controlTransfer != 0);
  // Setup transfer and submit it
  qDebug() << "Submitting CHECK_ABORT_BULK_IN_STATUS ...";
  controlTransfer->setupCheckAbortBulkInStatus(pvBulkInDescriptor.number(), 30000);
  if(!controlTransfer->submit()){
    pvLastErrorMutex.lock();
    pvLastError = controlTransfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
  }
}

void mdtUsbtmcTransferHandler::handleCheckAbortBulkInStatusResponse(mdtUsbtmcControlTransfer* transfer)
{
  Q_ASSERT(transfer != 0);
  Q_ASSERT(transfer->actualLength() > 0); // Can be > 2
  Q_ASSERT(transfer->usbtmcRequest() == mdtUsbtmcControlTransfer::USBTMCbRequest::CHECK_ABORT_BULK_IN_STATUS);

  if(currentState() != State_t::AbortingBulkIo){
    restoreControlTransferToPool(transfer, true);
    return;
  }
  // Check transfer size - must be 8
  qDebug() << "actual len: " << transfer->actualLength() << " - wLength: " << transfer->wLength();
  if(transfer->wLength() != 8){
    pvLastErrorMutex.lock();
    QString msg = deviceIdString() + QObject::tr("CHECK_ABORT_BULK_IN_STATUS request returned unexpected amount of data (expected: 8)");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    pvLastErrorMutex.unlock();
    setCurrentState(State_t::Error);
    return;
  }

  qDebug() << "Continue CHECK_ABORT_BULK_IN_STATUS - status: " << transfer->usbtmcStatusText();
  
  // Check status
  auto status = transfer->usbtmcStatus();

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
  auto state = currentState();
  ///if((state != State_t::Running) && (state != State_t::AbortingBulkOut) && (state != State_t::AbortingBulkIn)){
  if((state != State_t::Running) && (state != State_t::AbortingBulkIo)){
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
  // See if transfer contains a response that must be handled
  if(transfer->actualLength() > 0){
    switch(transfer->usbtmcRequest()){
      case mdtUsbtmcControlTransfer::USBTMCbRequest::INITIATE_ABORT_BULK_OUT:
        handleInitiateAbortBulkOutResponse(transfer);
        return;
      case mdtUsbtmcControlTransfer::USBTMCbRequest::CHECK_ABORT_BULK_OUT_STATUS:
        handleCheckAbortBulkOutStatusResponse(transfer);
        return;
      case mdtUsbtmcControlTransfer::USBTMCbRequest::INITIATE_ABORT_BULK_IN:
        handleInitiateAbortBulkInResponse(transfer);
        return;
      case mdtUsbtmcControlTransfer::USBTMCbRequest::CHECK_ABORT_BULK_IN_STATUS:
        handleCheckAbortBulkInStatusResponse(transfer);
        return;
      case mdtUsbtmcControlTransfer::USBTMCbRequest::INITIATE_CLEAR:
        // We simply let the transfer in pool's pending queue
        /// \todo Wrong !
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
        qDebug() << "processSyncControlTransfer(): LIBUSB_ERROR_INTERRUPTED - continue ...";
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

void mdtUsbtmcTransferHandler::restoreBulkOutTransferToPool(mdtUsbtmcBulkTransfer* transfer, bool lockMutex)
{
  Q_ASSERT(transfer != 0);

  if(lockMutex){
    pvBulkOutTransferMutex.lock();
  }
  pvBulkOutTransferPool.restoreTransfer(transfer);
  if(lockMutex){
    pvBulkOutTransferMutex.unlock();
  }
}

void mdtUsbtmcTransferHandler::restoreBulkInTransferToPool ( mdtUsbtmcBulkTransfer* transfer, bool lockMutex )
{
  Q_ASSERT(transfer != 0);

  if(lockMutex){
    pvBulkInTransferMutex.lock();
  }
  pvBulkInTransferPool.restoreTransfer(transfer);
  if(lockMutex){
    pvBulkInTransferMutex.unlock();
  }
}

void mdtUsbtmcTransferHandler::restoreControlTransferToPool ( mdtUsbtmcControlTransfer* transfer, bool lockMutex )
{
  Q_ASSERT(transfer != 0);

  if(lockMutex){
    pvControlTransferMutex.lock();
  }
  pvControlTransferPool.restoreTransfer(transfer);
  if(lockMutex){
    pvControlTransferMutex.unlock();
  }
}

void mdtUsbtmcTransferHandler::setLastErrorFromBulkOutTransferStatus(mdtUsbtmcBulkTransfer* transfer, mdtError::level_t level, bool lockMutex)
{
  Q_ASSERT(transfer != 0);

  QString msg = deviceIdString();

  // Set text part
  switch(level){
    case mdtError::NoError:
    case mdtError::Info:
      msg += QObject::tr("Bulk-OUT transfer status information.");
      break;
    case mdtError::Warning:
      msg += QObject::tr("Bulk-OUT transfer warning.");
      break;
    case mdtError::Error:
      msg += QObject::tr("Bulk-OUT transfer failed.");
      break;
  }
  // Setup and commit pvLastError
  if(lockMutex){
    pvLastErrorMutex.lock();
  }
  pvLastError.setError(msg, level);
  setLastErrorFromTransferStatus(transfer->status(), false);
  MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
  pvLastError.commit();
  if(lockMutex){
    pvLastErrorMutex.unlock();
  }
}

void mdtUsbtmcTransferHandler::setLastErrorFromBulkInTransferStatus(mdtUsbtmcBulkTransfer* transfer, mdtError::level_t level, bool lockMutex)
{
  Q_ASSERT(transfer != 0);

  QString msg = deviceIdString();

  // Set text part
  switch(level){
    case mdtError::NoError:
    case mdtError::Info:
      msg += QObject::tr("Bulk-IN transfer status information.");
      break;
    case mdtError::Warning:
      msg += QObject::tr("Bulk-IN transfer warning.");
      break;
    case mdtError::Error:
      msg += QObject::tr("Bulk-IN transfer failed.");
      break;
  }
  // Setup and commit pvLastError
  if(lockMutex){
    pvLastErrorMutex.lock();
  }
  pvLastError.setError(msg, level);
  setLastErrorFromTransferStatus(transfer->status(), false);
  MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
  pvLastError.commit();
  if(lockMutex){
    pvLastErrorMutex.unlock();
  }
}

void mdtUsbtmcTransferHandler::setLastErrorFromTransferStatus(libusb_transfer_status status, bool lockMutex)
{
  QString msg;

  // Build system error text
  switch(status){
    case LIBUSB_TRANSFER_COMPLETED:
      msg = "LIBUSB_TRANSFER_COMPLETED (" + QObject::tr("Transfer completed without error.") + ")";
      break;
    case LIBUSB_TRANSFER_ERROR:
      msg = "LIBUSB_TRANSFER_ERROR (" + QObject::tr("Transfer failed.") + ")";
      break;
    case LIBUSB_TRANSFER_TIMED_OUT:
      msg = "LIBUSB_TRANSFER_TIMED_OUT (" + QObject::tr("Transfer timed out.") + ")";
      break;
    case LIBUSB_TRANSFER_CANCELLED:
      msg = "LIBUSB_TRANSFER_CANCELLED (" + QObject::tr("Transfer was cancelled.") + ")";
      break;
    case LIBUSB_TRANSFER_STALL:
      msg = "LIBUSB_TRANSFER_STALL (" + QObject::tr("Halt condition or control request not supported.") + ")";
      break;
    case LIBUSB_TRANSFER_NO_DEVICE:
      msg = "LIBUSB_TRANSFER_NO_DEVICE (" + QObject::tr("Device was disconnected.") + ")";
      break;
    case LIBUSB_TRANSFER_OVERFLOW:
      msg = "LIBUSB_TRANSFER_OVERFLOW (" + QObject::tr("Device sent more data than requested (in USB layer)") + ")";
      break;
  }
  // Update pvLastError
  if(lockMutex){
    pvLastErrorMutex.lock();
  }
  pvLastError.setSystemError(status, msg);
  if(lockMutex){
    pvLastErrorMutex.unlock();
  }
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
