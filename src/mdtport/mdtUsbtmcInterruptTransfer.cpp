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
#include "mdtUsbtmcInterruptTransfer.h"
#include "mdtUsbtmcTransferHandler.h"

mdtUsbtmcInterruptTransfer::mdtUsbtmcInterruptTransfer ( mdtUsbtmcTransferHandler& th, mdtUsbEndpointDescriptor endpointDescriptor, libusb_device_handle* dev_handle, int bufferSize)
 : mdtUsbTransfer(dev_handle, bufferSize),
   pvTransferHandler(th),
   pvEndpointDescriptor(endpointDescriptor)
{
  Q_ASSERT(!pvEndpointDescriptor.isEmpty());

  pvBuffer.assign(bufferSize, 0);
}

void mdtUsbtmcInterruptTransfer::setup(unsigned int timeout)
{
  libusb_fill_interrupt_transfer(transfer(), deviceHandle(),
                                 pvEndpointDescriptor.address(), pvBuffer.data(), pvBuffer.capacity(),
                                 pvTransferHandler.interruptInTransferCallback, static_cast<void*>(this),
                                 timeout);
}
