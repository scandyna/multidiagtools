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
#include <algorithm>

#include <QDebug>

mdtUsbDeviceList::mdtUsbDeviceList (libusb_context* ctx)
 : pvUsbContext(ctx),
   pvLibusDeviceList(0)
{
}

mdtUsbDeviceList::~mdtUsbDeviceList()
{
  freeLibusbDeviceList();
}

bool mdtUsbDeviceList::scan()
{
  // Free previous results and build list
  clear();
  if(!buildDevicesList(true, true)){
    return false;
  }
  for(int i = 0; i < pvDeviceDescriptors.size(); ++i){
    ///qDebug() << " -> Vendor: " <<  pvDeviceDescriptors.at(i).vendorName() << ", product: " << pvDeviceDescriptors.at(i).productName() << ", SN: " << pvDeviceDescriptors.at(i).serialNumber();
    qDebug() << pvDeviceDescriptors.at(i).idString();
  }

  return true;
}

libusb_device_handle* mdtUsbDeviceList::openDevice ( uint16_t idVendor, uint16_t idProduct, const QString& serialNumber )
{
  libusb_device_handle *dev_handle = 0;
  int err;

  // Free previous results and build list
  clear();
  if(!buildDevicesList(true, false)){
    return 0;
  }
  // Search requested device
  auto foundIt = pvDeviceDescriptors.end();
  if(serialNumber.isEmpty()){
    // Search by idVendor and idProduct
    deviceMatchDataVidPid md;
    md.idProduct = idProduct;
    md.idVendor = idVendor;
    foundIt = std::find_if(pvDeviceDescriptors.begin(), pvDeviceDescriptors.end(), md);
  }else{
    // Search by idVendor, idProduct and SN
    deviceMatchDataVidPidSn md;
    md.idProduct = idProduct;
    md.idVendor = idVendor;
    md.serialNumber = serialNumber;
    foundIt = std::find_if(pvDeviceDescriptors.begin(), pvDeviceDescriptors.end(), md);
  }
  if(foundIt == pvDeviceDescriptors.end()){
    // Device not found
    clear();
    QString str;
    str = "ID ";
    str += QString("0x%1:0x%2 ").arg(idVendor, 4, 16, QChar('0')).arg(idProduct, 4, 16, QChar('0'));
    str += " (SN:" + serialNumber + ")";
    pvLastError.setError(QObject::tr("Could not find device ") + str, mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbDeviceList");
    pvLastError.commit();
    return 0;
  }
  // Device found, open it
  Q_ASSERT(foundIt->libusbDevice() != 0);
  err = libusb_open(foundIt->libusbDevice(), &dev_handle);
  if(err != 0){
    pvLastError.setError(QObject::tr("Failed to open device ") + foundIt->idString(), mdtError::Error);
    pvLastError.setSystemError(err, libusb_strerror((libusb_error)err));
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbDeviceList");
    pvLastError.commit();
    return 0;
  }
  // Ok, cleanup and return handle
  Q_ASSERT(dev_handle != 0);
  clear();

  return dev_handle;
}

bool mdtUsbDeviceList::buildDevicesList(bool fetchDeviceActiveConfigOnly, bool freeLibusbListAfterBuild)
{
  ssize_t devCount;
  ssize_t i;

  // Let libusb build his USB devices list
  devCount = libusb_get_device_list(pvUsbContext, &pvLibusDeviceList);
  if(devCount < 0){
    pvLastError.setError(QObject::tr("Failed to get devices list."), mdtError::Error);
    pvLastError.setSystemError(devCount, libusb_error_name(devCount));
    MDT_ERROR_SET_SRC(pvLastError, "mdtUsbDeviceList");
    pvLastError.commit();
    return false;
  }
  // Build descriptors for each device
  for(i = 0; i < devCount; ++i){
    Q_ASSERT(pvLibusDeviceList[i] != 0);
    mdtUsbDeviceDescriptor device;
    if(!device.fetchAttributes(pvLibusDeviceList[i], fetchDeviceActiveConfigOnly)){
      pvLastError = device.lastError();
      return false;
    }
    pvDeviceDescriptors.append(device);
  }
  // Free libusb's list
  if(freeLibusbListAfterBuild){
    freeLibusbDeviceList();
  }

  return true;
}

void mdtUsbDeviceList::freeLibusbDeviceList()
{
  if(pvLibusDeviceList != 0){
    libusb_free_device_list(pvLibusDeviceList, 1);
    pvLibusDeviceList = 0;
  }
}

void mdtUsbDeviceList::clear()
{
  pvDeviceDescriptors.clear();
  freeLibusbDeviceList();
}
