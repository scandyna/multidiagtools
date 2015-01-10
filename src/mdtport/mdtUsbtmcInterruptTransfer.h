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

  /*! \brief Format
   *
   * Represents bits D7 and D6 in bNotify1 field.
   *
   * See USBTMC 1.0 specifications, section 3.4, table 13
   *  and USBTMC-USB488 1.0 specifications, section 3.4
   */
  enum class bNotify1Format_t : uint8_t
  {
    Subclass = 2,       /*!< Defined in applicable subclass */
    VendorSpecific = 1, /*!< Vendor specific format */
    Usbtmc = 0          /*!< Reserved for USBTMC use */
  };

  /*! \brief Constructor
   *
   * \todo Check for a realistic bufferSize
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

  /*! \brief Setup transfer
   */
  void setup(unsigned int timeout);

  /*! \brief Get bNotify1 format
   */
  bNotify1Format_t bNotify1Format() const
  {
    Q_ASSERT(pvBuffer.capacity() > 0);
    uint8_t bNotify1 = pvBuffer[0];
    if(bNotify1 & 0b10000000){
      return bNotify1Format_t::Subclass;
    }
    return static_cast<bNotify1Format_t>((bNotify1 & 0b11000000) >> 6);
  }

  /*! \brief Set value at offset
   *
   * \pre offset must be < internal buffer's capacity
   */
  void setByteValue(unsigned int offset, uint8_t value)
  {
    Q_ASSERT(offset < pvBuffer.capacity());
    pvBuffer[offset] = value;
  }

  /*! \brief Get value at offset
   *
   * \pre offset must be < internal buffer's capacity
   */
  uint8_t byteValue(unsigned int offset) const
  {
    Q_ASSERT(offset < pvBuffer.capacity());
    return pvBuffer[offset];
  }

  /*! \brief Get transfer handler
   */
  mdtUsbtmcTransferHandler & transferHandler()
  {
    return pvTransferHandler;
  }

 private:

  mdtUsbtmcTransferHandler & pvTransferHandler;
  mdtUsbEndpointDescriptor pvEndpointDescriptor;
  std::vector<unsigned char> pvBuffer;
};

#endif // #ifndef MDT_USBTMC_INTERRUPT_TRANSFER_H
