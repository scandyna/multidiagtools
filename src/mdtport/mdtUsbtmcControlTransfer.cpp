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
#include "mdtUsbtmcControlTransfer.h"
#include "mdtUsbtmcTransferHandler.h"
#include <QtGlobal>
#include <cstring>

mdtUsbtmcControlTransfer::mdtUsbtmcControlTransfer (mdtUsbtmcTransferHandler & th, libusb_device_handle* dev_handle, int bufferSize )
 : mdtUsbControlTransfer(dev_handle, bufferSize),
   pvTransferHandler(th)
{
}

void mdtUsbtmcControlTransfer::setupClearEndpointHalt(uint8_t endpointNumber, unsigned int timeout)
{
  setupClearFeature(pvTransferHandler.controlTransferCallback,
                    mdtUsbControlTransfer::RequestRecipient_t::Endpoint, mdtUsbControlTransfer::StandardFeatureSelector_t::ENDPOINT_HALT, endpointNumber,
                    timeout);
}
