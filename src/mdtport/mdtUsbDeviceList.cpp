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
#include "mdtUsbDeviceList.h"
#include <QObject>

#include <QDebug>

mdtUsbDeviceList::mdtUsbDeviceList (libusb_context* ctx)
 : pvUsbContext(ctx),
   pvDeviceList(0)
{
}

mdtUsbDeviceList::~mdtUsbDeviceList()
{
  if(pvDeviceList != 0){
    libusb_free_device_list(pvDeviceList, 1);
  }
}

bool mdtUsbDeviceList::scan()
{
  if(!buildDevicesList()){
    return false;
  }
  for(int i = 0; i < pvDeviceDescriptors.size(); ++i){
    qDebug() << " -> Vendor: " <<  pvDeviceDescriptors.at(i).vendorName() << ", product: " << pvDeviceDescriptors.at(i).productName() << ", SN: " << pvDeviceDescriptors.at(i).serialNumber();
  }

  return true;
}

bool mdtUsbDeviceList::buildDevicesList()
{
  ssize_t devCount;
  ssize_t i;
  int err;

  // Let libusb build his USB devices list
  devCount = libusb_get_device_list(pvUsbContext, &pvDeviceList);
  if(devCount < 0){
    pvLastError.setError(QObject::tr("Failed to get devices list."), mdtError::Error);
    pvLastError.setSystemError(devCount, libusb_error_name(devCount));
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbDeviceList");
    pvLastError.commit();
    return false;
  }
  // Build descriptors for each device
  for(i = 0; i < devCount; ++i){
    Q_ASSERT(pvDeviceList[i] != 0);
    mdtUsbDeviceDescriptor device;
    err = device.fetchAttributes(pvDeviceList[i], true);
    if(err != 0){
      qDebug() << "mdtUsbDeviceList::buildDevicesList() - error while fetching device descriptor";
    }else{
      pvDeviceDescriptors.append(device);
    }
  }
  // Free libusb's list
  libusb_free_device_list(pvDeviceList, 1);
  pvDeviceList = 0;

  return true;
}
