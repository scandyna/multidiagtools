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
#include "mdtUsbControlTransfer.h"

mdtUsbControlTransfer::mdtUsbControlTransfer(libusb_device_handle* dev_handle, int bufferSize )
 : mdtUsbTransfer(dev_handle, bufferSize)
{
  Q_ASSERT(bufferSize >= 8);

  pvBuffer.assign(bufferSize, 0);
}

void mdtUsbControlTransfer::fillTransfer(libusb_transfer_cb_fn callback,
                                         uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength,
                                         unsigned int timeout)
{
  Q_ASSERT((int)pvBuffer.capacity() >= (8 + wLength));

  libusb_fill_control_setup(pvBuffer.data(), bmRequestType, bRequest, wValue, wIndex, wLength);
  libusb_fill_control_transfer(transfer(), deviceHandle(), pvBuffer.data(), callback, static_cast<void*>(this), timeout);
}

void mdtUsbControlTransfer::fillTransfer(libusb_transfer_cb_fn callback,
                                         uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, const std::vector< unsigned char>& data,
                                         unsigned int timeout )
{
  int16_t wLength = data.size();

  Q_ASSERT((int16_t)pvBuffer.capacity() >= (8 + wLength));

  ::memcpy(pvBuffer.data()+8, data.data(), wLength);
  libusb_fill_control_setup(pvBuffer.data(), bmRequestType, bRequest, wValue, wIndex, wLength);
  libusb_fill_control_transfer(transfer(), deviceHandle(), pvBuffer.data(), callback, static_cast<void*>(this), timeout);
}
