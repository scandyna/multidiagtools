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
#include "mdtBasicStateMachine.h"
#include <QObject>
#include <QString>
#include <libusb-1.0/libusb.h>
#include <cstdint>
#include <mutex>

class mdtUsbtmcControlTransfer;
class mdtUsbtmcBulkTransfer;

/*! \brief USBTMC transfer handler
 */
class mdtUsbtmcTransferHandler
{
 public:

  /*! \brief USBTMC transfer handler state
   */
  enum class State_t
  {
    Stopped = 0,  /*!< USB port thread is stopped */
    Setup,        /*!< USB port thread is starting and has called setup() */
    Running,      /*!< USB port thread is running (ready for I/O) */
    Stopping,     /*!< USB port thread is stopping (No I/O, submit no more transfer) */
    Error         /*!< USB port thread is stopped due to a error (No I/O) */
  };

  /*! \brief Constructor
   */
  mdtUsbtmcTransferHandler(libusb_context *usbContext);

  /*! \brief Setup handler
   */
  bool setup(libusb_device_handle *handle, const mdtUsbDeviceDescriptor & descriptor, uint8_t bInterfaceNumber);

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

  /*! \brief Begin to abort a bulk-OUT transfer
   *
   * Will submit INITIATE_ABORT_BULK_OUT request
   *  and return. When complete, continueAbortBulkOutTransfer() will be called.
   */
  bool beginAbortBulkOutTransfer();

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

  /*! \brief Handle control transfer complete
   */
  void handleControlTransferComplete(mdtUsbtmcControlTransfer *transfer);

  /*! \brief Bulk-OUT transfer callback
   */
  static void bulkOutTransferCallback(libusb_transfer *transfer);

  /*! \brief Handle Bulk-OUT transfer complete
   */
  void handleBulkOutTransferComplete(mdtUsbtmcBulkTransfer *transfer);

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
  void handleSyncEvents();

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

 private:

  /*! \brief Submit a GET_CAPABILITIES request and \todo Comment..
   *
   * This function does synchronous transfer, be care about.
   *
   * \sa See handleSyncEvents()
   */
  bool getCapabilities(uint8_t interfaceNumber, unsigned int timeout);

  /*! \brief Continue to abort a bulk-OUT transfer
   *
   * This function does synchronous transfer, be care about.
   *
   * \sa See handleSyncEvents()
   */
  bool continueAbortBulkOutTransfer();

  /*! \brief Continue clear bulk I/O
   *
   * Note: will restore transfer back to pool once done.
   * This function does synchronous transfer, be care about.
   *
   * \sa See handleSyncEvents()
   */
  void continueClearBulkIo(mdtUsbtmcControlTransfer * transfer);

  /*! \brief Process synchronous control transfer
   *
   * Note: must be called only from port thread, but not from a callback.
   *  See handleSyncEvents() for some explaination.
   *
   * This function handles no lock and no transfer pool,
   *  caller is responsible of this.
   */
  bool processSyncControlTransfer(mdtUsbtmcControlTransfer *transfer);

  /*! \brief Get device identification string
   *
   * Helper function for error messages generation.
   *  Will simply get pvDeviceDescriptor::idString() with some "decoration text", will not query device.
   */
  QString deviceIdString() const;

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
  std::mutex pvBulkOutTransferMutex;
  // Control transfer members
  mdtUsbTransferPool<mdtUsbtmcControlTransfer> pvControlTransferPool;
  std::mutex pvControlTransferMutex;
  // Error members
  mdtError pvLastError;
  std::mutex pvLastErrorMutex;
};

#endif  // #ifndef MDT_USBTMC_TRANSFER_HANDLER_H
