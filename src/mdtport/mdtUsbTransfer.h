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
#ifndef MDT_USB_TRANSFER_H
#define MDT_USB_TRANSFER_H

#include "mdtError.h"
#include <libusb-1.0/libusb.h>
#include <cstdint>

/*! \brief USB transfer
 *
 * libusb_transfer wrapper class.
 */
class mdtUsbTransfer
{
 public:

  /*! \brief Constructor
   */
  mdtUsbTransfer(libusb_device_handle *dev_handle, int isoPacketsCount = 0);

  /*! \brief Destructor
   */
  ~mdtUsbTransfer();

  /*! \brief Check that allocation not failed
   */
  bool allocOk() const
  {
    return (pvTransfer != 0);
  }

  /*! \brief Get transfer flags
   */
  uint8_t flags() const{
    Q_ASSERT(pvTransfer != 0);
    return pvTransfer->flags;
  }

  /*! \brief Submit transfer
   *
   * Will use libusb_submit_transfer()
   *
   * \pre Internall transfer must be valid ( see allocOk() ).
   */
  bool submit();

  /*! \brief Get transfer status
   *
   * \pre Internall transfer must be valid ( see allocOk() ).
   */
  libusb_transfer_status status() const
  {
    Q_ASSERT(pvTransfer != 0);
    return pvTransfer->status;
  }

  /*! \brief Get last error
   */
  mdtError lastError() const
  {
    return pvLastError;
  }

  // Disable copy
  mdtUsbTransfer(const mdtUsbTransfer & rhs) = delete;
  mdtUsbTransfer & operator=(const mdtUsbTransfer & rhs) = delete;

 protected:

  /*! \brief Get device handle
   */
  inline libusb_device_handle *deviceHandle()
  {
    return pvDeviceHandle;
  }

  /*! \brief Access libusb transfer
   */
  inline libusb_transfer *transfer()
  {
    return pvTransfer;
  }

 private:

  libusb_transfer *pvTransfer;
  libusb_device_handle *pvDeviceHandle;
  mdtError pvLastError;
};

#endif // MDT_USB_TRANSFER_H
