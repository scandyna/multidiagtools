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
#ifndef MDT_USBTMC_CONTROL_TRANSFER_H
#define MDT_USBTMC_CONTROL_TRANSFER_H

#include "mdtUsbControlTransfer.h"
#include <vector>
#include <cstdint>

class mdtUsbtmcTransferHandler;

/*! \brief USBTMC control transfer
 */
class mdtUsbtmcControlTransfer : public mdtUsbControlTransfer
{
 public:

  /*! \brief USBTMC bRequest
   *
   * See Table 15 in USBTMC 1.0 specifications, section 4.2.1.
   */
  enum class USBTMCbRequest : uint8_t
  {
    None = 0,                         /*!< Marked as reserved in specification, but... */
    INITIATE_ABORT_BULK_OUT = 1,      /*!< Abort a Bulk-OUT transfer. */
    CHECK_ABORT_BULK_OUT_STATUS = 2,  /*!< Returns the status of the previously sent INITIATE_ABORT_BULK_OUT request. */
    INITIATE_ABORT_BULK_IN = 3,       /*!< Abort a Bulk-IN transfer. */
    CHECK_ABORT_BULK_IN_STATUS = 4,   /*!< Returns the status of the previously sent INITIATE_ABORT_BULK_IN request. */

    Other = 3       /*!< Other recipient */
  };

  /*! \brief Constructor
   *
   * \todo Fix a realistic bufferSize
   *
   * \pre bufferSize must be >= 8.
   */
  mdtUsbtmcControlTransfer(mdtUsbtmcTransferHandler & th, libusb_device_handle *dev_handle, int bufferSize = 1024);

  /*! \brief Setup a clear endpoint halt transfer
   *
   * See USBTMC specifications, section 4.1.1 (CLEAR_FEATURE request wValue = ENDPOINT_HALT)
   *
   * \param transferHandler This transferHandler will be passed to user_data in libusb_transfer.
   * \param endpointNumber Endpoint number. \todo Must be number or raw address ?
   * \param timeout timeout for the transfer in milliseconds.
   */
  void setupClearEndpointHalt(uint8_t endpointNumber, unsigned int timeout);

  /*! \brief Get transfer handler
   */
  mdtUsbtmcTransferHandler & transferHandler()
  {
    return pvTransferHandler;
  }

  // Disable copy
  mdtUsbtmcControlTransfer(const mdtUsbtmcControlTransfer & rhs) = delete;
  mdtUsbtmcControlTransfer & operator=(const mdtUsbtmcControlTransfer & rhs) = delete;

 private:

  mdtUsbtmcTransferHandler & pvTransferHandler;
};

#endif // #ifndef MDT_USBTMC_CONTROL_TRANSFER_H
