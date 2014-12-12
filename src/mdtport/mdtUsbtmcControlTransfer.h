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

  /*! \brief Constructor
   *
   * \todo Fix a realistic bufferSize
   *
   * \pre bufferSize must be >= 8.
   */
  mdtUsbtmcControlTransfer(libusb_device_handle *dev_handle, int bufferSize = 1024);

  // Disable copy
  mdtUsbtmcControlTransfer(const mdtUsbtmcControlTransfer & rhs) = delete;
  mdtUsbtmcControlTransfer & operator=(const mdtUsbtmcControlTransfer & rhs) = delete;

 private:

  /*! \brief Fill transfer
   *
   * This version is mostly usefull for queries without data,
   *  or with data sended from device to host.
   *
   * For details about USB specific arguments, see USB 3.1 spec, chap. 9.3 .
   *
   * \param bmRequestType Request characteristics (direction, type, recipient). Must be set correctly by caller.
   * \param bRquest Specific request (see USB spec.)
   * \param wValue See USB spec.
   * \param wIndex See USB spec.
   * \param wLength Number of bytes to transfer. Has only sense for direction device-to-host (should be 0 else).
   * \pre Internal buffer size (witch was given to constructor) must be >= 8 + wLength.
   */
  ///void fillTransfer(mdtUsbtmcTransferHandler & th, uint8_t bmRequestType, uint8_t bRquest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, unsigned int timeout);

  /*! \brief Fill transfer
   *
   * This version is mostly usefull for queries with data sended from host to device.
   *
   * For details about USB specific arguments, see USB 3.1 spec, chap. 9.3 .
   *
   * \param bmRequestType Request characteristics (direction, type, recipient). Must be set correctly by caller.
   * \param bRquest Specific request (see USB spec.)
   * \param wValue See USB spec.
   * \param wIndex See USB spec.
   * \param data Data that will be transferred to the device. Note: will also set wLength regarding data length.
   * \pre Internal buffer size (witch was given to constructor) must be >= 8 + data length.
   */
  ///void fillTransfer(mdtUsbtmcTransferHandler & th, uint8_t bmRequestType, uint8_t bRquest, uint16_t wValue, uint16_t wIndex, const std::vector<unsigned char> & data, unsigned int timeout);

  ///std::vector<unsigned char> pvBuffer;
};

#endif // #ifndef MDT_USBTMC_CONTROL_TRANSFER_H
