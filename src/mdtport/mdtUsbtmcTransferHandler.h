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
#ifndef MDT_USBTMC_TRANSFER_HANDLER_H
#define MDT_USBTMC_TRANSFER_HANDLER_H

#include "mdtError.h"
#include "mdtUsbTransferPool.h"
#include "mdtUsbDeviceDescriptor.h"
#include "mdtUsbInterfaceDescriptor.h"
#include "mdtUsbEndpointDescriptor.h"
#include "mdtUsbTransfer.h"
#include "mdtUsbtmcBulkTransfer.h"
#include "mdtUsbtmcMessage.h"
#include "mdtBasicStateMachine.h"
#include <QObject>
#include <QString>
#include <libusb-1.0/libusb.h>
#include <cstdint>
#include <mutex>

class mdtUsbtmcControlTransfer;
///class mdtUsbtmcBulkTransfer;

/*! \brief USBTMC transfer handler
 */
class mdtUsbtmcTransferHandler
{
 public:

  /*! \brief USBTMC transfer handler state
   */
  enum class State_t
  {
    Stopped = 0,      /*!< USB port thread is stopped */
    Setup,            /*!< USB port thread is starting and has called setup() */
    Running,          /*!< USB port thread is running (ready for I/O) */
    ///AbortingBulkOut,  /*!< Currently aborting a Bulk-OUT transfer - No bulk transfer possible */
    ///AbortingBulkIn,   /*!< Currently aborting a Bulk-IN transfer - No bulk transfer possible */
    AbortingBulkIo,      /*!< Currently aborting a Bulk-OU/Bulk-OUT transfer - No bulk transfer possible */
    Stopping,         /*!< USB port thread is stopping (No I/O, submit no more transfer) */
    Disconnected,     /*!< Device is no more present */
    Error             /*!< USB port thread is stopped due to a error (No I/O) */
  };

  /*! \brief Constructor
   */
  mdtUsbtmcTransferHandler(libusb_context *usbContext);

  /*! \brief Setup handler
   */
  bool setup(libusb_device_handle *handle, const mdtUsbDeviceDescriptor & descriptor, uint8_t bInterfaceNumber);

  /*! \brief Submit a command
   * 
   * \param responseExpected If message is a query that expects a response, like *IDN?, set this flag.
   */
  void submitCommand(mdtUsbtmcMessage & message, bool responseExpected, unsigned int timeout);

  /*! \brief Will cancel all pending bulk-OUT and bulk-IN transfers
   *
   * Once all transfers are cancelled, INITIATE_CLEAR and, if needed,
   *  CHECK_CLEAR_STATUS is sent to device.
   */
  void submitBulkTransfersCancellation();

  /*! \brief Abort a Bulk-OUT transfer
   */
  ///void submitAbortBulkOutTransfer();

  /*! \brief Abort a Bulk-IN transfer
   */
  ///void submitAbortBulkInTransfer();

  /*! \brief Submit cancellation of all bulk-OUT transfers
   */
  ///void submitBulkOutTransfersCancellation();

  /*! \brief Submit cancellation of all bulk-IN transfers
   */
  ///void submitBulkInTransfersCancellation();

  /*! \brief Submit a custom transfer
   *
   * Used for debug and testing.
   * \pre transferBufferLength must be <= Internal frame capacity
   */
  void dbgSubmitCustomBulkOutTransfer(uint8_t msgID, uint8_t bTag, uint8_t bTagInverse, uint32_t transferSize, uint8_t bmTransferAttributes, uint8_t termChar,
                                      bool responseExpected, int transferBufferLength, unsigned int timeout);

  /*! \brief Force to abort a pending Bulk-OUT transfer
   *
   * Will get a pending Bulk-OUT transfer and proceed abort it.
   *  Used for debug and testing.
   */
  void dbgSubmitAbortOnePendingBulkOutTransfer();

  /*! \brief Force to abort a pending Bulk-IN transfer
   *
   * Will get a pending Bulk-IN transfer and proceed abort it.
   *  Used for debug and testing.
   */
  void dbgSubmitAbortOnePendingBulkInTransfer();

  /*! \brief Submit a CLEAR_FEATURE control request with wValue = ENDPOINT_HALT for given endpoint
   */
  void submitClearEndpointHalt(uint8_t endpointNumber, unsigned int timeout);

  /*! \brief Submit a CLEAR_FEATURE control request with wValue = ENDPOINT_HALT for Bulk-OUT endpoint
   */
  void submitClearBulkOutEndpointHalt(unsigned int timeout)
  {
    submitClearEndpointHalt(pvBulkOutDescriptor.number(), timeout);
  }

  /*! \brief Submit a CLEAR_FEATURE control request with wValue = ENDPOINT_HALT for Bulk-IN endpoint
   */
  void submitClearBulkInEndpointHalt(unsigned int timeout)
  {
    submitClearEndpointHalt(pvBulkInDescriptor.number(), timeout);
  }

  /*! \brief Begin clear bulk I/O
   *
   * Will begin bulk I/O clear and submit INITIATE_CLEAR request and return.
   *  When completed, continueClearBulkIo() will be called.
   */
  void beginClearBulkIo();

  /*! \brief Submit control transfers cancellation
   */
  void submitControlTransfersCancel();

  /*! \brief Control transfer callback
   */
  static void controlTransferCallback(libusb_transfer *transfer);

  /*! \brief Bulk-OUT transfer callback
   */
  static void bulkOutTransferCallback(libusb_transfer *transfer);

  /*! \brief Bulk-IN transfer callback
   */
  static void bulkInTransferCallback(libusb_transfer *transfer);

  /*! \brief Start synchornous events
   *
   * It's not possible to start a synchronous transfer from a callback
   *  (or a function called from a callback).
   *  If done so, a deadlock will be produced in op_handle_events(),
   *  and it seems to be the libusb_context::open_devs_lock (true for linux_usbfs.c).
   *  The callback must return before any call of libusb_handle_events()
   *  (or variants, all are calling libusb_handle_events_timeout_completed()).
   *
   * As workaround, the USBTMC port thread will call current function once
   *  libusb_handle_events() returns.
   */
  ///void handleSyncEvents();

  /*! \brief Check if there are pending transfers
   */
  bool hasPendingTransfers() const;

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

  /*! \brief Set current state
   */
  void setCurrentState(State_t s){
    pvStateMachine.setCurrentState(s);
  }

  /*! \brief Get current state
   */
  inline State_t currentState() const
  {
    return pvStateMachine.currentState();
  }

  /*! \brief Check if handler must be stopped
   *
   * Is called by port thread to know if it must exit.
   */
  bool mustBeStopped() const
  {
    switch(currentState()){
      case State_t::Running:
      case State_t::AbortingBulkIo:
      ///case State_t::AbortingBulkOut:
      ///case State_t::AbortingBulkIn:
        return false;
      default:
        return true;
    }
  }

 private:

  /*! \brief Handle Bulk-OUT transfer complete
   */
  void handleBulkOutTransferComplete(mdtUsbtmcBulkTransfer *transfer);

  /*! \brief Begin to abort a bulk-OUT transfer
   *
   * Will submit the first INITIATE_ABORT_BULK_OUT request and return.
   *  Later on, handleControlTransferComplete() will receive a response,
   *  then calls handleInitiateAbortBulkOutResponse().
   */
  void beginAbortBulkOutTransfer(uint8_t bTag);

  /*! \brief Handle INITIATE_ABORT_BULK_OUT response
   *
   * This function is called by handleControlTransferComplete()
   *  when a INITIATE_ABORT_BULK_OUT response was received.
   *
   * Regarding USBTMC status:
   *  - INITIATE_ABORT_BULK_OUT can be resubmitted
   *  - CHECK_ABORT_BULK_OUT_STATUS request can be sent
   *  - If done, current state will be updated back to Running
   */
  void handleInitiateAbortBulkOutResponse(mdtUsbtmcControlTransfer *transfer);

  /*! \brief Handle CHECK_ABORT_BULK_OUT_STATUS resonse
   *
   * This function is called by handleControlTransferComplete()
   *  when a CHECK_ABORT_BULK_OUT_STATUS response was received.
   *
   * Regarding USBTMC status:
   * 
   */
  void handleCheckAbortBulkOutStatusResponse(mdtUsbtmcControlTransfer *transfer);

  /*! \brief Continue to abort a bulk-OUT transfer
   *
   * This function does synchronous transfer, be care about.
   *
   * \sa See handleSyncEvents()
   */
  ///void continueAbortBulkOutTransfer(mdtUsbtmcControlTransfer *transfer);

  /*! \brief Submit a REQUEST_DEV_DEP_MSG_IN
   */
  void submitRequestDevDepMsgIn(mdtUsbtmcBulkTransfer *transfer);

  /*! \brief Get next bTag for bulk-OUT endpoint
   */
  uint8_t nextBulkOutbTag()
  {
    ++pvBulkOutbTag;
    if(pvBulkOutbTag == 0){
      ++pvBulkOutbTag;
    }
    return pvBulkOutbTag;
  }

  /*! \brief Retire IRPs for given transfer
   */
  void submitRetireBulkIRPs(mdtUsbtmcBulkTransfer *transfer);

  /*! \brief Submit cancel for all Bulk-OUT transfers
   *
   * If a action is given as argument, it will be set to last transfer cancel request.
   */
  void submitCancelAllBulkOutTransfers(bool lockMutex, mdtUsbtmcBulkTransfer::SplitAction_t action = mdtUsbtmcBulkTransfer::SplitAction_t::NoAction);

  /*! \brief Submit cancel for all Bulk-IN transfers
   *
   * If a action is given as argument, it will be set to last transfer cancel request.
   */
  void submitCancelAllBulkInTransfers(bool lockMutex, mdtUsbtmcBulkTransfer::SplitAction_t action = mdtUsbtmcBulkTransfer::SplitAction_t::NoAction);

  /*! \brief Submit a Bulk-IN transfer
   */
  void submitBulkInTransfer(unsigned int timeout, mdtUsbtmcBulkTransfer::SplitAction_t action = mdtUsbtmcBulkTransfer::SplitAction_t::NoAction);

  /*! \brief Handle Bulk-IN transfer complete
   */
  void handleBulkInTransferComplete(mdtUsbtmcBulkTransfer *transfer);

  /*! \brief Begin to abort a bulk-IN transfer
   *
   * At first, current state is set to AbortingBulkIo,
   *  preventing new bulk request to be accepted.
   *
   * Will retire IRPs for all pending bulk-OUT transfers
   *  that expect a response and submit the first INITIATE_ABORT_BULK_IN request.
   *
   *  Later on, handleControlTransferComplete() will receive a response,
   *  then calls handleInitiateAbortBulkInResponse().
   */
  void beginAbortBulkInTransfer(uint8_t bTag);

  /*! \brief Handle INITIATE_ABORT_BULK_IN response
   *
   * This function is called by handleControlTransferComplete()
   *  when a INITIATE_ABORT_BULK_IN response was received.
   *
   * If device returned STATUS_SUCCESS, split action will be set to CHECK_ABORT_STATUS for a pending bulk-IN transfer.
   *  This way, handleInitiateAbortBulkInResponse() knows that it must call submitCheckAbortBulkInStatusRequest().
   *
   * Regarding other USBTMC status:
   *  - INITIATE_ABORT_BULK_IN can be resubmitted
   *  - If done, retires bulk-IN IRPs and current state will be updated back to Running
   */
  void handleInitiateAbortBulkInResponse(mdtUsbtmcControlTransfer *transfer);

  /*! \brief Submit CHECK_ABORT_BULK_IN_STATUS query
   *
   * Called by handleBulkInTransferComplete()
   *  if transfer split action was set to CHECK_ABORT_STATUS.
   */
  void submitCheckAbortBulkInStatusRequest(mdtUsbtmcControlTransfer *controlTransfer = 0);

  /*! \brief Handle CHECK_ABORT_BULK_IN_STATUS resonse
   *
   * This function is called by handleControlTransferComplete()
   *  when a CHECK_ABORT_BULK_IN_STATUS response was received.
   *
   * If device returned STATUS_PENDING with bmAbortBulkIn.D0=1 :
   *  - If a pending bulk-IN transfer exists, its action will be set to CHECK_ABORT_STATUS
   *  - Else, a new bulk-IN transfer is initiated with action set to CHECK_ABORT_STATUS
   *
   * If device returned STATUS_PENDING with bmAbortBulkIn.D0=0 :
   *  - submitCheckAbortBulkInStatusRequest() is called after some time.
   *
   * If device returned a other status, current state will be updated back to Running.
   */
  void handleCheckAbortBulkInStatusResponse(mdtUsbtmcControlTransfer *transfer);

  /*! \brief Submit INITIATE_CLEAR request
   *
   * This function is called by handleBulkOutTransferComplete() and/or handleBulkInTransferComplete() or directly by submitBulkTransfersCancellation()
   *  when completed transfer has split action INITIATE_CLEAR.
   *
   * This function will really submit the request if there is not pending
   *  bulk transfer, that to prevent sending INITIATE_CLEAR more than once.
   */
  void submitInitiateClearRequest();

  /*! \brief Handle INITIATE_CLEAR response
   *
   * This function is called by handleControlTransferComplete()
   *  when a INITIATE_CLEAR response was received.
   */
  void handleInitiateClearResponse(mdtUsbtmcControlTransfer *transfer);

  /*! \brief Submit CHECK_CLEAR_STATUS query
   *
   * Can be called by handleBulkInTransferComplete()
   *  if transfer split action was set to CHECK_CLEAR_STATUS
   */
  void submitCheckClearStatusRequest(mdtUsbtmcControlTransfer *controlTransfer = 0);

  /*! \brief Handle CHECK_CLEAR_STATUS response
   *
   * This function is called by handleControlTransferComplete()
   *  when a CHECK_CLEAR_STATUS response was received.
   */
  void handleCheckClearStatusResponse(mdtUsbtmcControlTransfer *transfer);

  /*! \brief Handle control transfer complete
   */
  void handleControlTransferComplete(mdtUsbtmcControlTransfer *transfer);

  /*! \brief Submit a GET_CAPABILITIES request and \todo Comment..
   *
   * This function does synchronous transfer, be care about.
   *
   * \sa See handleSyncEvents()
   */
  bool getCapabilities(uint8_t interfaceNumber, unsigned int timeout);

  /*! \brief Continue clear bulk I/O
   *
   * Note: will restore transfer back to pool once done.
   * This function does synchronous transfer, be care about.
   *
   * \sa See handleSyncEvents()
   */
  ///void continueClearBulkIo(mdtUsbtmcControlTransfer * transfer);

  /*! \brief Process synchronous control transfer
   *
   * Note: must be called only from port thread, but not from a callback.
   *  See handleSyncEvents() for some explaination.
   *
   * This function handles no lock and no transfer pool,
   *  caller is responsible of this.
   */
  bool processSyncControlTransfer(mdtUsbtmcControlTransfer *transfer);

  /*! \brief Restore bulk-OUT transfer to pool
   */
  void restoreBulkOutTransferToPool(mdtUsbtmcBulkTransfer *transfer, bool lockMutex);

  /*! \brief Restore bulk-IN transfer to pool
   */
  void restoreBulkInTransferToPool(mdtUsbtmcBulkTransfer *transfer, bool lockMutex);

  /*! \brief Restore control transfer to pool
   */
  void restoreControlTransferToPool(mdtUsbtmcControlTransfer *transfer, bool lockMutex);

  /*! \brief Get device identification string
   *
   * Helper function for error messages generation.
   *  Will simply get pvDeviceDescriptor::idString() with some "decoration text", will not query device.
   */
  QString deviceIdString() const;

  /*! \brief Set last error regarding bulk-OUT transfer status code
   */
  void setLastErrorFromBulkOutTransferStatus(mdtUsbtmcBulkTransfer *transfer, mdtError::level_t level, bool lockMutex);

  /*! \brief Set last error regarding bulk-IN transfer status code
   */
  void setLastErrorFromBulkInTransferStatus(mdtUsbtmcBulkTransfer *transfer, mdtError::level_t level, bool lockMutex);

  /*! \brief Set last error regarding transfer status code
   *
   * Note: will only set the systemError part
   */
  void setLastErrorFromTransferStatus(libusb_transfer_status status, bool lockMutex);

  /*! \brief Set state regarding libusb_error
   */
  void setCurrentStateFromLibusbError(libusb_error err);

  Q_DISABLE_COPY(mdtUsbtmcTransferHandler);

  libusb_context *pvUsbContext;
  libusb_device_handle *pvDeviceHandle;
  mdtBasicStateMachine<State_t> pvStateMachine;
  // Descriptors
  mdtUsbDeviceDescriptor pvDeviceDescriptor;
  mdtUsbInterfaceDescriptor pvInterfaceDescriptor;
  mdtUsbEndpointDescriptor pvBulkOutDescriptor;
  mdtUsbEndpointDescriptor pvBulkInDescriptor;
  mdtUsbEndpointDescriptor pvInterruptInDescriptor;
  // Bulk-OUT transfer members
  mdtUsbTransferPool<mdtUsbtmcBulkTransfer> pvBulkOutTransferPool;
  uint8_t pvBulkOutbTag;
  std::mutex pvBulkOutTransferMutex;
  // Bulk-IN transfer members
  mdtUsbTransferPool<mdtUsbtmcBulkTransfer> pvBulkInTransferPool;
  ///uint8_t pvBulkInbTag;
  std::mutex pvBulkInTransferMutex;
  // Control transfer members
  mdtUsbTransferPool<mdtUsbtmcControlTransfer> pvControlTransferPool;
  std::mutex pvControlTransferMutex;
  // Error members
  mdtError pvLastError;
  std::mutex pvLastErrorMutex;
};

#endif  // #ifndef MDT_USBTMC_TRANSFER_HANDLER_H
