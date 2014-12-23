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
#ifndef MDT_USBTMC_INTERRUPT_TRANSFER_H
#define MDT_USBTMC_INTERRUPT_TRANSFER_H

#include "mdtUsbTransfer.h"
#include "mdtUsbEndpointDescriptor.h"
#include <QByteArray>
#include <cstdint>
#include <vector>

class mdtUsbtmcTransferHandler;

/*! \brief USBTMC interrupt transfer
 *
 * See USBTMC 1.0 specifications, section 3.4
 *  and USBTMC-USB488 1.0 specifications, section 3.4
 */
class mdtUsbtmcInterruptTransfer : public mdtUsbTransfer
{
 public:

  /*! \brief Constructor
   *
   * \param th Transfer handler
   * \param endpointDescriptor Endpoint descriptor
   * \param dev_handle A handle to a open device
   * \param bufferSize Internal buffer size.
   * \pre endpointDescriptor must not be empty
   */
  mdtUsbtmcInterruptTransfer(mdtUsbtmcTransferHandler & th, mdtUsbEndpointDescriptor endpointDescriptor, libusb_device_handle *dev_handle, int bufferSize = 1024);

  // Disable copy
  mdtUsbtmcInterruptTransfer(const mdtUsbtmcInterruptTransfer & rhs) = delete;
  mdtUsbtmcInterruptTransfer & operator=(const mdtUsbtmcInterruptTransfer & rhs) = delete;

 private:

  mdtUsbtmcTransferHandler & pvTransferHandler;
  mdtUsbEndpointDescriptor pvEndpointDescriptor;
  std::vector<unsigned char> pvBuffer;
};

#endif // #ifndef MDT_USBTMC_INTERRUPT_TRANSFER_H
