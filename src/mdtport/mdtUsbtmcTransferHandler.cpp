/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtUsbtmcPort.h"
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono
#include "mdtUsbtmcTransferHandlerStateMachine.h"

#include <QDebug>


//   struct smWrapper::fsm : public boost::msm::back::state_machine<mdtUsbtmcTransferHandlerStateMachine::StateMachine_>
//   {
//     using boost::msm::back::state_machine<mdtUsbtmcTransferHandlerStateMachine::StateMachine_>::state_machine;
//   };
// 
//   smWrapper::smWrapper()
//   : pvFsm(new fsm)
//   {
// 
//   }


using namespace mdtUsbtmcTransferHandlerStateMachine;

mdtUsbtmcTransferHandler::mdtUsbtmcTransferHandler(libusb_context *usbContext, mdtUsbtmcPort & port)
 : pvUsbContext(usbContext),
   pvDeviceHandle(0),
   ///pvStateMachine(State_t::Stopped),
   ///pvStateMachineNew(new mdtUsbtmcTransferHandlerStateMachine::StateMachine(boost::ref(*this))),
   pvStateMachineNew(new /**mdtUsbtmcTransferHandlerStateMachine::*/StateMachine(this)),
   pvPort(port)
{
  Q_ASSERT(pvUsbContext != 0);
  
  
  ///mdtUsbtmcTransferHandlerStateMachine::StateMachine sm(boost::ref(*this));
}


// void mdtUsbtmcTransferHandler::fakeAction(const mdtUsbtmcTransferHandler::fakeEvent &)
// {
//   qDebug() << "** Fake action called !";
// }



bool mdtUsbtmcTransferHandler::setup(libusb_device_handle* handle, const mdtUsbDeviceDescriptor & descriptor, uint8_t bInterfaceNumber)
{
  Q_ASSERT(!descriptor.isEmpty());

  ///setCurrentState(State_t::Setup);
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
  // Setup interrupt IN endpoint
  if(!pvInterfaceDescriptor.interruptInEndpoints().isEmpty()){
    pvInterruptInDescriptor = pvInterfaceDescriptor.interruptInEndpoints().at(0);
    if(!pvInterruptInTransferPool.allocTransfers(1, *this, pvInterruptInDescriptor, handle)){
      std::lock_guard<std::mutex> lelg(pvLastErrorMutex);
      QString msg = deviceIdString();
      msg += QObject::tr("Allocation of interrupt-IN transfer pool failed.");
      pvLastError.setError(msg, mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
      pvLastError.commit();
      return false;
    }
  }
  // Get USBTMC capabilities
  if(!getCapabilities(pvInterfaceDescriptor.bInterfaceNumber(), 10000)){
    return false;
  }
  // Submit first Interrupt-IN transfer
  /**
  if(!pvInterfaceDescriptor.isEmpty()){
    submitInterruptInTransfer(50000);
  }
  */

  
  /// \todo Provisoire
  ///pvStateMachineNew->setTh(this);
  ///pvStateMachineNew->start();
  ///pvStateMachineNew->stop();
  ///pvStateMachineNew->process_event(StopRequestedEvent());
  ///pvStateMachineNew->process_event(ErrorOccuredEvent(pvLastError));
  ///start();
  ///pvStateMachineNew.pvFsm->setTh(this);
  ///pvStateMachineNew.pvFsm->start();
  
  return true;
}

void mdtUsbtmcTransferHandler::start()
{
  pvStateMachineNew->start();
}

void mdtUsbtmcTransferHandler::stop()
{
  pvStateMachineNew->stop();
}

bool mdtUsbtmcTransferHandler::submitCommand(mdtUsbtmcTxMessage & message, bool responseExpected, unsigned int timeout)
{
  // Check if we can accept command now
  if(!pvStateMachineNew->is_flag_active<ProcessingFlag>()){
    std::lock_guard<std::mutex> lelg(pvLastErrorMutex);
    QString msg = deviceIdString() + QObject::tr("Cannot send a command in a non Processing state.");
    pvLastError.setError(msg, mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    return false;
  }
  /**
  State_t state = currentState();
  if(state != State_t::Running){
    // If we are aborting a bulk transfer, warn
    if(state == State_t::AbortingBulkIo){
      std::lock_guard<std::mutex> lelg(pvLastErrorMutex);
      QString msg = deviceIdString() + QObject::tr("trying to send a command while aborting a bulk transfer - Will be ignored !");
      pvLastError.setError(msg, mdtError::Warning);
      MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
      pvLastError.commit();
    }
    return;
  }
  */
  // Lock bulk-OUT transfer pool
  std::lock_guard<std::mutex> bolg(pvBulkOutTransferMutex);
  
  qDebug() << "submitCommand() - Queues: control: " << pvControlTransferPool.pendingTransferCount() << " , bulk-OUT: " << pvBulkOutTransferPool.pendingTransferCount() << " , bulk-IN: " << pvBulkInTransferPool.pendingTransferCount();
  
  /*
   * As metionned in USBTMC 1.0 , section 3.2, rule 3
   *  we cannot send a new USBTMC bulk-OUT transfer
   *  if one is pending
   */
  if(pvBulkOutTransferPool.pendingTransferCount() > 0){
    std::lock_guard<std::mutex> lelg(pvLastErrorMutex);
    QString msg = deviceIdString() + QObject::tr("Cannot send a command while a bulk-OUT transfer is pending.");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    return false;
  }
  /*
   * As metionned in USBTMC 1.0 , section 3.3.2.4, table 12,
   *  we cannot send a new USBTMC bulk-OUT transfer that expects a response
   *  if a Bulk-IN transfer is pending
   */
  if(responseExpected){
    std::lock_guard<std::mutex> bilg(pvBulkInTransferMutex);
    if(pvBulkInTransferPool.pendingTransferCount() > 0){
      QString msg = deviceIdString() + QObject::tr("Cannot send a command expecting a response while a bulk-IN transfer is pending.");
      pvLastError.setError(msg, mdtError::Error);
      MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
      pvLastError.commit();
      return false;
    }
  }
  /*
   * Get a transfer from pool, setup bulk-OUT transfer and submit it
   */
  mdtUsbtmcBulkTransfer *transfer = pvBulkOutTransferPool.getTransfer(*this, pvBulkOutDescriptor);
  Q_ASSERT(transfer != 0);
  transfer->setupDevDepMsgOut(nextBulkOutbTag(), message, responseExpected, timeout);
  if(!transfer->submit()){
    ErrorOccuredEvent errorEvent(transfer->lastError());
    pvStateMachineNew->process_event(errorEvent);
    return false;
    /**
    pvBulkOutTransferPool.restoreTransfer(transfer);
    pvLastErrorMutex.lock();
    pvLastError = transfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
    */
  }

  return true;
}

void mdtUsbtmcTransferHandler::getReceivedData(mdtUsbtmcRxMessage & message)
{
  std::lock_guard<std::mutex> bilg(pvBulkInTransferMutex);
  auto transfers = pvBulkInTransferPool.getAllPendingTransfers();
  for(auto transfer : transfers){
    Q_ASSERT(transfer != 0);
    transfer->getData(message);
  }
  pvBulkInTransferPool.restorePendingTransfers();
}

void mdtUsbtmcTransferHandler::submitStopRequest()
{
  qDebug() << "submitStopRequest() - Queues: control: " << pvControlTransferPool.pendingTransferCount() << " , bulk-OUT: " << pvBulkOutTransferPool.pendingTransferCount() << " , bulk-IN: " << pvBulkInTransferPool.pendingTransferCount() << " , interrupt-IN: " << pvInterruptInTransferPool.pendingTransferCount();
  
  pvStateMachineNew->process_event(StopRequestedEvent());
  /**
  setCurrentState(State_t::Stopping);
  submitCancelAllBulkOutTransfers(true);
  submitCancelAllBulkInTransfers(true);
  submitCancelAllInterruptInTransfers(true);
  */
}

void mdtUsbtmcTransferHandler::submitBulkIoClear()
{
  pvStateMachineNew->process_event(ClearBulkIoRequestedEvent());
}

void mdtUsbtmcTransferHandler::beginBulkIoClear()
{
  unsigned int pendingBulkOutTransfersCount = 0;
  unsigned int pendingBulkInTransfersCount = 0;

  /**
  if(currentState() != State_t::Running){
    return;
  }
  */
  /*
   * Update current state to AbortingBulkIo to prevent new requests to be accepted
   */
  ///setCurrentState(State_t::AbortingBulkIo);
  /*
   * Retire IRPs of all pending Bulk-OUT transfers
   */
  pvBulkOutTransferMutex.lock();
  pendingBulkOutTransfersCount = pvBulkOutTransferPool.pendingTransferCount();
  submitCancelAllBulkOutTransfers(false, mdtUsbtmcBulkTransfer::SplitAction_t::INITIATE_CLEAR);
  pvBulkOutTransferMutex.unlock();
  /*
   * Retire IRPs of all pending Bulk-IN transfers
   */
  pvBulkInTransferMutex.lock();
  pendingBulkInTransfersCount = pvBulkInTransferPool.pendingTransferCount();
  submitCancelAllBulkInTransfers(false, mdtUsbtmcBulkTransfer::SplitAction_t::INITIATE_CLEAR);
  pvBulkInTransferMutex.unlock();
  /*
   * If no bulk transfer was pending,
   *  we submit INITIATE_CLEAR ourself to flush device
   */
  if((pendingBulkOutTransfersCount == 0) && (pendingBulkInTransfersCount == 0)){
    submitInitiateClearRequest();
  }
}

void mdtUsbtmcTransferHandler::dbgSubmitCustomBulkOutTransfer(uint8_t msgID, uint8_t bTag, uint8_t bTagInverse, uint32_t transferSize, uint8_t bmTransferAttributes, uint8_t termChar,
                                                              bool responseExpected, int transferBufferLength, unsigned int timeout)
{
  /**
  if(currentState() != State_t::Running){
    return;
  }
  */
  // Get a transfer from pool
  std::lock_guard<std::mutex> ctlg(pvBulkOutTransferMutex);
  mdtUsbtmcBulkTransfer *transfer = pvBulkOutTransferPool.getTransfer(*this, pvBulkOutDescriptor);
  Q_ASSERT(transfer != 0);
  // Setup transfer and submit it
  transfer->dbgSetupCustom(msgID, bTag, bTagInverse, transferSize, bmTransferAttributes, termChar, responseExpected, transferBufferLength, timeout);
  if(!transfer->submit()){
    ErrorOccuredEvent errorEvent(transfer->lastError());
    pvStateMachineNew->process_event(errorEvent);
    return;
    /**
    pvBulkOutTransferPool.restoreTransfer(transfer);
    pvLastErrorMutex.lock();
    pvLastError = transfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
    */
  }
}

void mdtUsbtmcTransferHandler::dbgSubmitAbortOnePendingBulkOutTransfer()
{
  /**
  if(currentState() != State_t::Running){
    return;
  }
  */
  std::lock_guard<std::mutex> ctlg(pvBulkOutTransferMutex);
  auto transfer = pvBulkOutTransferPool.getPendingTransfer();
  if(transfer == 0){
    return;
  }
  pvStateMachineNew->process_event(AbortBulkOutRequestedEvent(transfer));
//   setCurrentState(State_t::AbortingBulkIo);
//   transfer->setSplitAction(mdtUsbtmcBulkTransfer::SplitAction_t::INITIATE_ABORT);
//   submitRetireBulkIRPs(transfer);
}

void mdtUsbtmcTransferHandler::dbgSubmitAbortOnePendingBulkInTransfer()
{
  std::lock_guard<std::mutex> ctlg(pvBulkInTransferMutex);
  auto transfer = pvBulkInTransferPool.getPendingTransfer();
  if(transfer == 0){
    return;
  }
  pvStateMachineNew->process_event(AbortBulkInRequestedEvent(transfer));
//   if(currentState() != State_t::Running){
//     return;
//   }
//   // Find a transfer to cancel
//   uint8_t bTag = 0;
//   pvBulkOutTransferMutex.lock();
//   auto pendingBulkOutTransfers = pvBulkOutTransferPool.getAllPendingTransfers();
//   for(auto pendingTransfer : pendingBulkOutTransfers){
//     Q_ASSERT(pendingTransfer != 0);
//     if(pendingTransfer->responseExpected()){
//       bTag = pendingTransfer->bTag();
//       break;
//     }
//   }
//   pvBulkOutTransferMutex.unlock();
//   if(bTag != 0){
//     pvStateMachineNew->process_event(AbortBulkInRequestedEvent());
//   }
}

void mdtUsbtmcTransferHandler::submitClearEndpointHalt(uint8_t endpointNumber, unsigned int timeout)
{
  /**
  auto state = currentState();
  if((state != State_t::Running) && (state != State_t::AbortingBulkIo)){
    return;
  }
  */
  // Get a transfer from pool
  std::lock_guard<std::mutex> ctlg(pvControlTransferMutex);
  mdtUsbtmcControlTransfer *transfer = pvControlTransferPool.getTransfer(*this);
  Q_ASSERT(transfer != 0);
  // Setup transfer and submit it
  transfer->setupClearEndpointHalt(endpointNumber, timeout);
  if(!transfer->submit()){
    ErrorOccuredEvent errorEvent(transfer->lastError());
    pvStateMachineNew->process_event(errorEvent);
    return;
    /**
    pvControlTransferPool.restoreTransfer(transfer);
    pvLastErrorMutex.lock();
    pvLastError = transfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
    */
  }
}

void mdtUsbtmcTransferHandler::submitControlTransfersCancel()
{
  /*
   * By little experience (with a U3606A),
   * trying to cancel control transfer just before closing give bad results.
   * So, just do nothing.
   */
  /**
  if(currentState() != State_t::Running){
    return;
  }
  */
  // Get pending transfer, if available
  std::lock_guard<std::mutex> ctlg(pvControlTransferMutex);
  mdtUsbtmcControlTransfer *transfer = pvControlTransferPool.getPendingTransfer();
  if(transfer == 0){
    return;
  }
  // Cancel transfer
  qDebug() << "Submitting a control transfer cancel ...";
  if(!transfer->cancel()){
    ErrorOccuredEvent errorEvent(transfer->lastError());
    pvStateMachineNew->process_event(errorEvent);
    return;
    /**
    pvLastErrorMutex.lock();
    pvLastError = transfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
    */
  }
}

void mdtUsbtmcTransferHandler::controlTransferCallback(libusb_transfer* transfer)
{
  Q_ASSERT(transfer != 0);

  mdtUsbtmcControlTransfer *uct = static_cast<mdtUsbtmcControlTransfer*>(transfer->user_data);
  // For synchronous transfers, we only set them completed, and in any state
  if(uct->isSync()){
    uct->setCompleted();
    return;
  }
  ///uct->transferHandler().handleControlTransferComplete(uct);
  uct->transferHandler().pvStateMachineNew->process_event(ControlTransferCompletedEvent(uct));
}


void mdtUsbtmcTransferHandler::bulkOutTransferCallback(libusb_transfer* transfer)
{
  Q_ASSERT(transfer != 0);

  mdtUsbtmcBulkTransfer *ubt = static_cast<mdtUsbtmcBulkTransfer*>(transfer->user_data);
  ///ubt->transferHandler().handleBulkOutTransferComplete(ubt);
  qDebug() << "bulkOutTransferCallback - sending BulkOutTransferCompletedEvent ..";
  ubt->transferHandler().pvStateMachineNew->process_event(BulkOutTransferCompletedEvent(ubt));
}

void mdtUsbtmcTransferHandler::bulkInTransferCallback(libusb_transfer* transfer)
{
  Q_ASSERT(transfer != 0);

  mdtUsbtmcBulkTransfer *ubt = static_cast<mdtUsbtmcBulkTransfer*>(transfer->user_data);
  ///ubt->transferHandler().handleBulkInTransferComplete(ubt);
  ubt->transferHandler().pvStateMachineNew->process_event(BulkInTransferCompletedEvent(ubt));
}

void mdtUsbtmcTransferHandler::interruptInTransferCallback(libusb_transfer* transfer)
{
  Q_ASSERT(transfer != 0);

  mdtUsbtmcInterruptTransfer *uit = static_cast<mdtUsbtmcInterruptTransfer*>(transfer->user_data);
  ///uit->transferHandler().handleInterruptInTransferComplete(uit);
  uit->transferHandler().pvStateMachineNew->process_event(InterruptInTransferCompletedEvent(uit));
}

bool mdtUsbtmcTransferHandler::mustBeStopped() const
{
  return pvStateMachineNew->is_flag_active<MustBeStoppedFlag>();
}

void mdtUsbtmcTransferHandler::notifyError(const mdtError & error)
{
  /*
   * Because this function is called by state machine,
   * and that process_event() calls are serialized by a mutex,
   * we should not have races here
   */
  pvPort.thSetError(error);
}

void mdtUsbtmcTransferHandler::notifyBulkIoDone()
{
  pvPort.thSetBulkTransactionDone();
}

void mdtUsbtmcTransferHandler::notifyBulkIoAborted()
{
  pvPort.thSetBulkTransactionAborted();
}

template<typename EventType>
void mdtUsbtmcTransferHandler::sendEvent(const EventType & event)
{
  pvStateMachineNew->process_event(event);
}

bool mdtUsbtmcTransferHandler::hasPendingTransfers()
{
  // Lock all transfer pools
  std::lock_guard<std::mutex> bolg(pvBulkOutTransferMutex);
  std::lock_guard<std::mutex> bilg(pvBulkInTransferMutex);
  std::lock_guard<std::mutex> iilg(pvInterruptInTransferMutex);
  std::lock_guard<std::mutex> clg(pvControlTransferMutex);

  qDebug() << "hasPendingTransfers() - Queues: control: " << pvControlTransferPool.pendingTransferCount() << " , bulk-OUT: " << pvBulkOutTransferPool.pendingTransferCount() << " , bulk-IN: " << pvBulkInTransferPool.pendingTransferCount() << " , Interrupt-IN: " << pvInterruptInTransferPool.pendingTransferCount();
  
  // Check...
  if(pvBulkOutTransferPool.pendingTransferCount() > 0){
    return true;
  }
  if(pvBulkInTransferPool.pendingTransferCount() > 0){
    return true;
  }
  if(pvInterruptInTransferPool.pendingTransferCount() > 0){
    return true;
  }
  if(pvControlTransferPool.pendingTransferCount() > 0){
    return true;
  }
  return false;
}

void mdtUsbtmcTransferHandler::submitCancelAllTransfers()
{
  submitCancelAllBulkOutTransfers(true);
  submitCancelAllBulkInTransfers(true);
  submitCancelAllInterruptInTransfers(true);
}

void mdtUsbtmcTransferHandler::restoreAllTransfers()
{
  // Lock all transfer pools
  std::lock_guard<std::mutex> bolg(pvBulkOutTransferMutex);
  std::lock_guard<std::mutex> bilg(pvBulkInTransferMutex);
  std::lock_guard<std::mutex> iilg(pvInterruptInTransferMutex);
  std::lock_guard<std::mutex> clg(pvControlTransferMutex);

  qDebug() << "restoreAllTransfers() - Queues: control: " << pvControlTransferPool.pendingTransferCount() << " , bulk-OUT: " << pvBulkOutTransferPool.pendingTransferCount() << " , bulk-IN: " << pvBulkInTransferPool.pendingTransferCount() << " , Interrupt-IN: " << pvInterruptInTransferPool.pendingTransferCount();

  pvBulkOutTransferPool.restorePendingTransfers();
  pvBulkInTransferPool.restorePendingTransfers();
  pvInterruptInTransferPool.restorePendingTransfers();
  pvControlTransferPool.restorePendingTransfers();
}

void mdtUsbtmcTransferHandler::handleControlTransferComplete(mdtUsbtmcControlTransfer *transfer )
{
  Q_ASSERT(transfer != 0);

  ///QString msg;  /// \todo Overheat ?

  qDebug() << "Control transfer DONE - Control Queue: " << pvControlTransferPool.pendingTransferCount() << " - transfer status: " << transfer->status() << " - length: " << transfer->actualLength();
  
  
  // For synchronous transfers, we only set them completed, and in any state
  /**
  if(transfer->isSync()){
    transfer->setCompleted();
    return;
  }
  */
  // Check state before acting
  ///auto state = currentState();
  /*
   * If we are stoppoing, restore transfer to pool
   *  and set state to Stopped if possible now
   */
  /**
  if(state == State_t::Stopping){
    restoreControlTransferToPool(transfer, true);
    if(!hasPendingTransfers()){
      setCurrentState(State_t::Stopped);
    }
    return;
  }
  */
  /*
   * Handle transfer only in relevant state
   */
  /**
  if((state != State_t::Running) && (state != State_t::AbortingBulkIo)){
    restoreControlTransferToPool(transfer, true);
    qDebug() << "No more running, do nothing more ...";
    return;
  }
  */
  // Check transfer status
  switch(transfer->status()){
    case LIBUSB_TRANSFER_COMPLETED: // OK, not a error
      break;
    case LIBUSB_TRANSFER_STALL:
      // We sent a query that is not supported by device.
      {
        QString msg = deviceIdString() + QObject::tr("control query with bRequest '") + QString::number(transfer->bRequest()) + "' ";
        msg += QObject::tr("is not supported by device.");
        mdtError error(msg, mdtError::Error);
        MDT_ERROR_SET_SRC(error, "mdtUsbtmcTransferHandler");
        ErrorOccuredEvent errorEvent(error);
        pvStateMachineNew->process_event(errorEvent);
      }
      return;
      /**
      pvLastErrorMutex.lock();
      msg = deviceIdString() + QObject::tr("control query with bRequest '") + QString::number(transfer->bRequest()) + "' ";
      msg += QObject::tr("is not supported by device.");
      pvLastError.setError(msg, mdtError::Warning);
      MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
      pvLastError.commit();
      pvLastErrorMutex.unlock();
      // Restore frame to pool
      restoreControlTransferToPool(transfer, true);
      pvPort.thSetError(pvLastError);
      return;
      */
    case LIBUSB_TRANSFER_TIMED_OUT: /// \todo handle !
    case LIBUSB_TRANSFER_CANCELLED: /// \todo handle !
    case LIBUSB_TRANSFER_NO_DEVICE: /// \todo handle !
    case LIBUSB_TRANSFER_OVERFLOW: /// \todo handle !
    case LIBUSB_TRANSFER_ERROR: /// \todo handle !
    {
      QString msg = deviceIdString() + QObject::tr("Control transfer completed with a unhandled status.");
      mdtError error(msg, mdtError::Error);
      error.setSystemError(transfer->status(), libusbTransferStatusText(transfer->status()));
      MDT_ERROR_SET_SRC(error, "mdtUsbtmcTransferHandler");
      ErrorOccuredEvent errorEvent(error);
      pvStateMachineNew->process_event(errorEvent);
      return;
    }
    /**
      qDebug() << "Unhandled status from completed Control transfer: " << transfer->status();
      // Restore frame to pool
      restoreControlTransferToPool(transfer, true);
      setCurrentState(State_t::Error);
      pvPort.thSetError(pvLastError);
      return;
      */
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
        handleInitiateClearResponse(transfer);
        return;
      case mdtUsbtmcControlTransfer::USBTMCbRequest::CHECK_CLEAR_STATUS:
        handleCheckClearStatusResponse(transfer);
        return;
      default:
        // We received a response that we don't handle
        QString msg = deviceIdString() + QObject::tr("received a unhandled control response:");
        msg += " " + transfer->usbtmcRequestText() + " . ";
        msg += QObject::tr("Will be ignored.");
        mdtError error(msg, mdtError::Error);
        MDT_ERROR_SET_SRC(error, "mdtUsbtmcTransferHandler");
        ErrorOccuredEvent errorEvent(error);
        pvStateMachineNew->process_event(errorEvent);
        return;
        /**
        pvLastErrorMutex.lock();
        QString msg = deviceIdString() + QObject::tr("received a unhandled control response:");
        msg += " " + transfer->usbtmcRequestText() + " . ";
        msg += QObject::tr("Will be ignored.");
        pvLastError.setError(msg, mdtError::Warning);
        MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
        pvLastError.commit();
        pvLastErrorMutex.unlock();
        // Restore frame to pool
        restoreControlTransferToPool(transfer, true);
        pvPort.thSetError(pvLastError);
        return;
        */
    }
  }
  // If a abort or clear bulk transfer finishes and CLEAR_FEATURE to clear bulk endpoint request is done, we return to running state
  ///if( (state == State_t::AbortingBulkIo) && (transfer->isClearEndpointHaltRequest(pvBulkOutDescriptor.number())) ){
  /// \todo FIX !!
  /**
  if(transfer->isClearEndpointHaltRequest(pvBulkOutDescriptor.number())){
    qDebug() << "CLEAR ENDPOINT HALT (for bulk_OUT) done - returning to Running state";
    setCurrentState(State_t::Running);  /// \todo Remove
    pvPort.thSetControlTransactionDone();
  }
  */
  // Nothing more - Restore transfer to pool and signal
  restoreControlTransferToPool(transfer, true);
  pvStateMachineNew->process_event(ControlTransferSuccessfulEvent());
}

void mdtUsbtmcTransferHandler::restoreControlTransferToPool(mdtUsbtmcControlTransfer* transfer, bool lockMutex)
{
  Q_ASSERT(transfer != 0);

  if(lockMutex){
    pvControlTransferMutex.lock();
  }
  qDebug() << "restoreControlTransferToPool ...";
  pvControlTransferPool.restoreTransfer(transfer);
  qDebug() << "restoreControlTransferToPool DONE";
  if(lockMutex){
    pvControlTransferMutex.unlock();
  }
}

void mdtUsbtmcTransferHandler::handleBulkOutTransferComplete(mdtUsbtmcBulkTransfer* transfer)
{
  Q_ASSERT(transfer != 0);

  qDebug() << "Bulk-OUT transfer completed :-) - status: " << transfer->status();
  qDebug() << "-> bTag: " << transfer->bTag();

  // Check current state
  ///auto state = currentState();
  /*
   * If we are stoppoing, restore transfer to pool
   *  and set state to Stopped if possible now
   */
  /**
  if(state == State_t::Stopping){
    restoreBulkOutTransferToPool(transfer, true);
    if(!hasPendingTransfers()){
      setCurrentState(State_t::Stopped);
    }
    return;
  }
  */
  /*
   * If we are in Running or AbortingBulkIo state and transfer was cancelled,
   *  we restore transfer back to pool, and begin INITIATE action if required.
   */
  /**
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
        case mdtUsbtmcBulkTransfer::SplitAction_t::INITIATE_CLEAR:
          submitInitiateClearRequest();
          return;
        case mdtUsbtmcBulkTransfer::SplitAction_t::CHECK_ABORT_STATUS:
          qDebug() << "-> Cannot handle CHECK_ABORT_STATUS split action !";
        case mdtUsbtmcBulkTransfer::SplitAction_t::CHECK_CLEAR_STATUS:
          qDebug() << "-> Cannot handle CHECK_CLEAR_STATUS split action !";
        case mdtUsbtmcBulkTransfer::SplitAction_t::CLEAR_ENDPOINT_HALT:
          qDebug() << "-> Cannot handle CLEAR_ENDPOINT_HALT split action in cancelled transfer !";
          return;
      }
    }
  }
  */
  /*
   * All other states than Running (=currently not properly handled)
   */
  /**
  if(state != State_t::Running){
    restoreBulkOutTransferToPool(transfer, true);
    std::lock_guard<std::mutex> lelg(pvLastErrorMutex);
    QString msg = deviceIdString() + QObject::tr("Bulk-OUT transfer completed in a unhandled state. Will be lost.");
    pvLastError.setError(msg, mdtError::Warning);
    ///pvLastError.setSystemError(LIBUSB_ERROR_OTHER, libusb_strerror(LIBUSB_ERROR_OTHER));
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    pvPort.thSetError(pvLastError);
    return;
  }
  */
  /*
   * Here we are in Running state
   */
  ///Q_ASSERT(state == State_t::Running);

  // Check transfer status
  switch(transfer->status()){
    case LIBUSB_TRANSFER_COMPLETED: // OK, not a error
      break;
    case LIBUSB_TRANSFER_STALL: /// \todo handle !
      qDebug() << "Bulk-OUT error status: LIBUSB_TRANSFER_STALL";
      break;
    case LIBUSB_TRANSFER_TIMED_OUT: /// \todo handle !
      qDebug() << "Bulk-OUT error status: LIBUSB_TRANSFER_TIMED_OUT";
      restoreBulkOutTransferToPool(transfer, true);
      pvPort.thSetTimeoutOccured();
      return;
    case LIBUSB_TRANSFER_CANCELLED: /// \todo handle !
      qDebug() << "Bulk-OUT error status: LIBUSB_TRANSFER_CANCELLED";
      return;
    case LIBUSB_TRANSFER_NO_DEVICE: /// \todo handle !
      qDebug() << "Bulk-OUT error status: LIBUSB_TRANSFER_NO_DEVICE";
      restoreBulkOutTransferToPool(transfer, true);
      setLastErrorFromBulkOutTransferStatus(transfer, mdtError::Warning, true);
      ///setCurrentState(State_t::Disconnected);
      pvPort.thSetError(pvLastError);
      return;
//     case LIBUSB_TRANSFER_OVERFLOW: /// \todo handle !
//       qDebug() << "Bulk-OUT error status: LIBUSB_TRANSFER_OVERFLOW";
//       restoreBulkOutTransferToPool(transfer, true);
//       setLastErrorFromBulkOutTransferStatus(transfer, mdtError::Error, true);
//       setCurrentState(State_t::Error);
//       pvPort.thSetError(pvLastError);
//       return;
//     case LIBUSB_TRANSFER_ERROR: /// \todo handle !
//       qDebug() << "Bulk-OUT error status: LIBUSB_TRANSFER_ERROR";
//       restoreBulkOutTransferToPool(transfer, true);
//       setLastErrorFromBulkOutTransferStatus(transfer, mdtError::Error, true);
//       setCurrentState(State_t::Error);
//       pvPort.thSetError(pvLastError);
//       return;
    case LIBUSB_TRANSFER_OVERFLOW: /// \todo handle !
    case LIBUSB_TRANSFER_ERROR: /// \todo handle !
    {
      QString msg = deviceIdString() + QObject::tr("Bulk-OUT transfer completed with a unhandled status.");
      mdtError error(msg, mdtError::Error);
      error.setSystemError(transfer->status(), libusbTransferStatusText(transfer->status()));
      MDT_ERROR_SET_SRC(error, "mdtUsbtmcTransferHandler");
      ErrorOccuredEvent errorEvent(error);
      pvStateMachineNew->process_event(errorEvent);
      return;
    }
  }
  // ...
  if((transfer->msgID() == mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_OUT) && (transfer->responseExpected())){
    submitRequestDevDepMsgIn(transfer);
    return;
  }
  if(transfer->msgID() == mdtUsbtmcFrame::msgId_t::DEV_DEP_MSG_IN){
    submitBulkInTransfer(transfer->timeout());
    restoreBulkOutTransferToPool(transfer, true);
    return;
  }
  // Here, transfer is done and no response is expected
  restoreBulkOutTransferToPool(transfer, true);
  pvPort.thSetBulkTransactionDone();
}

void mdtUsbtmcTransferHandler::restoreBulkOutTransferToPool(mdtUsbtmcBulkTransfer* transfer, bool lockMutex)
{
  Q_ASSERT(transfer != 0);

  if(lockMutex){
    pvBulkOutTransferMutex.lock();
  }
  qDebug() << "restoreBulkOutTransferToPool ...";
  pvBulkOutTransferPool.restoreTransfer(transfer);
  qDebug() << "restoreBulkOutTransferToPool DONE";
  if(lockMutex){
    pvBulkOutTransferMutex.unlock();
  }
}

void mdtUsbtmcTransferHandler::restoreBulkOutTransferSA(mdtUsbtmcBulkTransfer* transfer)
{
  Q_ASSERT(transfer != 0);

  bool hasPendingTransfers;
  auto bTag = transfer->bTag();
  auto sa = transfer->splitAction();

  /// \todo Check transfer status, so we can detect error (such as device disconnected)
  
  // Restore transfer and check if we have pending transfers
  pvBulkOutTransferMutex.lock();
  pvBulkOutTransferPool.restoreTransfer(transfer);
  hasPendingTransfers = (pvBulkOutTransferPool.pendingTransferCount() > 0);
  pvBulkOutTransferMutex.unlock();
  // Execute SplitAction if we have no more pending transfers
  if(!hasPendingTransfers){
    switch(sa){
      case mdtUsbtmcBulkTransfer::SplitAction_t::NoAction:
        return;
      case mdtUsbtmcBulkTransfer::SplitAction_t::INITIATE_ABORT:
        submitInitiateAbortBulkOutRequest(bTag);
        return;
      case mdtUsbtmcBulkTransfer::SplitAction_t::INITIATE_CLEAR:
        submitInitiateClearRequest();
        return;
      case mdtUsbtmcBulkTransfer::SplitAction_t::CHECK_ABORT_STATUS:
        qDebug() << "-> restoreBulkOutTransferSA: Cannot handle CHECK_ABORT_STATUS split action !";
      case mdtUsbtmcBulkTransfer::SplitAction_t::CHECK_CLEAR_STATUS:
        qDebug() << "-> restoreBulkOutTransferSA: Cannot handle CHECK_CLEAR_STATUS split action !";
      case mdtUsbtmcBulkTransfer::SplitAction_t::CLEAR_ENDPOINT_HALT:
        submitClearBulkOutEndpointHalt(10000);
        return;
    }
  }
}

void mdtUsbtmcTransferHandler::submitCancelAllBulkOutTransfers(bool lockMutex, mdtUsbtmcBulkTransfer::SplitAction_t action)
{
  if(lockMutex){
    pvBulkOutTransferMutex.lock();
  }
  /*
   * Retire IRPs of all pending Bulk-OUT transfers
   */
  auto pendingBulkOutTransfer = pvBulkOutTransferPool.getAllPendingTransfers();
  mdtUsbtmcBulkTransfer *transfer;
  unsigned int pendingBulkOutTransfersCount = pendingBulkOutTransfer.size();
  if(pendingBulkOutTransfersCount > 0){
    for(unsigned int i = 0; i < (pendingBulkOutTransfersCount -1); ++i){
      transfer = pendingBulkOutTransfer[i];
      Q_ASSERT(transfer != 0);
      submitRetireBulkIRPs(transfer);
    }
    // For last transfer, we must tell handleBulkOutTransferComplete() if it has something to do
    transfer = pendingBulkOutTransfer[pendingBulkOutTransfersCount-1];
    Q_ASSERT(transfer != 0);
    transfer->setSplitAction(action);
    submitRetireBulkIRPs(transfer);
  }
  if(lockMutex){
    pvBulkOutTransferMutex.unlock();
  }
}

void mdtUsbtmcTransferHandler::handleBulkInTransferComplete(mdtUsbtmcBulkTransfer* transfer)
{
  Q_ASSERT(transfer != 0);

  qDebug() << "Bulk-IN transfer completed :-) - status: " << transfer->status();

  
  ///auto state = currentState();
  /*
   * If we are stopping, restore transfer to pool
   *  and set state to Stopped if possible now
   */
  /**
  if(state == State_t::Stopping){
    restoreBulkInTransferToPool(transfer, true);
    if(!hasPendingTransfers()){
      setCurrentState(State_t::Stopped);
    }
    return;
  }
  */
  /*
   * If we are in Running or AbortingBulkIo state,
   *  check if a Split action was requested.
   */
  /**
  if((state == State_t::Running) || (state == State_t::AbortingBulkIo)){
    // Check if we must submit CHECK_ABORT_STATUS
    if(transfer->splitAction() == mdtUsbtmcBulkTransfer::SplitAction_t::CHECK_ABORT_STATUS){
      restoreBulkInTransferToPool(transfer, true);
      submitCheckAbortBulkInStatusRequest();
      return;
    }
    // Check if we must submit INITIATE_CLEAR request
    if(transfer->splitAction() == mdtUsbtmcBulkTransfer::SplitAction_t::INITIATE_CLEAR){
      restoreBulkInTransferToPool(transfer, true);
      submitInitiateClearRequest();
      return;
    }
  }
  */
  /*
   * All other states than Running (=currently not properly handled)
   */
  /**
  if(state != State_t::Running){
    restoreBulkInTransferToPool(transfer, true);
    std::lock_guard<std::mutex> lelg(pvLastErrorMutex);
    QString msg = deviceIdString() + QObject::tr("Bulk-IN transfer completed in a unhandled state. Will be lost.");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    pvPort.thSetError(pvLastError);
    return;
  }
  */
  /*
   * Here we are in Running state
   */
  ///Q_ASSERT(state == State_t::Running);
  // Check transfer status
  ///auto bTag = transfer->bTag();
  switch(transfer->status()){
    case LIBUSB_TRANSFER_COMPLETED: // OK, not a error
      break;
    case LIBUSB_TRANSFER_STALL: /// \todo handle ! NOTE: happens also when device was disconnected !
      qDebug() << "Bulk-IN error status: LIBUSB_TRANSFER_STALL";
      break;
    case LIBUSB_TRANSFER_TIMED_OUT: /// \todo handle !
      qDebug() << "Bulk-IN error status: LIBUSB_TRANSFER_TIMED_OUT";
      restoreBulkInTransferToPool(transfer, true);
      pvPort.thSetTimeoutOccured();
      return;
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
      pvPort.thSetError(pvLastError);
      ///setCurrentState(State_t::Disconnected);
      return;
//     case LIBUSB_TRANSFER_OVERFLOW: /// \todo handle !
//       qDebug() << "Bulk-IN error status: LIBUSB_TRANSFER_OVERFLOW";
//       restoreBulkInTransferToPool(transfer, true);
//       setLastErrorFromBulkInTransferStatus(transfer, mdtError::Error, true);
//       pvPort.thSetError(pvLastError);
//       setCurrentState(State_t::Error);
//       return;
//     case LIBUSB_TRANSFER_ERROR: /// \todo handle !
//       qDebug() << "Bulk-IN error status: LIBUSB_TRANSFER_ERROR";
//       restoreBulkInTransferToPool(transfer, true);
//       setLastErrorFromBulkInTransferStatus(transfer, mdtError::Error, true);
//       pvPort.thSetError(pvLastError);
//       setCurrentState(State_t::Error);
//       return;
    case LIBUSB_TRANSFER_ERROR: /// \todo handle !
    case LIBUSB_TRANSFER_OVERFLOW: /// \todo handle !
    {
      QString msg = deviceIdString() + QObject::tr("Bulk-IN transfer completed with a unhandled status.");
      mdtError error(msg, mdtError::Error);
      error.setSystemError(transfer->status(), libusbTransferStatusText(transfer->status()));
      MDT_ERROR_SET_SRC(error, "mdtUsbtmcTransferHandler");
      ErrorOccuredEvent errorEvent(error);
      pvStateMachineNew->process_event(errorEvent);
      return;
    }
  }
  /*
   * Here we are in running state, and transfer status is OK
   * Now, check USBTMC transfer validity part
   */
  Q_ASSERT(transfer->status() == LIBUSB_TRANSFER_COMPLETED);
  qDebug() << "-> bTag: " << transfer->bTag() << " - OK: " << transfer->bTagIsOk();
  qDebug() << "-> All data RX: " << transfer->receivedAllData(transfer->actualLength());
  // Check bTag
  if(!transfer->bTagIsOk()){  /// \todo handle error !
    qDebug() << "-> bTag error !!";
    pvStateMachineNew->process_event(AbortBulkInRequestedEvent(transfer));
//     restoreBulkInTransferToPool(transfer, true);
//     beginAbortBulkInTransfer(bTag);
    return;
  }
  // Check that we received all data
  if(!transfer->receivedAllData(transfer->actualLength())){  /// \todo handle error !
    qDebug() << "-> RX not all data !!";
    pvStateMachineNew->process_event(AbortBulkInRequestedEvent(transfer));
//     restoreBulkInTransferToPool(transfer, true);
//     beginAbortBulkInTransfer(bTag);
    return;
  }
  /*
   * OK, here all seems good.
   * We simply let the transfer in pending queue.
   * getReceivedData() will restore it to pool.
   */
  std::lock_guard<std::mutex> bilg(pvBulkInTransferMutex);
  // Generat a warning if we have too much pending transfers
  if(pvBulkInTransferPool.pendingTransferCount() > 40){ /// \todo Should be higher ? Note: check also pool's limit
    std::lock_guard<std::mutex> lelg(pvLastErrorMutex);
    QString msg = deviceIdString() + QObject::tr("Have more than 40 completed non readed Bulk-IN transfers, will restore them. These data are loss !");
    pvLastError.setError(msg, mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    pvBulkInTransferPool.restorePendingTransfers();
    pvPort.thSetError(pvLastError);
    return;
  }
  if(transfer->isEOM()){
    pvPort.thSetBulkTransactionDone();
  }
}

void mdtUsbtmcTransferHandler::restoreBulkInTransferToPool(mdtUsbtmcBulkTransfer* transfer, bool lockMutex)
{
  Q_ASSERT(transfer != 0);

  if(lockMutex){
    pvBulkInTransferMutex.lock();
  }
  qDebug() << "restoreBulkInTransferToPool ...";
  pvBulkInTransferPool.restoreTransfer(transfer);
  qDebug() << "restoreBulkInTransferToPool DONE";
  if(lockMutex){
    pvBulkInTransferMutex.unlock();
  }
}

void mdtUsbtmcTransferHandler::restoreBulkInTransferSA(mdtUsbtmcBulkTransfer* transfer)
{
  Q_ASSERT(transfer != 0);

  bool hasPendingTransfers;
  ///auto bTag = transfer->bTag();
  auto sa = transfer->splitAction();

  /// \todo Check transfer status, so we can detect error (such as device disconnected)
  
  // Restore transfer and check if we have pending transfers
  pvBulkInTransferMutex.lock();
  pvBulkInTransferPool.restoreTransfer(transfer);
  hasPendingTransfers = (pvBulkInTransferPool.pendingTransferCount() > 0);
  pvBulkInTransferMutex.unlock();
  // Execute SplitAction if we have no more pending transfers
  if(!hasPendingTransfers){
    switch(sa){
      case mdtUsbtmcBulkTransfer::SplitAction_t::NoAction:
        return;
      case mdtUsbtmcBulkTransfer::SplitAction_t::INITIATE_ABORT:
        qDebug() << "-> Cannot handle INITIATE_ABORT split action !";
        return;
      case mdtUsbtmcBulkTransfer::SplitAction_t::INITIATE_CLEAR:
        submitInitiateClearRequest();
        return;
      case mdtUsbtmcBulkTransfer::SplitAction_t::CHECK_ABORT_STATUS:
        qDebug() << "-> Cannot handle CHECK_ABORT_STATUS split action !";
      case mdtUsbtmcBulkTransfer::SplitAction_t::CHECK_CLEAR_STATUS:
        qDebug() << "-> Cannot handle CHECK_CLEAR_STATUS split action !";
      case mdtUsbtmcBulkTransfer::SplitAction_t::CLEAR_ENDPOINT_HALT:
        qDebug() << "-> Cannot handle CLEAR_ENDPOINT_HALT split action !";
        return;
    }
  }
}

void mdtUsbtmcTransferHandler::submitCancelAllBulkInTransfers(bool lockMutex, mdtUsbtmcBulkTransfer::SplitAction_t action)
{
  if(lockMutex){
    pvBulkInTransferMutex.lock();
  }
  /*
   * Retire IRPs of all pending Bulk-IN transfers
   */
  auto pendingBulkInTransfer = pvBulkInTransferPool.getAllPendingTransfers();
  mdtUsbtmcBulkTransfer *transfer;
  unsigned int pendingBulkInTransfersCount = pendingBulkInTransfer.size();
  if(pendingBulkInTransfersCount > 0){
    for(unsigned int i = 0; i < (pendingBulkInTransfersCount -1); ++i){
      transfer = pendingBulkInTransfer[i];
      Q_ASSERT(transfer != 0);
      submitRetireBulkIRPs(transfer);
    }
    // For last transfer, we must tell handleBulkIntTransferComplete() if it has something to do
    transfer = pendingBulkInTransfer[pendingBulkInTransfersCount-1];
    Q_ASSERT(transfer != 0);
    transfer->setSplitAction(action);
    submitRetireBulkIRPs(transfer);
  }
  if(lockMutex){
    pvBulkInTransferMutex.unlock();
  }
}

/// \todo Check how to tell libusb that we want infinite timeout and adapt
void mdtUsbtmcTransferHandler::submitInterruptInTransfer(unsigned int timeout, mdtUsbtmcInterruptTransfer *transfer)
{
  /**
  auto state = currentState();
  if((state != State_t::Running) && (state != State_t::Setup) && (state != State_t::AbortingBulkIo)){
    return;
  }
  */

  qDebug() << "Submit interrupt-IN transfer ...";

  // Get a transfer from pool if required
  if(transfer == 0){
    std::lock_guard<std::mutex> ctlg(pvInterruptInTransferMutex);
    transfer = pvInterruptInTransferPool.getTransfer(*this, pvInterruptInDescriptor);
  }
  Q_ASSERT(transfer != 0);
  // Setup transfer and submit it
  transfer->setup(timeout);
  if(!transfer->submit()){
    ErrorOccuredEvent errorEvent(transfer->lastError());
    pvStateMachineNew->process_event(errorEvent);
    return;
    /**
    restoreInterruptInTransferToPool(transfer, true);
    pvLastErrorMutex.lock();
    pvLastError = transfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
    */
  }
}

void mdtUsbtmcTransferHandler::handleInterruptInTransferComplete(mdtUsbtmcInterruptTransfer* transfer)
{
  Q_ASSERT(transfer != 0);

  qDebug() << "interrupt-IN transfer complete :-) - transfer status: " << transfer->status();

  ///pvStateMachineNew->process_event(InterruptInTransferCompletedEvent());
  
  ///auto state = currentState();
  /*
   * If we are stoppoing, restore transfer to pool
   *  and set state to Stopped if possible now
   */
  /**
  if(state == State_t::Stopping){
    restoreInterruptInTransferToPool(transfer, true);
    if(!hasPendingTransfers()){
      setCurrentState(State_t::Stopped);
    }
    return;
  }
  */
  /*
   * Handle interrupt only on relevant state
   */
  /**
  if((state != State_t::Running) && (state != State_t::AbortingBulkIo) && (state != State_t::Setup)){
    restoreInterruptInTransferToPool(transfer, true);
    return;
  }
  */
  // Check transfer status
  switch(transfer->status()){
    case LIBUSB_TRANSFER_COMPLETED: // OK, not a error, handle content below
      break;
    case LIBUSB_TRANSFER_TIMED_OUT: // OK, we are still running, resubmit transfer
      submitInterruptInTransfer(50000, transfer);
      return;
    case LIBUSB_TRANSFER_STALL:     /// \todo handle !
    case LIBUSB_TRANSFER_CANCELLED: /// \todo handle !
    case LIBUSB_TRANSFER_NO_DEVICE: /// \todo handle !
    case LIBUSB_TRANSFER_OVERFLOW:  /// \todo handle !
    case LIBUSB_TRANSFER_ERROR:     /// \todo handle !
    {
      QString msg = deviceIdString() + QObject::tr("Interrupt-IN transfer completed with a unhandled status.");
      mdtError error(msg, mdtError::Error);
      error.setSystemError(transfer->status(), libusbTransferStatusText(transfer->status()));
      MDT_ERROR_SET_SRC(error, "mdtUsbtmcTransferHandler");
      ErrorOccuredEvent errorEvent(error);
      pvStateMachineNew->process_event(errorEvent);
      return;
    }
/*
      qDebug() << "Unhandled status from completed interrupt-IN transfer: " << transfer->status();
      // Restore frame to pool
      restoreInterruptInTransferToPool(transfer, true);
      setLastErrorFromInterruptInTransferStatus(transfer, mdtError::Error, true);
      pvPort.thSetError(pvLastError);
      setCurrentState(State_t::Error);
      return;*/
  }
  /// \todo Action regarding interrupt bNotify, etc...
  qDebug() << "bNotify1: " << transfer->byteValue(0);
  // Submit transfer for future interrupt
  submitInterruptInTransfer(50000, transfer);
}

void mdtUsbtmcTransferHandler::restoreInterruptInTransferToPool(mdtUsbtmcInterruptTransfer* transfer, bool lockMutex)
{
  Q_ASSERT(transfer != 0);

  if(lockMutex){
    pvInterruptInTransferMutex.lock();
  }
  
  qDebug() << "restoring interrupt-IN transfer - queue: " << pvInterruptInTransferPool.pendingTransferCount();
  
  pvInterruptInTransferPool.restoreTransfer(transfer);
  if(lockMutex){
    pvInterruptInTransferMutex.unlock();
  }
}

void mdtUsbtmcTransferHandler::beginAbortBulkOut(mdtUsbtmcBulkTransfer *transfer)
{
  Q_ASSERT(transfer != 0);
  // Cancel transfer and tell that INITIATE_ABORT_BULK_OUT split action must be performed
  transfer->setSplitAction(mdtUsbtmcBulkTransfer::SplitAction_t::INITIATE_ABORT);
  submitRetireBulkIRPs(transfer);
}

void mdtUsbtmcTransferHandler::beginAbortBulkIn(mdtUsbtmcBulkTransfer* transfer)
{
  Q_ASSERT(transfer != 0);

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
  // Submit INITIATE_ABORT_BULK_IN request
  submitInitiateAbortBulkInRequest(transfer->bTag());
}

void mdtUsbtmcTransferHandler::submitInitiateAbortBulkOutRequest(uint8_t bTag)
{
  // Get a control transfer from pool
  std::lock_guard<std::mutex> ctlg(pvControlTransferMutex);
  mdtUsbtmcControlTransfer *transfer = pvControlTransferPool.getTransfer(*this);
  Q_ASSERT(transfer != 0);
  // Setup transfer and submit it
  qDebug() << "Submitting INITIATE_ABORT_BULK_OUT for bTag " << bTag << " ...";
  transfer->setupInitiateAbortBulkOut(bTag, pvBulkOutDescriptor.number(), 30000);
  if(!transfer->submit()){
    ErrorOccuredEvent errorEvent(transfer->lastError());
    pvStateMachineNew->process_event(errorEvent);
    return;
  }
}

void mdtUsbtmcTransferHandler::handleInitiateAbortBulkOutResponse(mdtUsbtmcControlTransfer* transfer)
{
  Q_ASSERT(transfer != 0);
  Q_ASSERT(transfer->actualLength() > 0); // Can be > 2
  Q_ASSERT(transfer->usbtmcRequest() == mdtUsbtmcControlTransfer::USBTMCbRequest::INITIATE_ABORT_BULK_OUT);

  /**
  if(currentState() != State_t::AbortingBulkIo){
    restoreControlTransferToPool(transfer, true);
    return;
  }
  */
  // Check transfer size - must be 2
  qDebug() << "actual len: " << transfer->actualLength() << " - wLength: " << transfer->wLength();
  if(transfer->wLength() != 2){
    QString msg = deviceIdString() + QObject::tr("INITIATE_ABORT_BULK_OUT request returned unexpected amount of data (expected: 2)");
    mdtError error(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(error, "mdtUsbtmcTransferHandler");
    ErrorOccuredEvent errorEvent(error);
    pvStateMachineNew->process_event(errorEvent);
    return;
    /**
    pvLastErrorMutex.lock();
    QString msg = deviceIdString() + QObject::tr("INITIATE_ABORT_BULK_OUT request returned unexpected amount of data (expected: 2)");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    pvLastErrorMutex.unlock();
    setCurrentState(State_t::Error);
    pvPort.thSetError(pvLastError);
    return;
    */
  }

  qDebug() << "Continue INITIATE_ABORT_BULK_OUT - status: " << transfer->usbtmcStatusText() << " - bTag: " << transfer->byteValue(1);
  
  // Check status
  auto status = transfer->usbtmcStatus();
  // On any failed status, bulk-OUT fifo is empty and we are done
  if((status != mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_SUCCESS) && (status != mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_TRANSFER_NOT_IN_PROGRESS)){
    restoreControlTransferToPool(transfer, true);
    pvStateMachineNew->process_event(SplitActionDoneEvent());
    ///setCurrentState(State_t::Running);
    ///pvPort.thSetTimeoutOccured(); /// \todo FIX !!
    return;
  }
  // By STATUS_TRANSFER_NOT_IN_PROGRESS, let device some time and resubmit INITIATE_ABORT_BULK_OUT
  if(status == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_TRANSFER_NOT_IN_PROGRESS){
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    if(!transfer->submit()){
      ErrorOccuredEvent errorEvent(transfer->lastError());
      pvStateMachineNew->process_event(errorEvent);
      return;
      /**
      pvLastErrorMutex.lock();
      pvLastError = transfer->lastError();
      pvLastErrorMutex.unlock();
      setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
      return;
      */
    }
    return;
  }
  // Here we are in STATUS_SUCCESS - Lets begin check abort status
  Q_ASSERT(status == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_SUCCESS);
  transfer->setupCheckAbortBulkOutStatus(pvBulkOutDescriptor.number(), 30000);
  if(!transfer->submit()){
    ErrorOccuredEvent errorEvent(transfer->lastError());
    pvStateMachineNew->process_event(errorEvent);
    return;
    /**
    pvLastErrorMutex.lock();
    pvLastError = transfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
    */
  }
}

void mdtUsbtmcTransferHandler::handleCheckAbortBulkOutStatusResponse(mdtUsbtmcControlTransfer* transfer)
{
  Q_ASSERT(transfer != 0);
  Q_ASSERT(transfer->actualLength() > 0); // Can be > 2
  Q_ASSERT(transfer->usbtmcRequest() == mdtUsbtmcControlTransfer::USBTMCbRequest::CHECK_ABORT_BULK_OUT_STATUS);

  /**
  if(currentState() != State_t::AbortingBulkIo){
    restoreControlTransferToPool(transfer, true);
    return;
  }
  */
  // Check wLength
  if(transfer->wLength() != 4){
    QString msg = deviceIdString() + QObject::tr("CHECK_ABORT_BULK_OUT_STATUS request returned unexpected amount of data (expected: 4)");
    mdtError error(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(error, "mdtUsbtmcTransferHandler");
    ErrorOccuredEvent errorEvent(error);
    pvStateMachineNew->process_event(errorEvent);
    return;
    /**
    pvLastErrorMutex.lock();
    QString msg = deviceIdString() + QObject::tr("CHECK_ABORT_BULK_OUT_STATUS request returned unexpected amount of data (expected: 4)");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    pvLastErrorMutex.unlock();
    restoreControlTransferToPool(transfer, true);
    setCurrentState(State_t::Error);
    pvPort.thSetError(pvLastError);
    return;
    */
  }

  qDebug() << "CHECK_ABORT_BULK_OUT_STATUS - status: " << transfer->usbtmcStatusText();

  // Check status
  auto status = transfer->usbtmcStatus();
  // By STATUS_PENDING, let device some time and resubmit CHECK_ABORT_BULK_OUT_STATUS
  if(status == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_PENDING){
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    if(!transfer->submit()){
      ErrorOccuredEvent errorEvent(transfer->lastError());
      pvStateMachineNew->process_event(errorEvent);
      return;
      /**
      pvLastErrorMutex.lock();
      pvLastError = transfer->lastError();
      pvLastErrorMutex.unlock();
      restoreControlTransferToPool(transfer, true);
      setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
      return;
      */
    }
    return;
  }
  // Here we are done - Submit a CLEAR_HALT request for Bulk-OUT
  Q_ASSERT(status != mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_PENDING);
  restoreControlTransferToPool(transfer, true);
  pvStateMachineNew->process_event(ClearBulkOutEndpointHaltRequestedEvent());
  ///submitClearBulkOutEndpointHalt(10000);  /// \todo Send ClearBulkOutEndpointHaltRequestedEvent
  // handleControlTransferComplete() will return to Running state
}

void mdtUsbtmcTransferHandler::submitRetireBulkIRPs(mdtUsbtmcBulkTransfer* transfer)
{
  Q_ASSERT(transfer != 0);

  qDebug() << "Retire IRPs for transfer bTag " << transfer->bTag();
  if(!transfer->cancel()){
    ErrorOccuredEvent errorEvent(transfer->lastError());
    pvStateMachineNew->process_event(errorEvent);
    return;
    /**
    pvLastErrorMutex.lock();
    pvLastError = transfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
    */
  }
}

void mdtUsbtmcTransferHandler::submitRequestDevDepMsgIn(mdtUsbtmcBulkTransfer *transfer)
{
  Q_ASSERT(transfer != 0);

  /**
  if(currentState() != State_t::Running){
    return;
  }
  */
  
  qDebug() << "Submit REQUEST_DEV_DEP_MSG_IN ...";
  
  auto timeout = transfer->timeout();
  // Setup transfer and submit it
  /// \todo Check about termChar if supported by device, etc..
  transfer->setupRequestDevDepMsgIn(nextBulkOutbTag(), false, '\0', timeout);
  if(!transfer->submit()){
    ErrorOccuredEvent errorEvent(transfer->lastError());
    pvStateMachineNew->process_event(errorEvent);
    return;
    /**
    pvBulkOutTransferPool.restoreTransfer(transfer);
    pvLastErrorMutex.lock();
    pvLastError = transfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
    */
  }
}

void mdtUsbtmcTransferHandler::submitBulkInTransfer(unsigned int timeout, mdtUsbtmcBulkTransfer::SplitAction_t action)
{
  /**
  if(currentState() != State_t::Running){
    return;
  }
  */
  
  qDebug() << "Submit bulk-IN transfer ...";
  
  // Get a transfer from pool
  std::lock_guard<std::mutex> ctlg(pvBulkInTransferMutex);

  qDebug() << "Submit bulk-IN transfer - get transfer from pool ...";

  mdtUsbtmcBulkTransfer *transfer = pvBulkInTransferPool.getTransfer(*this, pvBulkInDescriptor);
  Q_ASSERT(transfer != 0);
  // Setup transfer and submit it
  transfer->setupBulkInRequest(timeout);
  transfer->setSplitAction(action);
  if(!transfer->submit()){
    ErrorOccuredEvent errorEvent(transfer->lastError());
    pvStateMachineNew->process_event(errorEvent);
    return;
    /**
    pvBulkInTransferPool.restoreTransfer(transfer);
    pvLastErrorMutex.lock();
    pvLastError = transfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
    */
  }
  qDebug() << "Submit bulk-IN transfer - submit done";
}


void mdtUsbtmcTransferHandler::submitInitiateAbortBulkInRequest(uint8_t bTag)
{
  // Update state to AbortingBulkIo to prevent new bulk requests to be accepted
  ///setCurrentState(State_t::AbortingBulkIo);
  // Send a INITIATE_ABORT_BULK_IN control request
  std::lock_guard<std::mutex> ctlg(pvControlTransferMutex);
  mdtUsbtmcControlTransfer *controlTransfer = pvControlTransferPool.getTransfer(*this);
  Q_ASSERT(controlTransfer != 0);
  // Setup transfer and submit it
  qDebug() << "Submitting INITIATE_ABORT_BULK_IN for bTag " << bTag << " ...";
  controlTransfer->setupInitiateAbortBulkIn(bTag, pvBulkInDescriptor.number(), 30000);
  if(!controlTransfer->submit()){
    ErrorOccuredEvent errorEvent(controlTransfer->lastError());
    pvStateMachineNew->process_event(errorEvent);
    return;
    /**
    pvLastErrorMutex.lock();
    pvLastError = controlTransfer->lastError();
    pvLastErrorMutex.unlock();
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
    */
  }
}

void mdtUsbtmcTransferHandler::handleInitiateAbortBulkInResponse(mdtUsbtmcControlTransfer* transfer)
{
  Q_ASSERT(transfer != 0);
  Q_ASSERT(transfer->actualLength() > 0); // Can be > 2
  Q_ASSERT(transfer->usbtmcRequest() == mdtUsbtmcControlTransfer::USBTMCbRequest::INITIATE_ABORT_BULK_IN);

  /**
  if(currentState() != State_t::AbortingBulkIo){
    restoreControlTransferToPool(transfer, true);
    return;
  }
  */
  // Check transfer size - must be 2
  qDebug() << "actual len: " << transfer->actualLength() << " - wLength: " << transfer->wLength();
  if(transfer->wLength() != 2){
    QString msg = deviceIdString() + QObject::tr("INITIATE_ABORT_BULK_IN request returned unexpected amount of data (expected: 2)");
    mdtError error(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(error, "mdtUsbtmcTransferHandler");
    ErrorOccuredEvent errorEvent(error);
    pvStateMachineNew->process_event(errorEvent);
    return;
    /**
    pvLastErrorMutex.lock();
    QString msg = deviceIdString() + QObject::tr("INITIATE_ABORT_BULK_IN request returned unexpected amount of data (expected: 2)");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    pvLastErrorMutex.unlock();
    restoreControlTransferToPool(transfer, true);
    setCurrentState(State_t::Error);
    pvPort.thSetError(pvLastError);
    return;
    */
  }

  qDebug() << "Continue INITIATE_ABORT_BULK_IN - status: " << transfer->usbtmcStatusText() << " - bTag: " << transfer->byteValue(1);
  
  // Check status
  auto status = transfer->usbtmcStatus();
  // For STATUS_TRANSFER_NOT_IN_PROGRESS, let device some time, and resubmit INITIATE_ABORT_BULK_IN request
  if(status == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_TRANSFER_NOT_IN_PROGRESS){
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    if(!transfer->submit()){
      ErrorOccuredEvent errorEvent(transfer->lastError());
      pvStateMachineNew->process_event(errorEvent);
      return;
      /**
      pvLastErrorMutex.lock();
      pvLastError = transfer->lastError();
      pvLastErrorMutex.unlock();
      restoreControlTransferToPool(transfer, true);
      setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
      return;
      */
    }
    return;
  }
  // For STATUS_SUCCESS we must continue reading and send CHECK_ABORT_BULK_IN_STATUS once Bulk-IN transfer done
  if(status == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_SUCCESS){
    pvBulkInTransferMutex.lock();
    auto bulkInTransfer = pvBulkInTransferPool.getPendingTransfer();
    if(bulkInTransfer != 0){
      // Tell handleBulkInTransferComplete() that it must submit CHECK_ABORT_BULK_IN_STATUS
      qDebug() << "Continue INITIATE_ABORT_BULK_IN - Has pending bulk-IN transfer -> tell callback to check status..";
      bulkInTransfer->setSplitAction(mdtUsbtmcBulkTransfer::SplitAction_t::CHECK_ABORT_STATUS);
    }
    pvBulkInTransferMutex.unlock();
    if(bulkInTransfer == 0){
      /*
       * No bulk-IN transfer pending (never initiated, or allready done).
       * If we init one here, we could block forever, so we submit CHECK_ABORT_BULK_IN_STATUS now
       */
      submitCheckAbortBulkInStatusRequest(transfer);
    }else{
      restoreControlTransferToPool(transfer, true);
    }
    /**
    if(bulkInTransfer == 0){
      // No pending bulk-IN transfer - Send a new request and tell handleBulkInTransferComplete() that it must submit CHECK_ABORT_BULK_IN_STATUS
      qDebug() << "Continue INITIATE_ABORT_BULK_IN - No pending bulk-IN transfer -> init a new one";
      submitBulkInTransfer(30000, mdtUsbtmcBulkTransfer::SplitAction_t::CHECK_ABORT_STATUS);
    }
    */
    return;
  }
  // For all other STATUS, we must retire IRPs and we are done
  /// \todo Check if we should only retire IRPs for transfer that has given bTag
  pvBulkInTransferMutex.lock();
  auto pendingBulkInTransfers = pvBulkInTransferPool.getAllPendingTransfers();
  for(auto pendingTransfer : pendingBulkInTransfers){
    Q_ASSERT(pendingTransfer != 0);
    submitRetireBulkIRPs(pendingTransfer);
  }
  pvBulkInTransferMutex.unlock();
  restoreControlTransferToPool(transfer, true);
  ///setCurrentState(State_t::Running);
  pvStateMachineNew->process_event(SplitActionDoneEvent());
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
    ErrorOccuredEvent errorEvent(controlTransfer->lastError());
    pvStateMachineNew->process_event(errorEvent);
    return;
    /**
    pvLastErrorMutex.lock();
    pvLastError = controlTransfer->lastError();
    pvLastErrorMutex.unlock();
    restoreControlTransferToPool(controlTransfer, true);
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
    */
  }
}

void mdtUsbtmcTransferHandler::handleCheckAbortBulkInStatusResponse(mdtUsbtmcControlTransfer* transfer)
{
  Q_ASSERT(transfer != 0);
  Q_ASSERT(transfer->actualLength() > 0); // Can be > 2
  Q_ASSERT(transfer->usbtmcRequest() == mdtUsbtmcControlTransfer::USBTMCbRequest::CHECK_ABORT_BULK_IN_STATUS);

  /**
  if(currentState() != State_t::AbortingBulkIo){
    restoreControlTransferToPool(transfer, true);
    return;
  }
  */
  // Check transfer size - must be 8
  qDebug() << "actual len: " << transfer->actualLength() << " - wLength: " << transfer->wLength();
  if(transfer->wLength() != 8){
    QString msg = deviceIdString() + QObject::tr("CHECK_ABORT_BULK_IN_STATUS request returned unexpected amount of data (expected: 8)");
    mdtError error(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(error, "mdtUsbtmcTransferHandler");
    ErrorOccuredEvent errorEvent(error);
    pvStateMachineNew->process_event(errorEvent);
    return;
    /**
    pvLastErrorMutex.lock();
    QString msg = deviceIdString() + QObject::tr("CHECK_ABORT_BULK_IN_STATUS request returned unexpected amount of data (expected: 8)");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    pvLastErrorMutex.unlock();
    restoreControlTransferToPool(transfer, true);
    setCurrentState(State_t::Error);
    pvPort.thSetError(pvLastError);
    return;
    */
  }

  qDebug() << "Continue CHECK_ABORT_BULK_IN_STATUS - status: " << transfer->usbtmcStatusText();
  
  // Check status
  auto status = transfer->usbtmcStatus();
  if(status == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_PENDING){
    // If device's bulk-IN FIFO has more data, send a new request and tell handleBulkInTransferComplete() that it must submit CHECK_ABORT_BULK_IN_STATUS
    if(transfer->byteValue(1) == 1){
      restoreControlTransferToPool(transfer, true);
      submitBulkInTransfer(30000, mdtUsbtmcBulkTransfer::SplitAction_t::CHECK_ABORT_STATUS);
    }else{
      // Resubmit request later
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      if(!transfer->submit()){
        ErrorOccuredEvent errorEvent(transfer->lastError());
        pvStateMachineNew->process_event(errorEvent);
        return;
        /**
        pvLastErrorMutex.lock();
        pvLastError = transfer->lastError();
        pvLastErrorMutex.unlock();
        restoreControlTransferToPool(transfer, true);
        setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
        return;
        */
      }
    }
    return;
  }
  // Here, we are done
  Q_ASSERT(status != mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_PENDING);
  restoreControlTransferToPool(transfer, true);
  ///setCurrentState(State_t::Running);
  ///pvPort.thSetTimeoutOccured(); /// \todo FIX !!!!!!!!
  pvStateMachineNew->process_event(SplitActionDoneEvent());
}

void mdtUsbtmcTransferHandler::submitInitiateClearRequest()
{
  ///Q_ASSERT(currentState() == State_t::AbortingBulkIo);

  qDebug() << "submitInitiateClearRequest() called ...";

  // Before submitting request, check that all bulk-transfers are cancelled
  /**
  pvBulkOutTransferMutex.lock();
  if(pvBulkOutTransferPool.getPendingTransfer() != 0){
    pvBulkOutTransferMutex.unlock();
    return;
  }
  pvBulkOutTransferMutex.unlock();
  pvBulkInTransferMutex.lock();
  if(pvBulkInTransferPool.getPendingTransfer() != 0){
    pvBulkInTransferMutex.unlock();
    return;
  }
  pvBulkInTransferMutex.unlock();
  */

  qDebug() << "Submitting INITIATE_CLEAR for interface " << pvInterfaceDescriptor.bInterfaceNumber() << " ...";
  // Submit INITIATE_CLEAR request
  std::lock_guard<std::mutex> ctlg(pvControlTransferMutex);
  auto controlTransfer = pvControlTransferPool.getTransfer(*this);
  // Send a INITIATE_ABORT_BULK_IN control request
  Q_ASSERT(controlTransfer != 0);
  // Setup transfer and submit it
  controlTransfer->setupInitiateClear(pvInterfaceDescriptor.bInterfaceNumber(), 30000);
  if(!controlTransfer->submit()){
    ErrorOccuredEvent errorEvent(controlTransfer->lastError());
    pvStateMachineNew->process_event(errorEvent);
    return;
    /**
    pvLastErrorMutex.lock();
    pvLastError = controlTransfer->lastError();
    pvLastErrorMutex.unlock();
    restoreControlTransferToPool(controlTransfer, true);
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
    */
  }
}

void mdtUsbtmcTransferHandler::handleInitiateClearResponse(mdtUsbtmcControlTransfer *transfer)
{
  Q_ASSERT(transfer != 0);
  Q_ASSERT(transfer->actualLength() > 0); // Can be > 1
  Q_ASSERT(transfer->usbtmcRequest() == mdtUsbtmcControlTransfer::USBTMCbRequest::INITIATE_CLEAR);

  /**
  if(currentState() != State_t::AbortingBulkIo){
    restoreControlTransferToPool(transfer, true);
    return;
  }
  */
  // Check transfer size - must be 1
  qDebug() << "actual len: " << transfer->actualLength() << " - wLength: " << transfer->wLength();
  if(transfer->wLength() != 1){
    QString msg = deviceIdString() + QObject::tr("INITIATE_CLEAR request returned unexpected amount of data (expected: 1)");
    mdtError error(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(error, "mdtUsbtmcTransferHandler");
    ErrorOccuredEvent errorEvent(error);
    pvStateMachineNew->process_event(errorEvent);
    return;
    /**
    pvLastErrorMutex.lock();
    QString msg = deviceIdString() + QObject::tr("INITIATE_CLEAR request returned unexpected amount of data (expected: 1)");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    pvLastErrorMutex.unlock();
    restoreControlTransferToPool(transfer, true);
    setCurrentState(State_t::Error);
    pvPort.thSetError(pvLastError);
    return;
    */
  }

  qDebug() << "Continue INITIATE_CLEAR - status: " << transfer->usbtmcStatusText();

  // If status is different than STATUS_SUCCESS, we are done
  if(transfer->usbtmcStatus() != mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_SUCCESS){
    restoreControlTransferToPool(transfer, true);
    ///setCurrentState(State_t::Running);
    pvStateMachineNew->process_event(SplitActionDoneEvent());
    return;
  }
  // Submit CHECK_CLEAR_STATUS request
  submitCheckClearStatusRequest(transfer);
}

void mdtUsbtmcTransferHandler::submitCheckClearStatusRequest(mdtUsbtmcControlTransfer* controlTransfer)
{
  // transfer can be Null - Get one if required
  if(controlTransfer == 0){
    std::lock_guard<std::mutex> ctlg(pvControlTransferMutex);
    controlTransfer = pvControlTransferPool.getTransfer(*this);
  }
  // Send a CHECK_CLEAR_STATUS control request
  Q_ASSERT(controlTransfer != 0);
  // Setup transfer and submit it
  qDebug() << "Submitting CHECK_CLEAR_STATUS ...";
  controlTransfer->setupCheckClearStatus(pvInterfaceDescriptor.bInterfaceNumber(), 30000);
  if(!controlTransfer->submit()){
    ErrorOccuredEvent errorEvent(controlTransfer->lastError());
    pvStateMachineNew->process_event(errorEvent);
    return;
    /**
    pvLastErrorMutex.lock();
    pvLastError = controlTransfer->lastError();
    pvLastErrorMutex.unlock();
    restoreControlTransferToPool(controlTransfer, true);
    setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
    return;
    */
  }
}

void mdtUsbtmcTransferHandler::handleCheckClearStatusResponse(mdtUsbtmcControlTransfer* transfer)
{
  Q_ASSERT(transfer != 0);
  Q_ASSERT(transfer->actualLength() > 0); // Can be > 2
  Q_ASSERT(transfer->usbtmcRequest() == mdtUsbtmcControlTransfer::USBTMCbRequest::CHECK_CLEAR_STATUS);

  /**
  if(currentState() != State_t::AbortingBulkIo){
    restoreControlTransferToPool(transfer, true);
    return;
  }
  */
  // Check transfer size - must be 2
  qDebug() << "actual len: " << transfer->actualLength() << " - wLength: " << transfer->wLength();
  if(transfer->wLength() != 2){
    QString msg = deviceIdString() + QObject::tr("CHECK_CLEAR_STATUS request returned unexpected amount of data (expected: 2)");
    mdtError error(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(error, "mdtUsbtmcTransferHandler");
    ErrorOccuredEvent errorEvent(error);
    pvStateMachineNew->process_event(errorEvent);
    return;
    /**
    pvLastErrorMutex.lock();
    QString msg = deviceIdString() + QObject::tr("CHECK_CLEAR_STATUS request returned unexpected amount of data (expected: 2)");
    pvLastError.setError(msg, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbtmcTransferHandler");
    pvLastError.commit();
    pvLastErrorMutex.unlock();
    restoreControlTransferToPool(transfer, true);
    setCurrentState(State_t::Error);
    pvPort.thSetError(pvLastError);
    return;
    */
  }

  qDebug() << "Continue CHECK_CLEAR_STATUS - status: " << transfer->usbtmcStatusText();
  qDebug() << "Queues: control: " << pvControlTransferPool.pendingTransferCount() << " , bulk-OUT: " << pvBulkOutTransferPool.pendingTransferCount() << " , bulk-IN: " << pvBulkInTransferPool.pendingTransferCount();

  // For STATUS_PENDING we must also check if device has more data in its Bulk-IN FIFO
  if(transfer->usbtmcStatus() == mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_PENDING){
    if(transfer->byteValue(1) == 1){
      /*
       * Device's Bulk-IN FIFO not empty
       * Because all bulk (OUT and IN) transfers has been cancelled before INITIATE_CLEAR request,
       *  we have no pending transfers.
       */
      submitBulkInTransfer(30000, mdtUsbtmcBulkTransfer::SplitAction_t::CHECK_CLEAR_STATUS);
      restoreControlTransferToPool(transfer, true);
    }else{
      // Let device some time and re-submit CHECK_CLEAR_STATUS reqiest
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      submitCheckClearStatusRequest(transfer);
    }
    return;
  }
  // Here we have status other than STATUS_PENDING
  Q_ASSERT(transfer->usbtmcStatus() != mdtUsbtmcControlTransfer::USBTMCstatus::STATUS_PENDING);
  restoreControlTransferToPool(transfer, true);
  ///submitClearBulkOutEndpointHalt(10000);
  // handleControlTransferComplete() will return to Running state
  pvStateMachineNew->process_event(ClearBulkOutEndpointHaltRequestedEvent());
}

void mdtUsbtmcTransferHandler::submitCancelAllInterruptInTransfers(bool lockMutex)
{
  if(lockMutex){
    std::lock_guard<std::mutex> itlg(pvInterruptInTransferMutex);
  }
  /*
   * Retire IRPs of all pending Interrupt-IN transfers
   */
  auto pendingTransfers = pvInterruptInTransferPool.getAllPendingTransfers();
  for(auto transfer : pendingTransfers){
    Q_ASSERT(transfer != 0);
    if(!transfer->cancel()){
      ErrorOccuredEvent errorEvent(transfer->lastError());
      pvStateMachineNew->process_event(errorEvent);
      return;
      /**
      pvLastErrorMutex.lock();
      pvLastError = transfer->lastError();
      pvLastErrorMutex.unlock();
      setCurrentStateFromLibusbError(static_cast<libusb_error>(pvLastError.systemNumber()));
      return;
      */
    }
  }
//   mdtUsbtmcInterruptTransfer *transfer;
//   unsigned int pendingTransfersCount = pendingTransfers.size();
//   if(pendingBulkOutTransfersCount > 0){
//     for(unsigned int i = 0; i < (pendingBulkOutTransfersCount -1); ++i){
//       transfer = pendingBulkOutTransfer[i];
//       Q_ASSERT(transfer != 0);
//       submitRetireBulkIRPs(transfer);
//     }
//     // For last transfer, we must tell handleBulkOutTransferComplete() if it has something to do
//     transfer = pendingBulkOutTransfer[pendingBulkOutTransfersCount-1];
//     Q_ASSERT(transfer != 0);
//     transfer->setSplitAction(action);
//     submitRetireBulkIRPs(transfer);
//   }
}



bool mdtUsbtmcTransferHandler::getCapabilities(uint8_t interfaceNumber, unsigned int timeout)
{
  /**
  if((currentState() != State_t::Running) && (currentState() != State_t::Setup)){
    return true;
  }
  */
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
  
  qDebug() << "Interface accepts INDICATOR_PULSE: " << transfer->interfaceAcceptsIndicatorPulse();
  
  // Restore transfer back to pool
  restoreControlTransferToPool(transfer, true);

  qDebug() << "GET_CAPABILITIES DONE";
  return true;
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
        pvPort.thSetError(pvLastError);
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

QString mdtUsbtmcTransferHandler::libusbTransferStatusText(libusb_transfer_status status) const
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

  return msg;
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

void mdtUsbtmcTransferHandler::setLastErrorFromInterruptInTransferStatus(mdtUsbtmcInterruptTransfer* transfer, mdtError::level_t level, bool lockMutex)
{
  Q_ASSERT(transfer != 0);

  QString msg = deviceIdString();

  // Set text part
  switch(level){
    case mdtError::NoError:
    case mdtError::Info:
      msg += QObject::tr("Interrupt-IN transfer status information.");
      break;
    case mdtError::Warning:
      msg += QObject::tr("Interrupt-IN transfer warning.");
      break;
    case mdtError::Error:
      msg += QObject::tr("Interrupt-IN transfer failed.");
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
  // Update pvLastError
  if(lockMutex){
    pvLastErrorMutex.lock();
  }
  pvLastError.setSystemError(status, libusbTransferStatusText(status));
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
      ///setCurrentState(State_t::Error);
      pvPort.thSetError(pvLastError);
  }
}
