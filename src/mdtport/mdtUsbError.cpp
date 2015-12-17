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
#include "mdtUsbError.h"
#include <QString>
#include <QStringBuilder>
#include <QObject>

namespace mdtUsbError
{

mdtError fromLibusbError(int usbError)
{
  mdtError error;
  error.setError<libusb_error>(static_cast<libusb_error>(usbError), libusb_strerror(static_cast<libusb_error>(usbError)), mdtError::Error);
  return error;
}

mdtError fromLibusbTransferStatus(libusb_transfer_status status)
{
  mdtError error;
  QString msg;

  // Build system error text
  switch(status){
    case LIBUSB_TRANSFER_COMPLETED:
      msg = "LIBUSB_TRANSFER_COMPLETED (" % QObject::tr("Transfer completed without error.") % ")";
      break;
    case LIBUSB_TRANSFER_ERROR:
      msg = "LIBUSB_TRANSFER_ERROR (" % QObject::tr("Transfer failed.") % ")";
      break;
    case LIBUSB_TRANSFER_TIMED_OUT:
      msg = "LIBUSB_TRANSFER_TIMED_OUT (" % QObject::tr("Transfer timed out.") % ")";
      break;
    case LIBUSB_TRANSFER_CANCELLED:
      msg = "LIBUSB_TRANSFER_CANCELLED (" % QObject::tr("Transfer was cancelled.") % ")";
      break;
    case LIBUSB_TRANSFER_STALL:
      msg = "LIBUSB_TRANSFER_STALL (" % QObject::tr("Halt condition or control request not supported.") % ")";
      break;
    case LIBUSB_TRANSFER_NO_DEVICE:
      msg = "LIBUSB_TRANSFER_NO_DEVICE (" % QObject::tr("Device was disconnected.") % ")";
      break;
    case LIBUSB_TRANSFER_OVERFLOW:
      msg = "LIBUSB_TRANSFER_OVERFLOW (" % QObject::tr("Device sent more data than requested (in USB layer)") % ")";
      break;
  }
  // Build error
  error.setError<libusb_transfer_status>(status, msg, mdtError::Error);

  return error;
}

} // namespace mdtUsbError
