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
#include "mdtUsbInterfaceDescriptor.h"
#include "mdtUsbEndpointDescriptor.h"
#include "mdtUsbTransfer.h"
#include "mdtBasicStateMachine.h"
#include <QtGlobal>
#include <libusb-1.0/libusb.h>
#include <cstdint>
#include <mutex>

class mdtUsbtmcControlTransfer;
class mdtUsbDeviceDescriptor;

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
    Running,      /*!< USB port thread is running (ready for I/O) */
    Stopping,     /*!< USB port thread is stopping (No I/O, submit no more transfer) */
    Error         /*!< USB port thread is stopped due to a error (No I/O) */
  };

  /*! \brief Constructor
   */
  mdtUsbtmcTransferHandler();

  /*! \brief Setup handler
   */
  bool setup(libusb_device_handle *handle, const mdtUsbDeviceDescriptor & descriptor, uint8_t bInterfaceNumber);

  bool submitClearEndpointHalt(uint8_t endpointNumber, unsigned int timeout);

  bool submitClearBulkOutEndpointHalt(unsigned int timeout)
  {
    return submitClearEndpointHalt(pvBulkOutDescriptor.number(), timeout);
  }


  bool submitClearBulkInEndpointHalt(unsigned int timeout)
  {
    return submitClearEndpointHalt(pvBulkInDescriptor.number(), timeout);
  }

  /*! \brief Submit control transfers cancellation
   */
  bool submitControlTransfersCancel();

  /*! \brief Submit a transfer cancellation
   *
   * Note: this function does not handle any transfer mutex,
   *  caller is responsible of this.
   */
  ///bool submitTransferCancel(mdtUsbTransfer *transfer);

  /*! \brief Control transfer callback
   */
  static void controlTransferCallback(libusb_transfer *transfer);

  /*! \brief Handle control transfer complete
   */
  void handleControlTransferComplete(mdtUsbtmcControlTransfer *transfer);

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

  Q_DISABLE_COPY(mdtUsbtmcTransferHandler);

  libusb_device_handle *pvDeviceHandle;
  mdtBasicStateMachine<State_t> pvStateMachine;
  // Descriptors
  mdtUsbInterfaceDescriptor pvInterfaceDescriptor;
  mdtUsbEndpointDescriptor pvBulkOutDescriptor;
  mdtUsbEndpointDescriptor pvBulkInDescriptor;
  mdtUsbEndpointDescriptor pvInterruptInDescriptor;
  // Control transfer members
  mdtUsbTransferPool<mdtUsbtmcControlTransfer> pvControlTransferPool;
  std::mutex pvControlTransferMutex;
  
  mdtError pvLastError;
};

#endif  // #ifndef MDT_USBTMC_TRANSFER_HANDLER_H
