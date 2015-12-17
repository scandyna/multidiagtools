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
#ifndef MDT_USB_ERROR_H
#define MDT_USB_ERROR_H

#include "mdtError.h"
#include <libusb-1.0/libusb.h>

namespace mdtUsbError
{
  /*! \brief Get a mdtError from given libusb error
   */
  mdtError fromLibusbError(int usbError);

  /*! \brief Get a mdtError from a libusb_transfer_status
   */
  mdtError fromLibusbTransferStatus(libusb_transfer_status status);

} // namespace mdtUsbError

#endif // #ifndef MDT_USB_ERROR_H
