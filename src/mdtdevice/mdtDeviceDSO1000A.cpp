/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtDeviceDSO1000A.h"
#include "mdtDeviceInfo.h"
#include "mdtPortTransaction.h"

mdtDeviceDSO1000A::mdtDeviceDSO1000A(QObject *parent)
 : mdtDeviceScpi(parent)
{
  pvCodec = new mdtFrameCodecScpi;
  // Setup port manager
  portManager()->config().setReadFrameSize(10000);
  portManager()->config().setReadQueueSize(5);
  portManager()->config().setReadTimeout(30000);
  portManager()->config().setWriteFrameSize(512);
  portManager()->config().setWriteQueueSize(1);
  connect(pvUsbtmcPortManager, SIGNAL(newReadenFrame(mdtPortTransaction)), this, SLOT(decodeReadenFrame(mdtPortTransaction)));
  connect(pvUsbtmcPortManager, SIGNAL(errorStateChanged(int)), this, SLOT(setStateFromPortError(int)));
}

mdtDeviceDSO1000A::~mdtDeviceDSO1000A()
{
  delete pvCodec;
}

mdtAbstractPort::error_t mdtDeviceDSO1000A::connectToDevice(const mdtDeviceInfo &devInfo)
{
  mdtDeviceInfo device;

  // Setup device info
  device = devInfo;
  device.setVendorId(0x0957);
  device.setProductId(0x0588);

  return mdtDeviceScpi::connectToDevice(device);
}

void mdtDeviceDSO1000A::decodeReadenFrame(mdtPortTransaction transaction)
{
}
