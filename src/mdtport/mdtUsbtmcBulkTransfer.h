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
#ifndef MDT_USBTMC_BULK_TRANSFER_H
#define MDT_USBTMC_BULK_TRANSFER_H

#include "mdtUsbTransfer.h"
#include "mdtUsbtmcFrame.h"
#include "mdtUsbEndpointDescriptor.h"
#include <QByteArray>
#include <cstdint>

class mdtUsbtmcTransferHandler;

/*! \brief USBTMC bulk transfer
 */
class mdtUsbtmcBulkTransfer : public mdtUsbTransfer
{
 public:

  /*! \brief Constructor
   *
   * \param th Transfer handler
   * \param endpointDescriptor Endpoint descriptor
   * \param dev_handle A handle to a open device
   * \param bufferSize Internal USBTMC frame's buffer size.
   * \pre endpointDescriptor must not be empty
   * \pre bufferSize must be at least 512 and a multiple of 4
   */
  mdtUsbtmcBulkTransfer(mdtUsbtmcTransferHandler & th, mdtUsbEndpointDescriptor endpointDescriptor, libusb_device_handle *dev_handle, int bufferSize = 4096);

  // Disable copy
  mdtUsbtmcBulkTransfer(const mdtUsbtmcBulkTransfer & rhs) = delete;
  mdtUsbtmcBulkTransfer & operator=(const mdtUsbtmcBulkTransfer & rhs) = delete;

  /*! \brief Setup DEV_DEP_MSG_OUT transfer
   *
   * See USBTMC 1.0 specifications, section 3.2.1.1
   *
   * \param bTag See USBTMC 1.0 specifications, Table 1 for details
   * \param message Message in witch data must be transferred.
   * \param timeout Timeout [ms]
   * 
   * \pre bTag must be in range [1;255]
   */
  void setupDevDepMsgOut(uint8_t bTag, mdtUsbtmcMessage & message, unsigned int timeout);

  /*! \brief Get transfer handler
   */
  mdtUsbtmcTransferHandler & transferHandler()
  {
    return pvTransferHandler;
  }

 private:

  mdtUsbtmcFrame pvFrame;
  mdtUsbtmcTransferHandler & pvTransferHandler;
  mdtUsbEndpointDescriptor pvEndpointDescriptor;
};

#endif // #ifndef MDT_USBTMC_BULK_TRANSFER_H
